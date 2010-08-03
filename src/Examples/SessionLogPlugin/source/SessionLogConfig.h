/****************************************************************************
**
** Copyright 2010, Jaco Naude
**
****************************************************************************/

#ifndef SESSIONLOGCONFIG_H
#define SESSIONLOGCONFIG_H

#include "IConfigPage.h"

#include <QObject>
#include <QStringList>
#include <QIcon>
#include <QWidget>

namespace Qtilities {
    namespace ApplicationGui {
        using namespace Qtilities::ApplicationGui::Interfaces;
        /*!
          \struct SessionLogConfigData
          \brief The SessionLogConfigData class stores private data used by the SessionLogConfig class.
         */
        struct SessionLogConfigData;

        /*!
        \class SessionLogConfig
        \brief The config page for the session log.
          */
        class SessionLogConfig : public QObject, public IConfigPage
        {
            Q_OBJECT
            Q_INTERFACES(Qtilities::ApplicationGui::Interfaces::IConfigPage)

        public:
            SessionLogConfig(QObject* parent = 0);
            ~SessionLogConfig();

            // --------------------------------------------
            // IConfigPage Implementation
            // --------------------------------------------
            QIcon configPageIcon() const;
            QWidget* configPageWidget();
            QStringList configPageTitle() const;
            void configPageApply();

        private:
            SessionLogConfigData* d;
        };
    }
}

#endif // SESSIONLOGCONFIG_H
