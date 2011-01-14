/****************************************************************************
**
** Copyright (c) 2009-2011, Jaco Naude
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

#ifndef ILOGGEREXPORTALE_H
#define ILOGGEREXPORTALE_H

#include "Logging_global.h"

#include <QDataStream>

namespace Qtilities {
    namespace Logging {
        //! Namespace containing available interfaces which forms part of the Logging Module.
        namespace Interfaces {
            /*!
            \class ILoggerExportable
            \brief Logger engines can implement this interface if they are able to export and reconstruct themselves.
              */
            class LOGGING_SHARED_EXPORT ILoggerExportable {
            public:
                ILoggerExportable() {}
                virtual ~ILoggerExportable() {}

                enum ExportMode { Binary };
                Q_DECLARE_FLAGS(ExportModeFlags, ExportMode);
                Q_FLAGS(ExportModeFlags);

                //! Provides information about the export format(s) supported by your implementation of ILoggerExportable.
                virtual ExportModeFlags supportedFormats() const = 0;
                //! Allows exporting to a QDataStream. A reference to the QDataStream to which the object's information must be appended is provided.
                virtual bool exportBinary(QDataStream& stream) const = 0;
                //! Allows importing and reconstruction of the object state from information provided in a QDataStream. A reference to the QDataStream which contains the object's information is provided.
                virtual bool importBinary(QDataStream& stream) = 0;
                //! The factory tag of this interface.
                virtual QString factoryTag() const = 0;
                //! The instance name of the logger engine implementing this interface.
                virtual QString instanceName() const = 0;
            };

            Q_DECLARE_OPERATORS_FOR_FLAGS(ILoggerExportable::ExportModeFlags)
        }
    }
}

Q_DECLARE_INTERFACE(Qtilities::Logging::Interfaces::ILoggerExportable,"com.Qtilities.Logging.ILoggerExportable/1.0")

#endif // ILOGGEREXPORTALE_H
