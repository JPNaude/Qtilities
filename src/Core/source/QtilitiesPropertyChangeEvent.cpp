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

#include "QtilitiesPropertyChangeEvent.h"

Qtilities::Core::QtilitiesPropertyChangeEvent::QtilitiesPropertyChangeEvent(const QByteArray& property_name, int observer_id) : QEvent(QEvent::User)
{
    registerEventType();
    d_property_name = property_name;
    d_observer_id = observer_id;
}
