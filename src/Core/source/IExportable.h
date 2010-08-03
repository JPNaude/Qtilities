/****************************************************************************
**
** Copyright (c) 2009-2010, Jaco Naude
**
** This file is part of Qtilities which is released under the following
** licensing options.
**
** Option 1: Open Source
** Under this license Qtilities is free software: you can
** redistribute it and/or modify it under the terms of the GNU General
** Public License as published by the Free Software Foundation, either
** version 3 of the License, or (at your option) any later version.
**
** Qtilities is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Qtilities. If not, see http://www.gnu.org/licenses/.
**
** Option 2: Commercial
** Alternatively, this library is also released under a commercial license
** that allows the development of closed source proprietary applications
** without restrictions on licensing. For more information on this option,
** please see the project website's licensing page:
** http://www.qtilities.org/licensing.html
**
** If you are unsure which license is appropriate for your use, please
** contact support@qtilities.org.
**
****************************************************************************/

#ifndef IEXPORTALE_H
#define IEXPORTALE_H

#include "QtilitiesCore_global.h"
#include "IFactory.h"
#include "IObjectBase.h"

#include <QList>
#include <QPointer>
#include <QString>

namespace Qtilities {
    namespace Core {
        namespace Interfaces {
            using namespace Qtilities::Core;

            /*!
            \class IExportable
            \brief Objects can implement this interface if they are able to export and reconstruct themselves.
              */
            class QTILIITES_CORE_SHARED_EXPORT IExportable : virtual public IObjectBase {
            public:
                IExportable() {}
                virtual ~IExportable() {}

                enum ExportMode {
                    Binary = 1 << 0
                };
                Q_DECLARE_FLAGS(ExportModeFlags, ExportMode);
                Q_FLAGS(ExportModeFlags);

                //! The possible results of an export/import operation.
                enum Result { Complete,     /*!< Complete when all the information was successfully exported/imported. */
                              Incomplete,   /*!< Incomplete when some information could not be exported/imported. An example of this is when an Observer exports itself. When only a subset of the subjets observed by the observer implements the IExportable interface the Observer will return Partial because it was only exported partially. */
                              Failed        /*!< Failed when an error occured. The operation must be aborted in this case. */
                          };
                Q_ENUMS(Result)

                //! Provides information about the export format(s) supported by your implementation of IExportable.
                virtual ExportModeFlags supportedFormats() const = 0;
                //! The factory data which must be used when the exported object is reconstructed during an import.
                virtual IFactoryData factoryData() const = 0;
                //! Allows exporting to a QDataStream.
                /*!
                  \param stream A reference to the QDataStream to which the object's information must be appended is provided.
                  */
                virtual Result exportBinary(QDataStream& stream) const = 0;
                //! Allows importing and reconstruction of the object state from information provided in a QDataStream.
                /*!
                    \param stream The QDataStream which contains the object's information.
                    \param import_list All objects constructed during the import operation must be added to the import list. When the operation fails, all objects in this list will be deleted.
                    */
                virtual Result importBinary(QDataStream& stream, QList<QPointer<QObject> >& import_list) = 0;
            };
        }
    }
}

Q_DECLARE_INTERFACE(Qtilities::Core::Interfaces::IExportable,"com.Qtilities.Core.IExportable/1.0")

#endif // IEXPORTALE_H
