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

#ifndef EXTENSION_SYSTEM_GLOBAL_H
#define EXTENSION_SYSTEM_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(EXTENSION_SYSTEM_LIBRARY)
#  define EXTENSION_SYSTEM_SHARED_EXPORT Q_DECL_EXPORT
#else
#  define EXTENSION_SYSTEM_SHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // EXTENSION_SYSTEM_GLOBAL_H
