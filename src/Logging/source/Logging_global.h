/****************************************************************************
**
** Copyright (c) 2009-2013, Jaco Naudé
**
** This file is part of Qtilities.
**
** For licensing information, please see
** http://jpnaude.github.io/Qtilities/page_licensing.html
**
****************************************************************************/

#ifndef LOGGING_GLOBAL_H
#define LOGGING_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(LOGGING_LIBRARY)
#  define LOGGING_SHARED_EXPORT Q_DECL_EXPORT
#else
#  define LOGGING_SHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // LOGGING_GLOBAL_H
