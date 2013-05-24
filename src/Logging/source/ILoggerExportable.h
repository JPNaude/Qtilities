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
