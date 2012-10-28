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
