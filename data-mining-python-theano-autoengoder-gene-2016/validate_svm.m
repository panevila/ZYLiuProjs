function [ err ] = validate_svm( X_name, Y_name, X_test_name, Y_test_name )
%VALIDATE_PCA Summary of this function goes here
%   Detailed explanation goes here
    dirX = strcat('data_group/pretrained/',X_name,'.csv');
    dirY = strcat('data_group/pretrained/',Y_name,'.csv');
    
    dirX_test = strcat('data_group/pretrained/',X_test_name,'.csv');
    dirY_test = strcat('data_group/pretrained/',Y_test_name,'.csv');
    
    X = csvread(dirX);
    Y = csvread(dirY);
    
    X_test = csvread(dirX_test);
    Y_test = csvread(dirY_test);
    
    SVMModel = fitcsvm(X,Y,'KernelFunction','linear','CrossVal', 'on', 'Standardize',true,'ClassNames',{'1','2'});
    [label] = predict(SVMModel,X_test);
    len = length(label);
    nRight = 0;
    for i=1:len
        if strcmp(label(i), num2str(Y_test(i)))
            nRight = nRight + 1;
        end
    end
    err = nRight/len;
    if err<0.5
        err = 1-err;
    end
end

