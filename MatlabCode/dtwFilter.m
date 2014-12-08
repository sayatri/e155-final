function [ matchedSignal, second, third ] = dtwFilter( testSignal, allSignals, allSignalNames, delayFlag )
% Description of Matching Algorithm:
% 
% 
% input inputAudio - the normalized audio input in the time domain
%                    that we are comparing to bank
% input audioBank  - an matrix with X the normalized pre-recorded audio
%                    inputs in the time domain
% input audioName  - an array of string correlating to the word of the
%                    pre-recorded audio
% output matchedSignal - the string of the word that is matched
tic;


len = length(allSignals(1,:));


% initialize values 
finalIndex = 1;
secondInd = finalIndex;
thirdInd = secondInd;

% Analyze first signal
preRec = allSignals(:,1);

if delayFlag == 1
    delay = findDelay(testSignal, preRec);
else 
    delay = 0;
end

newEnd = length(testSignal) - abs(delay);
newStart = abs(delay);

if delay < 0
    minDist = dtw(testSignal(newStart:end), preRec(1:newEnd), 20);
elseif delay == 0
    minDist = dtw(testSignal, preRec, 20);
else
    minDist = dtw(testSignal(1:newEnd), preRec(newStart:end), 20);
end


secondDist = minDist;
thirdDist = secondDist;

% loop through all the the other signals and compare to previous analysis
for i = 2:len
    preRec = allSignals(:,i);
    
    if delayFlag == 1
        delay = findDelay(testSignal, preRec);
    else 
        delay = 0;
    end
    
    newEnd = length(testSignal)-abs(delay);
    newStart = abs(delay);
    
    if delay < 0
        newDist = dtw(testSignal(newStart:end), preRec(1:newEnd), 20);
    elseif delay == 0
        newDist = dtw(testSignal, preRec, 20);
    else
        newDist = dtw(testSignal(1:newEnd), preRec(newStart:end), 20);
    end
    
    if newDist < minDist
        thirdDist = secondDist;
        secondDist = minDist;
        minDist = newDist;
        
        thirdInd = secondInd;
        secondInd = finalIndex;
        finalIndex = i;
    end
    i = i + 1;
end



matchedSignal = allSignals(:,finalIndex);
second = allSignals(:,secondInd);
third = allSignals(:,thirdInd);

time = toc;
fprintf('\nYou said "%s", with a distance of %f\n',...
    allSignalNames{finalIndex}, minDist);
fprintf('The second closest match was "%s", with a distance of %f\n',...
    allSignalNames{secondInd}, secondDist);
fprintf('The third closest match was "%s", with a distance of %f\n',...
    allSignalNames{thirdInd}, thirdDist);
fprintf('It took %f seconds to calculate\n\n', time);


end

