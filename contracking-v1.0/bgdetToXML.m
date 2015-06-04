% function bgdetToXML(folder)
folder='D:\visinf\projects\ongoing\baark-praktikum\opencv-gpu\background_subtraction\Release\det\';

folderlisting=dir([folder '*.det']);

detections=[];
for f=1:length(folderlisting)
    fname=[folder folderlisting(f).name];
    alldets=load(fname);
    alldets=alldets';
    ndets=size(alldets,2);
        
    if ndets
    detections(f).bx=alldets(1,:);
    detections(f).by=alldets(2,:);
    detections(f).wd=alldets(3,:);
    detections(f).ht=alldets(4,:);
    
    detections(f).sc=ones(1,ndets);
    
    detections(f).xi=detections(f).bx+detections(f).wd/2;
    detections(f).yi=detections(f).by+detections(f).ht;
    
    detections(f).xp=detections(f).xi;detections(f).yp=detections(f).yi;
    end
end

if ~ndets
    detections(f).bx=[];detections(f).by=[];
    detections(f).wd=[];detections(f).ht=[];
    detections(f).yi=[];detections(f).xi=[];
    detections(f).yp=[];detections(f).xp=[];    
end
save('D:\diss\detections\hog-hof-linsvm\DA-ELS\s01\detections.mat','detections')
% end