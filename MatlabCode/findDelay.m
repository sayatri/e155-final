function [delay] = findDelay (a, b)

% DO NOT CHANGE THIS IS PERFECT %%
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
        result(n)= result(n) + ca*cb;
     
    end
end


[~,indexMax] = max(abs(result));

delay = ceil(length(result)/2) - indexMax;
