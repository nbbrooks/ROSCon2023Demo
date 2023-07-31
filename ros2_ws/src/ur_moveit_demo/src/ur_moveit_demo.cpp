#include <memory>

#include <thread>
#include <chrono>
#include <rclcpp/rclcpp.hpp>
// #include <rcutils/logging.h>
#include <control_msgs/action/gripper_command.hpp>
#include <tf2_msgs/msg/tf_message.hpp>

#include <moveit/move_group_interface/move_group_interface.h>
#include <moveit_visual_tools/moveit_visual_tools.h>
#include <moveit/planning_scene_interface/planning_scene_interface.h>

#include "tf2_ros/transform_listener.h"

#include <iostream>



bool SendGripperGrip(rclcpp_action::Client<control_msgs::action::GripperCommand>::SharedPtr client_ptr);


bool SendGripperrelease(rclcpp_action::Client<control_msgs::action::GripperCommand>::SharedPtr client_ptr);

geometry_msgs::msg::Pose getBoxLocation(rclcpp::Node::SharedPtr node, tf2_ros::Buffer& tf_buffer_, std::string box_name = "Box3/box")
{
  geometry_msgs::msg::TransformStamped box_transform;
  try {
    box_transform = tf_buffer_.lookupTransform(
        "world", box_name,
        tf2::TimePointZero);
  } catch (const tf2::TransformException & ex) {
    std::cerr << "Could not transform " <<  ex.what() << std::endl;
    std::abort();
  }
  geometry_msgs::msg::Pose box_pose;
  box_pose.position.x = box_transform.transform.translation.x;
  box_pose.position.y = box_transform.transform.translation.y;
  box_pose.position.z = box_transform.transform.translation.z  +  0.3134952 * 0.3339678;
  box_pose.orientation.x = box_transform.transform.rotation.x;
  box_pose.orientation.y = box_transform.transform.rotation.y;
  box_pose.orientation.z = box_transform.transform.rotation.z;
  box_pose.orientation.w = box_transform.transform.rotation.w;
  return box_pose;
}

geometry_msgs::msg::Pose toPose(geometry_msgs::msg::TransformStamped transform)
{
  geometry_msgs::msg::Pose pose;
  pose.position.x = transform.transform.translation.x;
  pose.position.y = transform.transform.translation.y;
  pose.position.z = transform.transform.translation.z;
  pose.orientation.x = transform.transform.rotation.x;
  pose.orientation.y = transform.transform.rotation.y;
  pose.orientation.z = transform.transform.rotation.z;
  pose.orientation.w = transform.transform.rotation.w;
  return pose;
}

geometry_msgs::msg::Pose getPalletLocation(rclcpp::Node::SharedPtr node, tf2_ros::Buffer& tf_buffer_)
{
  geometry_msgs::msg::TransformStamped box_transform;
  try {
    box_transform = tf_buffer_.lookupTransform(
        "world", "EuroPallet1/europallet",
        tf2::TimePointZero);
  } catch (const tf2::TransformException & ex) {
    std::cerr << "Could not transform " <<  ex.what() << std::endl;
    std::abort();
  }
  geometry_msgs::msg::Pose box_pose;
  box_pose.position.x = box_transform.transform.translation.x;
  box_pose.position.y = box_transform.transform.translation.y;
  box_pose.position.z = box_transform.transform.translation.z;
  box_pose.orientation.x = box_transform.transform.rotation.x;
  box_pose.orientation.y = box_transform.transform.rotation.y;
  box_pose.orientation.z = box_transform.transform.rotation.z;
  box_pose.orientation.w = box_transform.transform.rotation.w;
  return box_pose;
}



Eigen::Quaterniond fromMsg(const geometry_msgs::msg::Quaternion& msg)
{
      Eigen::Quaterniond q;
      q.x() = msg.x;
      q.y() = msg.y;
      q.z() = msg.z;
      q.w() = msg.w;
      return q;
}

Eigen::Vector3d fromMsg(const geometry_msgs::msg::Point msg)
{
      Eigen::Vector3d v{msg.x, msg.y, msg.z};
      return v;
}
Eigen::Vector3d fromMsg(const geometry_msgs::msg::Vector3 msg)
{
      Eigen::Vector3d v{msg.x, msg.y, msg.z};
      return v;
}

const geometry_msgs::msg::Point toMsgPoint(Eigen::Vector3d v)
{
      geometry_msgs::msg::Point msg;
      msg.x = v.x();
      msg.y = v.y();
      msg.z = v.z();
      return msg;
}

