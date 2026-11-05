#pragma once

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstring>
#include <optional>

namespace damiao {

struct CanFrame {
  std::uint32_t arbitration_id{0};
  bool is_extended_id{false};
  std::uint8_t dlc{8};
  std::array<std::uint8_t, 8> data{};
};

class ICanTransport {
 public:
  virtual ~ICanTransport() = default;
  virtual bool send(const CanFrame& frame) = 0;
  virtual bool recv(CanFrame& frame, int timeout_ms) = 0;
};

enum class ControlMode : std::uint32_t {
  MIT = 1,
  POSITION_VELOCITY = 2,
  VELOCITY = 3,
  FORCE_POSITION = 4,
};

enum class BaudCode : std::uint32_t {
  B125K = 0,
  B200K = 1,
  B250K = 2,
  B500K = 3,
  B1M = 4,
  B2M = 5,
  B2_5M = 6,
  B3_2M = 7,
  B4M = 8,
  B5M = 9,
};

enum class ErrorCode : std::uint8_t {
  DISABLED = 0x0,
  ENABLED = 0x1,
  OVERVOLTAGE = 0x8,
  UNDERVOLTAGE = 0x9,
  OVERCURRENT = 0xA,
  MOS_OVER_TEMP = 0xB,
  COIL_OVER_TEMP = 0xC,
  COMMUNICATION_LOST = 0xD,
  OVERLOAD = 0xE,
};

enum class RegisterId : std::uint8_t {
  MODE = 0x0A,
  BAUD = 0x23,
  PRECISE_POSITION = 0x50,
};

struct MappingRange {
  float p_min{-12.5f};
  float p_max{12.5f};
  float v_min{-30.0f};
  float v_max{30.0f};
  float t_min{-10.0f};
  float t_max{10.0f};
};

struct FeedbackFrame {
  std::uint32_t message_id{0};
  std::uint8_t node_low_nibble{0};
  std::uint8_t err{0};
  float pos{0.0f};
  float vel{0.0f};
  float torque{0.0f};
  std::uint8_t t_mos_c{0};
  std::uint8_t t_rotor_c{0};
};

class DamiaoCanDriver {
 public:
  explicit DamiaoCanDriver(ICanTransport& transport,
                           std::uint16_t motor_id = 0x01,
                           std::uint16_t master_id = 0x00,
                           MappingRange mapping = {})
      : transport_(transport), motor_id_(motor_id), master_id_(master_id), mapping_(mapping) {}

  std::uint16_t motor_id() const { return motor_id_; }
  std::uint16_t master_id() const { return master_id_; }
  const MappingRange& mapping() const { return mapping_; }

  bool send_mit(float p_des, float v_des, float kp, float kd, float t_ff) {
    const std::uint16_t p_u16 = float_to_uint(p_des, mapping_.p_min, mapping_.p_max, 16);
    const std::uint16_t v_u12 = static_cast<std::uint16_t>(float_to_uint(v_des, mapping_.v_min, mapping_.v_max, 12));
    const std::uint16_t kp_u12 = static_cast<std::uint16_t>(float_to_uint(kp, 0.0f, 500.0f, 12));
    const std::uint16_t kd_u12 = static_cast<std::uint16_t>(float_to_uint(kd, 0.0f, 5.0f, 12));
    const std::uint16_t t_u12 = static_cast<std::uint16_t>(float_to_uint(t_ff, mapping_.t_min, mapping_.t_max, 12));

    CanFrame frame = make_frame(motor_id_);
    frame.data[0] = static_cast<std::uint8_t>((p_u16 >> 8) & 0xFF);
    frame.data[1] = static_cast<std::uint8_t>(p_u16 & 0xFF);
    frame.data[2] = static_cast<std::uint8_t>((v_u12 >> 4) & 0xFF);
    frame.data[3] = static_cast<std::uint8_t>(((v_u12 & 0x0F) << 4) | ((kp_u12 >> 8) & 0x0F));
    frame.data[4] = static_cast<std::uint8_t>(kp_u12 & 0xFF);
    frame.data[5] = static_cast<std::uint8_t>((kd_u12 >> 4) & 0xFF);
    frame.data[6] = static_cast<std::uint8_t>(((kd_u12 & 0x0F) << 4) | ((t_u12 >> 8) & 0x0F));
    frame.data[7] = static_cast<std::uint8_t>(t_u12 & 0xFF);
    return transport_.send(frame);
  }

