/*
 * Copyright (c) 2005 by Ulrich Ring
 * 2010: Houssem BDIOUI: ported to Qt4
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
 * RCS: @(#) $Id: tclnotify.cpp,v 1.25 2004/03/08 08:33:40 ur Exp $
 */

#include "tclnotify.h"
#include <qapplication.h>
#include <qsocketnotifier.h>
#include <qtimer.h>

/*
 * The following structure is what is added to the Tcl event queue when
 * file handlers are ready to fire.
 */
 
struct FileHandlerEvent
{
        Tcl_Event header;
        /* Information that is standard for
         * all events. */
        int fd;
        /* File descriptor that is ready.  Used
         * to find the FileHandler structure for
         * the file (can't point directly to the
         * FileHandler structure because it could
         * go away while the event is queued). */
};

//
// The following static structure contains the state information for the
// Qt based implementation of the Tcl notifier.
//

class NotifierState
{
        public:
                NotifierState(QApplication *app);
                ~NotifierState();
                QApplication *qapp;		 // The Qt application
                QTimer *timer;			 // A QTimer
                                                                 // Handle timeouts
                TclTimeNotifier *timeNotifier;
                int currentTimeout;		 // Non 0, if timer active
                                                                 // Pointer to head of file handler list
                TclFileHandler *firstFileHandlerPtr;
};

static NotifierState * notifier = 0;

/*
 * Static routines defined in this file.
 */

static int      FileHandlerEventProc( Tcl_Event *evPtr, int flags );
static void     NotifierExitHandler( ClientData clientData );
static void     CreateFileHandler( int fd, int mask,
Tcl_FileProc * proc, ClientData clientData );
static void     DeleteFileHandler( int fd );
static void     SetTimer( Tcl_Time * timePtr );
static int      WaitForEvent( Tcl_Time * timePtr );

/*
 *----------------------------------------------------------------------
 *
 * FileHandlerEventProc --
 *
 *	This procedure is called by Tcl_ServiceEvent when a file event
 *	reaches the front of the event queue.  This procedure is
 *	responsible for actually handling the event by invoking the
 *	callback for the file handler.
 *
 * Results:
 *	Returns 1 if the event was handled, meaning it should be removed
 *	from the queue.  Returns 0 if the event was not handled, meaning
 *	it should stay on the queue.  The only time the event isn't
 *	handled is if the TCL_FILE_EVENTS flag bit isn't set.
 *
 * Side effects:
 *	Whatever the file handler's callback procedure does.
 *
 *----------------------------------------------------------------------
 */

static int
FileHandlerEventProc(
Tcl_Event *evPtr,				 /* Event to service. */
int flags)
/* Flags that indicate what events to
 * handle, such as TCL_FILE_EVENTS. */
{
        TclFileHandler *filePtr;
        FileHandlerEvent *fileEvPtr = (FileHandlerEvent *) evPtr;
        int mask;

        if (!(flags & TCL_FILE_EVENTS)) {
                return 0;
        }

        /*
         * Search through the file handlers to find the one whose handle matches
         * the event.  We do this rather than keeping a pointer to the file
         * handler directly in the event, so that the handler can be deleted
         * while the event is queued without leaving a dangling pointer.
         */

        for (filePtr = notifier->firstFileHandlerPtr; filePtr != NULL;
        filePtr = filePtr->nextPtr) {
                if (filePtr->fd != fileEvPtr->fd) {
                        continue;
                }

                /*
                 * The code is tricky for two reasons:
                 * 1. The file handler's desired events could have changed
                 *    since the time when the event was queued, so AND the
                 *    ready mask with the desired mask.
                 * 2. The file could have been closed and re-opened since
                 *    the time when the event was queued.  This is why the
                 *    ready mask is stored in the file handler rather than
                 *    the queued event:  it will be zeroed when a new
                 *    file handler is created for the newly opened file.
                 */

                mask = filePtr->readyMask & filePtr->mask;
                filePtr->readyMask = 0;
                if (mask != 0) {
                        (*filePtr->proc)(filePtr->clientData, mask);
                }
                break;
        }
        return 1;
}

//-----------------------------------------------------------------
NotifierState::NotifierState(QApplication *app) : qapp(app),
currentTimeout(0), firstFileHandlerPtr(0)
{
        timer = new QTimer();
        timeNotifier = new TclTimeNotifier();
        timeNotifier->connect( timer, SIGNAL(timeout()), SLOT(gong()) );
}

