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

#ifndef IOBJECTBASE_H
#define IOBJECTBASE_H

#include "QtilitiesCore_global.h"

#include <QObject>

namespace Qtilities {
    namespace Core {
        //! Namespace containing available interfaces which forms part of the Core Module.
        namespace Interfaces {
            /*!
            \class IObjectBase
            \brief Interface through which QObjects implementing interfaces can be accessed.
              */
            class QTILIITES_CORE_SHARED_EXPORT IObjectBase {
            public:
                IObjectBase() {}
                virtual ~IObjectBase() {}

                //! Returns the QObject* base of the interface.
                virtual QObject* objectBase() = 0;
                //! Returns a const QObject* base of the interface.
                /*!
                  This function can be used in const methods, but make sure that you don't modify things that
                  should stay const. An example where this is used is when we want to get a property on an object
                  in a const method. This is a const operation.
                  */
                virtual const QObject* objectBase() const = 0;
                //! Allows interfaces to provide some sort of source identification.
                /*!
                  This is usefull in cases where interfaces need to provide information about their origin. For example, in a plugin based application plugin's can set the object source ID as the plugin name. This allows querying of an object's origin.

                  \sa setObjectOriginID()
                  */
                QString objectOriginID() const { return d_object_origin_id; }
                //! Allows setting of the object source ID of this interface implementation.
                /*!
                  \sa objectOriginID()
                  */
                void setObjectOriginID(const QString& object_origin_id) { d_object_origin_id = object_origin_id; }

            private:
                QString d_object_origin_id;
            };
        }
    }
}

Q_DECLARE_INTERFACE(Qtilities::Core::Interfaces::IObjectBase,"com.Qtilities.Core.IObjectBase/1.0")

#endif // IOBJECTBASE_H
