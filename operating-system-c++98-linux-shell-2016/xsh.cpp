
#include "definition.h"
#include "customshell.h"

//DEFINITIONS
#define NUM_HISTORY 100

#define STATUS_RUN 1
#define STATUS_EXIT 0

CCustomShell XSH;

void SigHandler(int sig)
{
	XSH.ClassSigHandler(sig);
}
void SigHandlerReg()
{
    signal(SIGINT, SigHandler);
    signal(SIGQUIT, SigHandler);
    signal(SIGCONT, SigHandler);
    signal(SIGTSTP, SigHandler);
    sigset_t masklist;
    sigemptyset(&masklist);
    sigaddset(&masklist, SIGABRT);
    sigaddset(&masklist, SIGALRM);
    sigaddset(&masklist, SIGHUP);
    sigaddset(&masklist, SIGTERM);
    sigaddset(&masklist, SIGUSR1);
    sigaddset(&masklist, SIGUSR2);
    sigprocmask(SIG_SETMASK, &masklist, NULL);
}
//Entry
int main(int argc, char *argv[], char *envp[])
{
	//Run With Arguments
	for (int i = 0; i < argc; i++)
	{
		if ((string)argv[i]=="-x")
			XSH.SetDispSubs(true);

		else if ((string)argv[i] == "-d")
		{
			if(i < argc-1 && (string)argv[i+1]!="-f" && (string)argv[i+1]!="-x")
				XSH.SetDebugLevel(argv[i+1]);
			else
				XSH.SetDebugLevel("1");
		}

		else if ((string)argv[i] == "-f")
		{
			if(i < argc-1)
				XSH.SetBatchMode(argv[i+1]);

			else
			{
				cout << "Error: File input is incorrect." << endl;
				return 0;
			}
			for(;i < argc-2 && (string)argv[i+2]!="-d" && (string)argv[i+2]!="-x";i++)
				XSH.PushVariable(argv[i+2]);
		}
	}

	SigHandlerReg();
	XSH.GetExecutablePath(argv[0]);
	XSH.GetEnviroVars(envp);
	XSH.Run();

	return 0;
}