const geometry_msgs::msg::Quaternion toMsg(const Eigen::Quaterniond& q)
{
      geometry_msgs::msg::Quaternion msg;
      msg.x = q.x();
      msg.y = q.y();
      msg.z = q.z();
      msg.w = q.w();
      return msg;
}

moveit_msgs::msg::CollisionObject CreateBoxCollision(const std::string& name, const Eigen::Vector3d dimension,
                                                     const Eigen::Vector3d location, const Eigen::Quaterniond& rot = Eigen::Quaterniond::Identity())
{
      moveit_msgs::msg::CollisionObject collision_object;
      collision_object.header.frame_id = "world";
      collision_object.id = name;
      shape_msgs::msg::SolidPrimitive primitive;
      primitive.type = primitive.BOX;
      primitive.dimensions.push_back(dimension.x());
      primitive.dimensions.push_back(dimension.y());
      primitive.dimensions.push_back(dimension.z());
      collision_object.primitives.push_back(primitive);
      geometry_msgs::msg::Pose box_pose;
      box_pose.position = toMsgPoint(location);
      box_pose.orientation = toMsg(rot);
      collision_object.primitive_poses.push_back(box_pose);
      collision_object.operation = collision_object.ADD;
      return collision_object;
}

constexpr float BoxHeight = 0.3f;
const Eigen::Vector3d TableDimension{0.950, 0.950, 0.611};
const Eigen::Vector3d ConveyorDimensions{2.0, 0.7, 0.15};
const Eigen::Vector3d PickupLocation{0.890, 0, 0.049};
// const Eigen::Vector3d BoxDimension{0.2, 0.2, 0.2};
const Eigen::Vector3d BoxDimension{1.8231806 * 0.3768892 * 0.3339678, 0.889312 * 0.7703997 * 0.3339678, 1.0 * 0.626915 * 0.3339678};
const Eigen::Vector3d PalletDimensions{0.769, 1.11,  0.111};
const Eigen::Vector3d RobotBase{-3.1591539, 0.2811049,  0.6189841};



// Robot configuration to pickup

//const  std::map<std::string, double> PickupConfig
//    {
//        {"wrist_1_joint", -0.8600329160690308},
//        {"wrist_2_joint", 1.563552975654602},
//        {"elbow_joint", -1.6101552248001099},
//        {"shoulder_pan_joint", -2.6487786769866943},
//        {"shoulder_lift_joint", -2.2290878295898438},
//        {"wrist_3_joint", -5.687946796417236}
//    };


const  std::map<std::string, double> PickupConfig
    {
        {"wrist_1_joint", -0.8385483622550964},
        {"wrist_2_joint", 1.5643877983093262},
        {"elbow_joint", -1.550349235534668},
        {"shoulder_pan_joint", -2.7139534950256348},
        {"shoulder_lift_joint", -2.314471483230591},
        {"wrist_3_joint", -5.752989768981934}
    };

      const  std::map<std::string, double> LiftConfig
    {
        {"wrist_1_joint", -1.6993759870529175},
        {"wrist_2_joint", 1.5634684562683105},
        {"elbow_joint", -1.0284128189086914},
        {"shoulder_pan_joint", -2.644500255584717},
        {"shoulder_lift_joint", -1.9712634086608887},
        {"wrist_3_joint", -5.683835983276367}
    };
const  std::map<std::string, double> DropConfig
    {
        {"wrist_1_joint", -1.5789473056793213},
        {"wrist_2_joint", 1.5672444105148315},
        {"elbow_joint", -0.9531064033508301},
        {"shoulder_pan_joint", -0.15679530799388885},
        {"shoulder_lift_joint", -2.199610710144043},
        {"wrist_3_joint", -3.1959822177886963}
    };


const Eigen::Vector3d DropLocation {-1.0, 0.0, 0.420};
const Eigen::Quaterniond DropOrientation {0.036, 0.699, -0.037, 0.714};

constexpr double BoxSize = 0.12;
const std::vector<Eigen::Vector3d> Pattern {
    {-1,-1, -2.0},
    {-1, 1, -2.0},
    {1, -1, -2.0},
    {1,  1, -2.0},

    {0,  0, 0},

};