  bool send_posvel(float p_des, float v_des) {
    CanFrame frame = make_frame(0x100u + motor_id_);
    put_f32_le(frame.data, 0, p_des);
    put_f32_le(frame.data, 4, v_des);
    return transport_.send(frame);
  }

  bool send_velocity(float v_des) {
    CanFrame frame = make_frame(0x200u + motor_id_);
    put_f32_le(frame.data, 0, v_des);
    frame.data[4] = 0;
    frame.data[5] = 0;
    frame.data[6] = 0;
    frame.data[7] = 0;
    return transport_.send(frame);
  }

  bool send_force_position(float p_des, float v_limit_rad_s, float i_limit_pu) {
    const std::uint16_t v_scaled =
        static_cast<std::uint16_t>(clamp(v_limit_rad_s * 100.0f, 0.0f, 10000.0f));
    const std::uint16_t i_scaled =
        static_cast<std::uint16_t>(clamp(i_limit_pu * 10000.0f, 0.0f, 10000.0f));
    CanFrame frame = make_frame(0x300u + motor_id_);
    put_f32_le(frame.data, 0, p_des);
    put_u16_le(frame.data, 4, v_scaled);
    put_u16_le(frame.data, 6, i_scaled);
    return transport_.send(frame);
  }

  bool enable_motor() { return send_magic_cmd(0xFC); }
  bool disable_motor() { return send_magic_cmd(0xFD); }
  bool set_zero_position() { return send_magic_cmd(0xFE); }

  bool read_register(std::uint8_t rid) {
    CanFrame frame = make_frame(0x7FFu);
    frame.data[0] = static_cast<std::uint8_t>(motor_id_ & 0xFF);
    frame.data[1] = static_cast<std::uint8_t>((motor_id_ >> 8) & 0xFF);
    frame.data[2] = 0x33;
    frame.data[3] = rid;
    frame.data[4] = frame.data[5] = frame.data[6] = frame.data[7] = 0;
    return transport_.send(frame);
  }

  bool write_register_u32(std::uint8_t rid, std::uint32_t value) {
    CanFrame frame = make_frame(0x7FFu);
    frame.data[0] = static_cast<std::uint8_t>(motor_id_ & 0xFF);
    frame.data[1] = static_cast<std::uint8_t>((motor_id_ >> 8) & 0xFF);
    frame.data[2] = 0x55;
    frame.data[3] = rid;
    put_u32_le(frame.data, 4, value);
    return transport_.send(frame);
  }

  bool write_register_f32(std::uint8_t rid, float value) {
    std::uint32_t bits = 0;
    std::memcpy(&bits, &value, sizeof(bits));
    return write_register_u32(rid, bits);
  }

  bool store_parameters() {
    CanFrame frame = make_frame(0x7FFu);
    frame.data[0] = static_cast<std::uint8_t>(motor_id_ & 0xFF);
    frame.data[1] = static_cast<std::uint8_t>((motor_id_ >> 8) & 0xFF);
    frame.data[2] = 0xAA;
    frame.data[3] = 0x01;
    frame.data[4] = frame.data[5] = frame.data[6] = frame.data[7] = 0;
    return transport_.send(frame);
  }

  bool switch_mode(ControlMode mode) {
    return write_register_u32(static_cast<std::uint8_t>(RegisterId::MODE), static_cast<std::uint32_t>(mode));
  }

  bool set_can_baud_code(BaudCode code) {
    return write_register_u32(static_cast<std::uint8_t>(RegisterId::BAUD), static_cast<std::uint32_t>(code));
  }

