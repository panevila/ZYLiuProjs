/*
 * CustomShell.cpp
 *
 *  Created on: Mar 6, 2016
 *      Author: panevila
 */

#include "customshell.h"

CCustomShell::CCustomShell() {
	// TODO Auto-generated constructor stub
	ShellPID = getpid();
	RunningMode = CFlag::M_NORMAL;
	HistBufPointer = 0;
	isDispSubs = false;
	OriginOut = dup(STDOUT_FILENO);
	OriginIn = dup(STDIN_FILENO);
	InputRedirect = false;
	OutputRedirect = false;
	debugLevel = 0;
	NumVar = 1;
	ExitCode = 0;
}

CCustomShell::~CCustomShell() {
	// TODO Auto-generated destructor stub
}


void CCustomShell::Run()
{
	int s_status = 0;
	while(1)
	{
		if(RunningMode == CFlag::M_NORMAL)
			printf("xsh >> ");

		else if(RunningMode == CFlag::M_EXIT)
			break;

		//Read Input Commands
		s_status = ReadInput();		//Read from input/repeat/file return 0 if the command are all spaces
		if(s_status== CFlag::S_BLANK) continue;

		nPipedCmds = ParseByPipeCmds();		DebugInfo(1, "ParseByPipeCmds\n");
		//Check if there are splits when splits by "|"

		if(!nPipedCmds) continue;

		if(nPipedCmds == 1)		//If no pipe symbol involved, there are one segment after splitting
		{
			ParseIntoTokens();					DebugInfo(1, "No pipe commands \n");
			s_status = CheckIoRedirection();
			if(s_status == CFlag::S_ERR)
			{
				cout<<"I/O Redirection Syntax Error.\n";
				continue;
			}
			ApplyIoRedirection(CFlag::MODE_INTERNAL);
			s_status = DoInternal();
			UndoIoRedirection();
			if(s_status == CFlag::S_NA)
				DoExternal();					//Try external commands
		}

		else	//There are commands splitted by "|"
		{
			ParseIntoTokens(nPipedCmds-1);			//parse the last segment to check I/O Redirection
			s_status = CheckIoRedirection();
			if(s_status == CFlag::S_ERR)
			{
				cout<<"I/O Redirection Syntax Error.\n";
				continue;
			}
			ApplyIoRedirection(CFlag::MODE_EXTERNAL);
			DoPipe();		//Run each segment splited by "|"
			UndoIoRedirection();
		}	//End of Executions

		//History Records
		if(RunningMode != CFlag::M_REPEAT && InputTokens[0]!="repeat")	//"repeat" will not be recorded
			RecordHistory();

		//Reinitialize
		InputTokens.clear();
		PipeSepInputs.clear();
		UndoIoRedirection();
		InputRedirect = false;
		OutputRedirect = false;
	}
}

void CCustomShell::SetBatchMode(char* path)		//set the running mode to batch mode
{
	RunningMode = CFlag::M_BATCH;
	char realPathBuf[CFlag::SIZE_BUFFER];
	realpath(path,realPathBuf);
	int fd = open(realPathBuf, O_RDONLY);
	if (fd <= 0)
	{
		cout << "Error: Opening file failed" << endl;
		close(fd);
		return;
	}
	close(fd);
	InputFStream.open(realPathBuf);
	if (!InputFStream.is_open())
		cout << "Error: Opening file failed" << endl;
}

//Read initial input line, either from terminal or from file
int CCustomShell::ReadInput()
{
	bool allWhite = true;
	bool commentRemoved = false;

	fflush(stdin);
	//select reading source based on the running mode
	if(RunningMode == CFlag::M_NORMAL)
		getline(cin, InputLnBuf, '\n');

	else if(RunningMode == CFlag::M_BATCH)
	{
		if (!InputFStream.eof())
			getline(InputFStream, InputLnBuf, '\n');

		if (InputFStream.eof())
			RunningMode = CFlag::M_EXIT;
	}

	//If the status is repeat, what we need is aready injected into the buffer
	else if(RunningMode == CFlag::M_REPEAT)
		RunningMode = CFlag::M_NORMAL;

	//Check if the inputs are all blank
	for (unsigned int i = 0; i < InputLnBuf.length(); i++)
	{
		if (InputLnBuf[i] != ' ')
			allWhite = false;

		if (InputLnBuf[i] == '#')
		{
			InputLnBuf.erase(InputLnBuf.begin() + i,InputLnBuf.end());
			commentRemoved = true;
		}

		if (allWhite == false && commentRemoved == true)
		{
			allWhite = false;
			break;
		}
	}

	if (InputLnBuf.length() == 0 || allWhite == true)	//This is an empty input
		return CFlag::S_BLANK;
	else
		return CFlag::S_OK;
}

