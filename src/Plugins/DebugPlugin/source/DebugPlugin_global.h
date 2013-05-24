/****************************************************************************
**
** Copyright (c) 2009-2013, Jaco Naudé
**
** This file is part of the Qtilities Debug Plugin.
** See Dependencies.pri for the licensing requirements of the Debug plugin.
** Alternatively, see http://www.qtilities.org/licensing.html.
**
****************************************************************************/

#ifndef DEBUG_PLUGIN_GLOBAL_H
#define DEBUG_PLUGIN_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(DEBUG_PLUGIN_LIBRARY)
#  define DEBUG_PLUGIN_SHARED_EXPORT Q_DECL_EXPORT
#else
#  define DEBUG_PLUGIN_SHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // DEBUG_PLUGIN_GLOBAL_H
