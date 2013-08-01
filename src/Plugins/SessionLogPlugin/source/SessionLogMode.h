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
              \struct SessionLogModePrivateData
              \brief The SessionLogModePrivateData struct stores private data used by the SessionLogMode class.
             */
            struct SessionLogModePrivateData;

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

                signals:
                    void modeIconChanged();

                public slots:
                    void handle_dockVisibilityChanged(bool visible);

                private:
                    SessionLogModePrivateData* d;
            };
        }
    }
}

#endif // SESSIONLOGMODE_H