//parse input line by "|" and return the num of commands.
int CCustomShell::ParseByPipeCmds()
{
	PipeSepInputs.clear();					DebugInfo(1, "Command parsed by '|' \n");
	stringstream ss0(InputLnBuf);
	string tmpstr;
	while (getline(ss0, tmpstr, '|'))
		if(tmpstr.length()>0 && tmpstr != "\\s") PipeSepInputs.push_back(tmpstr);

	return PipeSepInputs.size();
}

//parse a single (unpiped) command into tokens.
int CCustomShell::ParseIntoTokens()
{
	string tmpstr;
	InputTokens.clear();
	int len = PipeSepInputs[0].length();
	for(int i=0;i<len;i++)
	{
		if(PipeSepInputs[0][i] == '\t')
			PipeSepInputs[0][i] = ' ';
	}
	stringstream ss(PipeSepInputs[0]);

	while (getline(ss, tmpstr, ' '))
		if(tmpstr.length()>0 && tmpstr!="//s") InputTokens.push_back(tmpstr);
	VarSubsitution();
	return InputTokens.size();
}

//parse a certain (piped) command into tokens.
int CCustomShell::ParseIntoTokens(int iPipedCmd)
{
	string tmpstr;
	InputTokens.clear();
	int len = PipeSepInputs[0].length();
	for(int i=0;i<len;i++)
	{
		if(PipeSepInputs[0][i] == '\t')
			PipeSepInputs[0][i] = ' ';
	}
	stringstream ss(PipeSepInputs[iPipedCmd]);
	while (getline(ss, tmpstr, ' '))
		if(tmpstr.length()>0 && tmpstr!="\\s+") InputTokens.push_back(tmpstr);
	VarSubsitution();
	return InputTokens.size();
}

//Execute External Files;
int CCustomShell::DoExternal()
{
	string tmpstr;
	bool isBackground = false;
	int nTokens = InputTokens.size();

	//check background
	if(InputTokens.back() == "!")
	{
		isBackground = true;
		InputTokens.pop_back();
	}

	//get the path
	string PathOfSubProc = InputTokens[0];

	//search in all paths, and transform the path to an valid state
	int s_status = SearchExternalPath(PathOfSubProc);
	if(s_status == CFlag::S_ERR)
		return CFlag::S_ERR;

	//assemble a argument vector
	char *args[nTokens+1];
	for(int i1=0;i1<nTokens;i1++)
		args[i1] = strdup(InputTokens[i1].c_str());		//mount arguments
	args[nTokens] = (char*)0;							//mount end;

	//create an environment variable vector;
	int nEvar = EnvVarForChild.size();
	char *envps[nEvar+1];
	for(int i1=0;i1<nEvar;i1++)
		envps[i1]=strdup(EnvVarForChild[i1].c_str());
	envps[nEvar] = (char*)0;							//mount end;

	int s_ret;
	pid_t pid = fork();								//Create a child image

	if (pid > 0)	//parent-only code
	{
		if(isBackground)
			BackgroundPIDs.push_back(pid);
		else
		{
			ForegroundPIDs.push_back(pid);			//record the PID
			waitpid(pid,&s_ret,0);					//Wait for the external process to complete
			ostringstream os;
			os<<s_ret;
			LocalVars["?"] = os.str();				//For variable subsitution
		}
	}

	else if(pid == 0)		//child
	{
		ApplyIoRedirection(CFlag::MODE_EXTERNAL);
		if(execve(PathOfSubProc.c_str(), args, envps)<0)
			cerr<<"[ERROR] External Execution Failed\n";
		exit(0);
	};

	//release the argument buffer
	for(int i1=0;i1<=nTokens;i1++)
		free(args[i1]);
	if(!isBackground)
		ForegroundPIDs.pop_back();		//Remove pid from list

	isBackground = false;
	return CFlag::S_OK;
}

