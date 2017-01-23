

autoenc1 = trainAutoencoder(X',1000,'UseGPU',true,'MaxEpochs',400);
H1 = encode(autoenc1,X')';

autoenc2 = trainAutoencoder(H1',600,'UseGPU',true,'MaxEpochs',400);
H2 = encode(autoenc2,H1')';

autoenc3 = trainAutoencoder(H2',300,'UseGPU',true,'MaxEpochs',400);
H3 = encode(autoenc3,H2')';

autoenc4 = trainAutoencoder(H3',200,'UseGPU',true,'MaxEpochs',400);
H4 = encode(autoenc4,H3')';

autoenc5 = trainAutoencoder(H4',100,'UseGPU',true,'MaxEpochs',400);
H5 = encode(autoenc5,H4')';

autoenc5 = trainAutoencoder(H5',50,'UseGPU',true,'MaxEpochs',400);
H6 = encode(autoenc5,H5')';

autoenc5 = trainAutoencoder(H6',20,'UseGPU',true,'MaxEpochs',400);
H7 = encode(autoenc5,H6')';

autoenc5 = trainAutoencoder(H7',20,'UseGPU',true,'MaxEpochs',400);
H8 = encode(autoenc5,H7')';

autoenc5 = trainAutoencoder(H8',50,'UseGPU',true,'MaxEpochs',400);
H9 = encode(autoenc5,H8')';