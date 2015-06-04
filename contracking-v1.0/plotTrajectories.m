function plotTrajectories(stateVec,stateInfo,plot3d)
% plot trajectories
% 
% (C) Anton Andriyenko, 2012
%
% The code may be used free of charge for non-commercial and
% educational purposes, the only requirement is that this text is
% preserved within the derivative work. For any other purpose you
% must contact the authors for permission. This code may not be
% redistributed without written permission from the authors.


hold on

% plot3d=1;
if ~exist('plot3d','var')
    plot3d=0;
end

lw=2;

N=stateInfo.N;
targetsExist=stateInfo.targetsExist;
[X Y]=vectorToMatrices(stateVec,stateInfo);

if plot3d
     for id=1:N
        tarFrames=(targetsExist(id,1):targetsExist(id,2))';
%         tarFrames=tarFrames(tarFrames<=50);
        plot3(X(tarFrames,id),Y(tarFrames,id),tarFrames, ...
            'color',getColorFromID(id),'linewidth',lw);
    end   
    
else
    for id=1:N
        tarFrames=(targetsExist(id,1):targetsExist(id,2))';
        plot(X(tarFrames,id),Y(tarFrames,id), ...
            'color',getColorFromID(id),'linewidth',lw);
    end
end
drawnow
end