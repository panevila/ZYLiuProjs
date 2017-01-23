% eX = [ err_X1 err_X2 err_X3 err_X4 err_X5 ];
eH = [ 0.5782 0.5527 0.5537 0.5589 0.5587 0.5583 0.5513 0.5522 0.5522];
eOrg = [0.5928 0.5928 0.5928 0.5928 0.5928 0.5928 0.5928 0.5928 0.5928];
iStack = [ 1 2 3 4 5 6 7 8 9];

plot(iStack, eH, iStack, eOrg);
xlabel('number of hidden variables');
ylabel('k-Means average accuracy');
title('k-Means average accuracy correspond to number of stacks. nFeature=100');
legend('encoded dataset','origin accuracy');
