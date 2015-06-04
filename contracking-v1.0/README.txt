###################################################################
#                                                                 #
#     Multi-Target Tracking by Continuous Energy Minimization     #
#              Anton Andriyenko and Konrad Schindler              #
#                                                                 #
#                 Copyright 2012 Anton Andriyenko                 #
#                                                                 #
###################################################################



ABOUT:
This software implements our approach to multi-target tracking
using continuous energy minimization [1].

The additional packages
 - minimize.m
 - splinefit
are released under a different license and are included for your convenience.



==========================================================================
DISCLAIMER:
This demo software has been rewritten for the sake of simplifying the
implementation. Therefore, the results produced by the code may differ
from those presented in the paper [1].
==========================================================================


IMPORTANT:
To use this software, you should cite the following in any resulting publication:
    [1] Multi-target Tracking by Continuous Energy Minimization
        A. Andriyenko and K. Schindler. 
        In CVPR, Colorado Springs, USA, June 2011

Furthermore, if you use occlusion reasoning, you should cite
    [2] An Analytical Formulation of Global Occlusion Reasoning for Multi-Target Tracking
        A. Andriyenko, S. Roth, and K. Schindler. 
        In IEEE International Workshop on Visual Surveillance (in conjunction with ICCV), Barcelona, Spain, November 2011 


INSTALLING & RUNNING
1.	Unpack contracker-v1.0.zip

2.	Start MATLAB and run compileMex.m to build the utilities binaries.
    (This step can be omitted if you are using MacOSX 32/64 bit, Windows 32/64 bit or Linux 64 bit.)

3.	run cemTrackerDemo.m



CHANGES
	May 25, 2012	Initial public release