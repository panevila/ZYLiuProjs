
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <iterator>
#include <fstream>
#include <map>
#include <algorithm>

using namespace std;

vector<int> memoryFrames;
vector<int> pageReferences;
unsigned int nFrames = 0;
int f_algorithm = -1;
string filepath = "";

int nFaults = 0;

void printframe(int last_in)
{
	unsigned int len = memoryFrames.size();
	printf("%2d: [", last_in);
	for (unsigned int i = 0; i < nFrames; i++)
	{
		if (i < len)
			printf("%2d", memoryFrames[i]);
		else
			printf("  ");

		if (i < nFrames - 1)
			printf("|");
		else
			printf("]\n");
	}
}

void simulate_opt()
{
	// The find is not the most efficient way 
	// but since there are at most 100 slots, the impact won't be great
	unsigned int len = pageReferences.size();
	int tmpNum;
	int pRep = 0;
	int timeToNext = 0;
	int timestamp = 1;
	vector<int> nextRef(nFrames, 0);
	vector<int>::iterator iter = memoryFrames.begin();
	vector<int>::iterator iterNextRef = memoryFrames.begin();
	for (unsigned int i = 0; i < len; i++)
	{
		tmpNum = pageReferences[i];
		iter = find(memoryFrames.begin(), memoryFrames.end(), tmpNum); //check if exist
		if (iter == memoryFrames.end())					//if not exist
		{
			if (memoryFrames.size() < nFrames)			//if still have slot
				memoryFrames.push_back(tmpNum);
			else										//if replacement needed
			{
				nFaults++;
				for (unsigned int j = 0; j < nFrames; j++)		//for each frame, check the next occurance
				{
					iterNextRef = find(pageReferences.begin() + i, pageReferences.end(), memoryFrames[j]);
					//by the first time not found, select this one to replace
					if (iterNextRef == pageReferences.end())      
					{
						pRep = j;
						break;
					}
					else
						nextRef[j] = iterNextRef - pageReferences.begin() + i;
				}
				pRep = max_element(nextRef.begin(), nextRef.end()) - nextRef.begin();  //find the pos of max
				memoryFrames[pRep] = tmpNum;
			}
		}
		printframe(tmpNum);   //display the frame status
	}
}

void simulate_lru()
{
	// The find is not the most efficient way 
	// but since there are at most 100 slots, the impact won't be great
	unsigned int len = pageReferences.size();
	int tmpNum;
	int pRep = 0;
	int timestamp = 1;
	vector<int> lastVisitTime;
	vector< int >::iterator iter = memoryFrames.begin();
	for (unsigned int i = 0; i < len; i++)
	{
		tmpNum = pageReferences[i];
		iter = find(memoryFrames.begin(), memoryFrames.end(), tmpNum);
		if (iter == memoryFrames.end())
		{
			if (memoryFrames.size() < nFrames)
			{
				memoryFrames.push_back(tmpNum);
				lastVisitTime.push_back(timestamp);
			}
			else
			{
				nFaults++;
				//find the pos of min
				pRep = min_element(lastVisitTime.begin(), lastVisitTime.end()) - lastVisitTime.begin();  
				memoryFrames[pRep] = tmpNum;
				lastVisitTime[pRep] = timestamp;
			}
		}
		else
			lastVisitTime[iter - memoryFrames.begin()] = timestamp;

		printframe(tmpNum);	//display the frame status
		timestamp++;

		//prevent the value to grow without limit
		if (timestamp > 100000000)
		{
			timestamp -= 100000000;
			for (unsigned int j = 0; j < nFrames; j++)
				lastVisitTime[j] -= 100000000;
		}
	}
}

void simulate_fifo()
{
	unsigned int len = pageReferences.size();
	int tmpNum;
	int pRep=0;
	nFaults = 0;
	for (unsigned int i = 0; i < len; i++)
	{
		tmpNum = pageReferences[i];
		if (find(memoryFrames.begin(), memoryFrames.end(), tmpNum) == memoryFrames.end())
		{
			if (memoryFrames.size() < nFrames)
				memoryFrames.push_back(tmpNum);
			else
			{
				nFaults++;
				memoryFrames[pRep] = tmpNum;

				//circular pointer
				if (pRep == nFrames - 1)
					pRep = 0;
				else
					pRep++;
			}
		}
		printframe(tmpNum);		//display the frame status
	}
}

int main(int argc, char *argv[], char *envp[])
{
	if (argc != 4)
	{
		cout << "Syntax: vmsim [number of frames] [page ref sequence] [algorithm]\n";
		return 0;
	}
	string tmpstr;
	tmpstr.assign(argv[1]);
	nFrames = atoi(tmpstr.c_str());

	filepath.assign(argv[2]);

	//Read file
	ifstream ifs;
	ifs.open(filepath.c_str());
	if (!ifs.is_open())
	{
		cout << "Cannot open page references file" << endl;
		return 0;
	}
	while (!ifs.eof())
	{
		getline(ifs, tmpstr, ' ');
		if(tmpstr==" ") continue;
		pageReferences.push_back(atoi(tmpstr.c_str()));
	}


	tmpstr.assign(argv[3]);
	if (tmpstr == "OPT" || tmpstr == "opt")
		simulate_opt();
	else if (tmpstr == "LRU" || tmpstr == "lru")
		simulate_lru();
	else if (tmpstr == "FIFO" || tmpstr == "fifo")
		simulate_fifo();
	else
	{
		cout << "Syntax: vmsim [number of frames] [page ref sequence] [algorithm]\n";
		cout << "Invalid algorithm name.\nPossible names are: \"OPT\",\"LRU\",\"FIFO\",\"opt\",\"lru\",\"fifo\"";
		return 0;
	}
	int nRef = pageReferences.size();
	float missrate = 100 * (float)nFaults / (float)nRef;

	printf("Miss rate = %d / %d = %.2f%%\n", nFaults, nRef, missrate); //print the result

	return 0;
}
