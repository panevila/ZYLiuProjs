function [ correct_rate_kmeans ] = validate_kmeans( X, Y )
%VALIDATE_KMEANS Summary of this function goes here
%   Detailed explanation goes here
    nCount = 0;
    sumVal = 0;
    for r=1:100
        idx = kmeans(X,2,'Distance','sqeuclidean','Start','uniform');
        len = length(idx);
        nRight = 0;
        for i=1:len
            if idx(i)~=Y(i)
                nRight = nRight + 1;
            end
        end
        rateRight = nRight/len;
        if rateRight<0.5
            rateRight = 1-0.5;
        end
        if rateRight > 0.51
            sumVal = sumVal + rateRight;
            nCount = nCount + 1;
        end
    end
    correct_rate_kmeans = sumVal/nCount;
end

