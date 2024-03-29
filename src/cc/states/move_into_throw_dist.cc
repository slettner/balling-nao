// Move into throw distance class definition

//
// Created by hrs_b on 10.01.19.
//

#include "move_into_throw_dist.h"
#include "../core/controller.h"
#include "execute_throw_motion.h"

MoveIntoThrowDistanceState::MoveIntoThrowDistanceState():
    State()
{

}

void MoveIntoThrowDistanceState::go_next(Controller &controller) {


    // 1. Use Vision Module to get location of the hoop
    ros::spinOnce();
    auto hoop_position = controller.brain().vision_module().get_marker_mat_t_hoop();
    auto marker_x = hoop_position.at<float>(0);
    auto marker_z = hoop_position.at<float>(2);

    // 2. Use Motion Module to walk towards hoop (don't lose sight of hoop)
    float walking_x = marker_z - 0.65f; // Stop 55 cm in front of the marker
    auto walking_angle = (float)tan((double)marker_x/(double)marker_z);

    ROS_INFO_STREAM("Walking: " + std::to_string(walking_x));
    ROS_INFO_STREAM("Turning: " + std::to_string(walking_angle));

    controller.motion_module().request_move_to_position(0.0, 0.0, -walking_angle);
    controller.motion_module().request_move_to_position(walking_x, 0.0, 0.0);


    // until dunking distance is reached
    // 3. if lost_hoop -> nextState: DetectHoop
    // if reached_dist -> nextState: ExecuteDunkMotion

    // for now
    controller.set_state(new ExecuteThrowMotionState());


}