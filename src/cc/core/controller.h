// Header file for the controller class
// The controller handles the state navigation
// structure : single class

//
// Created by hrs_b on 10.01.19.
//

#ifndef BALLING_NAO_CONTROLLER_H
#define BALLING_NAO_CONTROLLER_H

#include "brain.h"
#include "../states/state.h"
#include <memory>


class Controller {

    // The controller is the logical center of the program
    // It holds the brain and the current state
    // Its run function executes state after state,
    // where the states set the current state of the controller.
    // This way the controller moves through the state machine.

public:

    // Initializes the controller.
    // Sets the current state to the RequestBallState
    // and calls its go_next function.
    explicit Controller(ros::NodeHandle& node_handle);

    // Default Destruction
    ~Controller() = default;


    // Copy of the controllers are not allowed
    Controller(Controller&& controller) = delete;

    // Implements state transition.
    // Calls the go next function of the current state.
    //
    void go_next();

    // Prints out logging messages
    void LOG(std::string msg);

    // The main function of the controller
    // Starts the state machine.
    void run();

    // Getter
    Speech& speech_module() { return _brain.speech_module(); }
    Vision& vision_module() { return _brain.vision_module(); }
    Motion& motion_module() { return _brain.motion_module(); }
    Tactile& tactile_module() { return _brain.tactile_module(); }
    Brain& brain() { return _brain; }

    // Set the current state.
    // This function will take ownership of the state passed to it
    // and destroy it after setting it in the unique ptr as a member variable
    //
    // Args:
    //     state: The new state.
    //
    void set_state(State* state);

private:
    
    Brain _brain;

    std::unique_ptr<State> _current_state;

};

#endif //BALLING_NAO_CONTROLLER_H
