/***************************************************************************
													qtclconsole.cpp  -  description
														 -------------------
		begin                : mar mar 15 2005
		copyright            : (C) 2005 by Houssem BDIOUI
		email                : houssem.bdioui@gmail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "qtclconsole.h"
#include "commands.h"
#include "commandsManager.h"

#include <QRegExp>

int ConsoleOutput(ClientData, CONST char * buf,
		int toWrite, int *errorCode)
{
		static bool lastCR = false;
		*errorCode = 0;
		Tcl_SetErrno(0);
		if (!lastCR)
		{
			 QtclConsole *console = QtclConsole::getInstance();
			 console->setTextColor(console->outColor() );
			 console->append(buf);
		}
		lastCR = !lastCR;
		return toWrite;
}

int ConsoleError(ClientData, CONST char * buf,
		int toWrite, int *errorCode)
{
		static bool lastCR = false;
		*errorCode = 0;
		Tcl_SetErrno(0);
		if (!lastCR)
		{
			 QtclConsole *console = QtclConsole::getInstance();
			 console->setTextColor(console->errColor() );
			 console->append(buf);
		}
		lastCR = !lastCR;
		return toWrite;
}



/*Tcl_DriverCloseProc(ClientData instanceData, Tcl_Interp* interp);*/
int closeProcedureForBothTcl_ChannelType(ClientData instanceData, Tcl_Interp *interp){
	return EINVAL;
}


Tcl_ChannelType consoleOutputChannelType =
{
	(char*)"console1", /* const char* typeName*/
	NULL, /*Tcl_ChannelTypeVersion version*/
	(closeProcedureForBothTcl_ChannelType) /*NULL*/, /*Tcl_DriverCloseProc* closeProc*/
	NULL, /*Tcl_DriverInputProc* inputProc*/
	ConsoleOutput, /*Tcl_DriverOutputProc* outputProc*/
	NULL, /*Tcl_DriverSeekProc* seekProc*/
	NULL, /*Tcl_DriverSetOptionProc*  setOptionProc*/
	NULL, /*Tcl_DriverGetOptionProc* getOptionProc*/
	NULL, /*Tcl_DriverWatchProc* watchProc*/
	NULL, /*Tcl_DriverGetHandleProc* getHandleProc*/
	NULL, /*Tcl_DriverClose2Proc* close2Proc*/
	NULL, /*Tcl_DriverBlockModeProc* blockModeProc*/
	NULL, /*Tcl_DriverFlushProc* flushProc*/
	NULL, /*Tcl_DriverHandlerProc * handlerProc*/
	NULL, /*Tcl_DriverWideSeekProc * wideSeekProc*/
	NULL, /*Tcl_DriverThreadActionProc* threadActionProc*/
	NULL /*Tcl_DriverTruncateProc* truncateProc*/
};

//Tcl_ChannelType consoleOutputChannelType =
//{
//		(char*)"console1", NULL, NULL, NULL, ConsoleOutput,
//		NULL, NULL, NULL, NULL, NULL, NULL, NULL,
//		NULL, NULL, NULL, NULL
//};
Tcl_ChannelType consoleErrorChannelType =
{
	(char*)"console2", /* const char* typeName*/
   NULL, /*Tcl_ChannelTypeVersion version*/
   (closeProcedureForBothTcl_ChannelType) /*NULL*/, /*Tcl_DriverCloseProc* closeProc*/
   NULL, /*Tcl_DriverInputProc* inputProc*/
	ConsoleError, /*Tcl_DriverOutputProc* outputProc*/
   NULL, /*Tcl_DriverSeekProc* seekProc*/
   NULL, /*Tcl_DriverSetOptionProc*  setOptionProc*/
   NULL, /*Tcl_DriverGetOptionProc* getOptionProc*/
   NULL, /*Tcl_DriverWatchProc* watchProc*/
   NULL, /*Tcl_DriverGetHandleProc* getHandleProc*/
   NULL, /*Tcl_DriverClose2Proc* close2Proc*/
   NULL, /*Tcl_DriverBlockModeProc* blockModeProc*/
	NULL, /*Tcl_DriverFlushProc* flushProc*/
   NULL, /*Tcl_DriverHandlerProc * handlerProc*/
   NULL, /*Tcl_DriverWideSeekProc * wideSeekProc*/
   NULL, /*Tcl_DriverThreadActionProc* threadActionProc*/
   NULL /*Tcl_DriverTruncateProc* truncateProc*/
};



//Tcl_ChannelType consoleErrorChannelType =
//{
//		(char*)"console2", NULL, NULL, NULL, ConsoleError,
//		NULL, NULL, NULL, NULL, NULL, NULL, NULL,
//		NULL, NULL, NULL, NULL
//};

//callback method that implements the history command
int QtclConsole::showHistory(ClientData, Tcl_Interp* interp, int argc, const char *argv[])
{
		QString usageMsg = QString("Usage: %1\n").arg(argv[0]);
		// Reset result data
		Tcl_ResetResult(interp);

		//Help message in case of wrong parameters
		if (argc != 1)
		{
				Tcl_AppendResult(interp, qPrintable(usageMsg), (char*) NULL);
				return TCL_ERROR;
		}

		uint index = 1;
		for ( QStringList::Iterator it = history.begin(); it != history.end(); ++it )
		{
				Tcl_AppendResult(interp, qPrintable(QString("%1\t%2\n").arg(index).arg(*it)), (char*) NULL);
				index ++;
		}

		return TCL_OK;
}

