#pragma once
#include <string>
#include "driver.hpp"
#include <linux/can.h>
#include <linux/can/raw.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <cstdio>
#include <cerrno>
#include <fcntl.h>
#include <chrono>

class SocketCANTransport : public damiao::ICanTransport {
public:
    SocketCANTransport(const std::string& interface = "can0") {
        sock_ = socket(PF_CAN, SOCK_RAW, CAN_RAW);
        if (sock_ < 0) {
            std::fprintf(stderr, "SocketCANTransport(%s): socket() failed: %s\n",
                          interface.c_str(), std::strerror(errno));
            return;
        }

        fcntl(sock_, F_SETFL, O_NONBLOCK);

        // Default SO_RCVBUF is easily outrun by 8 motors' worth of feedback
        // frames arriving in a burst (e.g. right after the USB-CAN bridge's
        // own poll cycle flushes a batch onto this vcan interface); widen it
        // so a burst queues instead of being silently dropped by the kernel
        // before recv() ever sees it.
        int rcvbuf = 1 << 20;  // 1 MiB
        setsockopt(sock_, SOL_SOCKET, SO_RCVBUF, &rcvbuf, sizeof(rcvbuf));

        // CAN_RAW sockets don't receive bus error frames (bus-off, arbitration
        // loss, controller overrun, ...) unless explicitly opted in - without
        // this, this transport has zero visibility into bus-level faults and
        // recv() just looks like "no data" during a bus problem.
        can_err_mask_t err_mask = CAN_ERR_MASK;
        setsockopt(sock_, SOL_CAN_RAW, CAN_RAW_ERR_FILTER, &err_mask, sizeof(err_mask));

        struct ifreq ifr{};
        std::strncpy(ifr.ifr_name, interface.c_str(), IFNAMSIZ - 1);
        if (ioctl(sock_, SIOCGIFINDEX, &ifr) < 0) {
            std::fprintf(stderr, "SocketCANTransport(%s): SIOCGIFINDEX failed: %s\n",
                          interface.c_str(), std::strerror(errno));
            return;
        }

        struct sockaddr_can addr{};
        addr.can_family = AF_CAN;
        addr.can_ifindex = ifr.ifr_ifindex;

        if (bind(sock_, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
            std::fprintf(stderr, "SocketCANTransport(%s): bind() failed: %s\n",
                          interface.c_str(), std::strerror(errno));
        }
    }

    bool send(const damiao::CanFrame& frame) override {
        struct can_frame cf{};
        cf.can_id = frame.arbitration_id;
        cf.can_dlc = frame.dlc;

        std::memcpy(cf.data, frame.data.data(), 8);

        return write(sock_, &cf, sizeof(cf)) == sizeof(cf);
    }

    bool recv(damiao::CanFrame& frame, int timeout_ms) override {
        (void)timeout_ms;
        struct can_frame cf{};
        int n = read(sock_, &cf, sizeof(cf));

        if (n < 0) {
            // EAGAIN/EWOULDBLOCK is the expected "nothing pending right now"
            // case at this loop's rate and isn't worth logging. Anything else
            // (ENOBUFS = kernel RX buffer overflowed and frames were dropped,
            // ENETDOWN = interface down, ...) is a real transport problem
            // that was previously indistinguishable from "no data" and
            // silently dropped. Rate-limited so a sustained fault doesn't
            // flood stderr from a 100-200 Hz read loop.
            if (errno != EAGAIN && errno != EWOULDBLOCK) {
                dropped_frames_++;
                auto now = std::chrono::steady_clock::now();
                if (now - last_error_log_ >= std::chrono::seconds(1)) {
                    std::fprintf(stderr,
                        "SocketCANTransport: recv error: %s (dropped_frames=%llu)\n",
                        std::strerror(errno),
                        static_cast<unsigned long long>(dropped_frames_));
                    last_error_log_ = now;
                }
            }
            return false;
        }

        if (n == 0) return false;

        if (cf.can_id & CAN_ERR_FLAG) {
            // Bus-level error frame (see CAN_RAW_ERR_FILTER above), not a
            // motor data frame - surface it but don't treat it as feedback.
            dropped_frames_++;
            auto now = std::chrono::steady_clock::now();
            if (now - last_error_log_ >= std::chrono::seconds(1)) {
                std::fprintf(stderr,
                    "SocketCANTransport: CAN bus error frame 0x%08x (dropped_frames=%llu)\n",
                    cf.can_id, static_cast<unsigned long long>(dropped_frames_));
                last_error_log_ = now;
            }
            return false;
        }

        frame.arbitration_id = cf.can_id;
        frame.dlc = cf.can_dlc;
        std::memcpy(frame.data.data(), cf.data, 8);

        return true;
    }

    uint64_t dropped_frames() const { return dropped_frames_; }

private:
    int sock_ = -1;
    uint64_t dropped_frames_ = 0;
    std::chrono::steady_clock::time_point last_error_log_{};
};
