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

#ifndef ICONTEXT_H
#define ICONTEXT_H

#include "QtilitiesCore_global.h"
#include "IObjectBase.h"

#include <QObject>
#include <QString>

namespace Qtilities {
    namespace Core {
        namespace Interfaces {
            /*!
            \class IContext
            \brief Objects implementing this interface will have a context associated with them.
              */
            class QTILIITES_CORE_SHARED_EXPORT IContext : virtual public IObjectBase {
            public:
                IContext() {}
                virtual ~IContext() {}

                //! Returns a context string for the context represented by this interface.
                virtual QString contextString() const = 0;
                //! Returns a help ID for this context.
                virtual QString contextHelpId() const { return QString(); }
            };
        }
    }
}

Q_DECLARE_INTERFACE(Qtilities::Core::Interfaces::IContext,"com.Qtilities.Core.IContext/1.0")

#endif // ICONTEXT_H