  std::optional<CanFrame> recv(int timeout_ms = 200) {
    CanFrame out;
    if (!transport_.recv(out, timeout_ms)) return std::nullopt;
    return out;
  }

  std::optional<FeedbackFrame> decode_feedback(const CanFrame& frame) const {
    if (frame.dlc < 8) return std::nullopt;
    const auto& d = frame.data;
    const std::uint8_t id_err = d[0];
    const std::uint8_t node_low_nibble = static_cast<std::uint8_t>(id_err & 0x0F);
    const std::uint8_t err = static_cast<std::uint8_t>((id_err >> 4) & 0x0F);
    const std::uint16_t pos_raw = static_cast<std::uint16_t>((d[1] << 8) | d[2]);
    const std::uint16_t vel_raw = static_cast<std::uint16_t>((d[3] << 4) | (d[4] >> 4));
    const std::uint16_t tq_raw = static_cast<std::uint16_t>(((d[4] & 0x0F) << 8) | d[5]);

    FeedbackFrame fb;
    fb.message_id = frame.arbitration_id;
    fb.node_low_nibble = node_low_nibble;
    fb.err = err;
    fb.pos = uint_to_float(pos_raw, mapping_.p_min, mapping_.p_max, 16);
    fb.vel = uint_to_float(vel_raw, mapping_.v_min, mapping_.v_max, 12);
    fb.torque = uint_to_float(tq_raw, mapping_.t_min, mapping_.t_max, 12);
    fb.t_mos_c = d[6];
    fb.t_rotor_c = d[7];
    return fb;
  }

 private:
  ICanTransport& transport_;
  std::uint16_t motor_id_;
  std::uint16_t master_id_;
  MappingRange mapping_;

  static float clamp(float value, float lo, float hi) {
    return std::max(lo, std::min(hi, value));
  }

  static std::uint32_t float_to_uint(float x, float x_min, float x_max, int bits) {
    x = clamp(x, x_min, x_max);
    const float span = x_max - x_min;
    if (span <= 0.0f) return 0;
    const std::uint32_t max_int = (1u << bits) - 1u;
    return static_cast<std::uint32_t>((x - x_min) * static_cast<float>(max_int) / span);
  }

  static float uint_to_float(std::uint32_t x, float x_min, float x_max, int bits) {
    const float span = x_max - x_min;
    if (span <= 0.0f) return x_min;
    const std::uint32_t max_int = (1u << bits) - 1u;
    return (static_cast<float>(x) * span / static_cast<float>(max_int)) + x_min;
  }

  static void put_u16_le(std::array<std::uint8_t, 8>& dst, int offset, std::uint16_t v) {
    dst[offset] = static_cast<std::uint8_t>(v & 0xFF);
    dst[offset + 1] = static_cast<std::uint8_t>((v >> 8) & 0xFF);
  }

  static void put_u32_le(std::array<std::uint8_t, 8>& dst, int offset, std::uint32_t v) {
    dst[offset] = static_cast<std::uint8_t>(v & 0xFF);
    dst[offset + 1] = static_cast<std::uint8_t>((v >> 8) & 0xFF);
    dst[offset + 2] = static_cast<std::uint8_t>((v >> 16) & 0xFF);
    dst[offset + 3] = static_cast<std::uint8_t>((v >> 24) & 0xFF);
  }

  static void put_f32_le(std::array<std::uint8_t, 8>& dst, int offset, float v) {
    std::uint32_t bits = 0;
    std::memcpy(&bits, &v, sizeof(bits));
    put_u32_le(dst, offset, bits);
  }

  static CanFrame make_frame(std::uint32_t arbitration_id) {
    CanFrame frame;
    frame.arbitration_id = arbitration_id;
    frame.is_extended_id = false;
    frame.dlc = 8;
    frame.data.fill(0);
    return frame;
  }

  bool send_magic_cmd(std::uint8_t tail_byte) {
    CanFrame frame = make_frame(motor_id_);
    frame.data.fill(0xFF);
    frame.data[7] = tail_byte;
    return transport_.send(frame);
  }
};

}  // namespace damiao
