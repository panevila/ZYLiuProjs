function [ TreeCell ] = ForestTrainer( X, Y, numBags )
%BAGGEDTREES Returns out-of-bag classification error of an ensemble of
%numBags CART decision trees on the input dataset, and also plots the error
%as a function of the number of bags from 1 to numBags
%   Inputs:
%       X : Matrix of training data
%       Y : Vector of classes of the training examples
%       numBags : Number of trees to learn in the ensemble
%
%   You may use "fitctree" but do not use "TreeBagger" or any other inbuilt
%   bagging function

    %1. Initialization
    nData = length(Y);
    TreeCell = cell(1,numBags);
    ResultA = Y(1);
    ResultB = 0;
    Middle = 0;
    for i_n = 1:nData
        if Y(i_n) ~= ResultA
            ResultB = Y(i_n);
            Middle = (ResultA + ResultB)/2; %This middle is used for getting consensus
            break;
        end
    end
    
    %2 Baggings
    BagList = randi(nData,nData,numBags);
    for i_CurrentBag = 1:numBags
        X_Bagged = X(BagList(:,i_CurrentBag),:);
        Y_Bagged = Y(BagList(:,i_CurrentBag));
        TreeCell{1,i_CurrentBag} = fitctree(X_Bagged,Y_Bagged,'Prune','off', 'SplitCriterion', 'deviance');
    end
end

