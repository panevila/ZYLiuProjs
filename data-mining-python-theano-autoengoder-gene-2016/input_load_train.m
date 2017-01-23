D_train = csvread('data_group/data_train.csv');
X = D_train(:,1:8560);
Y = D_train(:,8561);

D_test = csvread('data_group/data_test.csv');
X_test = D_test(:,1:8560);
Y_test = D_test(:,8561);
