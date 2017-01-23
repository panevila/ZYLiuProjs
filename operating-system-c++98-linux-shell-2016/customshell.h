/*
 * CustomShell.h
 *
 *  Created on: Mar 6, 2016
 *      Author: panevila
 */


#ifndef CUSTOMSHELL_H_
#define CUSTOMSHELL_H_

#include "definition.h"

class CCustomShell {
public:
	CCustomShell();
	virtual ~CCustomShell();

	void GetEnviroVars(char *envp[]);
	void GetExecutablePath(char *path);
	void Run();
	void ClassSigHandler(int sig);
	void SetDispSubs(bool isDisp);
	int RunningMode;
	void SetDebugLevel(const char* str);
	void SetBatchMode(char* path);
	void PushVariable(char* value);

private:
	//Inputs
	vector<string> InputTokens;
	vector<string> PipeSepInputs;

	int nPipedCmds;
	string InputLnBuf;
	string CmdBuf;
	int ShellPID;
	bool isDispSubs;
	int debugLevel;
	ifstream InputFStream;

	//System
	vector<pid_t> ForegroundPIDs;
	vector<pid_t> BackgroundPIDs;
	map<string,string> EnviroVars;
	map<string,string> LocalVars;
	string ExePath;
	vector<string> EnvVarForChild;
	int NumVar;
	int ExitCode;

	//Shell Running / Maintance
	int ReadInput();
	int ParseByPipeCmds();
	int ParseIntoTokens();
	int ParseIntoTokens(int iPipedCmd);

	int DoInternal();
	int DoExternal();
	int DoPipe();
	void SigHandlerReg();
	void VarSubsitution();
	string DispSubs(vector<string> input);
	void DebugInfo(int level, string str);

	//IO Redirection
	int CheckIoRedirection();
	int ApplyIoRedirection(int mode);
	int UndoIoRedirection();
	bool InputRedirect;
	bool OutputRedirect;
	int OriginIn;
	int OriginOut;
	string RedirectPathIn;
	string RedirectPathOut;
	bool ForInternalCmd;

	//History Recording
	vector<string> HistoryList;
	int HistBufPointer;
	void RecordHistory();

	//Internal commands
	int cmd_exit();
	int cmd_show();
	int cmd_clr();
	int cmd_echo();
	int cmd_environ();
	int cmd_export();
	int cmd_unexport();
	int cmd_set();
	int cmd_unset();
	int cmd_dir();
	int cmd_chdir();
	int cmd_history();
	int cmd_repeat();
	int cmd_kill();
	int cmd_wait();
	int cmd_pause();
	int cmd_help();

	int cmd_test();

	//Help Functions
	int SearchExternalPath(string &name);
};

#endif /* CUSTOMSHELL_H_ */
