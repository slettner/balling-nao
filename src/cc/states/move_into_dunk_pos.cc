// Move into dunk position class defintion

//
// Created by hrs_b on 10.01.19.
//

#include "move_into_dunk_pos.h"
#include "../core/controller.h"
#include "execute_dunk_motion.h"
#include <unistd.h>
#include <math.h>

MoveIntoDunkPositionState::MoveIntoDunkPositionState():
    State()
{

}

void MoveIntoDunkPositionState::go_next(Controller &controller) {

    // 1. Use Vision Module to get location of the hoop
    ros::spinOnce();
    auto hoop_position = controller.brain().vision_module().get_marker_mat_t_hoop();
    auto marker_x = hoop_position.at<float>(0);
    auto marker_z = hoop_position.at<float>(2);

    // 2. Use Motion Module to walk towards hoop
    float walking_x = marker_z - 0.35f; // Stop 35 cm in front of the marker
    auto walking_angle = (float)tan((double)marker_x/(double)marker_z);

    ROS_INFO_STREAM("Move into dunk position. Walking: " + std::to_string(walking_x));
    ROS_INFO_STREAM("Move into dunk position. Turning: " + std::to_string(walking_angle));

    controller.motion_module().request_move_to_position(0.0, 0.0, -walking_angle);  // Adjust angle
    controller.motion_module().request_move_to_position(walking_x/2, 0.0, 0.0); // Only walk HALF the dist

    // Make another search
    SEARCH_MODE mode = SEARCH_MODE::MARKER_CLOSE;
    float found_at_head_angle;
    std::function<bool ()> f = [&]() { return controller.vision_module().hoop_visible(); };
    bool hoop_visible = controller.brain().search().search_routine(
            mode,
            f,
            found_at_head_angle
    );
    // If the hoop eas hound get its position and walk towards it again (up to 30 cm)
    if (hoop_visible) {
        controller.motion_module().request_move_to_position(0.0, 0.0, found_at_head_angle);
        ros::spinOnce();
        hoop_position = controller.brain().vision_module().get_marker_mat_t_hoop();
        marker_x = hoop_position.at<float>(0);
        marker_z = hoop_position.at<float>(2);
        walking_angle = (float)tan((double)marker_x/(double)marker_z);
        walking_x = marker_z - 0.30f; // Stop 30 cm in front of the marker
        controller.motion_module().request_move_to_position(0.0, 0.0, -walking_angle);
        controller.motion_module().request_move_to_position(walking_x, 0.0, 0.0); // Only walk half the dist
    }
    // If the hoop has not been found on the intermediate stop we walk the distance calculated
    // from the last time we saw the marker and hope we land somewhere close the hoop
    else {
        controller.motion_module().request_move_to_position(walking_x/2, 0.0, 0.0); // Walk the remaining dist
    }

    // Now we should be very close to the hoop
    // We make another search to adjust or position one last time
    mode = SEARCH_MODE::MARKER_CLOSE;
    f = [&]() { return controller.vision_module().hoop_visible(); };
    hoop_visible = controller.brain().search().search_routine(
            mode,
            f,
            found_at_head_angle
    );
    // If we found the hoop, get the position and adjust
    if(hoop_visible) {
        ROS_INFO_STREAM("Found Hoop");
        controller.brain().motion_module().request_move_to_position(0.0, 0.0, found_at_head_angle);
        hoop_position = controller.brain().vision_module().get_marker_mat_t_hoop();
        marker_x = hoop_position.at<float>(0);
        marker_z = hoop_position.at<float>(2);
        walking_angle = (float)tan((double)marker_x/(double)marker_z);
        walking_x = marker_z - 0.27f; // Stop 55 cm in front of the marker
        controller.motion_module().request_move_to_position(0.0, 0.0, -walking_angle);
        controller.motion_module().request_move_to_position(walking_x, 0.0, 0.0); // Only walk half the dist
    }

    // Move to the next state
    controller.set_state(new ExecuteDunkMotionState);

}