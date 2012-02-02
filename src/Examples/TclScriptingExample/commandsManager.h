/***************************************************************************
													commandsManager.h  -  description
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

#ifndef COMMANDS_MANAGER_H
#define COMMANDS_MANAGER_H

#include <tcl.h>
#include <QStringList>
#include <QMap>

/* Singleton helper class to register/unregister tcl callback functions & vars
	 along with help messages */
class commandsManager
{
		public:
		typedef int (*commandType) (void*, Tcl_Interp*, int, const char**);
		public:
		~commandsManager();
		static commandsManager *getInstance(Tcl_Interp* interp = NULL);
		Tcl_Interp *tclInterp() {return interp;}
		void registerFunction(char *commandName, commandType function, char *helpMsg, void *param = NULL);
		void unregisterFunction(const char *commandName);
		//Registers an integer variable
		void registerVariable(char *varName, int &Var, char *helpMsg);
		//Registers a boolean variable
		void registerVariable(char *varName, bool &Var, char *helpMsg);
		//Registers a string variable
		void registerVariable(char *varName, char *&Var, char *helpMsg);
		//callback method that displays a help message
	 int help(ClientData client_data, Tcl_Interp* interp, int argc,
				 const char *argv[]);
		private:
		Tcl_Interp* interp;
		bool createdInterp;
		static commandsManager *theInstance;
		QMap<QString, QString> commandsHelp;
		QMap<QString, QString> varsHelp;
		private:
		commandsManager(Tcl_Interp* interp = NULL);
};

/* Template Helper class that enables registering methods
	 as callbacks. Usage:
	 TclCallBack<ClassType>::registerMethod(classInstance, "tcl_command_name", &ClassType::methodName, "Help message");
 */
template <class Class>
class TclCallBack
{
		private:
		typedef int (Class::*commandType) (void*, Tcl_Interp*, int, const char**);

		static int callBackMethod(void* client_data, Tcl_Interp* interp, int argc, const char** argv)
		{
				typename QList<commandType>::iterator it1 = method.begin();
				for ( QStringList::Iterator it = methodNames.begin(); it != methodNames.end(); ++it )
				{
						if (*it == argv[0])
								break;
						it1 ++;
				}
				commandType meth = *it1;
				return (instance->*meth)(client_data, interp, argc, argv);
		}

		public:
		static void registerMethod(Class *_instance, char * commandName, commandType _method, char *helpMsg, void *param = NULL)
		{
				instance = _instance;
				method.push_back(_method);
				methodNames.append(commandName);
				commandsManager::getInstance()->registerFunction(commandName, callBackMethod, helpMsg, param);
		}

		static void unregisterAll()
		{
				for ( QStringList::Iterator it = methodNames.begin(); it != methodNames.end(); ++it )
						commandsManager::getInstance()->unregisterFunction(qPrintable(*it));
				method.clear();
				methodNames.clear();
		}

		private:
		static Class *instance;
		static QStringList methodNames;
		static QList<commandType> method;
};

template <class Class>
Class * TclCallBack<Class>::instance;

template <class Class>
QList<typename TclCallBack<Class>::commandType> TclCallBack<Class>::method;

template <class Class>
QStringList TclCallBack<Class>::methodNames;

#endif
