% 
% (C) Anton Andriyenko, 2012
%
% The code may be used free of charge for non-commercial and
% educational purposes, the only requirement is that this text is
% preserved within the derivative work. For any other purpose you
% must contact the authors for permission. This code may not be
% redistributed without written permission from the authors.


% compile mex files

srcFiles={'Edet_mex','Edyn_mex','Eexc_mex','Eper_mex','CLEAR_MOT_mex','vectorToMatrices_mex','computeOcclusions2new_mex'};
srcExt='c';
srcdir=fullfile('mex','src');
outdir=fullfile('mex','bin');

if ~exist(outdir,'dir'), mkdir(outdir); end

for k=1:length(srcFiles)
    eval(sprintf('mex -outdir %s %s.%s',outdir,fullfile(srcdir,char(srcFiles(k))),srcExt));
end

%% compile utils
utildir=fullfile('utils');
srcFiles={'allWorldToImage_mex'};
srcExt='c';
srcdir=fullfile(utildir,'mex','src');
outdir=fullfile(utildir,'mex','bin');

if ~exist(outdir,'dir'), mkdir(outdir); end
for k=1:length(srcFiles)
    eval(sprintf('mex -outdir %s %s.%s',outdir,fullfile(srcdir,char(srcFiles(k))),srcExt));
end