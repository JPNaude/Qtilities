/***************************************************************************
                          main.cpp  -  description
                             -------------------
    begin                : mar mar 8 2005
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

#include <qapplication.h>
#include <qmainwindow.h>
#include "qpyconsole.h"

/* Define the following to fix __ctype_* from GLIBC2.3 and upper
   if not compiled using the same GLIBC */
//#define FIX__CTYPE_

#ifdef FIX__CTYPE_
#include <ctype.h>
__const unsigned short int *__ctype_b;
__const __int32_t *__ctype_tolower;
__const __int32_t *__ctype_toupper;

void ctSetup()
{
  __ctype_b = *(__ctype_b_loc());
  __ctype_toupper = *(__ctype_toupper_loc());
  __ctype_tolower = *(__ctype_tolower_loc());
}
#endif

//The main entry
int main( int argc, char ** argv )
{
#ifdef FIX__CTYPE_
    ctSetup();
#endif
    QApplication a( argc, argv );

    //Create and show the main window
    QMainWindow mw;
    mw.setMinimumSize(640, 480);
    //Instantiate and set the focus to the QPyConsole
    QPyConsole *console = QPyConsole::getInstance(&mw);
    mw.setFocusProxy((QWidget*)console);
    mw.setCentralWidget((QWidget*)console);
    mw.show();
                    
    return a.exec();
}
