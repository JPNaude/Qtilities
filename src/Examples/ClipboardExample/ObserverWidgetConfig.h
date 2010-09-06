/****************************************************************************
**
** Copyright (c) 2009-2010, Jaco Naude
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

#ifndef OBSERVERWIDGETCONFIG_H
#define OBSERVERWIDGETCONFIG_H

#include <QWidget>

#include <QtilitiesCoreGui>
using namespace QtilitiesCoreGui;

namespace Ui {
    class ObserverWidgetConfig;
}

namespace Qtilities {
    namespace Examples {
        //! Namespace containing all the classes which forms part of the Clipboard Example.
        namespace Clipboard {
            /*!
            \class ObserverWidgetConfig
            \brief The ObserverWidgetConfig widget is used in the Clipboard Example to demonstrate the settings update request capability of QtilitiesApplication.

            QtilitiesApplication::newSettingsUpdateRequest() allows settings update requests to be sent anywhere in an application. This
            allows objects which depends on the settings to update themselves when the settings updates.

            This class shows options for the two observer widgets found in the Clipboard Example. When the user clicks
            apply, the widgets will receive the settings update requests and update themselves accordingly. The update requests
            are delivered by the QtilitiesApplication::instance()->settingsUpdateRequest() signal.
              */
            class ObserverWidgetConfig : public QWidget, public IConfigPage {
                Q_OBJECT
                Q_INTERFACES(Qtilities::CoreGui::Interfaces::IConfigPage)

            public:
                ObserverWidgetConfig(QWidget *parent = 0);
                ~ObserverWidgetConfig();

                // --------------------------------
                // IObjectBase Implementation
                // --------------------------------
                QObject* objectBase() { return this; }

                // --------------------------------------------
                // IConfigPage Implementation
                // --------------------------------------------
                QIcon configPageIcon() const;
                QWidget* configPageWidget();
                QtilitiesCategory configPageCategory() const;
                QString configPageTitle() const;
                void configPageApply();
                bool supportsApply() const { return true; }

            public slots:
                void handle_comboWidgetMode1(const QString& new_value);
                void handle_comboWidgetMode2(const QString& new_value);

            protected:
                void changeEvent(QEvent *e);

            private:
                Ui::ObserverWidgetConfig *ui;
            };
        }
    }
}

#endif // OBSERVERWIDGETCONFIG_H
