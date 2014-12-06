function [ normSignals ] = normalizeSignal( allSignals )
% Description: Normalizes the magnitude of the signal in the time
%              domain. Also windows signal to 2000 samples of interest. 
%
% Note: I didn't use windowSignal as a helper function in order
%       to combine magnitude normalization and concatenation in one
%       for loop.
%               
% input signal - audio input in time domain, can take single column or
%                matrix.
% output normSignal - normalized signal in the time domain

% get max amplitude from signal


len = length(allSignals(1,:)); % get number of audio

normSignals = ones(2000, len); % prepare output

    for i= 1:len      
        % get index of maximum value
        [~, index] = max(abs(allSignals(:, i)));
        
        % find window around max value
        if index <= 500
            startIndex = 1;
            endIndex = 2000;
        else 
            startIndex = index - 500;
            endIndex = index + 1499;
        end
        
        % Concatenates the signal
        normSignals(:,i) = allSignals(startIndex:endIndex, i);
        
        % normalize amplitude
        maxAmp = max(abs(normSignals(:,i)));
        normSignals(:,i) = normSignals(:,i)./maxAmp;
    end
    
    
end
