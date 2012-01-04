/****************************************************************************
**
** Copyright (c) 2009-2012, Jaco Naude
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

#ifndef ABSTRACTFORMATTINGENGINE_H
#define ABSTRACTFORMATTINGENGINE_H

#include <QString>
#include <QList>
#include <QVariant>

#include "Logger.h"

namespace Qtilities {
    namespace Logging {
        /*!
        \class AbstractFormattingEngine
        \brief The base class of all formatting engines.
          */
        class AbstractFormattingEngine : public QObject
        {
        Q_OBJECT
        Q_PROPERTY(QString FileExtension READ fileExtension);

        public:
            AbstractFormattingEngine() {}
            virtual ~AbstractFormattingEngine() {}

            //! Function which is used to provide the initialization string for the logger engine. This string will be sent to the logger engine before any messages.
            virtual QString initializeString() const = 0;
            //! Function which is used to provide the finalization string for the logger engine. This string will be sent to the logger engine after all messages.
            virtual QString finalizeString() const = 0;
            //! Function which is called to format the message.
            virtual QString formatMessage(Logger::MessageType message_type, const QList<QVariant>& messages) const = 0;
            //! Function which provides a name for this formatting engine.
            virtual QString name() const = 0;
            //! Function which provides a file extension which will be used if the logger engine is a File logger engine.
            /*!
                If this is not applicable just return QString().
                Example format = QString("log"). Note that the point is not part of the extension.
                */
            virtual QString fileExtension() const = 0;
            //! Function which provides the end of line character used by the formatting engine.
            virtual QString endOfLineChar() const = 0;
        };
    }
}

#endif // ABSTRACTFORMATTINGENGINE_H