bool PlanAndGo(moveit::planning_interface::MoveGroupInterface & move_group_interface, const std::map<std::string, double>& config)
{
    move_group_interface.setJointValueTarget(config);

    using moveit::planning_interface::MoveGroupInterface;
    moveit::planning_interface::MoveGroupInterface::Plan plan;
    auto isOk = move_group_interface.plan(plan);
    if (!isOk)
    {
        return false;
    }
    auto successExecution = move_group_interface.execute(plan);

    if (!successExecution)
    {
        return false;
    }
    return true;
}

bool PlanAndGo(moveit::planning_interface::MoveGroupInterface & move_group_interface)
{
    using moveit::planning_interface::MoveGroupInterface;
    moveit::planning_interface::MoveGroupInterface::Plan plan;
    auto isOk = move_group_interface.plan(plan);
    if (!isOk)
    {
        return false;
    }
    auto successExecution = move_group_interface.execute(plan);

    if (!successExecution)
    {
        return false;
    }
    return true;
}

bool PlanAndGo(moveit::planning_interface::MoveGroupInterface & move_group_interface, Eigen::Vector3d position, Eigen::Quaterniond orientation)
{
    using moveit::planning_interface::MoveGroupInterface;

    geometry_msgs::msg::Pose pose;
    pose.position = toMsgPoint(position);
    pose.orientation = toMsg(orientation);
    move_group_interface.setApproximateJointValueTarget(pose, "gripper_link");
//        move_group_interface.setPoseTarget(pose, "gripper_link");

    moveit::planning_interface::MoveGroupInterface::Plan plan;
    auto isOk = move_group_interface.plan(plan);
    if (!isOk)
    {
        return false;
    }
    auto successExecution = move_group_interface.execute(plan);

    if (!successExecution)
    {
        return false;
    }
    return true;
}

void tf_callback(moveit::planning_interface::PlanningSceneInterface& scene, tf2_msgs::msg::TFMessage tf)
{
  for (auto tf_ : tf.transforms)
  {
      if(tf_.child_frame_id == "Box3/box")
      {
        std::cerr << "got tf of " << tf_.header.frame_id << " to " << tf_.child_frame_id << "\n";
        auto box_pose = toPose(tf_);
        scene.applyCollisionObject(CreateBoxCollision("box", BoxDimension, fromMsg(box_pose.position)));
      }
  }
}

void tf_callback_2(rclcpp::Publisher<moveit_msgs::msg::CollisionObject>::SharedPtr publisher, tf2_msgs::msg::TFMessage tf)
{
  for (auto tf_ : tf.transforms)
  {
      if(tf_.child_frame_id == "Box3/box")
      {
        std::cerr << "got tf of " << tf_.header.frame_id << " to " << tf_.child_frame_id << "  " << tf_.transform.translation.x <<" " << tf_.transform.translation.y << tf_.transform.translation.z << "\n";


        auto box_pose = toPose(tf_);
                
        auto message = CreateBoxCollision("box", BoxDimension, fromMsg(box_pose.position));
        publisher->publish(message);
      }
  }
}

        
// geometry_msgs::msg::Pose getPalletLocation(rclcpp::Node::SharedPtr node, tf2_ros::Buffer& tf_buffer_)
void tf_callback_3(rclcpp::Publisher<moveit_msgs::msg::CollisionObject>::SharedPtr publisher, rclcpp::Node::SharedPtr node, tf2_ros::Buffer& tf_buffer_, tf2_msgs::msg::TFMessage tf)
{
  // for (auto tf_ : tf.transforms)
  // {
  //     if(tf_.child_frame_id == "Box3/box")
  //     {
  //       std::cerr << "got tf of " << tf_.header.frame_id << " to " << tf_.child_frame_id << "  " << tf_.transform.translation.x <<" " << tf_.transform.translation.y << tf_.transform.translation.z << "\n";


  //       auto box_pose = getBoxLocation(node, tf_buffer_);


  //       std::cerr << "sending box pose " <<  box_pose.position.x <<" " << box_pose.position.y << box_pose.position.z << "\n";

  //       auto message = CreateBoxCollision("box", BoxDimension, fromMsg(box_pose.position));


  //       // message.operation = message.MOVE;
  //       // if(publish)
  //       // {
  //         // publisher->publish(message);
  //       // }
  //     }
  // }
}


std::vector<std::string> box_names = {
  "Box3/box",
  "Box6/box",
  "Box4/box",
  "Box5/box",
  "Box7/box",
  "Box8/box",
  "Box1/box",
};


