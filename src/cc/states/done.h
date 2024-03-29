// Done state header file
// This is the final state of the state machine
// structure : single class

//
// Created by hrs_b on 10.01.19.
//

#ifndef BALLING_NAO_DONE_H
#define BALLING_NAO_DONE_H


#include "state.h"

class Controller;


class DoneState: public State {

public:
    // Constructor
    DoneState();

    // Implements the go next function.
    // Raises an exception if called since this is the last state.
    //
    // Args:
    //     controller: The controller holding the current state and the brain.
    void go_next(Controller& controller) override;

    const std::string get_state_name() override { return _state_name; }

private:

    // This is used in the controller to detect when we reached a Done State.
    const std::string _state_name = "Done";

};


#endif //BALLING_NAO_DONE_H
