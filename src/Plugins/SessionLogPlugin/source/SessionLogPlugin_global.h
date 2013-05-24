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

#ifndef SESSION_LOG_PLUGIN_GLOBAL_H
#define SESSION_LOG_PLUGIN_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(SESSION_LOG_PLUGIN_LIBRARY)
#  define SESSION_LOG_PLUGIN_SHARED_EXPORT Q_DECL_EXPORT
#else
#  define SESSION_LOG_PLUGIN_SHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // SESSION_LOG_PLUGIN_GLOBAL_H
