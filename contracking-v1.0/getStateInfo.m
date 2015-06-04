function [stateVec N F targetsExist X Y]=getStateInfo(stateInfo)
% unmake stateInfo
% get number of targets, number of frames
% state vector, mapping and matrices
% 
% (C) Anton Andriyenko, 2012
%
% The code may be used free of charge for non-commercial and
% educational purposes, the only requirement is that this text is
% preserved within the derivative work. For any other purpose you
% must contact the authors for permission. This code may not be
% redistributed without written permission from the authors.


stateVec=stateInfo.stateVec;
N=stateInfo.N;F=stateInfo.F;
targetsExist=stateInfo.targetsExist;

[X Y]=vectorToMatrices(stateVec,stateInfo);