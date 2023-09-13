
#include <otto_deliberation/otto_autonomy.h>
#include <lane_provider_msgs/srv/list_tracks.hpp>
#include <chrono>
#include <functional>
#include <rclcpp/rclcpp.hpp>

class OttoNode : public rclcpp::Node
{
public:
	OttoNode() : Node("otto_deliberation_node"), m_autonomy(shared_from_this()) {
		RCLCPP_INFO(get_logger(), "Otto deliberation node starting");
		m_namespace = get_namespace();
		m_timer = this->create_wall_timer(std::chrono::milliseconds(50), std::bind(&OttoNode::TimerCallback, this));
	}

	bool InitializeNode() {
		std::string assigned_lane_name = declare_parameter<std::string>("assigned_lane", "");
		get_parameter<std::string>("assigned_lane", assigned_lane_name);

		if (assigned_lane_name.empty()) {
			RCLCPP_ERROR(get_logger(), "No assigned lane, terminating");
			return false;
		}

		std::string lane_track_service = declare_parameter<std::string>("lane_track_service", "/lane_track_service");
		get_parameter<std::string>("lane_track_service", lane_track_service);
		auto laneTracksClient = create_client<lane_provider_msgs::srv::ListTracks>(lane_track_service);
		if (!laneTracksClient->wait_for_service(std::chrono::seconds(2))) {
			RCLCPP_ERROR(get_logger(), "Lane track service named %s not available", lane_track_service.c_str());
			return false;
		}

		auto lane_request = std::make_shared<lane_provider_msgs::srv::ListTracks::Request>();
		lane_request->lane_name = assigned_lane_name;
		auto lane_track_response_future = laneTracksClient->async_send_request(lane_request);
		if (rclcpp::spin_until_future_complete(shared_from_this(), lane_track_response_future) != rclcpp::FutureReturnCode::SUCCESS) {
			RCLCPP_ERROR(get_logger(), "Failed to call service %s", lane_track_service.c_str());
			return false;
		}
		auto lane_paths = lane_track_response_future.get()->lane_paths;
		if (lane_paths.empty()) {
			RCLCPP_ERROR(get_logger(), "No paths returned for lane %s", assigned_lane_name.c_str());
			return false;
		}
		auto assigned_lane = lane_paths[0];

		if (assigned_lane.path_names.empty() || assigned_lane.path_names.size() != assigned_lane.lane_paths.size()) {
			RCLCPP_ERROR(get_logger(), "Empty or incorrectly specified paths specified for lane %s",
			             assigned_lane_name.c_str());
			return false;
		}

		Tasks tasks;
		for (int i = 0; i < assigned_lane.path_names.size(); ++i) {  // Construct Tasks
			tasks.push(MakeTask(assigned_lane.path_names[i], assigned_lane.lane_paths[i].poses));
		}
		m_autonomy.SetTasks(tasks);
		return true;
	}

private:
	void TimerCallback() {  // Get tasks, updates etc. here
		m_autonomy.Update();
	}

	Task MakeTask(std::string path_name, NavPath path) {
		Task t;
		t.m_taskKey = path_name;
		t.m_path = path;
		// TODO - map names to statuses / reverse / lock. Use const map or parameters for mapping.

		return t;
	}

	// TODO:
	// Listen to routine (task list), set it for otto autonomy
	// Listen to events of loading / unloading for your lane, ignore if not in correct status (e.g. WAIT_LOAD).

	rclcpp::TimerBase::SharedPtr m_timer;
	std::string m_namespace;
	OttoAutonomy m_autonomy;
};

int main(int argc, char** argv) {
	rclcpp::init(argc, argv);
	auto otto_node = std::make_shared<OttoNode>();
	rclcpp::spin(otto_node);
	rclcpp::shutdown();
	RCLCPP_INFO(otto_node->get_logger(), "Otto terminating");
	return 0;
}
