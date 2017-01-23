function [ w, e_in ] = logistic_reg( X, Y, max_its, w, eta, epsilon )
%LOGISTIC_REG Learn logistic regression model using gradient descent
%   Inputs:
%       X : data matrix
%       Y : data labels (plus or minus 1)
%	max_its : max iteration number
%	w : initial weight vector
%	eta : learning rate
%	epsilon : algorithm terminate tolerance
%   Outputs:
%       w : weight vector
%       e_in : in-sample error (as defined in LFD)

    nNumDat = length(X(:,1));    %Number of data;
    Xn = [ones(nNumDat,1) X]; %Xn: n,d+1 for Calculation.
    nDimW = length(Xn(1,:));  %Dimension of data;
    e_in = 0;                  %Init E_in
    
    for iT=1:max_its             %for t = 0,1,2...
        Gt = zeros(nDimW,1);     %gradient Gt=0
        for iN=1:nNumDat            
             Gt = Gt + (Y(iN,1)*Xn(iN,:)')/(1 + exp(Y(iN,1)*Xn(iN,:)*w));   %Sum of gradient
        end
        Gt = -Gt/nNumDat;
        
        bIsSmall = 1;   %threadshod test
        for iD=1:nDimW
            if abs(Gt(iD,1)) >= epsilon    %if any not below epsilon, stop checking
                bIsSmall = 0;
                break;          
            end
        end
        if bIsSmall == 1    %if no not below epsilon, stop iteration
            break;
        end
        w = w - eta*Gt;     %fetch direction and update the weight( Vt = -grad_t (omitted), w=w+eta*Vt)
    end
    
    for iN = 1:nNumDat
        e_in = e_in + log( 1+exp( -Y(iN,1)*Xn(iN,:)*w) );
    end
    e_in = e_in/nNumDat;
end

