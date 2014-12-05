audioCat = xlsread('recordStop.xlsx', 'A1:A4000');

modCatAudio = .5 - audioCat./1024;

plot(audioCat);

figure
plot(modCatAudio);
sound(modCatAudio, 6819);