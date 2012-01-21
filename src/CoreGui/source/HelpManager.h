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
              Make sure you call initialize() before using the help engine. The help plugin uses this same engine.
              */
            QHelpEngine* helpEngine();
            //! Initializes the help engine.
            /*!
              Initialize will set up the internal help engine and start indexing of all documentation registered in the manager.

              */
            void initialize();

            //! Register files in the help engine.
            /*!
              \note Registered files will only be saved between sessions when QtilitiesCoreApplication::qtilitiesSettingsPathEnabled() is enabled.
              */
            void clearRegisterFiles(bool initialize_after_change = true);
            //! Register files in the help engine.
            /*!
              \note Registered files will only be saved between sessions when QtilitiesCoreApplication::qtilitiesSettingsPathEnabled() is enabled.
              */
            void registerFiles(const QStringList& files, bool initialize_after_change = true);
            //! Registers a file in the help engine.
            /*!
              \note Registered files will only be saved between sessions when QtilitiesCoreApplication::qtilitiesSettingsPathEnabled() is enabled.
              */
            void registerFile(const QString &file, bool initialize_after_change = true);
            //! Gets the registered files in the help engine.
            /*!
              \note Registered files will only be saved between sessions when QtilitiesCoreApplication::qtilitiesSettingsPathEnabled() is enabled.
              */
            QStringList registeredFiles() const;
            //! Unregister files in the help engine.
            /*!
              \note Registered files will only be saved between sessions when QtilitiesCoreApplication::qtilitiesSettingsPathEnabled() is enabled.
              */
            void unregisterFiles(const QStringList& files, bool initialize_after_change = true);
            //! Unregisters a file in the help engine.
            /*!
              \note Registered files will only be saved between sessions when QtilitiesCoreApplication::qtilitiesSettingsPathEnabled() is enabled.
              */
            void unregisterFile(const QString& file, bool initialize_after_change = true);

        private slots:
            //! Logs warning messages from the help engine in the logger.
            void logMessage(const QString& message);

        private:
            //! Reads the help settings.
            void readSettings(bool initialize_after_change = true);
            //! Writes the help settings.
            void writeSettings();

            HelpManagerPrivateData* d;
        };
    }
}

#endif // HELP_MANAGER_H
