allscen=[23 42 80 25 27 71 72];

allmets2d=zeros(max(allscen),14,100);
allmets3d=zeros(max(allscen),14,100);
allopts=[];

for exprun=1:100
    % random sample
    rnmeans=[1 .1 .5 .5 .5 .1];
    rmvars=[.1 .1 .1 .1 .1 .01];
    randopts=rnmeans + rmvars.*randn(1,6);
    opt.weightEdet=randopts(1);
    opt.weightEdyn=randopts(2);
    opt.weightEexc=randopts(3);
    opt.weightEper=randopts(4);
    opt.weightEreg=randopts(5);
    allopts=[allopts; randopts];
    
    for scenario=allscen
        randopts
        scenario
        [metrics2d metrics3d]=cemTrackerSearch(scenario,randopts);
        allmets2d(scenario,:,exprun)=metrics2d;
        allmets3d(scenario,:,exprun)=metrics3d;
    end
    
    save('testrun.mat','LOG_*');    
end


