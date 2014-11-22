function [ denoiseSignal ] = denoiseSignal( signal )
% Description: Denoise the audio signal
% input signal - audio input in time domain
% output filtSignal - denoised signal in the time domain

% Take the FFT of the signal
fsignal = fft(signal);
deltaf = 5000 / size(signal);
nyquist = 5000 / 2;

freqs = [-nyquist:deltaf:(nyquist - deltaf)];

denoiseSignal = signal;


end
