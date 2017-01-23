function [ train_err, test_err ] = AdaBoost( X_tr, y_tr, X_te, y_te, n_trees )
%AdaBoost: Implement AdaBoost using decision stumps learned
%   using information gain as the weak learners.
%   X_tr: Training set
%   y_tr: Training set labels
%   X_te: Testing set
%   y_te: Testing set labels
%   n_trees: The number of trees to use

    train_err = 0;
    test_err = 0;
    nData_tr = length(y_tr);
    nData_te = length(y_te);
    nFeatures = length(X_tr(1,:));
    
    X_tr_Buffer = X_tr;
    ResultA = y_tr(1);
    ResultB = 0;
    Middle = 0;
    
    Stumps = zeros(n_trees,3);
    %Check result range and convert y to {-1,1}
    for i_n = 1:nData_tr
        if y_tr(i_n) ~= ResultA
            ResultB = y_tr(i_n);
            Middle = (ResultA + ResultB)/2; 
            break;
        end
    end
    Y_sign_train = sign(y_tr - Middle);
    Y_sign_test = sign(y_te - Middle);
    
    Weight = (1/nData_tr).*ones(nData_tr,1);     %init weight 1/N
    alpha_m = zeros(n_trees,1);
    hypo_m = zeros(nData_tr,n_trees);   %Record the stumps hypothesis
    hypo_test_m = zeros(nData_te,n_trees);    %Record the stumps hypothesis (for test)
    
    plot_index = 1:1:n_trees;
    plot_train_err = zeros(1,n_trees);
    plot_test_err = zeros(1,n_trees);
    for i_t = 1:n_trees
        %Fit stumps
        %=============IG Computation=============
        %Calculate Overall Entropy
        P_OVAPos = sum(Weight(Y_sign_train>0)); %"/nData" already inherited in "Weight"
        P_OVANeg = sum(Weight(Y_sign_train<0));
	
        Etp_Overall = -P_OVAPos*log2(P_OVAPos)-P_OVANeg*log2(P_OVANeg);
        IG = zeros(1,nFeatures);
        BestSplitValues = zeros(nFeatures,1);
        SplitDirection = zeros(nFeatures,1);
        err_f = zeros(nFeatures,1);
		%Calculate Children Entropy & Info gain for different split/value
        for i_Feature = 1:nFeatures
            DataMatrix = [X_tr_Buffer(:,i_Feature) Y_sign_train Weight];
            %Sort the rows by X, calculate err for every possible split
            D_temp = sortrows(DataMatrix, 1);
            X_SortedByX = D_temp(:,1);
            Y_SortedByX = D_temp(:,2);
            W_SortedByX = D_temp(:,3);
            
            Hypo = ones(nData_tr,1);   %Before classifying by fst data, Hypo is all 1
            Hypo_bw = -1.*ones(nData_tr,1); %"_bw" means reverse direction x<=a -> +1 | x>a -> -1
            %For model x<=a -> -1 | x>a -> +1 or x<=a -> +1 | x>a -> -1, 
            %its split point can only at places where next X is different
            %Or the classifier cannot make splits between two same value
            nPossibleSplits = sum(length(unique(X_SortedByX)))-1;
            if nPossibleSplits == 0
                IG(i_Feature) = -Inf;       %no possible splits, drop it and consider next one
                continue;
            end
            
            err_cls = zeros(nPossibleSplits,2);     %Store cls err for possible splits
            err_cls_bw = zeros(nPossibleSplits,2);      %Store cls err for opposite hypos
            i_ps = 0;
            for i_n = 1:nData_tr-1
                Hypo(i_n,1) = -1;
                Hypo_bw(i_n,1) = +1;
                if X_SortedByX(i_n+1)~=X_SortedByX(i_n)
                    i_ps = i_ps+1;
                    err_cls(i_ps,1) = sum(W_SortedByX( (Hypo ~= Y_SortedByX) ));
                    err_cls(i_ps,2) = i_n;
                    err_cls_bw(i_ps,1) = sum(W_SortedByX( (Hypo_bw ~= Y_SortedByX) ));
                    err_cls_bw(i_ps,2) = i_n;
                end
            end
            [errfw,Best_i_ps] = min(err_cls(:,1));
            BestSplit = err_cls(Best_i_ps,2);
            [errbw,Best_i_ps] = min(err_cls_bw(:,1));
            BestSplit_bw = err_cls_bw(Best_i_ps,2);
            % apply the direction which can yield lower cls err
            if errfw<=errbw
                BestSplitValues(i_Feature,1) = X_SortedByX(BestSplit,1);
                SplitDirection(i_Feature,1) = 1;
                err_f(i_Feature,1)=errfw;
                SplitIndex = BestSplit;
            else
                BestSplitValues(i_Feature,1) = X_SortedByX(BestSplit_bw,1);
                SplitDirection(i_Feature,1) = -1;
                err_f(i_Feature,1)=errbw;
                SplitIndex = BestSplit_bw;
            end
            %Calculate Entropy of this split
            pPos1 = W_SortedByX(1:SplitIndex)' * (Y_SortedByX(1:SplitIndex)>0 );
            pNeg1 = W_SortedByX(1:SplitIndex)' * (Y_SortedByX(1:SplitIndex)<0 );
            
            pPos2 = W_SortedByX(SplitIndex+1:nData_tr)' * (Y_SortedByX(SplitIndex+1:nData_tr)>0 );
            pNeg2 = W_SortedByX(SplitIndex+1:nData_tr)' * (Y_SortedByX(SplitIndex+1:nData_tr)<0 );
            
            f1 = sum( W_SortedByX(1:SplitIndex) );
            f2 = sum( W_SortedByX(SplitIndex+1:nData_tr) );
            
            if pPos1 == 0 || pNeg1 ==0
                Entropy1 = 0;
            else
                Entropy1 = -pPos1*log2(pPos1)-pNeg1*log2(pNeg1);
            end
            
            if pPos2 == 0 || pNeg2 ==0
                Entropy2 = 0;
            else
                Entropy2 = -pPos2*log2(pPos2)-pNeg2*log2(pNeg2);
            end
            
            IG(i_Feature) = Etp_Overall - f1*Entropy1 - f2*Entropy2;
        end
        
        %==============Get Best Stump==============
        [~,IndexMaxIG] = max(IG);
