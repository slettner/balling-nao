//
// Created by hrs_b on 10.01.19.
//

#include "controller.h"
#include "../states/request_ball.h"
#include <iostream>

// Todo: remove all states but initial state: request_ball.h
//to debug only
#include "../states/take_ball.h"
#include "../states/detect_hoop.h"
#include "../states/execute_throw_motion.h"
#include "../states/execute_dunk_motion.h"
#include "../states/search_defender.h"
#include "../states/avoid_defender.h"


// Todo: remove command, this is to start in SearchDefenderState(command)
std::string command = "dunk ";

Controller::Controller(ros::NodeHandle& node_handle):
    _current_state(new SearchDefenderState(command)),
    _brain(node_handle)
{
    // Nothing to do here
}


void Controller::go_next() {

    // Print out the state name as a log message
    LOG("Entered " + _current_state->get_state_name());

    // Create a message for the speech module to speak out the new states name
    std::string msg("Nao entered ");
    msg += _current_state->get_state_name();

    speech_module().talk(msg);

    _current_state->go_next(*this);

}


void Controller::LOG(std::string msg) {

    std::cout << "[CONTROLLER] " << msg << std::endl;

}


void Controller::run() {

    LOG("Starting Run Function");

    // Increases stability
    ros::Rate loop_rate(10);
    int warmup = 10;
    while (warmup > 0) {
        ros::spinOnce();
        loop_rate.sleep();
        warmup--;
    }

    motion_module().go_to_posture("StandInit",0.5);

    // As long as we don't enter the 'Done' state we move on
    while(_current_state->get_state_name() != "Done") {
        go_next();
        std::cout << _current_state->get_state_name() << std::endl;
        ros::spinOnce();
        loop_rate.sleep();
    }
    motion_module().go_to_posture("Crouch", 0.2);
    motion_module().disable_stiffness(3);
    LOG("Reached Done State");

}


void Controller::set_state(State* state) {

    // Set to the new state will be copied.
    _current_state.reset(state);

}