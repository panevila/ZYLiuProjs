
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>

using namespace std;


int main(int argc, char *argv[], char *envp[])
{
	if(argc!=4)
	{
		cout<<"Syntax: vmgenU [range of page ref] [length] [filename]\n";
		return 0;
	}
	int rNum = 0;
	int range = atoi(argv[1]);
	if(range>100)
	{
		cout<<"Please specify a range no greater than 100\n";
		return 0;
	}	
	int length = atoi(argv[2]);
	srand (time(NULL));
	ofstream ofs;
	ofs.open(argv[3]);
	for(int i=0;i<length-1;i++)
	{
		rNum = rand() % range + 1; //0~x
		ofs<<rNum<<" ";
	}
	rNum = rand() % range + 1; //0~x
	ofs<<rNum;
	ofs.close();
	return 0;
}
