%  load('preRecAudio.mat');

audioMat = [Cat Close Dog Horse]; % Mouse Off On Open Pause Start Stop];
audioNam = {'Cat' 'Close' 'Dog' 'Horse'}; % 'Mouse' 'Off' 'On' 'Open'...
            %    'Pause' 'Start' 'Stop'];

tic;
matchedSignal = convs(Horse, audioMat, audioNam);
time = toc;

fprintf('It took this long %f', time);