NotifierState::~NotifierState()
{
        if (notifier->currentTimeout) {
                notifier->timer->stop();
        }
        delete timer;
        delete timeNotifier;
        for (; notifier->firstFileHandlerPtr != NULL; ) {
                DeleteFileHandler(notifier->firstFileHandlerPtr->fd);
                /*
                 * original calls Tcl_DeleteFileHandler() which actually
                 * is redirected to DeleteFileHandler()
                 */
        }
}

//-----------------------------------------------------------------
TclTimeNotifier::TclTimeNotifier() :
QObject(0)
{
    setObjectName("TclTimeNotifier");
}

TclTimeNotifier::~TclTimeNotifier()
{
}

void
TclTimeNotifier::gong()
{
        notifier->currentTimeout = 0;
        Tcl_ServiceAll();
}

//-----------------------------------------------------------------
TclFileHandler::TclFileHandler( int f ) :
fd(f), mask(0), readyMask(0),
read(0), write(0), except(0)
{
        // insert in list of file handlers
        nextPtr = notifier->firstFileHandlerPtr;
        notifier->firstFileHandlerPtr = this;
}

TclFileHandler::~TclFileHandler()
{
        if (read) delete read;
        if (write) delete write;
        if (except) delete write;
}

void
TclFileHandler::fileactive( int f )
{
        /*
         * Determine which event happened.
         */
        const QObject *s = sender();
        int current_mask = 0;

        if (s == read) {
                current_mask = TCL_READABLE;
        }
        else if (s == write) {
                current_mask = TCL_WRITABLE;
        }
        else if (s == except) {
                current_mask = TCL_EXCEPTION;
        }
        else {
                // FIXME: should panic
        }

        /*
         * Ignore unwanted or duplicate events.
         */

        if (!(mask & current_mask) || (readyMask & current_mask)) {
                return;
        }

        /*
         * This is an interesting event, so put it onto the event queue.
         */

        FileHandlerEvent *fileEvPtr;
        readyMask |= mask;
        fileEvPtr = (FileHandlerEvent *) ckalloc(sizeof(FileHandlerEvent));
        fileEvPtr->header.proc = FileHandlerEventProc;
        fileEvPtr->fd = f;
        Tcl_QueueEvent((Tcl_Event *) fileEvPtr, TCL_QUEUE_TAIL);

        /*
         * Process events on the Tcl event queue before returning to Xt.
         */

        Tcl_ServiceAll();
}

static void FinalizeNotifier( ClientData )
{
        // do nothing
}

/*
 *----------------------------------------------------------------------
 *
 * Qtk_InitNotifier --
 *
 *	Initializes the notifier state.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Creates a new exit handler.
 *
 *----------------------------------------------------------------------
 */

void
Qtk_InitNotifier(QApplication *qapp)
{
        // The (global) QApplication must exists
        Q_ASSERT(qapp);

        /*
         * Only reinitialize if we are not in exit handling. The notifier
         * can get reinitialized after its own exit handler has run, because
         * of exit handlers for the I/O and timer sub-systems (order dependency).
         */

        if (TclInExit()) {
                return;
        }

        Tcl_NotifierProcs notifier_info;
        notifier_info.createFileHandlerProc = CreateFileHandler;
        notifier_info.deleteFileHandlerProc = DeleteFileHandler;
        notifier_info.setTimerProc = SetTimer;
        notifier_info.waitForEventProc = WaitForEvent;
        notifier_info.initNotifierProc = 0;
        notifier_info.finalizeNotifierProc = FinalizeNotifier;
        notifier_info.alertNotifierProc = 0;
        notifier_info.serviceModeHookProc = 0;
        Tcl_SetNotifier(&notifier_info);

        /*
         * DO NOT create the application context yet; doing so would prevent
         * external applications from setting it for us to their own ones.
         */
        Tcl_CreateExitHandler(NotifierExitHandler, NULL);

        if (!notifier) {
                static NotifierState ns(qapp);
                notifier = &ns;
        }
}

/*
 *----------------------------------------------------------------------
 *
 * NotifierExitHandler --
 *
 *	This function is called to cleanup the notifier state before
 *	Tcl is unloaded.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Destroys the notifier window.
 *
 *----------------------------------------------------------------------
 */

static void
NotifierExitHandler(
ClientData /* clientData */)
{
}

/*
 *----------------------------------------------------------------------
 *
 * SetTimer --
 *
 *	This procedure sets the current notifier timeout value.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Replaces any previous timer.
 *
 *----------------------------------------------------------------------
 */

