//
// Created by hrs_b on 10.01.19.
//

#include "request_task.h"
#include "../modules/controller.h"
#include "search_defender.h"


RequestTaskState::RequestTaskState():
    State()
{
    _available_cmds.emplace_back("throw");
    _available_cmds.emplace_back("dunk");
}


void RequestTaskState::go_next(Controller &controller) {

    // 1. Use speech module to say "What should I do?"
    // 2. Receive the answer (Dunk or Throw)

    ros::Rate loop_rate(10); //request_response_block doesnt seem to work too well
    while(true) {
        ros::spinOnce();
        loop_rate.sleep();
    }
    std::string request("what should i do");
    std::string response;
    controller.speech_module().request_response_block(this, request, response);



    // 3. Move to SearchDefenderState
    //controller.set_state(new SearchDefenderState(response));

}