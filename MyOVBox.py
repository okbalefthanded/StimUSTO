# TODO : - Online copy mode
#       - Online free mode
#       - Feedback
#
#
#
#

import socket
import time
import random

OVTK_StimulationId_ExperimentStart = 0x00008001
OVTK_StimulationId_ExperimentStop = 0x00008002
OVTK_StimulationId_TrialStart = 0x00008005
OVTK_StimulationId_TrialStop = 0x00008006
OVTK_StimulationId_Target = 0x00008205
OVTK_StimulationId_NonTarget = 0x00008206
OVTK_StimulationLabel_Base = 0x00008100
Letters = [ 'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O',
            'P','Q','R','S','T','V', 'W', 'X', 'Y', 'Z','0','1','2','3',
            '4','5','6','7','8','9','0'
            ]

class MyOVBox(OVBox):
    def __init__(self):
        OVBox.__init__(self)
        self.isi = 0.100
        self.cl_output = 0
        self.cl_output_list = []
        self.feedback_data = 0
        self.hostname = '127.0.0.1'
        self.feedback_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.feedback_port = 12345
        self.do_feedback = False

    def initialize(self):
        # nop
        # self.feedback_socket.bind((self.hostname, self.feedback_port))
        return

    def process(self):
        for chunkIndex in range(len(self.input[0])):
            chunk = self.input[0].pop()

            if type(chunk) == OVStimulationSet:
                for stimIdx in range(len(chunk)):
                    stim = chunk.pop()
                    # print 'Received stim', stim.identifier, 'stamped at', stim.date, 's'

                    if stim.identifier == OVTK_StimulationId_TrialStop:                        
                        # TODO : feedback
                        self.feedback_data = self.feedback_data + 1
                        data = Letters[random.randint(0,35)]
                        self.feedback_socket.sendto(data, (self.hostname, self.feedback_port) )                         
                        print 'sending feedback', data
                        
                    elif stim.identifier == OVTK_StimulationId_ExperimentStop:
                        # TODO : stop
                        print 'ExperimentStop'


    def uninitialize(self):
        # nop
        return

box = MyOVBox()
