/*
 * Copyright (c) 2005 by Ulrich Ring
 *
 * This is a clone of the file unix/tclNotify.c from the
 * Tcl distribution.
 *
 * It contained the notifier driver implementation for the
 * Xt intrinsics.
 *
 * Copyright (c) 1997 by Sun Microsystems, Inc.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * RCS: @(#) $Id: tclnotify.h,v 1.3 2011/05/18 19:12:33 tbelair Exp $
 */

#ifndef tclnotify_h_
#define tclnotify_h_

// NOTE: This file is input to Qt's moc.

#include <tcl.h>
extern "C" int TclInExit(void);

#include <qobject.h>
class QSocketNotifier;

class TclTimeNotifier : public QObject
{
	Q_OBJECT
		public:
		TclTimeNotifier();
		~TclTimeNotifier();
	public slots:
		void gong();
};

class TclFileHandler : public QObject
{
	Q_OBJECT
		public:
		TclFileHandler( int f );
		~TclFileHandler();

		int fd;
		int mask;				 /* Mask of desired events: TCL_READABLE, etc. */
		int readyMask;
		/* Events that have been seen since the
		 * last time FileHandlerEventProc was called
		 * for this file. */
		QSocketNotifier *read;
		QSocketNotifier *write;
		QSocketNotifier *except;
		Tcl_FileProc *proc;
		/* Procedure to call, in the style of
		 * CreateFileHandler. */
		ClientData clientData;	 /* Argument to pass to proc. */
		TclFileHandler *nextPtr; /* Next in list of all files we care about. */
	public slots:
		void fileactive( int );

};
#endif
