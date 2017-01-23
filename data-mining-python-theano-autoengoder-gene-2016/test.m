% autoenc = trainAutoencoder(X',200,'UseGPU',true,'MaxEpochs',200);
% H1 = encode(autoenc,X')';
% X1 = decode(autoenc,H1')';
% csvwrite('data_group/test/H1.csv',H1);
% csvwrite('data_group/test/X1.csv',X1);

autoenc2 = trainAutoencoder(X',20,'UseGPU',true,'MaxEpochs',200);
H2 = encode(autoenc2,X')';
X2 = decode(autoenc2,H2')';
csvwrite('data_group/test/H2.csv',H2);
csvwrite('data_group/test/X2.csv',X2);

% autoenc = trainAutoencoder(X1',100,'UseGPU',true,'MaxEpochs',200);
% H2 = encode(autoenc,X1')';
% X2 = decode(autoenc,H2')';
% csvwrite('data_group/test/H2.csv',H2);
% csvwrite('data_group/test/X2.csv',X2);
% 
% autoenc = trainAutoencoder(X2',20,'UseGPU',true,'MaxEpochs',200);
% H3 = encode(autoenc,X2')';
% X3 = decode(autoenc,H3')';
% csvwrite('data_group/test/H3.csv',H3);
% csvwrite('data_group/test/X3.csv',X3);