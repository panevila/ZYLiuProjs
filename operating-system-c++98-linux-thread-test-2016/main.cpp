/*
 * main.cpp
 *
 *  Created on: Apr 5, 2016
 *      Author: panevila
 */
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
//#include <fcntl.h>
//#include <dirent.h>
//#include <stdio.h>
//#include <string.h>
#include <stdlib.h>
#include <iostream>
//#include <string>
#include <vector>
//#include <sstream>
//#include <iterator>
//#include <fstream>
//#include <map>
//#include <algorithm>
#include <semaphore.h>
#include <signal.h>
#include <time.h>
#include <assert.h>
#include <errno.h>


using namespace std;

vector<sem_t> items;
sem_t rooms = {0};
vector<bool> IsEating;
int num_phil;
int num_unfinished;
sem_t count_mute = {1};


int i;

void printstatus()
{
	for(int i=0;i<num_phil;i++)
	{
		if(IsEating[i]==true)
			cout<<"*";
		else
			cout<<" ";
	}
	cout<<"\n";
}
void ThinkForSomeTime()
{
	int randtime = rand() % 900001 + 100000; //1~10 sec
	usleep(randtime);
}
void EatForSomeTime()
{
	int randtime = rand() % 200001 + 100000; //1~3 sec
	usleep(randtime);
}
void philosopher(int i)
{
	for(int j=0;;j++)
	{
		if(j==5)
		{
			sem_wait(&count_mute);
			num_unfinished--;
			sem_post(&count_mute);
		}
		if(num_unfinished<1)
			break;
		//start thinking
		IsEating[i] = false;
		printstatus();
		//think for some time
		ThinkForSomeTime();
		//want to eat
		if(num_phil>1)
		{
			sem_wait(&rooms);
			sem_wait(&(items[i]));
			sem_wait(&items[(i+1) % num_phil]);
		}
		//start eating
		IsEating[i] = true;
		printstatus();
		//eat for some time
		EatForSomeTime();
		if(num_phil>1)
		{
			sem_post(&items[(i+1) % num_phil]);
			sem_post(&(items[i]));
			sem_post(&rooms);
		}
	}
}

void *thr_entry(void *arg)
{
	int b = *(int*)arg;
	philosopher(b);
	IsEating[b] = false;
	return((void *)0);
}

int main(){
	vector<pthread_t> tids;
	vector<unsigned int> args;

	cout<<"Please enter the number of philosophers(1~15): \n";
	cin>>num_phil;
	while(num_phil<1||num_phil>15)
	{
		cout<<"Please enter a number of between 1~15...: \n";
		cin>>num_phil;
	}

	cout<<"Eating Activity \n";
	char tmpchr;
	num_unfinished = num_phil;
	if(num_phil!=1)
		tmpchr = num_phil-1;
	else
		tmpchr = 1;

	sem_t initsem = {1};

	sem_t initrooms = {tmpchr};
	rooms = initrooms;

	if(num_phil>10)
	{
		cout<<"          ";
		for(int i=0;i<num_phil-10;i++)
			cout<<"1";
		cout<<"\n";
	}
	for(int i=0;i<num_phil;i++)
	{
		tids.push_back(0);
		args.push_back(i);
		items.push_back(initsem);
		IsEating.push_back(false);
		cout<<(i+1)%10;
	}
	cout<<"\n";

	srand (time(NULL));

	for(int i=0;i<num_phil;i++)
		pthread_create(&tids[i], 0, thr_entry, &args[i]);

	for(int i=0;i<num_phil;i++)
		pthread_join(tids[i], NULL);

	cout<<"Exiting...\n"<<endl;
	return 0;
}



