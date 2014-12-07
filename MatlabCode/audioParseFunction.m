function [ audio, modaudio ] = audioParseFunction( filename )
%UNTITLED2 Summary of this function goes here
%   Detailed explanation goes here

audio = zeros(2000,5);

audio(:,1) = xlsread( filename, 'B11:B2010');
audio(:,2) = xlsread( filename, 'B2021:B4020');
audio(:,3) = xlsread( filename, 'B4031:B6030');
audio(:,4) = xlsread( filename, 'B6041:B8040');
audio(:,5) = xlsread( filename, 'B8051:B10050');

subplot(5,1,1);
plotAll(audio, {'1', '2', '3','4', '5'})

modaudio = audio./1023;
    

end

