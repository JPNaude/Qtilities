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

#ifndef HELP_PLUGIN_GLOBAL_H
#define HELP_PLUGIN_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(HELP_PLUGIN_LIBRARY)
#  define HELP_PLUGIN_SHARED_EXPORT Q_DECL_EXPORT
#else
#  define HELP_PLUGIN_SHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // HELP_PLUGIN_GLOBAL_H
