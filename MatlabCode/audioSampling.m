%% Training

%% Cat
recCat = audiorecorder(5000, 8, 1);
disp('Say Cat.')
recordblocking(recCat, 2);
disp('End of Recording.');

% Play back the recording.
play(recCat);

% Store data in double-precision array.
Cat = getaudiodata(recCat);

%% Dog
recDog = audiorecorder(5000, 8, 1);
disp('Say Dog.')
recordblocking(recDog, 2);
disp('End of Recording.');

% Play back the recording.
play(recDog);

% Store data in double-precision array.
Dog = getaudiodata(recDog);

%% Mouse
recMouse = audiorecorder(5000, 8, 1);
disp('Say Off.')
recordblocking(recMouse, 2);
disp('End of Recording.');

% Play back the recording.
play(recMouse);

% Store data in double-precision array.
Off = getaudiodata(recMouse);

%% Horse
recHorse = audiorecorder(5000, 8, 1);
disp('Say Horse.')
recordblocking(recHorse, 2);
disp('End of Recording.');

% Play back the recording.
play(recHorse);

% Store data in double-precision array.
Horse = getaudiodata(recHorse);


%% New Signal

recSignal = audiorecorder(5000, 8, 1);
disp('Start speaking to test the system.')
recordblocking(recSignal, 2);
disp('End of Recording.');

% Play back the recording.
play(recSignal);

% Store data in double-precision array.
newSignal = getaudiodata(recSignal);


%% Matched Signal Implementation

rawSignal = newSignal(:,1);
disp('normalizing input signal');
inputSignal = normalizeSignal(rawSignal);  %% normalize

% audioBank{1} = Cat;
% audioBank{2} = Dog;
% audioBank{3} = Mouse;
% audioBank{4} = Horse;
audioBank = [Cat Dog Mouse Horse];
audioName = {'Cat' 'Dog' 'Mouse' 'Horse'};

disp('normalizing signal');
normBank = normalizeSignal(audioBank);

disp('looking for match');
matchedSignal = corrFilterS(inputSignal, normBank, audioName);


%%

% Plot the waveform.
%signals = [audioBank
plot(audioBank, audioName);

%%
spectrogram(Cat)

figure
spectro