int CCustomShell::DoPipe()
{
	int s_status;
	string tmpstr;
	string resultBuf;		//Buffer for the intermediate results
	string bufstr;
	int cmdPipe[2];			//Pipe used for sending command
	resultBuf.clear();
	bool isBackground = false;

	//Check if it is a background execution
	InputTokens.clear();
	int nTokens = ParseIntoTokens(nPipedCmds-1);
	string PathOfSubProc = InputTokens[0];
	if(InputTokens.back() == "!")
	{
		isBackground = true;
		InputTokens.pop_back();
	}

	//Create environment variable array;
	int nEvar = EnvVarForChild.size();
	char *envps[nEvar+1];
	for(int i1=0;i1<nEvar;i1++)
		envps[i1]=strdup(EnvVarForChild[i1].c_str());
	envps[nEvar] = (char*)0;							//mount end;

	int passdInputPipe;

	//Process the first n-1 command and pass their output to next input
	for(int i0=0;i0<nPipedCmds-1;i0++)
	{
		InputTokens.clear();
		int nTokens = ParseIntoTokens(i0);				//split a segment of command(between "|"s) to tokens
		string PathOfSubProc = InputTokens[0];
		s_status = SearchExternalPath(PathOfSubProc);
		if(s_status == CFlag::S_ERR) break;

		//argument vector for executing
		char *args[nTokens+1];
		for(int i1=0;i1<nTokens;i1++)
			args[i1] = strdup(InputTokens[i1].c_str());
		args[nTokens] = (char*)0;

		pipe(cmdPipe);

		pid_t pid = fork ();

		//record the PID
		if(isBackground)
			BackgroundPIDs.push_back(pid);
		else
			ForegroundPIDs.push_back(pid);

		if (pid == 0)
		{
			dup2(passdInputPipe, STDIN_FILENO);
			dup2(cmdPipe[1], STDOUT_FILENO);
			execve(PathOfSubProc.c_str(), args, envps);
			cout<<"Exec Error while handling pipes\n";
			exit(0);
		}
		close (cmdPipe [1]);
		passdInputPipe = cmdPipe [0];
		if(!isBackground)
			waitpid(pid,NULL,0);

		if(!isBackground)
			ForegroundPIDs.push_back(pid);

		for(int i1=0;i1<=nTokens;i1++)
			free(args[i1]);
	}

	if (passdInputPipe != 0)
		dup2 (passdInputPipe, 0);

	pid_t pidlast;
	pidlast = fork();

	if(isBackground)
		BackgroundPIDs.push_back(pidlast);
	else
		ForegroundPIDs.push_back(pidlast);

	if(pidlast == 0)
	{
		//split a segment of command(between "|"s) to tokens
		InputTokens.clear();
		int nTokens = ParseIntoTokens(nPipedCmds-1);
		string PathOfSubProc = InputTokens[0];

		//Check the path
		s_status = SearchExternalPath(PathOfSubProc);
		if(s_status == CFlag::S_ERR)
			return CFlag::S_ERR;

		char *args[nTokens+1];							//argument vector for executing
		for(int i1=0;i1<nTokens;i1++)
			args[i1] = strdup(InputTokens[i1].c_str());
		args[nTokens] = (char*)0;

		execve (PathOfSubProc.c_str(), args, envps);
		cout<<"Exec Error while handling pipes\n";
		exit(0);
	}

	if(!isBackground)
		waitpid(pidlast, NULL, 0);

	//record the PID
	if(!isBackground)
		ForegroundPIDs.pop_back();

	dup2 (OriginIn, STDIN_FILENO);
	dup2 (OriginOut, STDOUT_FILENO);
	close (cmdPipe [0]);
	close (cmdPipe [1]);

	return CFlag::S_OK;
}

//read the input tokens and check if redirections are needed
int CCustomShell::CheckIoRedirection()
{
	int iErase[2];
	int nTokens = InputTokens.size();
	for(int i0=0;i0<nTokens;i0++)
	{
		if(InputTokens[i0][0] == '<')
		{
			if(i0 != nTokens-1)
			{
				InputRedirect = true;
				RedirectPathIn = InputTokens[i0+1];
				iErase[0]=i0;
			}
			else
			{
				cout<<"Syntax: cmd W1 W2.. < F1 > F2 \n";
				return CFlag::S_ERR;
			}
		}
		if(InputTokens[i0][0] == '>')
		{
			if(i0 != nTokens-1)
			{
				OutputRedirect = true;
				RedirectPathOut = InputTokens[i0+1];
				iErase[1]=i0;
			}
			else
			{
				cout<<"Syntax: cmd W1 W2.. < F1 > F2 \n";
				return CFlag::S_ERR;
			}
		}
	}

	//remove the io direction related commands
	if(InputRedirect)
	{
		InputTokens.erase(InputTokens.begin()+iErase[0]);
		InputTokens.erase(InputTokens.begin()+iErase[0]+1);
	}
	if(OutputRedirect)
	{
		InputTokens.erase(InputTokens.begin()+iErase[1]);
		InputTokens.erase(InputTokens.begin()+iErase[1]+1);
	}

	return CFlag::S_OK;
}

