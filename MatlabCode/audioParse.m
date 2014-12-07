
%% HELLO------------------------------------------------------------------
[audioHello, modaudioHello] = audioParseFunction('recordHello.xlsx');

%% Sound
sound(modaudioHello(:,2), 6819);

%%----
recHello = audioHello(:,2);
%---

%% BYE---------------------------------------------------

[audioBye ,modaudioBye] = audioParseFunction('recordBye.xlsx');

%%
sound(modaudioBye(:,3), 6819);

%%
recBye = audioBye(:,3);


%% YES -------------------------------------------------------

[audioYes, modaudioYes] = audioParseFunction('recordYes.xlsx');

%%
sound(modaudioYes(:,5), 6819);

%%
recYes = audioYes(:,3);

%% NO-----------------------------------------------------------------

[audioNo, modaudioNo] = audioParseFunction('recordNo.xlsx');

%%
sound(modaudioNo(:,5), 6819);

%%
recNo = audioNo(:,5);




%%
figure
[audioRed, modaudioRed] = audioParseFunction('recordRed.xlsx');
suptitle('Red');

figure
[audioGreen, modaudiGreen] = audioParseFunction('recordGreen.xlsx');
suptitle('Green');

figure
[audioYellow, modaudioYello] = audioParseFunction('recordYellow.xlsx');
suptitle('Yellow');

figure
[audioOrange, modaudioOrange] = audioParseFunction('recordOrange.xlsx');
supitle('Orange');


%%

sound(modaudioOrange(:,2), 6819);

%%
recRed = audioRed(:,2);
recYellow = audioYellow(:,2);
recGreen = audioGreen(:,5);
recOrange = audioOrange(:,2);

%%
save('recordedAudio.mat');

%%
%%xlswrite('allrecordedAudioConvo.xlsx', '//Hello', 'A0');
xlswrite('deletemeOrange.xlsx', recOrange, 'A1:A2000');
% xlswrite('allrecordedAudioConvo.xlsx', '//Bye', 'A2001');
% xlswrite('allrecordedAudioConvo.xlsx', recBye, 'A2002:A4001');
% xlswrite('allrecordedAudioConvo.xlsx', '//Hello', 'A4002');
% xlswrite('allrecordedAudioConvo.xlsx', recHello, 'A4003:A6002');
% xlswrite('allrecordedAudioConvo.xlsx', '//Hello', 'A6003');
% xlswrite('allrecordedAudioConvo.xlsx', recHello, 'A6004:A8003');



