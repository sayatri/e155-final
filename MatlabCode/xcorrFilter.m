function [ matchedSignal, second, third ] = ...
    xcorrFilter( normTestAudio, normSignals, allSignalNames )

% Description of Matching Algorithm:
% 
% Use cross-correlation to match audio signals
% 
% input concatTestAudio - the normalized audio input in the time domain
%                    that we are comparing to bank
% input concatSignals  - an matrix with X the normalized pre-recorded audio
%                    inputs in the time domain
% input allSignalNames  - an array of string correlating to the word of the
%                    pre-recorded audio
% output matchedSignal - the string of the word that is matched

% close all
% normTestAudio = normalizeSignal(Off);
% normSignals = normalizeSignal(allSignals);

tic;

len = length(normSignals(1,:));

maxValue = 0;
secondVal = 0;
thirdVal = 0;

finalIndex = 1;
secondInd = 1;
thirdInd = 1;

fftxCorr = zeros(3999,11);

for i = 1:len
     currXCorr = xcorr_fft(normTestAudio', normSignals(:,i)')';
     currentMax = max(abs(currXCorr));
     
     if currentMax > maxValue
         thirdVal = secondVal;
         secondVal = maxValue;
         maxValue = currentMax;
        
        thirdInd = secondInd;
        secondInd = finalIndex;
        finalIndex = i;
     end
     
     fftxCorr(:,i) = currXCorr;
         
     matlabxCorr(:,i) = xcorr(normTestAudio, normSignals(:,i));
end

% figure
% plotAll(matlabxCorr, allSignalNames)
% suptitle('Result of Matlab xcorr Function');

figure
%plotAll(fftxCorr, allSignalNames)
suptitle('Result of fft xcorr Function');

% figure
% autocorr = xcorr(normTestAudio);
% plot(autocorr);
% title('Result of AutoCorrelation from Matlab Function');
% 
% figure
% autocorr = xcorr_fft(normTestAudio', normTestAudio');
% plot(autocorr);
% title('Result of AutoCorrelation from FFT Function');




time = toc;

%Displays the results on the screen
fprintf('\nYou said "%s"\n', allSignalNames{finalIndex});
fprintf('It took %f seconds to calculate\n\n', time);

matchedSignal = normSignals(:,finalIndex);
second = normSignals(:,secondInd);
third = normSignals(:,thirdInd);

time = toc;
fprintf('\nYou said "%s", with a distance of %f\n',...
    allSignalNames{finalIndex}, maxValue);
fprintf('The second closest match was "%s", with a distance of %f\n',...
    allSignalNames{secondInd}, secondVal);
fprintf('The third closest match was "%s", with a distance of %f\n',...
    allSignalNames{thirdInd}, thirdVal);
fprintf('It took %f seconds to calculate\n\n', time);


end

