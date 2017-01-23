csvwrite('data_group/pretrained/Y.csv',Y);
csvwrite('data_group/pretrained/X.csv',X);

autoenc = trainAutoencoder(X',20,'UseGPU',true,'MaxEpochs',400);
H1 = encode(autoenc,X')';
X1 = decode(autoenc,H1')';
csvwrite('data_group/pretrained/H1.csv',H1);
csvwrite('data_group/pretrained/X1.csv',X1);

autoenc = trainAutoencoder(X1',20,'UseGPU',true,'MaxEpochs',400);
H2 = encode(autoenc,X1')';
X2 = decode(autoenc,H2')';
clearvars H1 X1;
csvwrite('data_group/pretrained/H2.csv',H2);
csvwrite('data_group/pretrained/X2.csv',X2);

autoenc = trainAutoencoder(X2',20,'UseGPU',true,'MaxEpochs',400);
H3 = encode(autoenc,X2')';
X3 = decode(autoenc,H3')';
clearvars H2 X2;
csvwrite('data_group/pretrained/H3.csv',H3);
csvwrite('data_group/pretrained/X3.csv',X3);

autoenc = trainAutoencoder(X3',20,'UseGPU',true,'MaxEpochs',400);
H4 = encode(autoenc,X3')';
X4 = decode(autoenc,H4')';
clearvars H3 X3;
csvwrite('data_group/pretrained/H4.csv',H4);
csvwrite('data_group/pretrained/X4.csv',X4);

autoenc = trainAutoencoder(X4',20,'UseGPU',true,'MaxEpochs',400);
H5 = encode(autoenc,X4')';
X5 = decode(autoenc,H5')';
clearvars H4 X4;
csvwrite('data_group/pretrained/H5.csv',H5);
csvwrite('data_group/pretrained/X5.csv',X5);
