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

#ifndef HELP_PLUGIN_CONFIG_H
#define HELP_PLUGIN_CONFIG_H

#include <IConfigPage.h>
#include <StringListWidget>

#include <QWidget>

namespace Ui {
    class HelpPluginConfig;
}

namespace Qtilities {
    namespace Plugins {
        namespace Help {
            using namespace Qtilities::CoreGui;
            using namespace Qtilities::CoreGui::Interfaces;

            /*!
            \class HelpPluginConfig
            \brief The HelpPluginConfig class provides a ready to use configuration widget for the help plugin.

            The HelpPluginConfig widget exposes settings of the help manager through a configuration page shown below:

            \image html config_page_help.jpg "Help Configuration Page"
             */
            class HelpPluginConfig : public QWidget, public IConfigPage {
                Q_OBJECT
                Q_INTERFACES(Qtilities::CoreGui::Interfaces::IConfigPage)

            public:
                HelpPluginConfig(QWidget *parent = 0);
                ~HelpPluginConfig();

                // --------------------------------
                // IObjectBase Implementation
                // --------------------------------
                QObject* objectBase() { return this; }
                const QObject* objectBase() const { return this; }

                // --------------------------------------------
                // IConfigPage Implementation
                // --------------------------------------------
                QIcon configPageIcon() const;
                QWidget* configPageWidget();
                QtilitiesCategory configPageCategory() const;
                QString configPageTitle() const;
                void configPageApply();
                bool supportsApply() const { return true; }
                void configPageInitialize();

            private slots:
                void handleFilesChanged(const QStringList& files);

            protected:
                void changeEvent(QEvent *e);

            private:
                Ui::HelpPluginConfig *ui;
                StringListWidget files_widget;
            };
        }
    }
}


#endif // HELP_PLUGIN_CONFIG_H
