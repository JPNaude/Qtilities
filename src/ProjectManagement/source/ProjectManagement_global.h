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

#ifndef PROJECT_MANAGEMENT_GLOBAL_H
#define PROJECT_MANAGEMENT_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(PROJECT_MANAGEMENT_LIBRARY)
#  define PROJECT_MANAGEMENT_SHARED_EXPORT Q_DECL_EXPORT
#else
#  define PROJECT_MANAGEMENT_SHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // PROJECT_MANAGEMENT_GLOBAL_H
