/*
 * Definition.h
 *
 *  Created on: Mar 6, 2016
 *      Author: panevila
 */

#ifndef DEFINITION_H_
#define DEFINITION_H_

#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
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

class CFlag {
public:
	CFlag();
	virtual ~CFlag();
	static const int M_EXIT = 0;
	static const int M_NORMAL = 1;
	static const int M_REPEAT = 2;
	static const int M_BATCH = 3;
	static const int S_OK = 0;
	static const int S_ERR = 1;
	static const int S_BLANK = -1;
	static const int S_NA = -1;
	static const int NUM_HIST = 100;
	static const int MODE_INTERNAL = 0;
	static const int MODE_EXTERNAL = 1;
	static const unsigned int SIZE_BUFFER = 500;
};
using namespace std;

#endif /* DEFINITION_H_ */
