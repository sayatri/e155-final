
%% How you can take an audio recording from the PIC and convert to something
% comparable to data recorded by the laptop

audioCat = xlsread('recordCat.xlsx', 'A1:A4000');

modCatAudio = (.5 - audioCat./1024)*2;

plot(audioCat);

figure
plot(modCatAudio);
sound(modCatAudio, 6819);


%%



normSignal = normalizeSignal(allSignals);
normTestSignal = normalizeSignal(testSignal);

disp('Begin processing data with the DTW Filter...');

xcorrFilter(normTestSignal, normSignal, allSignalNames);
disp(' ');
disp('Begin processing data with the xcorr Filter...');

xcorrFilter(normTestSignal, normSignal, allSignalNames);



xcorrFilter( normTestAudio, allSignals, allSignalNames )