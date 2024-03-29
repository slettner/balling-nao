// Brain header file
// The brain is a super module that can access all sensor and actors to make cognition tasks that require "thinking"
// structure : one class

//
// Created by hrs_b on 31.01.19.
//

#ifndef BALLING_NAO_BRAIN_H
#define BALLING_NAO_BRAIN_H

#include "../sensors_actors/motion.h"
#include "../sensors_actors/speech.h"
#include "../sensors_actors/vision.h"
#include "../sensors_actors/tactile.h"
#include "cognition/search.h"
#include "cognition/path_planning.h"


class Brain {
    
    // The brain is a super module that can access all sensor and actors to make cognition tasks that require "thinking"

public:

    // Constructor
    //
    // Args:
    //     node_handle: The ros node handle
    //
    explicit Brain(ros::NodeHandle& node_handle);

    // Getter
    Speech& speech_module() { return _speech; }
    Vision& vision_module() { return _vision; }
    Motion& motion_module() { return _motion; }
    Tactile& tactile_module() { return _tactile; }
    Search& search() { return _search; }
    PathPlanning path_planning() { return _path_planning; }


private:
    
    Vision _vision;
    Speech _speech;
    Motion _motion;
    Tactile _tactile;

    Search _search;
    PathPlanning _path_planning;
    
};


#endif //BALLING_NAO_BRAIN_H
