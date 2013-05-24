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

#ifndef EXTENSIONSYSTEMCONFIG_H
#define EXTENSIONSYSTEMCONFIG_H

#include <IConfigPage>
#include <ObserverWidget>

#include <QWidget>

namespace Ui {
    class ExtensionSystemConfig;
}

namespace Qtilities {
    namespace ExtensionSystem {
        using namespace Qtilities::CoreGui;
        using namespace Qtilities::CoreGui::Interfaces;

        /*!
          \class ExtensionSystemConfig
          \brief The ExtensionSystemConfig class provides a ready to use configuration widget for the extension system.

        The ExtensionSystemConfig widget exposes settings of the extension system through a configuration page shown below:

        \image html extension_system_configuration_widget.jpg "Extension System Configuration Widget"

        To add this page to your application's ConfigurationWidget, do the following:
\code
EXTENSION_SYSTEM->initialize();

// Register extension system config page.
OBJECT_MANAGER->registerObject(EXTENSION_SYSTEM->configWidget());
\endcode

        For more information see the \ref page_extension_system article.
         */
        class ExtensionSystemConfig : public QWidget, public IConfigPage {
            Q_OBJECT
            Q_INTERFACES(Qtilities::CoreGui::Interfaces::IConfigPage)

            friend class ExtensionSystemCore;

        public:
            ExtensionSystemConfig(QWidget *parent = 0);
            ~ExtensionSystemConfig();

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
            void configPageApply() {}
            bool supportsApply() const { return false; }
            void configPageInitialize();

        public slots:
            void handleBtnDetailsClicked();
            void handleSelectionChanged(QList<QObject*> selection);
            void handleSelectionDoubleClicked(QObject* selection);
            void setStatusMessage(const QString& status_message);
            void updatePluginPaths();

        protected:
            void changeEvent(QEvent *e);

        private:
            void setPluginListWidget(QWidget* plugin_list_widget);
            ObserverWidget* observer_widget;
            bool initialized;
            Ui::ExtensionSystemConfig *ui;
        };
    }
}


#endif // EXTENSIONSYSTEMCONFIG_H
