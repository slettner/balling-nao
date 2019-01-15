//
// Created by hrs_b on 14.01.19.
//

#include "tactile.h"


Tactile::Tactile(ros::NodeHandle &node_handle)
{
    _tactile_head = node_handle.subscribe("/tactile_though", 1, &Tactile::update_button_tracker, this);
}


void Tactile::update_button_tracker(const naoqi_bridge_msgs::HeadTouch::ConstPtr& tactile_state) {

    if (tactile_state->button == tactile_state->buttonFront && tactile_state->stateReleased) {
        _button_tracker.buttons["front"] = ButtonStates::WAS_PRESSED;
    }
    if (tactile_state->button == tactile_state->buttonMiddle && tactile_state->stateReleased) {
        _button_tracker.buttons["middle"] = ButtonStates::WAS_PRESSED;
    }
    if (tactile_state->button == tactile_state->buttonRear && tactile_state->stateReleased) {
        _button_tracker.buttons["rear"] = ButtonStates::WAS_PRESSED;
    }

}

void Tactile::detect_button_pressed(std::string button_name) {

    _button_tracker.buttons[button_name] = ButtonStates::RELEASED;
    while(_button_tracker.buttons[button_name] != ButtonStates::WAS_PRESSED) {
        // Wait for the button to be pressed
    }
    _button_tracker.buttons[button_name] = ButtonStates::RELEASED;
}