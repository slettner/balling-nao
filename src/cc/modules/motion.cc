//
// Created by hrs_b on 10.01.19.
//

#include "motion.h"

#include "sensor_msgs/JointState.h"
#include <std_srvs/Empty.h>
#include <std_msgs/Bool.h>
#include <geometry_msgs/PoseStamped.h>
#include <geometry_msgs/Pose2D.h>
#include "../errors/not_implemented_error.h"
#include "balling_nao/GetPosition.h"
#include "balling_nao/Movement.h"
#include "balling_nao/GetTransform.h"
#include "balling_nao/MoveJoints.h"
#include "balling_nao/HandControl.h"

float PI_180 = (float)3.141592/180;


Motion::Motion(ros::NodeHandle &node_handle)
{
    _client_get_position = node_handle.serviceClient<balling_nao::GetPosition>("get_position_server");
    _client_set_position = node_handle.serviceClient<balling_nao::Movement>("set_position_server");
    _client_get_transform = node_handle.serviceClient<balling_nao::GetTransform>("get_transformation");
    _client_set_joints = node_handle.serviceClient<balling_nao::MoveJoints>("set_joints_server");
    _client_set_hands = node_handle.serviceClient<balling_nao::HandControl>("set_hands_server");
    _body_stiffness_enable = node_handle.serviceClient<std_srvs::Empty>("/body_stiffness/enable");
    _body_stiffness_disable = node_handle.serviceClient<std_srvs::Empty>("/body_stiffness/disable");
    _walk_pub=node_handle.advertise<geometry_msgs::Pose2D>("/cmd_pose", 1);
    _footContact_sub = node_handle.subscribe<std_msgs::Bool>("/foot_contact", 1, &Motion::footContactCallback, this);
}


void Motion::request_ball_position() {
    ROS_INFO("REQUESTING BALL");
    //http://doc.aldebaran.com/1-14/family/robots/joints_robot.html#robot-joints-v4-right-arm-joints
    std::vector<std::string> names = {"RShoulderPitch", "RShoulderRoll", "RElbowYaw", "RWristYaw", "RElbowRoll"};
    float RShoulderRoll = (float) -5.0 * PI_180;
    float RShoulderPitch = (float) 60.0 * PI_180;
    float RElbowYaw = (float) 110.0 * PI_180;
    float RWristYaw = (float) 100.0 * PI_180;
    float RElbowRoll =(float) 85.0 * PI_180;

    std::vector<float> angles{RShoulderPitch, RShoulderRoll, RElbowYaw, RWristYaw, RElbowRoll};
    float fractionMaxSpeed = 0.2;

    balling_nao::MoveJointsResponse response;

    request_joint_movement(names, angles, fractionMaxSpeed, response);

    finger_movement(0);

    while(!check_movement_success(angles, response.new_angles)) {
        request_joint_movement(names, angles, fractionMaxSpeed, response);
    }

}


void Motion::finger_movement(int type) { //0 to open, 1 to close
    while(! request_hand_action("RHand", type));
}

std::vector<float> Motion::request_cartesian_movement(std::string &name, std::vector<float> &position, float time) {

    // Request movement using Interpolation and return the new position.
    balling_nao::Movement srv; //initialise a srv file
    srv.request.name = name;
    srv.request.coordinates = position;
    srv.request.time = time;
    srv.request.type = 1;
    if (_client_set_position.call(srv)) {
        std::vector<float> coordinates_6D(srv.response.coordinates.begin(), srv.response.coordinates.end()); //obtain the current position of the robot
        return coordinates_6D;
    }
    else {
        throw std::runtime_error("Could not send movement request");
    }
}

void Motion::request_joint_movement(
        std::vector<std::string>& names,
        std::vector<float> &angles,
        float fractionMaxSpeed,
        balling_nao::MoveJointsResponse& response
        ) {

    balling_nao::MoveJoints srv;
    srv.request.names = names;
    srv.request.angles = angles;
    srv.request.fractionMaxSpeed = fractionMaxSpeed;

    if (_client_set_joints.call(srv)) {
        response = srv.response;
    }

}

bool Motion::request_hand_action(std::string handName, int state){ //state = 0 to open the hand, 1 to close
    balling_nao::HandControl srv;
    srv.request.action_type = state;
    srv.request.hand_name = handName;
    if (_client_set_hands.call(srv)){
        bool success = srv.response.success;
        return success;
    }
}

void Motion::footContactCallback(const std_msgs::BoolConstPtr& contact)
{
    bool has_contact = contact->data;
    std::cout << "Has contact: " << has_contact << std::endl;
    if (1 != has_contact) {
        stop_walking();
    }
}

void Motion::walk_to_position(double x, double y, double theta)
{
    std_srvs::Empty empty;
    _body_stiffness_enable.call(empty);
    ROS_INFO("Requesting to walk");
    geometry_msgs::Pose2D msg;
    msg.x = x;
    msg.y = y;
    msg.theta = theta;
    _walk_pub.publish(msg);
}


void Motion::stop_walking()
{
    std_srvs::Empty msg;
    if (_stop_walk_srv.call(msg)) {
        ROS_INFO("Requesting to StopWalk");
        _body_stiffness_disable.call(msg);
    }

}

bool Motion::check_movement_success(std::vector<float> &angles_request, std::vector<float> &angle_response) {



    for(size_t i = 0; i < angles_request.size(); i++) {
        float dif = (angles_request[i] - angle_response[i]) / PI_180;
        if(dif > 15.0 or dif < -15.0) return false;
    }
    return true;
}

