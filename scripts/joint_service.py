#!/usr/bin/env python
import rospy
import time
import almath
import sys
from naoqi import ALProxy
from balling_nao.srv import MoveJoints


NAO_LIMITS = {
    "HeadYaw": [-2, 2],
    "HeadPitch": [-0.45, 0.45],
    "LShoulderRoll":[0.3, 1.3],
    "LShoulderPitch": [-2, 2]
}



class NaoController(object):

    def __init__(self, ip, port):

        self.nao_proxy = ALProxy("ALMotion", ip, port)
        rospy.init_node('move_joints_server')
        rospy.Service("set_joints_server", MoveJoints, self.process_request)

    def process_request(self, req):

        self.set_stiffness(req, 1.0)

        # kill task call
        task_list = self.nao_proxy.getTaskList()
        for task in task_list:
            self.nao_proxy.killTask(task[1])

        req.angles = list(req.angles)

        if(req.movement_type == 0):
            self.check_limits(req)
            self.normal_movement(req)

        elif(req.movement_type == 1):

            self.check_limits(req)
            self.interpolated_movement(req)

        else:
            raise InvalidArgumentError("Received invalid argument for req.movement_type: {}".format(req.movement_type))

        time.sleep(req.sleep_time)
        for name, angle in zip(req.names, req.angles):
            print "Performed angle change of {} to {}".format(name, angle)
        self.set_stiffness(req, 0.0)

        return True

    def set_stiffness(self, req, val):
        for name in req.names:
            for i in xrange(20):
                self.nao_proxy.setStiffnesses(name, val)

    def normal_movement(self, req):

        self.nao_proxy.setAngles(req.names, req.angles, 0.05)

    def interpolated_movement(self, req):

        self.nao_proxy.post.angleInterpolation(req.names, req.angles, req.times, True)

    def check_limits(self, req):

        is_valid = True

        for i, name in enumerate(req.names):
            limits = NAO_LIMITS[name]

            if req.angles[i] < limits[0]:
                req.angles[i] = limits[0]

            if req.angles[i] > limits[1]:
                req.angles[i] = limits[1]



if __name__ == '__main__':

    robotIP=str(sys.argv[1])
    PORT=int(sys.argv[2])
    print sys.argv[2]
    controller = NaoController(robotIP, PORT)
    rospy.spin()
			
		