%         A stump here can be represented by: 
%         1)Which feature applied     f_split ,Stored at 1st col of "Stump"
%         2)Split threshod a          v_threshod ,Stored at 2nd col
%         3)Direction x<=a -> -1 | x>a -> +1 or x<=a -> +1 | x>a -> -1  direction ,Stored at 3rd col
        Stumps(i_t,:) = [IndexMaxIG BestSplitValues(IndexMaxIG) SplitDirection(IndexMaxIG,1)];
        
        f_split = Stumps(i_t,1);
        v_threshod = Stumps(i_t,2);
        direction = Stumps(i_t,3);
        hypo_m(:,i_t) = direction * sign( (X_tr(:,f_split) > v_threshod)-0.5 );
        
        %Compute error & alpha_m
        err_m = sum(Weight(hypo_m(:,i_t) ~= Y_sign_train))/sum(Weight);
        alpha_m(i_t,1) = log((1-err_m)/err_m)/2;
        
        %Update Weights
        bIsRight = hypo_m(:,i_t) == Y_sign_train;
        Weight(bIsRight) = Weight(bIsRight).*exp(-alpha_m(i_t,1));
        
        bIsWrong = hypo_m(:,i_t) ~= Y_sign_train;
        Weight(bIsWrong) = Weight(bIsWrong).*exp(alpha_m(i_t,1));
        
        Weight = Weight./sum(Weight);
        
        %==============Train and record result==============
        %Apply on train dataset 
        ConHypo_train = sign(hypo_m(:,1:i_t)*alpha_m(1:i_t,1));
        bIsWrong = ConHypo_train ~= Y_sign_train;
        train_err = sum(bIsWrong)/nData_tr;
        plot_train_err(1,i_t)=train_err;
        
        %Apply on test dataset
        f_split = Stumps(i_t,1);
        v_threshod = Stumps(i_t,2);
        direction = Stumps(i_t,3);
        hypo_test_m(:,i_t) = direction * sign( (X_te(:,f_split) > v_threshod)-0.5 );
        
        ConHypo_test = sign( hypo_test_m(:,1:i_t)*alpha_m(1:i_t,1) );
        bIsWrong = ConHypo_test ~= Y_sign_test;
        test_err = sum(bIsWrong)/nData_tr;
        plot_test_err(1,i_t)=test_err;
    end
    
    plot(plot_index,plot_train_err);
    hold on;
    plot(plot_index,plot_test_err);
    hold on;
    legend('Train Error','Test Error');
    xlabel('Number of Stumps(Weak Learners) Trained');
    ylabel('Classification Error');
    hold on;
    
end

