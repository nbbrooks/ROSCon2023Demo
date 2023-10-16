#include "gripper.h"
#include <chrono>

using namespace std::chrono_literals;

namespace Gripper
{
    GripperController::GripperController(std::shared_ptr<rclcpp::Node> node, const std::string& topic)
    {
        m_client_ptr = rclcpp_action::create_client<control_msgs::action::GripperCommand>(node, topic);
    }

    bool GripperController::SendGripCommand(bool shouldAttach)
    {
        auto goal = control_msgs::action::GripperCommand::Goal();
        goal.command.position = shouldAttach ? 0.0 : 1.0;
        goal.command.max_effort = 10000.0;
        auto future = m_client_ptr->async_send_goal(goal);

        future.wait();

        auto goal_handle = future.get();

        auto result_future = m_client_ptr->async_get_result(goal_handle);

        std::future_status status;
        do {
          switch (status = result_future.wait_for(5s); status) {
          case std::future_status::deferred:
            std::cout << "deferred\n";
            break;
          case std::future_status::timeout:
            std::cout << "timeout\n";
            break;
          case std::future_status::ready:
            std::cout << "ready!\n";
            break;
          }
        } while (status != std::future_status::ready);

        auto result = result_future.get().result;

        return result->reached_goal;
    }

    bool GripperController::Grip()
    {
        return SendGripCommand(true);
    }

    bool GripperController::Release()
    {
        return SendGripCommand(false);
    }
} // namespace Gripper
