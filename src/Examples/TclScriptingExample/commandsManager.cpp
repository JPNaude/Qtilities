/***************************************************************************
 commandsManager.cpp  -  description
 -------------------
 begin                : lun jun 27 2005
 copyright            : (C) 2005 by houssem
 email                : houssem@localhost
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "commandsManager.h"

commandsManager *commandsManager::theInstance = NULL;

commandsManager::commandsManager(Tcl_Interp* _interp)
{
	 createdInterp = false;
	 if (!_interp)
	 {
			//Initialize the Tcl interpreter
			interp = Tcl_CreateInterp();
			Tcl_Init(interp);
			createdInterp = true;
	 } else
			interp = _interp;
}

commandsManager::~commandsManager()
{
	 TclCallBack<commandsManager>::unregisterAll();
	 if (interp && createdInterp)
	 {
			//Delete the Tcl interpreter
			Tcl_DeleteInterp(interp);
			interp = NULL;
	 }
}

commandsManager *commandsManager::getInstance(Tcl_Interp* interp)
{
	 if (!theInstance)
	 {
			theInstance = new commandsManager(interp);
			//Register the help command
			TclCallBack<commandsManager>::registerMethod(theInstance, (char*)"help",
						&commandsManager::help, (char*)"displays this help message");
	 }
	 return theInstance;
}

void commandsManager::registerFunction(char *commandName, commandType function,
			char *helpMsg, void *param)
{
	 commandsHelp[commandName] = helpMsg;
	 Tcl_CreateCommand(interp, commandName, function, param,
				 (Tcl_CmdDeleteProc*) NULL);
}

void commandsManager::unregisterFunction(const char *commandName)
{
	 commandsHelp.remove(commandName);
	 Tcl_DeleteCommand(interp, commandName);
}

void commandsManager::registerVariable(char *varName, int &Var, char *helpMsg)
{
	 varsHelp[varName] = helpMsg;
	 Tcl_LinkVar(interp, varName, (char *)&Var, TCL_LINK_INT);
}

void commandsManager::registerVariable(char *varName, bool &Var, char *helpMsg)
{
	 varsHelp[varName] = helpMsg;
	 Tcl_LinkVar(interp, varName, (char *)&Var, TCL_LINK_BOOLEAN);
}

void commandsManager::registerVariable(char *varName, char* &Var, char *helpMsg)
{
	 varsHelp[varName] = helpMsg;
	 Tcl_LinkVar(interp, varName, (char *)&Var, TCL_LINK_STRING);
}

int commandsManager::help(ClientData, Tcl_Interp* interp, int argc,
			const char *[])
{
	 // Reset result data
	 Tcl_ResetResult(interp);

	 //Help message in case of wrong parameters
	 if (argc != 1)
	 {
			Tcl_AppendResult(interp, "Usage: help\n", (char*) NULL);
			return TCL_ERROR;
	 }

	 QMap<QString, QString>::Iterator it;
	 //determine the max length of the commands/vars to improve formatting
	 int maxCommandLength = 0;
	 for (it = commandsHelp.begin(); it != commandsHelp.end(); ++it)
			if (maxCommandLength < (int)it.key().length())
				 maxCommandLength = it.key().length();
	 for (it = varsHelp.begin(); it != varsHelp.end(); ++it)
			if (maxCommandLength < (int)it.key().length())
				 maxCommandLength = it.key().length();
	 if (commandsHelp.size())
	 {
			Tcl_AppendResult(interp, "Commands :\n", (char*) NULL);
			Tcl_AppendResult(interp, "==========\n", (char*) NULL);
			for (it = commandsHelp.begin(); it != commandsHelp.end(); ++it)
				 Tcl_AppendResult(interp, qPrintable(QString("%1:%2%3\n").arg(it.key()).arg(QString().fill(' ',
										 maxCommandLength - it.key().length() + 1)).arg(it.value())), (char*) NULL);
	 }

	 if (varsHelp.size())
	 {
			Tcl_AppendResult(interp, "\nVariables :\n", (char*) NULL);
			Tcl_AppendResult(interp, "===========\n", (char*) NULL);
			for (it = varsHelp.begin(); it != varsHelp.end(); ++it)
				 Tcl_AppendResult(interp, qPrintable(QString("%1:%2%3\n").arg(it.key()).arg(QString().fill(' ', maxCommandLength - it.key().length() + 1)).arg(it.value())), (char*) NULL);
	 }

	 return TCL_OK;
}
