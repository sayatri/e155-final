
close all
% Control to make sure correlation is correct
a = [1 2 3 4];
b = [-1 2 1 -1];

matlabXcorr = xcorr(a, b)
figure
plot(matlabXcorr)
title('Result of matlab xcorr');

%%
%%
figure
plot(conv(a,fliplr(b)))
title('Result of xcorr with conv');

%% DO NOT CHANGE THIS IS PERFECT %%
result = zeros(2*length(a) - 1,1);
for n=1:(2*length(a) - 1)
    for k = 0:n-1
        if n-k <= length(a)
            ca = a(n-k);
        else
            ca = 0;
        end
        
        if ((length(b)-k) > 0)
            cb = b(end-k);
        else
            cb = 0;
        end
        result(n)= result(n) + ca*cb
     
    end
end

figure
plot(result)
title('Result of xcorr with for loop');
%%




result = zeros(19999,1);
for i=1:(lena+lenb-1)
    for n=1:(lena+lenb-1)
        result(i) = result(i) + za(n)*zb(i);
        
    end
end
        
result
figure
plot(result)
title('Result of manual xcorr');

%%
figure
plot(ifft(fft(za).*conj(fft(zb))))
title('Result of old manual xcorr');

