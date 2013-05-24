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

#ifndef TESTING_GLOBAL_H
#define TESTING_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(TESTING_LIBRARY)
#  define TESTING_SHARED_EXPORT Q_DECL_EXPORT
#else
#  define TESTING_SHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // TESTING_GLOBAL_H