//Apply redirections based on stored informations
int CCustomShell::ApplyIoRedirection(int mode)
{

	if(InputRedirect)
	{
		if(mode != CFlag::MODE_INTERNAL)		//not supported for internal commands
		{
			int fdIn = open(RedirectPathIn.c_str(), O_RDONLY);
			dup2(fdIn, STDIN_FILENO);
			close(fdIn);
		}
	}

	if(OutputRedirect)
	{
		int fdOut = creat(RedirectPathOut.c_str(), 0644) ;
		dup2(fdOut, STDOUT_FILENO);
		close(fdOut);
	}

	return CFlag::S_OK;
}

//restore all redirections to stdios
int CCustomShell::UndoIoRedirection()
{
	if (InputRedirect)
		dup2(OriginIn, STDIN_FILENO);

	if (OutputRedirect)
		dup2(OriginOut, STDOUT_FILENO);

	fflush(stdin);
	fflush(stdout);

	return CFlag::S_OK;
}

//Function used to record history.
void CCustomShell::RecordHistory()
{
	if (HistoryList.size() < CFlag::NUM_HIST)
		HistoryList.push_back(InputLnBuf);
	else
		HistoryList[HistBufPointer] = InputLnBuf;

	if (HistBufPointer == CFlag::NUM_HIST - 1)
		HistBufPointer = 0;
	else
		HistBufPointer++;
}

//Internal Commands:
int CCustomShell::DoInternal()
{
	if(InputTokens[0] == "")
		return CFlag::S_ERR;

	else if(InputTokens[0] == "echo") return cmd_echo();
	else if(InputTokens[0] == "show") return cmd_show();
	else if(InputTokens[0] == "exit") return cmd_exit();
	else if(InputTokens[0] == "clr") return cmd_clr();
	else if(InputTokens[0] == "environ") return cmd_environ();
	else if(InputTokens[0] == "export") return cmd_export();
	else if(InputTokens[0] == "unexport") return cmd_unexport();
	else if(InputTokens[0] == "set") return cmd_set();
	else if(InputTokens[0] == "unset") return cmd_unset();
	else if(InputTokens[0] == "dir") return cmd_dir();
	else if(InputTokens[0] == "chdir") return cmd_chdir();
	else if(InputTokens[0] == "history") return cmd_history();
	else if(InputTokens[0] == "repeat") return cmd_repeat();
	else if(InputTokens[0] == "kill") return cmd_kill();
	else if(InputTokens[0] == "pause") return cmd_pause();
	else if(InputTokens[0] == "test") return cmd_test();
	else if(InputTokens[0] == "help") return cmd_help();
	else return CFlag::S_NA;
}

int CCustomShell::cmd_echo()
{
	if(InputTokens.size()<2)
		cout<<"Syntax: echo W1 W2...\n";

	for (unsigned int i = 1; i < InputTokens.size(); i++)
	{
		if (i>1) printf(" ");
		printf("%s", InputTokens[i].c_str());
	}
	printf("\n");
	return CFlag::S_OK;
}

int CCustomShell::cmd_show()
{
	if(InputTokens.size()<2)
		cout<<"Syntax: show W1 W2...\n";

	for (unsigned int i = 1; i < InputTokens.size(); i++)
		printf("%s\n", InputTokens[i].c_str());
	return CFlag::S_OK;
}

int CCustomShell::cmd_exit()
{
	if(InputTokens.size()>2)
		cout<<"Syntax: exit [I]\n";

	if(InputTokens.size() == 2)
	{
		istringstream is(InputTokens[2]);
		is >> ExitCode;
	}

	else
		ExitCode = 0;

	RunningMode = CFlag::M_EXIT;
	return CFlag::S_OK;
}

int CCustomShell::cmd_clr()
{
	cout << "\033[2J\033[0;0H";
	return CFlag::S_OK;
}

int CCustomShell::cmd_environ()
{
	for (map<string,string>::iterator it = EnviroVars.begin(); it != EnviroVars.end(); it++)
		cout << it->first << "=" << it->second << "\n";
	return CFlag::S_OK;
}

int CCustomShell::cmd_export()
{
	if(InputTokens.size()!=3)
	{
		cout<<"Syntax: export W1 W2\n";
		return CFlag::S_ERR;
	}
	setenv(InputTokens[1].c_str(), InputTokens[2].c_str(), true);
	EnviroVars[InputTokens[1]] = InputTokens[2];
	string tmpstr = InputTokens[1] + "=" + InputTokens[2];
	EnvVarForChild.push_back(tmpstr);
	return CFlag::S_OK;
}

int CCustomShell::cmd_unexport()
{
	if(InputTokens.size() != 2)
	{
		cout<<"Syntax: environ W\n";
		return CFlag::S_ERR;
	}
	if(getenv(InputTokens[1].c_str()) == NULL)
	{
		cout<<"Could not find global variable \""<<InputTokens[1]<<"\"\n";
		return CFlag::S_ERR;
	}
	unsetenv(InputTokens[1].c_str());
	string tmpstr = InputTokens[1] + "=" + EnviroVars[InputTokens[1]];
	vector<string>::iterator it = std::find(EnvVarForChild.begin(), EnvVarForChild.end(), tmpstr);
	EnviroVars.erase(InputTokens[1]);
	if(it != EnvVarForChild.end())
		EnvVarForChild.erase(it);
	return CFlag::S_OK;
}

