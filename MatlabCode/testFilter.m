allModAudio = normalizeSignal(allSignals);
plotAll(allModAudio, allSignalNames);


%% If you want to record a signal to test

recSignal = audiorecorder(5000, 8, 1);
disp('Start speaking to test the system.')
recordblocking(recSignal, 2);
disp('End of Recording.');

% Play back the recording.
play(recSignal);

% Store data in double-precision array.
newSignal = getaudiodata(recSignal);


% Assign whether or not you want to test a new signal, or signal in the
% audio bank.

testSignal = newSignal;
normTestSignal = normalizeSignal(testSignal);

%%


% concatenate the signals

% disp('Begin processing data with the DTW Filter...');
% dtwFilter(normTestSignal, normSignal, allModNames);




disp(' ');
disp('Begin processing data with the shortened signals.');

dtwFilter(normTestSignal, normalizeSignal([Horse Stop Open On]), ... 
    {'Start', 'Stop', 'Open', 'On'}, 0);

disp('not concat');

dtwFilter(testSignal, [Horse Stop Open On], ... 
    {'Start', 'Stop', 'Open', 'On'}, 0 );






