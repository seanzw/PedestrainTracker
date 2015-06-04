function printMoveFailure(m)
% 
% (C) Anton Andriyenko, 2012
%
% The code may be used free of charge for non-commercial and
% educational purposes, the only requirement is that this text is
% preserved within the derivative work. For any other purpose you
% must contact the authors for permission. This code may not be
% redistributed without written permission from the authors.


failstr='unknown move';
    switch(m)
        case 1
            failstr='merging does no good!';
        case 2
            failstr='split does no good!';
        case 3
            failstr='growing does no good!';
        case 4
            failstr='shrinking does no good!';
        case 5
            failstr='purge does no good!';
        case 6
            failstr='adding does no good!';
    end
    printMessage(2,'%s\n',failstr);
end