int CCustomShell::cmd_set()
{
	if(InputTokens.size()!=3)
	{
		cout<<"Syntax: environ W1 W2\n";
		return CFlag::S_ERR;
	}
	LocalVars[InputTokens[1]] = InputTokens[2];
	return CFlag::S_OK;
}

int CCustomShell::cmd_unset()
{
	if(InputTokens.size() != 2)
	{
		cout<<"Syntax: environ W\n";
		return CFlag::S_ERR;
	}
	if(LocalVars.find(InputTokens[1]) == LocalVars.end())
	{
		cout<<"Could not find local variable \""<<InputTokens[1]<<"\"\n";
		return CFlag::S_ERR;
	}
	LocalVars.erase(InputTokens[1]);
	return CFlag::S_OK;
}

int CCustomShell::cmd_dir()
{
	DIR* dp;
	struct dirent* ep;
	dp = opendir("./");
	if (dp != NULL)
	{
		while ((ep = readdir (dp))!=NULL)
			cout<< ep->d_name <<endl;
		closedir (dp);
	}
	else
	{
		cout<< "Display directory error\n";
		return CFlag::S_ERR;
	}
	return CFlag::S_OK;
}

int CCustomShell::cmd_chdir()
{
	if(InputTokens.size() != 2)
	{
		cout<<"Syntax: chdir W\n";
		return CFlag::S_ERR;
	}
	chdir(InputTokens[1].c_str());
	return CFlag::S_OK;
}

int CCustomShell::cmd_history()
{
	if(InputTokens.size() > 2)
	{
		cout<<"Syntax: history [n]\n";
		return CFlag::S_ERR;
	}
	int iBuf = 0;
	int nToPrint = 0;
	int histSize = HistoryList.size();
	if(histSize < CFlag::NUM_HIST)
	{
		if(InputTokens.size() == 1)
		{
			nToPrint = histSize;
			iBuf = 0;
		}
		else
		{
			istringstream is(InputTokens[1]);
			is >> nToPrint;
			if(nToPrint>histSize)
				nToPrint = histSize;
			iBuf = HistBufPointer - nToPrint;
		}
	}
	else
	{
		if(InputTokens.size() == 1)
		{
			nToPrint = histSize;
			iBuf = HistBufPointer;
		}
		else
		{
			istringstream is(InputTokens[1]);
			is >> nToPrint;
			if(nToPrint>histSize)
				nToPrint = histSize;
			iBuf = HistBufPointer - nToPrint;
			if(iBuf<0)
				iBuf += histSize;
		}
	}

	for (int i = 0; i < nToPrint; i++)
	{
		cout << i + 1 << "  " << HistoryList[iBuf] << endl;
		if (iBuf == CFlag::NUM_HIST - 1) iBuf = 0;
		else iBuf++;
	}
	return CFlag::S_OK;
}

int CCustomShell::cmd_repeat()
{
	if(InputTokens.size() > 2)
	{
		cout<<"Syntax: repeat [n]\n";
		return CFlag::S_ERR;
	}
	int iBuf = HistBufPointer - 1;
	int histSize = HistoryList.size();

	if(histSize < CFlag::NUM_HIST)
		iBuf = 0;
	else
		iBuf = HistBufPointer;

	for (int i = 0; i < histSize; i++)
	{
		cout << i + 1 << "  " << HistoryList[iBuf] << endl;
		if (iBuf == CFlag::NUM_HIST - 1) iBuf = 0;
		else iBuf++;
	}

	int nBackward;
	int iTarget;
	if(InputTokens.size() == 1)
		iTarget = HistBufPointer - 1;
	else
	{
		istringstream is(InputTokens[1]);
		is >> nBackward;
		if(nBackward > histSize)
		{
			cout<<"The argument is too large.\n";
			return CFlag::S_ERR;
		}
		iTarget = HistBufPointer - nBackward;
		if(iTarget<0) iTarget += CFlag::NUM_HIST;
	}

	InputLnBuf = HistoryList[iTarget];
	RunningMode = CFlag::M_REPEAT;
	return CFlag::S_OK;
}

