[r c]=size(D);
D_n = D;
for i=1:r
    D_n(i,:) = D_n(i,:) + awgn(D(i,:),-50);
end

X = D_n(:,1:8560);
Y = D_n(:,8561);