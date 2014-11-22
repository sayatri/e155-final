function [ matchedSignal ] = corrFilterS( inputAudio, audioBank, audioName )
% Description of Matching Algorithm:
% 
% 
% input inputAudio - the normalized audio input in the time domain
%                    that we are comparing to bank
% input audioBank  - a matrix with X the normalized pre-recorded audio
%                    inputs in the time domain
% input audioName  - an array of string correlating to the word of the
%                    pre-recorded audio
% output matchedSignal - the string of the word that is matched

% Get the signal in the frequency domain
maxfsignal = max(fft(inputAudio));
padsignal = [zeros(length(inputAudio) - 1, 1); inputAudio];
fsignal = fft(padsignal);

% Make a matrix to store the result of the correlations
result = zeros(1, size(audioBank,2));
corrs = zeros(length(padsignal), size(audioBank,2));
matlab = zeros(length(padsignal), size(audioBank,2));
matchIndex = 1;
maxCorr = 0;

% Compare the input signal to every signal in the bank
for i = 1:size(audioBank,2)
    comp = audioBank(:,i);
    padcomp = [comp; zeros(length(comp) - 1, 1)];
    fcomp = fft(padcomp);
    conjcomp = conj(fcomp);

    tempcorr = (ifft(fsignal .* conjcomp));
    corrs(:,i) = tempcorr;
    result(i) = max(tempcorr);
    temp = xcorr(inputAudio, comp);
    display(size(temp));
    matlab(:,i) = temp;
    
    if result(i) > maxCorr
        maxCorr = tempcorr;
        matchIndex = i;
    end
end
    
matchedSignal = audioName{matchIndex};

plotAll(corrs, audioName)
%plotAll(matlab, audioName)

% Displays the results on the screen
disp('The result of the filter is ...')
disp(result)
fprintf('\nYou said "%s"\n', matchedSignal);
end