int CCustomShell::cmd_kill()
{
	int pid = 0;
	int signal = 0;
	int ret;
	if(InputTokens.size() == 2)
	{
		istringstream is(InputTokens[1]);
		is >> pid;

		vector<pid_t>::iterator it = std::find(ForegroundPIDs.begin(), ForegroundPIDs.end(), pid);
		if(it != ForegroundPIDs.end())
			ForegroundPIDs.erase(it);
		else
		{
			it  = std::find(ForegroundPIDs.begin(), BackgroundPIDs.end(), pid);
			if(it != ForegroundPIDs.end())
				BackgroundPIDs.erase(it);
		}

		ret = kill(pid, SIGKILL);
	}
	else if(InputTokens.size() == 3)
	{
		istringstream is(InputTokens[1]);
		is >> signal;
		istringstream is2(InputTokens[2]);
		is2 >> pid;

		vector<pid_t>::iterator it = std::find(ForegroundPIDs.begin(), ForegroundPIDs.end(), pid);
		if(it != ForegroundPIDs.end())
			ForegroundPIDs.erase(it);
		else
		{
			it  = std::find(ForegroundPIDs.begin(), BackgroundPIDs.end(), pid);
			if(it != ForegroundPIDs.end())
				BackgroundPIDs.erase(it);
		}

		ret = kill(pid, signal);
	}
	if(ret < 0)
	{
		cout<<"Error While Killing\n";
		return CFlag::S_ERR;
	}
	return CFlag::S_OK;
}

int CCustomShell::cmd_wait()
{
	pid_t pid = 0;
	int s_ret;
	if(InputTokens.size() != 2)
	{
		cout<<"Syntax: wait I\n";
		return CFlag::S_ERR;
	}
	istringstream is(InputTokens[1]);
	is>>pid;
	if(pid == -1)
		waitpid(0,&s_ret,0);
	else
		waitpid(pid,&s_ret,0);
	return CFlag::S_OK;
}

int CCustomShell::cmd_pause()
{
	cout<<"Shell Paused. Press \"Enter\" to Continue.\n";
	getchar();
	return CFlag::S_OK;
}

int CCustomShell::cmd_help()
{
	string path = "readme";
	if(SearchExternalPath(path) == CFlag::S_ERR)
	{
		cout << "Couldn't find the help document"<< endl;
		return CFlag::S_ERR;
	}

	InputLnBuf = "more " + path;
	RunningMode = CFlag::M_REPEAT;
	return CFlag::S_OK;
}


void CCustomShell::ClassSigHandler(int sig)
{
    for(unsigned int i0 = 0; i0<ForegroundPIDs.size();i0++)
    {
    	cout<<"\nKilling Process: PID: "<<ForegroundPIDs[i0]<<endl;
        int ret = kill(ForegroundPIDs[i0], sig);
        if(ret>=0)
        	cout<<"Killing Successful.\n";
    }
}

//variable subsitution
void CCustomShell::VarSubsitution()
{
	string tmpstr;
	ostringstream ssShellPID;
	ostringstream ssFGVal;
	ostringstream ssBGPid;
	string sub;
	string tmpsubstr;
	bool subsituted = false;
	ssShellPID << getpid();
	int len;

	for (unsigned int i0 = 0; i0 < InputTokens.size(); i0++)
	{
		if(InputTokens[i0].length()>=2 && InputTokens[i0][0] == '$')
		{
			if(InputTokens[i0] == "$$")
			{
				InputTokens[i0] = ssShellPID.str();
				subsituted = true;
			}

			else if(InputTokens[i0] == "$?")
			{
				if(LocalVars.find("?") == LocalVars.end())
				{
					cout<<"Could not find local variable \"?\"\n";
					continue;
				}
				InputTokens[i0] = LocalVars["?"];
				subsituted = true;
			}
			else if(InputTokens[i0] == "$!")
			{
				if(BackgroundPIDs.size()>0)
				{
					ssFGVal << BackgroundPIDs.back();
					InputTokens[i0] = ssFGVal.str();
					subsituted = true;
				}
				else
					InputTokens[i0] = "0";
			}
			else		//$XY Case
			{
				len = InputTokens[i0].length();
				for(int j=1;j<len;j++)
				{
					sub = InputTokens[i0].substr(1,j);	//last pos j;
					if(EnviroVars.find(sub) != EnviroVars.end())
					{
						if(j==len-1)
							InputTokens[i0] = EnviroVars[InputTokens[i0].substr(1)];	//subsitute the value
						else
						{
							tmpsubstr = InputTokens[i0].substr(j-1,len-j+1);
							InputTokens[i0] = EnviroVars[sub] + tmpsubstr;
						}
						subsituted = true;
						break;
					}
					else if(LocalVars.find(sub) != LocalVars.end())
					{
						if(j==len-1)
							InputTokens[i0] = LocalVars[InputTokens[i0].substr(1)];	//subsitute the value
						else
						{
							tmpsubstr = InputTokens[i0].substr(j-1,len-j+1);
							InputTokens[i0] = LocalVars[sub] + tmpsubstr;
						}
						subsituted = true;
						break;
					}
					else if(j==len-1)
					{
						cout<<"Subsitution Ignored. Could not find variable \""<<InputTokens[i0].substr(1)<<"\"\n";
						continue;
					}
				}
			}
		}
	}
	if(isDispSubs == true)
	{
		if(subsituted == true)
		{
			for(unsigned int i0=0;i0<InputTokens.size();i0++)
				cout<< "Subsitution: "<< InputTokens[i0] <<" ";

			cout<< "\n";
		}
	}
}

