function [ error ] = validate_kmeans_ext( X_name, Y_name )
%VALIDATE_KMEANS_EXT Summary of this function goes here
%   Detailed explanation goes here
    dirX = strcat('data_group/pretrained/',X_name,'.csv');
    dirY = strcat('data_group/pretrained/',Y_name,'.csv');
    X = csvread(dirX);
    Y = csvread(dirY);
    [ correct_rate_kmeans ] = validate_kmeans( X, Y );
    error = correct_rate_kmeans;
end

