% function cemTracker(detfile, opt)
% Multi-Target Tracking by Continuous Energy Minimization
%
%
%
% STATE  VECTOR
% There are two ways in which the state is represented.
% (1) compact
% One is a d-x-1 vector, containing x,y coordinates in
% the order
%
% Target 1        Target 2        Target N
% x1,y1,x2,y2,...,x1,y1,x2,y2,...,xN,yN
%
% (2) visual
% The other representation consists of two matrices
% for x and y locations respectively
% row i column j corresponds to target j at frame i
%
%
%
% This code contains modifications compared
% to the one that was used
% to produce results for our CVPR 2011 and VS 2011
% papers
%
% 
% (C) Anton Andriyenko, 2012
%
% The code may be used free of charge for non-commercial and
% educational purposes, the only requirement is that this text is
% preserved within the derivative work. For any other purpose you
% must contact the authors for permission. This code may not be
% redistributed without written permission from the authors.



clear all; clear global
% set start time

global cemStartTime
cemStartTime=tic;

addpath(genpath('.'))
% addpath('../dctracking')
% addpath('../dctracking/splinefit')
%% declare global variables
global detMatrices detections sceneInfo opt globiter gtInfo
globiter=0;

global LOG_allens LOG_allmets2d LOG_allmets3d %for debug output

%% setup options and scene
% fill options struct
opt=getOptions;

% fill scene info
scenario=25;
sceneInfo=getSceneInfo(scenario);

%% cut ground truth
if  sceneInfo.gtAvailable && opt.track3d
    gtInfo=cutGTToTrackingArea(gtInfo);
end
%% load detections
[detections nDets]=parseDetections(sceneInfo); fr=1:length(detections);
% fr=1:50; detections=detections(fr);  % !!!!!!! REMOVE
% for t=1:length(detections), detections(t).sc=1; end

% load('/home/aanton/diss/others/yangbo/TUD/TUD_Stadtmitte.avi.detection.mat')
% load('/home/aanton/diss/others/yangbo/PETS09/PETS09_View001_S2_L1_000to794.avi.detection.mat');
F=size(detections,2);
stateInfo.F=F;                          % number of frames

% cut detections to tracking area if needed
[detections nDets]=cutDetections(detections,nDets);
detMatrices=getDetectionMatrices(detections);

% evaluate detections
% evaluateDetections(detMatrices,gtInfo);

%% init solution
% ...
% initsolfile=fullfile(getHomeFolder,'diss','ekftracking','output','s0080','e00012d.mat');
% initsolfile=fullfile(getHomeFolder,'diss','ekftracking','output','s0080','e0003.mat');
% initsolfile=fullfile(getHomeFolder,'diss','ekftracking','output','s0023','e0003.mat');
% initsolfile=fullfile(getHomeFolder,'diss','ekftracking','output','s0042','e0003.mat');

X=[]; Y=[];

initsolfile=fullfile(getHomeFolder,'diss','ekftracking','output',sprintf('s%04d',scenario),'e0001.mat');
% initsolfile=fullfile(getHomeFolder,'diss','others','okuma','BPF','myresult.mat');
% initsolfile=fullfile(getHomeFolder,'diss','others','okuma','BPF',sprintf('myresult-s%04d.mat',scenario));
% initsolfile='tmp.mat';

% sampling
% alldpoints=createAllDetPoints;
% nInit=round(nDets/F);
% mhs=getSplineProposals(alldpoints,nInit,F);
% for id=1:length(mhs)
%     tt=mhs(id).start:mhs(id).end;
%     allvals=ppval(mhs(id),tt);
%     X(tt',id)=allvals(1,:)';Y(tt',id)=allvals(2,:)';
% end
% [X, Y, stateInfo]=cleanState(X, Y, stateInfo);
% [X Y]=checkInitSolution(X,Y,stateInfo.F);



if 0%exist(initsolfile,'file')
load(initsolfile);

% X=X(fr,:);Y=Y(fr,:);H=H(fr,:); % !!!!!! REMOVE
[X, Y, stateInfo]=cleanState(X, Y, stateInfo);
[X Y]=checkInitSolution(X,Y,stateInfo.F);
end

% global gtInfo
% X=gtInfo.X; Y=gtInfo.Y;

stateInfo.N=size(X,2);
stateInfo.targetsExist=getTracksLifeSpans(X);
stateInfo.frameNums=sceneInfo.frameNums;
stateInfo=matricesToVector(X,Y,stateInfo);
[stateVec N F targetsExist stateInfo.X stateInfo.Y]=getStateInfo(stateInfo);

assert(isempty(X) || size(X,1)==length(detections),'Initial Solution must be full length');

printSceneInfo;

%% initial gradient descent (if initial solution available)
if ~isempty(stateInfo.stateVec)
    [stateInfo.stateVec Evalue nIterations]=minimize(stateInfo.stateVec,'E',opt.maxIterCGD,stateInfo);
end

%% now do main optimization
converged=false;
epoch=0;
while ~converged && epoch<opt.maxEpochs
    epoch=epoch+1;
    printMessage(1,'---- JUMP  MOVES  ROUND %i ----\n',epoch);
    jumpExecuted=false(1,6);
    
    for jumpMove=opt.jumpsOrder
        stateInfoOld=stateInfo;
        eval(sprintf('stateInfo=%s(stateInfo);',getJumpMoveFunction(jumpMove)))
         
        % did we jump?
        if isequal(stateInfoOld,stateInfo) % no
            printMoveFailure(jumpMove)
        
        %  perform conjugate gradient descent if move was successful
        else  
            jumpExecuted(jumpMove)=1;
            [stateInfo.stateVec Evalue nIterations]=minimize(stateInfo.stateVec,'E',opt.maxIterCGD,stateInfo);
        end
    end
    
    % if no jumps were perfomed, we're done
    if all(~jumpExecuted)
        converged=true;
        printMessage(1,'No jumps were executed. Optimization has converged after %i epochs.\n',epoch);
    end
    
    % if last epoch
    if epoch>=opt.maxEpochs
        printMessage(1,'Max number of rounds reached.\n');
    end
end % converged
% basically we are done
printMessage(1,'All done (%.2f min = %.2fh = %.2f sec per frame)\n',toc(cemStartTime)/60,toc(cemStartTime)/3600,toc(cemStartTime)/F);




%% post processing
% get X Y matrices
[stateVec N F targetsExist stateInfo.X stateInfo.Y]=getStateInfo(stateInfo);
stateInfo=postProcessState(stateInfo);



%% if we have ground truth, evaluate results
printFinalEvaluation(stateInfo)

%% clean up (remove zero rows from logs)
itinfo=find(sum(LOG_allens,2));
LOG_allmets2d=LOG_allmets2d(~~sum(LOG_allmets2d,2),:);
LOG_allmets3d=LOG_allmets3d(~~sum(LOG_allmets3d,2),:);
LOG_allens=LOG_allens(~~sum(LOG_allens,2),:);


% you can display the results with
% displayTrackingResult(sceneInfo,stateInfo)

% end