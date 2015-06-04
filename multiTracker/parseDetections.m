function [detections nDets]=parseDetections(sceneInfo,frames)
% read detection file and create a struct array
% 
% (C) Anton Andriyenko, 2012
%
% The code may be used free of charge for non-commercial and
% educational purposes, the only requirement is that this text is
% preserved within the derivative work. For any other purpose you
% must contact the authors for permission. This code may not be
% redistributed without written permission from the authors.

global opt
nDets=0;

detfile=sceneInfo.detfile;
% first determine the type
[pathstr, filename, fileext]=fileparts(detfile);
% is there a .mat file available?
matfile=fullfile(pathstr,[filename '.mat']);
if exist(matfile,'file')
    load(matfile,'detections')
    detections=setDetectionPositions(detections,opt);
    
    % check if all info is available
    if (~isfield(detections,'xp') || ...
            ~isfield(detections,'yp') || ...
            ~isfield(detections,'sc') || ...
            ~isfield(detections,'wd') || ...
            ~isfield(detections,'ht'))
        error('detections must have fields xp,yp,sc,wd,ht');
    end
    
    if nargin==2
        detections=detections(frames);
    end
    
    % count detections
    if ~nDets
        for t=1:length(detections),nDets=nDets+length(detections(t).xp);end
    end
    
    return;
end

if      strcmpi(fileext,'.idl'); detFileType=1;
elseif  strcmpi(fileext,'.xml'); detFileType=2;
else    error('Unknown type of detections file.');
end

%% now parse

if detFileType==1
    % idl type
    % format:
    % "filename": (left top right bottom):score ...
    
    idlBoxes=readIDL(detfile);
    F=size(idlBoxes,2);
    frToParse=1:F;
    if nargin==2, frToParse=frames; end
    
    for t=frToParse
        
        nboxes=size(idlBoxes(t).bb,1); % how many detections in current frame
        xis=zeros(1,nboxes);
        yis=zeros(1,nboxes);
        heights=zeros(1,nboxes);
        widths=zeros(1,nboxes);
        boxleft=zeros(1,nboxes);
        boxtop=zeros(1,nboxes);
        scores=zeros(1,nboxes);
        for boxid=1:nboxes
            
            bbox=idlBoxes(t).bb(boxid,:);
            bbox([1 3])=bbox([1 3]);  bbox([2 4])=bbox([2 4]);
            
            % box extent
            heights(boxid)=bbox(4)-bbox(2);
            widths(boxid)=bbox(3)-bbox(1);
            
            % box left top corner
            boxleft(boxid)=bbox(1);
            boxtop(boxid)=bbox(2);
            
            % foot position
            xis(boxid) = bbox(1)+widths(boxid)/2;   % horizontal (center)
            yis(boxid) = bbox(4);                       % vertical   (bottom)
            
            
            % score
            scores(boxid)=idlBoxes(t).score(boxid);
            
            
        end
        
        detections(t).bx=boxleft;
        detections(t).by=boxtop;
        detections(t).xp=xis;
        detections(t).yp=yis;
        detections(t).ht=heights;
        detections(t).wd=widths;
        detections(t).sc=scores;
        
        detections(t).xi=xis;
        detections(t).yi=yis;
        
        nDets=nDets+length(xis);
        
    end
    
    % sigmoidify
    sigA=0;    sigB=1;
    for t=1:length(detections)
        detections(t).sc=1./(1+exp(sigA-sigB*detections(t).sc));
    end
    
elseif detFileType==2
    xDoc=xmlread(detfile);
    allFrames=xDoc.getElementsByTagName('frame');
    F=allFrames.getLength;
    frameNums=zeros(1,F);
    
    
    %%
    frToParse=1:F;
    if nargin==2, frToParse=frames; end
    
    for t=frToParse
        if ~mod(t,10), fprintf('.'); end
        % what is the frame
        frame=str2double(allFrames.item(t-1).getAttribute('number'));
        frameNums(t)=frame;
        
        objects=allFrames.item(t-1).getElementsByTagName('object');
        Nt=objects.getLength;
        nboxes=Nt; % how many detections in current frame
        xis=zeros(1,nboxes);
        yis=zeros(1,nboxes);
        heights=zeros(1,nboxes);
        widths=zeros(1,nboxes);
        boxleft=zeros(1,nboxes);
        boxtop=zeros(1,nboxes);
        scores=zeros(1,nboxes);
        
        
        for i=0:Nt-1
            % score
            boxid=i+1;
            scores(boxid)=str2double(objects.item(i).getAttribute('confidence'));
            box=objects.item(i).getElementsByTagName('box');
            
            % box extent
            heights(boxid) = str2double(box.item(0).getAttribute('h'));
            widths(boxid) = str2double(box.item(0).getAttribute('w'));
            
            % foot position
            xis(boxid) = str2double(box.item(0).getAttribute('xc'));
            yis(boxid) = str2double(box.item(0).getAttribute('yc'))+heights(boxid)/2;
            
        end
        
        
        % box left top corner
        boxleft=xis-widths/2;
        boxtop=yis-heights;
        
        detections(t).bx=boxleft;
        detections(t).by=boxtop;
        detections(t).xp=xis;
        detections(t).yp=yis;
        detections(t).ht=heights;
        detections(t).wd=widths;
        detections(t).sc=scores;
        
        detections(t).xi=xis;
        detections(t).yi=yis;
        
        nDets=nDets+length(xis);
    end
    
end



%% if we want to track in 3d, project onto ground plane
detections=projectToGP(detections);

%% set xp and yp accordingly
detections=setDetectionPositions(detections,opt);

% save detections in a .mat file
save(matfile,'detections');

end


function detections=projectToGP(detections)
    global opt sceneInfo
    F=length(detections);
    if opt.track3d
        heightweight=zeros(F,0);
        % height prior:
        muh=1.7; sigmah=.7; factorh=1/sigmah/sqrt(2*pi);

        [mR mT]=getRotTrans(sceneInfo.camPar);

        for t=1:length(detections)
            ndet=length(detections(t).xp);
            detections(t).xw=zeros(1,ndet);
            detections(t).yw=zeros(1,ndet);

            for det=1:ndet
                [xw yw zw]=imageToWorld(detections(t).xi(det), detections(t).yi(det), sceneInfo.camPar);
                detections(t).xw(det)=xw;
                detections(t).yw(det)=yw;

                % one meter
                xi=detections(t).xi(det);yi=detections(t).yi(det);
                [xiu yiu]=worldToImage(xw,yw,1000,mR,mT,sceneInfo.camPar.mInt,sceneInfo.camPar.mGeo);
                onemeteronimage=norm([xi yi]-[xiu yiu]);
                worldheight=detections(t).ht(det)/onemeteronimage; % in meters
                weight=normpdf(worldheight,muh,sigmah)/factorh;

                detections(t).sc(det)=detections(t).sc(det)*weight;
            end

            %         detections(t).xp=detections(t).xw;
            %         detections(t).yp=detections(t).yw;
        end
    end
end

function detections=setDetectionPositions(detections,opt)
% set xp,yp to xi,yi if tracking is in image (2d)
% set xp,yp to xw,yi if tracking is in world (3d)
F=length(detections);
if opt.track3d
    assert(isfield(detections,'xw') && isfield(detections,'yw'), ...
        'for 3D tracking detections must have fields ''xw'' and ''yw''');
    
    for t=1:F,  detections(t).xp=detections(t).xw;detections(t).yp=detections(t).yw;        end
else
    for t=1:F,  detections(t).xp=detections(t).xi;detections(t).yp=detections(t).yi;        end
end
end