static void
SetTimer(
Tcl_Time *timePtr)				 /* Timeout value, may be NULL. */
{
        Q_ASSERT(notifier);

        if (notifier->currentTimeout != 0) {
                notifier->timer->stop();
        }

        if (timePtr) {
                int timeout = timePtr->sec * 1000 + timePtr->usec / 1000;
                notifier->timer->start( timeout);
                notifier->currentTimeout = 1;
        }
        else {
                notifier->currentTimeout = 0;
        }
}

/*
 *----------------------------------------------------------------------
 *
 * CreateFileHandler --
 *
 *	This procedure registers a file handler with the Xt notifier.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Creates a new file handler structure and registers one or more
 *	input procedures with Xt.
 *
 *----------------------------------------------------------------------
 */

static void
CreateFileHandler(
int fd,							 /* Handle of stream to watch. */
int mask,
/* OR'ed combination of TCL_READABLE,
 * TCL_WRITABLE, and TCL_EXCEPTION:
 * indicates conditions under which
 * proc should be called. */
Tcl_FileProc *proc,				 /* Procedure to call for each selected event. */
ClientData clientData)			 /* Arbitrary data to pass to proc. */
{
        Q_ASSERT(notifier);
        TclFileHandler *filePtr;

        for (filePtr = notifier->firstFileHandlerPtr; filePtr != NULL;
        filePtr = filePtr->nextPtr) {
                if (filePtr->fd == fd) {
                        break;
                }
        }
        if (filePtr == NULL) {
                filePtr = new TclFileHandler( fd );
        }
        filePtr->proc = proc;
        filePtr->clientData = clientData;

        /*
         * Create a QSocketNotifier with file fd.
         */

        if (mask & TCL_READABLE) {
                if (!(filePtr->mask & TCL_READABLE)) {
                        filePtr->read = new QSocketNotifier( fd, QSocketNotifier::Read );
                        filePtr->connect( filePtr->read,
                                SIGNAL(activated(int)), SLOT(fileactive(int)) );
                }
        }
        else {
                if (filePtr->mask & TCL_READABLE) {
                        delete filePtr->read;
                }
        }
        if (mask & TCL_WRITABLE) {
                if (!(filePtr->mask & TCL_WRITABLE)) {
                        filePtr->write = new QSocketNotifier( fd, QSocketNotifier::Write );
                        filePtr->connect( filePtr->write,
                                SIGNAL(activated(int)), SLOT(fileactive(int)) );
                }
        }
        else {
                if (filePtr->mask & TCL_WRITABLE) {
                        delete filePtr->write;
                }
        }
        filePtr->mask = mask;
}

/*
 *----------------------------------------------------------------------
 *
 * DeleteFileHandler --
 *
 *	Cancel a previously-arranged callback arrangement for
 *	a file.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	If a callback was previously registered on file, remove it.
 *
 *----------------------------------------------------------------------
 */

static void
DeleteFileHandler(
int fd)
/* Stream id for which to remove
 * callback procedure. */
{
        Q_ASSERT(notifier);
        TclFileHandler *filePtr, *prevPtr;

        /*
         * Find the entry for the given file (and return if there
         * isn't one).
         */

        for (prevPtr = NULL, filePtr = notifier->firstFileHandlerPtr; ;
        prevPtr = filePtr, filePtr = filePtr->nextPtr) {
                if (filePtr == NULL) {
                        return;
                }
                if (filePtr->fd == fd) {
                        break;
                }
        }

        /*
         * Clean up information in the callback record.
         */

        if (prevPtr == NULL) {
                notifier->firstFileHandlerPtr = filePtr->nextPtr;
        }
        else {
                prevPtr->nextPtr = filePtr->nextPtr;
        }
        delete filePtr;
}

/*
 *----------------------------------------------------------------------
 *
 * WaitForEvent --
 *
 *	This function is called by Tcl_DoOneEvent to wait for new
 *	events on the message queue.  If the block time is 0, then
 *	Tcl_WaitForEvent just polls without blocking.
 *
 * Results:
 *	Returns 1 if an event was found, else 0.  This ensures that
 *	Tcl_DoOneEvent will return 1, even if the event is handled
 *	by non-Tcl code.
 *
 * Side effects:
 *	Queues file events that are detected by the select.
 *
 *----------------------------------------------------------------------
 */

static int
WaitForEvent(
Tcl_Time *timePtr)				 /* Maximum block time, or NULL. */
{
        Q_ASSERT(notifier);

        int timeout = 0;
        if (timePtr) {
                timeout = timePtr->sec * 1000 + timePtr->usec / 1000;
        }

#if QT_VERSION >= 0x040000
        notifier->qapp->processEvents( QEventLoop::AllEvents, timeout );
#else
        notifier->qapp->processEvents( timeout );
#endif
        return 1;
}
