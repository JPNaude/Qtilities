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

#ifndef QtilitiesCoreGui_global_H
#define QtilitiesCoreGui_global_H

#include <QtCore/qglobal.h>

#if defined(QTILITIES_CORE_GUI_LIBRARY)
#  define QTILITIES_CORE_GUI_SHARED_EXPORT Q_DECL_EXPORT
#else
#  define QTILITIES_CORE_GUI_SHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // QtilitiesCoreGui_global_H
