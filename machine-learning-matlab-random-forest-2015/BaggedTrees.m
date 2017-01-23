function [ oobErr ] = BaggedTrees( X, Y, numBags )
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
    TreesApplied = zeros(nData,numBags);
    OOB = zeros(1,numBags);
    OOBValeResult = zeros(nData,1);
    TreeCell = cell(1,numBags); %Cell for stored trees (not necessary, only for backup)
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
        TreesApplied( BagList(:,i_CurrentBag), i_CurrentBag) = 1;
    end
    
    for i_CurrentBag = 1:numBags
        X_Bagged = X(BagList(:,i_CurrentBag),:);
        Y_Bagged = Y(BagList(:,i_CurrentBag));
        TreeCell{1,i_CurrentBag} = fitctree(X_Bagged,Y_Bagged,'Prune','off', 'SplitCriterion', 'deviance');
        
        %compute error
        ErrCnt = 0;
        nOOBData = 0;
        nAllApplied = 0;
        for i_n = 1:nData
            if sum(TreesApplied(i_n,1:i_CurrentBag)) == i_CurrentBag
                nAllApplied = nAllApplied+1;
                continue;
            end
            if TreesApplied(i_n,i_CurrentBag)==0
                OOBValeResult(i_n) = OOBValeResult(i_n) + predict(TreeCell{1,i_CurrentBag},X(i_n,:)) - Middle;
                nOOBData = nOOBData+1;
            end
            PredictConsensus = sign(OOBValeResult(i_n)); %Convert cumulative result back to classes
            if PredictConsensus > 0
                PredictConsensus = max(ResultA,ResultB);
            elseif PredictConsensus < 0
                PredictConsensus = min(ResultA,ResultB);
            end %If both side have equal vote, treat it as wrong anyway
            if PredictConsensus ~= Y(i_n)
                ErrCnt = ErrCnt + 1;
            end
        end
        
        OOB(i_CurrentBag) = ErrCnt/(nData - nAllApplied);
    end
    id = 1:1:numBags;
    plot(id, OOB);
    hold on;
    oobErr = OOB(numBags);
end

