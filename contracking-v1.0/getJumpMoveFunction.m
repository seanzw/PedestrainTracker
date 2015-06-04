function moveFct=getJumpMoveFunction(m)
% returns the name of the function
% corresponding to jump move m
% 
% (C) Anton Andriyenko, 2012
%
% The code may be used free of charge for non-commercial and
% educational purposes, the only requirement is that this text is
% preserved within the derivative work. For any other purpose you
% must contact the authors for permission. This code may not be
% redistributed without written permission from the authors.


switch(m)
    case 1
        moveFct='makeMergeMove';
    case 2
        moveFct='makeSplitMove';
    case 3
        moveFct='makeGrowMove';
    case 4
        moveFct='makeShrinkMove';
    case 5
        moveFct='makeRemoveMove';
    case 6
        moveFct='makeAddMove';
end
end