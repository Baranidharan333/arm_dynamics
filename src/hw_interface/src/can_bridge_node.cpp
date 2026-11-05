// Brings up the native SocketCAN interface can0 for the arm motors.
//
// `ip link set can0 down` (failure ignored, e.g. already down),
// `ip link set can0 type can bitrate <BITRATE>`, `ip link set can0 up`, all via `sudo -S`.
// The node then stays alive until shutdown so bringup.launch.py can keep treating it as the
// CAN service process.
//
// Environment:
//   VCAN_SUDO_PASSWORD  sudo password (required)
//   CAN_BITRATE         bitrate in bit/s (default 1000000)
//
// The arms' ros2_control blocks must use can_interface can0.

#include <sys/wait.h>
#include <unistd.h>

#include <cstdlib>
#include <stdexcept>
#include <string>
#include <vector>

#include "rclcpp/rclcpp.hpp"

namespace {

// Runs `sudo -S <args...>` with the password on stdin. Returns the exit status.
// args go straight to execvp, never through a shell.
int run_sudo(const std::vector<std::string>& args, const std::string& password) {
  int stdin_pipe[2];
  if (pipe(stdin_pipe) != 0) {
    throw std::runtime_error("pipe() failed");
  }

  const pid_t pid = fork();
  if (pid < 0) {
    close(stdin_pipe[0]);
    close(stdin_pipe[1]);
    throw std::runtime_error("fork() failed");
  }

  if (pid == 0) {
    dup2(stdin_pipe[0], STDIN_FILENO);
    close(stdin_pipe[0]);
    close(stdin_pipe[1]);

    std::vector<char*> argv;
    argv.push_back(const_cast<char*>("sudo"));
    argv.push_back(const_cast<char*>("-S"));
    for (const auto& a : args) argv.push_back(const_cast<char*>(a.c_str()));
    argv.push_back(nullptr);

    execvp("sudo", argv.data());
    _exit(127);
  }

  close(stdin_pipe[0]);
  const std::string input = password + "\n";
  // A failed write only means sudo did not read the password; its exit status still counts.
  [[maybe_unused]] const ssize_t written = write(stdin_pipe[1], input.data(), input.size());
  close(stdin_pipe[1]);

  int status = 0;
  if (waitpid(pid, &status, 0) < 0) {
    throw std::runtime_error("waitpid() failed");
  }
  return WIFEXITED(status) ? WEXITSTATUS(status) : -1;
}

std::string join(const std::vector<std::string>& args) {
  std::string out;
  for (const auto& a : args) out += (out.empty() ? "" : " ") + a;
  return out;
}

void sudo_checked(const std::vector<std::string>& args, const std::string& password) {
  const int rc = run_sudo(args, password);
  if (rc != 0) {
    throw std::runtime_error("'sudo " + join(args) + "' failed with exit code " +
                             std::to_string(rc));
  }
}

}  // namespace

int main(int argc, char** argv) {
  rclcpp::init(argc, argv);
  auto node = std::make_shared<rclcpp::Node>("can_bridge_node");
  const auto logger = node->get_logger();

  const char* password_env = std::getenv("VCAN_SUDO_PASSWORD");
  const char* bitrate_env = std::getenv("CAN_BITRATE");
  const std::string bitrate = bitrate_env ? bitrate_env : "1000000";

  if (password_env == nullptr) {
    RCLCPP_FATAL(logger, "Error: VCAN_SUDO_PASSWORD environment variable not set.");
    rclcpp::shutdown();
    return 1;
  }
  const std::string password = password_env;

  try {
    // Down may fail if the interface is already down; that is fine.
    run_sudo({"ip", "link", "set", "can0", "down"}, password);

    sudo_checked({"ip", "link", "set", "can0", "type", "can", "bitrate", bitrate}, password);
    sudo_checked({"ip", "link", "set", "can0", "up"}, password);
  } catch (const std::exception& e) {
    RCLCPP_FATAL(logger, "%s", e.what());
    rclcpp::shutdown();
    return 1;
  }

  RCLCPP_INFO(logger, "CAN interface can0 is up at %s bit/s.", bitrate.c_str());

  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}