//get the path of the xsh
void CCustomShell::GetExecutablePath(char *path)
{
	char buf[CFlag::SIZE_BUFFER];
	realpath(path,buf);	//Get the real path
	ExePath.assign(buf);
}

//get environment variable and store
void CCustomShell::GetEnviroVars(char *envp[])
{
	string tmpstr;
	size_t pos;
	int len;
	EnvVarForChild.clear();
	tmpstr = "parent="+ExePath;
	EnvVarForChild.push_back(tmpstr);
	for(int i=0;envp[i]!=NULL;i++)
	{
		tmpstr.assign(envp[i]);
		pos = tmpstr.find_first_of('=');
		len = tmpstr.length();
		EnviroVars[tmpstr.substr(0,pos)] = tmpstr.substr(pos+1, len-pos-1);
		EnvVarForChild.push_back(tmpstr);
	}

}

void CCustomShell::SetDispSubs(bool isDisp)
{
	isDispSubs = isDisp;
}

void CCustomShell::DebugInfo (int level, string str)
{
	if (debugLevel == 0)
		return;

	if (debugLevel >= level)
		cout << "Start to debug: " << str << endl;
}

void CCustomShell::SetDebugLevel (const char* str)
{
	debugLevel = atoi(str);
}

void CCustomShell::PushVariable(char* value)
{
	ostringstream numss;
	string valuestr(value);
	numss << NumVar;
	LocalVars[numss.str().c_str()] = valuestr;
}

