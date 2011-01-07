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

#ifndef SESSIONLOGMODE_H
#define SESSIONLOGMODE_H

#include "IMode.h"

#include <QObject>

// Session Mode Parameters
const char * const CONTEXT_SESSION_LOG_MODE     = "Context.SessionLogMode";

using namespace Qtilities::CoreGui::Interfaces;

namespace Qtilities {
    namespace Plugins {
        namespace SessionLog {
            /*!
              \struct SessionLogModeData
              \brief The SessionLogModeData struct stores private data used by the SessionLogMode class.
             */
            struct SessionLogModeData;

            /*!
            \class SessionLogMode
            \brief The session log mode is used to display output from the %Qtilities %Logging module as a mode in the Qtilities::CoreGui::QtilitiesMainWindow.
              */
            class SessionLogMode : public QObject, public IMode
            {
                Q_OBJECT
                Q_INTERFACES(Qtilities::CoreGui::Interfaces::IMode)

                public:
                    SessionLogMode(QObject* parent = 0);
                    ~SessionLogMode();

                    // --------------------------------
                    // IObjectBase Implementation
                    // --------------------------------
                    QObject* objectBase() { return this; }
                    const QObject* objectBase() const { return this; }

                    // --------------------------------------------
                    // IMode Implementation
                    // --------------------------------------------
                    QWidget* modeWidget();
                    void initializeMode() {}
                    QIcon modeIcon() const;
                    bool setModeIcon(QIcon icon);
                    QString modeName() const;
                    QString contextString() const { return CONTEXT_SESSION_LOG_MODE; }

                public slots:
                    void handle_dockVisibilityChanged(bool visible);

                private:
                    SessionLogModeData* d;
            };
        }
    }
}

#endif // SESSIONLOGMODE_H
