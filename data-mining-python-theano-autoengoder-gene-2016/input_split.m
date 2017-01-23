D_sf = D(randperm(size(D,1)),:);
D_train = D_sf(1:291,:);
csvwrite('data_group/data_train.csv',D_train);
D_test = D_sf(292:364,:);
csvwrite('data_group/data_test.csv',D_test);
