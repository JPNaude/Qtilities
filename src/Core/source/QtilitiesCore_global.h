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

#ifndef QtilitiesCore_global_H
#define QtilitiesCore_global_H

#include <QtCore/qglobal.h>

#if defined(QTILITIESCORE_LIBRARY)
#  define QTILIITES_CORE_SHARED_EXPORT Q_DECL_EXPORT
#else
#  define QTILIITES_CORE_SHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // QtilitiesCore_global_H