int CCustomShell::SearchExternalPath(string &path)
{
	//Check the path format
	int inputlen = path.length();
	if(inputlen == 0)
	{
		cout<<"Path Error.\n";
		return CFlag::S_ERR;
	}

	if(path[inputlen-1] == '/')
	{
		cout<< path <<" Is a Directory.\n";
		return CFlag::S_ERR;
	}

	int fd;

	//Check if the origin path works
	fd = open(path.c_str(), O_RDONLY);
	if(fd>0)
	{
		close(fd);
		return CFlag::S_OK;
	}
	close(fd);

	//Check if its absolute path works
	string tmpstr;
	string tmpstr2;
	char real[CFlag::SIZE_BUFFER];
	realpath(path.c_str(),real);
	fd = open(real, O_RDONLY);
	if(fd>0)
	{
		close(fd);
		path.assign(real);
		return CFlag::S_OK;
	}
	close(fd);

	//If we reach here and it's an absolute path, try treat it as relative
	if(path[0] == '/')
		path.erase(0,1);

	//String for storing the entire "PATH" variable
	string PATHValue = EnviroVars["PATH"];
	string envpath;
	string combinedPath;

	//Search the command in the PATH list.
	int envlen = 0;
	stringstream ss1(PATHValue);
	while (getline(ss1, envpath, ' '))	// <--split by ' '. (Requirement)
	{
		envlen = envpath.length();
		if(envlen == 0) continue;		//if there are empty paths, ignore them

		char real1[CFlag::SIZE_BUFFER];			//Check if its absolute path works
		realpath(envpath.c_str(),real1);
		envpath.assign(real1);

		if(envpath[envlen-1] != '/')	//add the "/" into the back of the directory
			envpath.append("/");

		combinedPath = envpath + path;

		fd = open(combinedPath.c_str(), O_RDONLY);
		if(fd>0)
		{
			close(fd);
			path.assign(combinedPath);
			return CFlag::S_OK;
		}
		close(fd);
	}
	//In places seen, PATHs are usually split by ":".
	//Redo for it in case we misunderstood the requirements
	stringstream ss2(PATHValue);
	while (getline(ss2, envpath, ':'))
	{
		envlen = envpath.length();
		if(envlen == 0) continue;

		char real1[CFlag::SIZE_BUFFER];			//Check if its absolute path works
		realpath(envpath.c_str(),real1);
		envpath.assign(real1);

		if(envpath[envlen-1] != '/')	//add the "/" into the back of the directory
			envpath.append("/");

		combinedPath = envpath + path;

		fd = open(combinedPath.c_str(), O_RDONLY);
		if(fd>0)
		{
			close(fd);
			path.assign(combinedPath);
			return CFlag::S_OK;
		}
		close(fd);
	}

	//In the test script, PATHs are usually split by ";".
	//Redo for it in case we misunderstood the requirements
	stringstream ss3(PATHValue);
	while (getline(ss3, envpath, ';'))
	{
		envlen = envpath.length();
		if(envlen == 0) continue;

		char real1[CFlag::SIZE_BUFFER];			//Check if its absolute path works
		realpath(envpath.c_str(),real1);
		envpath.assign(real1);

		if(envpath[envlen-1] != '/')	//add the "/" into the back of the directory
			envpath.append("/");

		combinedPath = envpath + path;

		fd = open(combinedPath.c_str(), O_RDONLY);
		if(fd>0)
		{
			close(fd);
			path.assign(combinedPath);
			return CFlag::S_OK;
		}
		close(fd);
	}

	//====================For second time ,only single name==============

	string name;
	size_t pos = path.find_last_of('/');
	if(pos != string::npos && pos<path.length()-1)
		name = path.substr(pos+1, path.length()-pos);
	else
	{
		cout<< "\""<< path <<"\" cannot found command or directory: .\n";
		return CFlag::S_ERR;
	}

	inputlen = name.length();		//Check the path format

	fd = open(path.c_str(), O_RDONLY);		//Check if there are target by read it
	if(fd>0)
	{
		cout<< "Didn't find \""<< path <<"\" but found another place for \""<< name <<"\", replaced .\n";
		close(fd);
		return CFlag::S_OK;
	}
	close(fd);

	PATHValue = EnviroVars["PATH"];		//String for storing the entire "PATH" variable

	//Search the command in the PATH list.
	envlen = 0;
	stringstream ss4(PATHValue);
	while (getline(ss4, envpath, ' '))	// <--split by ' '. (Requirement)
	{
		envlen = envpath.length();
		if(envlen == 0) continue;		//if there are empty paths, ignore them

		char real1[CFlag::SIZE_BUFFER];			//Check if its absolute path works
		realpath(envpath.c_str(),real1);
		envpath.assign(real1);

		if(envpath[envlen-1] != '/')	//add the "/" into the back of the directory
			envpath.append("/");

		combinedPath = envpath + name;

		fd = open(combinedPath.c_str(), O_RDONLY);
		if(fd>0)
		{
			close(fd);
			cout<< "Didn't find \"/"<< path <<"\" but found another place \""<< combinedPath <<"\", replaced .\n\n";
			path.assign(combinedPath);
			return CFlag::S_OK;
		}
		close(fd);
	}
	//In places seen, PATHs are usually split by ":".
	//Redo for it in case we misunderstood the requirements
	stringstream ss5(PATHValue);
	while (getline(ss5, envpath, ':'))
	{
		envlen = envpath.length();
		if(envlen == 0) continue;

		char real1[CFlag::SIZE_BUFFER];			//Check if its absolute path works
		realpath(envpath.c_str(),real1);
		envpath.assign(real1);

		if(envpath[envlen-1] != '/')	//add the "/" into the back of the directory
			envpath.append("/");

		combinedPath = envpath + name;

		fd = open(combinedPath.c_str(), O_RDONLY);
		if(fd>0)
		{
			close(fd);
			cout<< "Didn't find \"/"<< path <<"\" but found another place \""<< combinedPath <<"\", replaced .\n\n";
			path.assign(combinedPath);
			return CFlag::S_OK;
		}
		close(fd);
	}

	//In the test script, PATHs are usually split by ";".
	//Redo for it in case we misunderstood the requirements
	stringstream ss6(PATHValue);
	while (getline(ss6, envpath, ';'))
	{
		envlen = envpath.length();
		if(envlen == 0) continue;

		char real1[CFlag::SIZE_BUFFER];			//Check if its absolute path works
		realpath(envpath.c_str(),real1);
		envpath.assign(real1);

		if(envpath[envlen-1] != '/')	//add the "/" into the back of the directory
			envpath.append("/");

		combinedPath = envpath + name;

		fd = open(combinedPath.c_str(), O_RDONLY);
		if(fd>0)
		{
			close(fd);
			cout<< "Didn't find \"/"<< path <<"\" but found another place \""<< combinedPath <<"\", replaced .\n\n";
			path.assign(combinedPath);
			return CFlag::S_OK;
		}
		close(fd);
	}

	cout<< "\""<< path <<"\" cannot found command or directory: .\n";
	return CFlag::S_ERR;
}

int CCustomShell::cmd_test()
{
	return 0;
}