// std::vector<std::string> box_names = {
//   "Box1/box",
//   "Box2/box",
//   "Box3/box",
//   "Box4/box",
//   "Box5/box",
//   "Box6/box",
//   "Box7/box",
//   "Box8/box"
// };


int main(int argc, char * argv[])
{
  // Initialize ROS and create the Node
  rclcpp::init(argc, argv);
  auto const node = std::make_shared<rclcpp::Node>(
    "hello_moveit",
    rclcpp::NodeOptions().automatically_declare_parameters_from_overrides(true)
  );

  // Create a ROS logger
  auto const logger = rclcpp::get_logger("ur_moveit_demo");

  // Spin up a SingleThreadedExecutor for MoveItVisualTools to interact with ROS
  rclcpp::executors::SingleThreadedExecutor executor;
  executor.add_node(node);
  auto spinner = std::thread([&executor]() { executor.spin(); });

  robot_model_loader::RobotModelLoader robot_model_loader(node);
  const moveit::core::RobotModelPtr& kinematic_model = robot_model_loader.getModel();
  RCLCPP_INFO(logger, "Model frame: %s", kinematic_model->getModelFrame().c_str());

  moveit::core::RobotStatePtr robot_state(new moveit::core::RobotState(kinematic_model));
  robot_state->setToDefaultValues();
  const moveit::core::JointModelGroup* joint_model_group = kinematic_model->getJointModelGroup("ur_manipulator");

  const std::vector<std::string>& joint_names = joint_model_group->getVariableNames();

  for (auto joint_name : joint_names)
  {
      RCLCPP_INFO(logger, "Joint %s", joint_name.c_str());
  }

  // Create the MoveIt MoveGroup Interface
  using moveit::planning_interface::MoveGroupInterface;
  auto move_group_interface = MoveGroupInterface(node, "ur_manipulator");

  auto client_ptr = rclcpp_action::create_client<control_msgs::action::GripperCommand>(node, "/gripper_server");
  auto tf_buffer_ = tf2_ros::Buffer(node->get_clock());

//   auto timer_interface = std::make_shared<tf2_ros::CreateTimerROS>(
//       this->get_node_base_interface(),
//       this->get_node_timers_interface());
//   tf2_buffer_->setCreateTimerInterface(timer_interface);

  auto tf_listener_ = std::make_shared<tf2_ros::TransformListener>(tf_buffer_);



//   point_sub_.subscribe(this, "/tf");
//       tf2_filter_ = std::make_shared<tf2_ros::MessageFilter<geometry_msgs::msg::PointStamped>>(
//       point_sub_, *tf2_buffer_, target_frame_, 100, this->get_node_logging_interface(),
//       this->get_node_clock_interface(), buffer_timeout);
//     // Register a callback with tf2_ros::MessageFilter to be called when transforms are available
//     tf2_filter_->registerCallback(&PoseDrawer::msgCallback, this);


//   message_filters::Subscriber<geometry_msgs::msg::PointStamped> point_sub_;
//   std::shared_ptr<tf2_ros::MessageFilter<geometry_msgs::msg::PointStamped>> tf2_filter_;


  moveit::planning_interface::PlanningSceneInterface planning_scene_interface;
 
  planning_scene_interface.applyCollisionObject(CreateBoxCollision("table", TableDimension, Eigen::Vector3d{0,0, -TableDimension.z()/2.0}));
  planning_scene_interface.applyCollisionObject(CreateBoxCollision("conveyor", ConveyorDimensions, Eigen::Vector3d{ConveyorDimensions.x()/2+0.75,0, -0.2 -0.04}));


  // auto callback_tf_ = [&planning_scene_interface] (tf2_msgs::msg::TFMessage tf_m) {tf_callback(planning_scene_interface, tf_m);}; 
  // auto listener = node->create_subscription<tf2_msgs::msg::TFMessage>("/tf", 10, callback_tf_);


  // auto collision_publisher = node->create_publisher<moveit_msgs::msg::CollisionObject>("/collision_object", 10);


  // auto callback_tf_2 = [collision_publisher] (tf2_msgs::msg::TFMessage tf_m) {tf_callback_2(collision_publisher, tf_m);}; 
  // auto listener = node->create_subscription<tf2_msgs::msg::TFMessage>("/tf", 10, callback_tf_2);


  // auto callback_tf_3 = [collision_publisher, node, &tf_buffer_] (tf2_msgs::msg::TFMessage tf_m) {tf_callback_3(collision_publisher, node, tf_buffer_, tf_m);}; 
  // auto listener = node->create_subscription<tf2_msgs::msg::TFMessage>("/tf", 10, callback_tf_3);

      
  // auto collision_message = CreateBoxCollision("table2", TableDimension * 3, Eigen::Vector3d{0,0, -TableDimension.z()/2.0});
      
  // collision_publisher->publish(collision_message);


  move_group_interface.clearPathConstraints();
  move_group_interface.clearPoseTargets();
  move_group_interface.allowReplanning(true);
  move_group_interface.setPlanningTime(10);
  move_group_interface.setNumPlanningAttempts(10);
  move_group_interface.setMaxVelocityScalingFactor(1.0);
  move_group_interface.setMaxAccelerationScalingFactor(1.0);

  move_group_interface.setGoalTolerance(0.001);
  move_group_interface.setGoalOrientationTolerance(0.001);
  move_group_interface.setGoalJointTolerance(0.001);
  move_group_interface.setJointValueTarget(PickupConfig);
  move_group_interface.startStateMonitor();


  // if (!PlanAndGo(move_group_interface, LiftConfig)) {
  //         RCLCPP_ERROR(logger, "Execution failed!");
  //         rclcpp::shutdown();
  //         spinner.join();
  //         return 0;
  //       }

  {
    using namespace std::chrono_literals;
    rclcpp::sleep_for(1000ms);
  }

  auto palletPose = getPalletLocation(node, tf_buffer_);
  planning_scene_interface.applyCollisionObject(CreateBoxCollision("pallet", PalletDimensions, fromMsg(palletPose.position)));

  // collision_publisher->publish(CreateBoxCollision("box", BoxDimension, fromMsg(boxPose.position)));

  // for (auto box: box_names) 
  // {
  //     auto boxPose = getBoxLocation(node, tf_buffer_, box);
  //     planning_scene_interface.applyCollisionObject(CreateBoxCollision(box, BoxDimension, fromMsg(boxPose.position)));
  // }

  // planning_scene_interface.applyCollisionObject(CreateBoxCollision("Box3/box", BoxDimension, fromMsg(boxPose.position)));



  auto frame = tf_buffer_.allFramesAsString();
  RCLCPP_INFO(logger, "all frames %s", frame.c_str());


  // for (auto offset : Pattern) {  
  for (size_t i = 0; i < Pattern.size(); i++) {
      auto offset =  Pattern[i];
      auto box_name = box_names[i];

      {
        auto boxPose = getBoxLocation(node, tf_buffer_, box_name);
        planning_scene_interface.applyCollisionObject(CreateBoxCollision(box_name, BoxDimension, fromMsg(boxPose.position)));
      }

      if (!PlanAndGo(move_group_interface, LiftConfig)) {
        RCLCPP_ERROR(logger, "Execution failed!");
        rclcpp::shutdown();
        spinner.join();
        return 0;
      }
      
      auto box_pose_a = getBoxLocation(node, tf_buffer_, box_name).position;
      box_pose_a.z += BoxDimension[2] / 2;
      auto box_location = fromMsg(box_pose_a);

      if (!PlanAndGo(move_group_interface, box_location, DropOrientation)) {
        RCLCPP_ERROR(logger, "Execution failed!");
        rclcpp::shutdown();
        spinner.join();
        return 0;
      }


      // if (!PlanAndGo(move_group_interface, PickupConfig)) {
      //   RCLCPP_ERROR(logger, "Execution failed!");
      //   rclcpp::shutdown();
      //   spinner.join();
      //   return 0;
      // }

      SendGripperGrip(client_ptr);

      // if (!SendGripperGrip(client_ptr)) {
      //   RCLCPP_ERROR(logger, "Execution failed!");
      //   rclcpp::shutdown();
      //   spinner.join();
      //   return 0;
      // }


      
      std::cerr << "\n\nknown objects: ";
      for(auto s: planning_scene_interface.getKnownObjectNames())
      {
        std::cerr << s << ",";
      }

      std::cerr << "\n";

      std::cerr << "\n\nAttaching object.\n";

      move_group_interface.attachObject(box_name, "gripper_link");

      std::cerr << "\n\nknown objects: ";
      for(auto s: planning_scene_interface.getKnownObjectNames())
      {
        std::cerr << s << ",";
      }

      std::cerr << "\n\n\n";
    


      if (!PlanAndGo(move_group_interface, LiftConfig)) {
        RCLCPP_ERROR(logger, "Execution failed!");
        rclcpp::shutdown();
        spinner.join();
        return 0;
      }

      if (!PlanAndGo(move_group_interface, DropConfig)) {
        RCLCPP_ERROR(logger, "Execution failed!");
        rclcpp::shutdown();
        spinner.join();
        return 0;
      }

      
      
      

      auto box_pose = getBoxLocation(node, tf_buffer_);
      auto palletPose = getPalletLocation(node, tf_buffer_);

      // planning_scene_interface.applyCollisionObject(CreateBoxCollision("box", BoxDimension, fromMsg(box_pose.position)));

      auto dropPose = palletPose.position;
      dropPose.z += 0.6;
      const auto dropLocation = fromMsg(dropPose);



    


      Eigen::Vector3d modUp = (offset*BoxSize);
      modUp.z() = 0;
      if (!PlanAndGo(move_group_interface, dropLocation + modUp, DropOrientation)) {
        RCLCPP_ERROR(logger, "Execution failed!");
        rclcpp::shutdown();
        spinner.join();
        return 0;
      }
      Eigen::Vector3d modDn = (offset*BoxSize);


      RCLCPP_INFO(logger, "modDn %f %f %f, modUp %f %f %f", modDn[0], modDn[1], modDn[2], modUp[0], modUp[1], modUp[2]);
      RCLCPP_INFO(logger, "Box pose %f %f %f", box_pose.position.x, box_pose.position.y, box_pose.position.z);
      RCLCPP_INFO(logger, "Pallet pose %f %f %f", palletPose.position.x, palletPose.position.y, palletPose.position.z);
      RCLCPP_INFO(logger, "Drop location hardcoded: %f %f %f", DropLocation[0], DropLocation[1], DropLocation[2]);
      RCLCPP_INFO(logger, "Drop location calculated: %f %f %f", dropLocation[0], dropLocation[1], dropLocation[2]);


      // if (!PlanAndGo(move_group_interface, dropLocation + modDn, DropOrientation)) {
      //   RCLCPP_ERROR(logger, "Execution failed!");
      //   rclcpp::shutdown();
      //   spinner.join();
      //   return 0;
      // }


      if (!PlanAndGo(move_group_interface, dropLocation + modDn, DropOrientation)) {
        RCLCPP_ERROR(logger, "Execution failed!");
        rclcpp::shutdown();
        spinner.join();
        return 0;
      }

      SendGripperrelease(client_ptr);


      std::cerr << "\n\nknown objects: ";
      for(auto s: planning_scene_interface.getKnownObjectNames())
      {
        std::cerr << s << ",";
      }

      std::cerr << "\n";

      std::cerr << "\n\nDetaching object.\n";


      move_group_interface.detachObject(box_name);

      // move_group_interface.attachObject("box", "gripper_link");

      std::cerr << "\n\nknown objects: ";
      for(auto s: planning_scene_interface.getKnownObjectNames())
      {
        std::cerr << s << ",";
      }

      std::cerr << "\n\n\n";
  }
  



  if (!PlanAndGo(move_group_interface, LiftConfig)) {
      RCLCPP_ERROR(logger, "Execution failed!");
      rclcpp::shutdown();
      spinner.join();
      return 0;
  }




  // Shutdown ROS
  rclcpp::shutdown();  // <--- This will cause the spin function in the thread to return
  spinner.join();  // <--- Join the thread before exiting
  return 0;
}

bool Grip(rclcpp_action::Client<control_msgs::action::GripperCommand>::SharedPtr client_ptr, bool grip)
{
  auto goal = control_msgs::action::GripperCommand::Goal();
  goal.command.position = grip ? 0.0 : 1.0;
  goal.command.max_effort = 10000.0;
  auto future = client_ptr->async_send_goal(goal);

  future.wait();

  auto goal_handle = future.get();


  auto result_future = client_ptr->async_get_result(goal_handle);

  result_future.wait();

  auto result = result_future.get().result;

  return result->reached_goal;
}


bool SendGripperGrip(rclcpp_action::Client<control_msgs::action::GripperCommand>::SharedPtr client_ptr)
{
  return Grip(client_ptr, true);
}


bool SendGripperrelease(rclcpp_action::Client<control_msgs::action::GripperCommand>::SharedPtr client_ptr)
{
  return Grip(client_ptr, false);
}



