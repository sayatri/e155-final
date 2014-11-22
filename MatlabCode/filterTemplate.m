function [ matchedSignal ] = filterTemplate( inputAudio, allSignals, allSignalNames )
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


% RUN THIS CODE BEFORE CALLING THE FUNCTION
% disp('Begin processing data...');

tic;


% Displays the results on the screen
fprintf('\nYou said "%s"\n', allSignalNames{finalIndex});
fprintf('It took %f seconds to calculate\n\n', time);


end

