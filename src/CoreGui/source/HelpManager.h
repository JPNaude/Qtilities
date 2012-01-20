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

#ifndef HELP_MANAGER_H
#define HELP_MANAGER_H

#include <QObject>

class QHelpEngine;

#include "QtilitiesCoreGui_global.h"

namespace Qtilities {
    namespace CoreGui {

        /*!
        \struct HelpManagerPrivateData
        \brief A structure storing private data in the HelpManager class.
          */
        struct HelpManagerPrivateData;

        /*!
        \class HelpManager
        \brief A class which represents a help manager.

          */
        class QTILITIES_CORE_GUI_SHARED_EXPORT HelpManager : public QObject
        {
            Q_OBJECT

        public:
            HelpManager(QObject* parent = 0);
            ~HelpManager();

            //! Returns a help engine to use in your application.
            /*!
              The help plugin uses this same engine.
              */
            QHelpEngine* helpEngine();
            //! Sets the registered files in the help engine.
            void setRegisteredFiles(const QStringList& files);
            //! Gets the registered files in the help engine.
            QStringList registeredFiles() const;

        private slots:
            //! Logs warning messages from the help engine in the logger.
            void logMessage(const QString& message);

        private:
            HelpManagerPrivateData* d;
        };
    }
}

#endif // HELP_MANAGER_H
