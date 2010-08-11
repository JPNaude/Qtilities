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

#ifndef OBJECT_MANAGEMENT_MODE_WIDGET
#define OBJECT_MANAGEMENT_MODE_WIDGET

#include <QWidget>

#include <IFactory.h>
#include <Factory.h>
#include <Observer.h>
#include <ObserverWidget.h>

namespace Ui
{
    class ObjectManagementModeWidget;
}

using namespace Qtilities::Core;
using namespace Qtilities::CoreGui;
using namespace Qtilities::Core::Interfaces;

namespace Qtilities {
    namespace Examples {
        namespace ObjectManagement {
            /*!
              \struct ObjectManagementModeWidgetData
              \brief The ObjectManagementModeWidgetData struct stores private data used by the ObjectManagementModeWidget class.
             */
            struct ObjectManagementModeWidgetData;

            /*!
            \class ObjectManagementModeWidget
            \brief The ObjectManagementModeWidget is a mode widget which allows management of an Observer class instance using the ObserverWidget class.
              */
            class ObjectManagementModeWidget : public QWidget, public IFactory
            {
                Q_OBJECT
                Q_INTERFACES(Qtilities::Core::Interfaces::IFactory)

                public:
                    ObjectManagementModeWidget(QWidget *parent = 0);
                    ~ObjectManagementModeWidget();

                    // -----------------------------------------
                    // IFactory Implementation
                    // -----------------------------------------
                    QStringList factoryTags() const;
                    QObject* createInstance(const IFactoryData& ifactory_data);

                private slots:
                    void addExampleObjects();
                    void addObject_triggered(QObject* observer = 0);
                    void selectionChanged(QList<QObject*> new_selection);
                    void handle_newObserverWidgetCreated(ObserverWidget* new_widget);

                    void handle_actionShowWidget();
                    void handle_actionHideWidget();
                    void handle_sliderWidgetOpacity(int value);

                private:
                    Ui::ObjectManagementModeWidget *ui;
                    ObjectManagementModeWidgetData* d;
            };
        }
    }
}

#endif // OBJECT_MANAGEMENT_MODE_WIDGET