//callback method that implements the set_prompt command
int QtclConsole::setPrompt(ClientData, Tcl_Interp* interp, int argc, const char *argv[])
{
		QString usageMsg = QString("Usage: %1 new_prompt\n").arg(argv[0]);
		// Reset result data
		Tcl_ResetResult(interp);

		//Help message in case of wrong parameters
		if (argc != 2)
		{
				Tcl_AppendResult(interp, qPrintable(usageMsg), (char*) NULL);
				return TCL_ERROR;
		}

		QConsole::setPrompt(argv[1], false);

		return TCL_OK;
}

//callback method that calls the saveScript() method
int QtclConsole::saveScript( ClientData, Tcl_Interp* interp, int argc, const char *argv[])
{
		QString usageMsg = QString("Usage: %1 script_file_name\n").arg(argv[0]);
		// Reset result data
		Tcl_ResetResult(interp);

		//Help message in case of wrong parameters
		if (argc != 2)
		{
				Tcl_AppendResult(interp, qPrintable(usageMsg), (char*) NULL);
				return TCL_ERROR;
		}

		if (!QConsole::saveScript(argv[1]))
				return TCL_ERROR;
		else
				return TCL_OK;
}

QtclConsole *QtclConsole::theInstance = NULL;

QtclConsole *QtclConsole::getInstance(QWidget *parent, const QString &welcomeText)
{
		if (!theInstance)
				theInstance = new QtclConsole(parent, welcomeText);
		return theInstance;
}

//QTcl console constructor (init the QTextEdit & the attributes)
QtclConsole::QtclConsole(QWidget *parent, const QString &welcomeText) : QConsole(parent, welcomeText)
{
		//Register the msgbox command
		TclCallBack<QtclConsole>::registerMethod(this, (char*)"history", &QtclConsole::showHistory,
			 (char*)"Shows the commands history");

		//Register the set_prompt command
		TclCallBack<QtclConsole>::registerMethod(this, (char*)"set_prompt", &QtclConsole::setPrompt,
			 (char*)"Set a new prompt");

		//Register the set_prompt command
		TclCallBack<QtclConsole>::registerMethod(this, (char*)"save_script", &QtclConsole::saveScript,
			 (char*)"Saves a script of executed commands");

		//Get the Tcl interpreter
		interp = commandsManager::getInstance()->tclInterp();

		//init tcl channels to redirect them to the console
		//stdout
		Tcl_Channel outConsoleChannel = Tcl_CreateChannel(&consoleOutputChannelType, "stdout",
																											(ClientData) TCL_STDOUT, TCL_WRITABLE);
		if (outConsoleChannel)
		{
				Tcl_SetChannelOption(NULL, outConsoleChannel,
														 "-translation", "lf");
				Tcl_SetChannelOption(NULL, outConsoleChannel,
														 "-buffering", "none");
				Tcl_RegisterChannel(interp, outConsoleChannel);
				Tcl_SetStdChannel(outConsoleChannel, TCL_STDOUT);
		}
		//stderr

		Tcl_Channel errConsoleChannel = Tcl_CreateChannel(&consoleErrorChannelType, "stderr",
													 (ClientData) TCL_STDERR, TCL_WRITABLE);
		if (errConsoleChannel)
		{
				Tcl_SetChannelOption(NULL, errConsoleChannel,
														 "-translation", "lf");
				Tcl_SetChannelOption(NULL, errConsoleChannel,
														 "-buffering", "none");
				Tcl_RegisterChannel(interp, errConsoleChannel);
				Tcl_SetStdChannel(errConsoleChannel, TCL_STDERR);

		}



		//set the Tcl Prompt
		QConsole::setPrompt("QTcl shell> ");
}

//Destructor
QtclConsole::~QtclConsole()
{
		//unregister all the methods
		TclCallBack<QtclConsole>::unregisterAll();
}

//Call the TCL interpreter to execute the command
//And retrieve back the result
QString QtclConsole::interpretCommand(const QString &command, int *res)
{
		if (!mutex.tryLock())
		{
			 *res = 1;
			 return "Command cannot be executed!";
		}
		QString result;
		if (!command.isEmpty())
		{
				QString modifiedCommand = command;
				if(modifiedCommand == "exit")
				{
						modifiedCommand = tr("dbgExit");
				}
				//Do the Tcl evaluation
				*res = Tcl_Eval( interp, qPrintable(modifiedCommand) );
				//Get the string result of the executed command
				result = Tcl_GetString(Tcl_GetObjResult(interp));
				//Call the parent implementation
				QConsole::interpretCommand(modifiedCommand, res);
		}
		mutex.unlock();
		return result;
}

bool QtclConsole::isCommandComplete(const QString &command)
{
		return Tcl_CommandComplete(qPrintable(command));
}

//auto-complete Tcl commands and sub-commands (located after [ ; { \n)
QStringList QtclConsole::suggestCommand(const QString &cmd, QString &prefix)
{
		QString commandToComplete = cmd;
		QStringList suggestions;
		prefix = "";
		int i = cmd.lastIndexOf(QRegExp("[\[{;\n]"));
		if (i != -1)
		{
				commandToComplete = cmd.right(cmd.length() - i - 1);
				prefix = cmd.left(i+1);
		}
		int res = Tcl_Eval( interp, qPrintable("info commands [join {" + commandToComplete + "*}]") );
		if (!res)
		{
				//Get the string result of the executed command
				QString result = Tcl_GetString(Tcl_GetObjResult(interp));
				if (!result.isEmpty())
				{
						suggestions = result.split(" ");
				}
		}
		return suggestions;
}
