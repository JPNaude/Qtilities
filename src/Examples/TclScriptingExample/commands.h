/***************************************************************************
													commands.h  -  description
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

#ifndef COMMANDS_H
#define COMMANDS_H

#include <tcl.h>

int CallShowWidget( ClientData client_data, Tcl_Interp* interp, int argc, char *argv[]);
int LogQtiMessage( ClientData client_data, Tcl_Interp* interp, int argc, char *argv[]);
int LogQtiPriorityMessage( ClientData client_data, Tcl_Interp* interp, int argc, char *argv[]);

#endif
