function plotAll( allSignals, allSignalNames )
% Takes in a matrix of audio samples and plots them
%   all in one window

len = length(allSignals(1,:)); % get number of audio samples
sqrtLen = sqrt(len);

height = floor(sqrtLen);
width = ceil(sqrtLen);


for i=1:len
    
    subplot(height,width,i);
    plot(allSignals(:,i));
    title(allSignalNames(i));
  %  plot(allSignals(:,i));
end

end

