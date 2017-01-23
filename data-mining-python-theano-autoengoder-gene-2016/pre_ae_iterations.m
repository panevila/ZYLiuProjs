csvwrite('data_group/pretrained_iter/Y.csv',Y);
csvwrite('data_group/pretrained_iter/X.csv',X);

autoenc = trainAutoencoder(X',1000,'UseGPU',true,'MaxEpochs',1000);
H1 = encode(autoenc,X')';
X1 = decode(autoenc,H1')';
csvwrite('data_group/pretrained_iter/H1_100.csv',H1);
csvwrite('data_group/pretrained_iter/X1_100.csv',X1);