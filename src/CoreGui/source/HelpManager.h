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

#ifndef HELP_MANAGER_H
#define HELP_MANAGER_H

#include <QObject>
#include <QUrl>

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
        \brief A help manager which provides functionality to help developers integrate the Qt Help System into their applications.

        The help manager allows you to integrate the Qt Help System into your applications. You can access the help manager using the \p HELP_MANAGER macro directly, or
        through Qtilities::CoreGui::QtilitiesApplication::helpManager().

        The helpEngine() function provides a \p QHelpEngine which you can use throughout your application. To use it is simple, you need to register all \p qch files
        that you want to use using either registerFile() or registerFiles() and then initialize the plugin. For example:

\code
// We register two example files. The false parameter indicates that we don't want the registerFile()
// function to initialize the manager after the changes made to it.
HELP_MANAGER->registerFile("myFile1.qch",false);
HELP_MANAGER->registerFile("myFile2.qch",false);
HELP_MANAGER->initialize();

// Now that it was initialized we can use the helpEngine().
HELP_MANAGER->helpEngine();
\endcode

        When initializing the help manager, two tasks will be created:
        - One task for setting up the help manager.
        - One task for indexing the documentation.

        Both these task will be registered in the global object pool for you, thus it will show up in your Qtilities::CoreGui::TaskSummaryWidget widgets. After initialization you can change the
        files you want to show through registerFile(), registerFiles(), clearRegisteredFiles(), unregisterFile() and unregisterFiles(). After you have registered and unregistered the files you want,
        you can call initialize() again which will set up and index the help system for the registered files found through registeredFiles().

        When registering files from the Qt Resource System the help manager will automatically create temporary local copies of the files to register, thus you don't need to worry about that.

        The Qtilities::Plugins::Help plugin provides a ready to use GUI frontend for the help system which allows you to view the contents of all help files, view an index or search the documentation:

        \image html plugins_help.jpg "Help Plugin Showing GUI Frontend For The Help Manager"

        When using the %Qtilities extension system, you can easily let plugins register help files providing help for their functionality. The important thing to remember is to register the files in the
        \p initializeDependencies() function of your plugin implementation. For example:

\code
bool MyPlugins::initializeDependencies(QStringList *error_strings) {
    HELP_MANAGER->registerFile(":/MyPlugin/plugin_help.qch",false);
}
\endcode

        In time, the API of the help manager will be extended to allow deeper integration with applications.

        <i>This class was added in %Qtilities v1.1.</i>
          */
        class QTILITIES_CORE_GUI_SHARED_EXPORT HelpManager : public QObject
        {
            Q_OBJECT

        public:
            HelpManager(QObject* parent = 0);
            ~HelpManager();

            //! Returns a help engine to use in your application.
            /*!
              Make sure you call initialize() before using the help engine since the help engine is constructed in initialize().
              */
            QHelpEngine* helpEngine();
            //! Initializes the help engine.
            /*!
              Initialize will set up the internal help engine and start indexing of all documentation registered in the manager.
              */
            void initialize();

            //! Sets the home page of your application.
            void setHomePage(const QUrl& home_page);
            //! Gets the home page of your application.
            QUrl homePage() const;

            //! Register files in the help engine.
            /*!
              \note Registered files will only be saved between sessions when QtilitiesCoreApplication::qtilitiesSettingsEnabled() is enabled.
              */
            void clearRegisteredFiles(bool initialize_after_change = true);
            //! Register files in the help engine.
            /*!
              \note Registered files will only be saved between sessions when QtilitiesCoreApplication::qtilitiesSettingsEnabled() is enabled.
              */
            void registerFiles(const QStringList& files, bool initialize_after_change = true);
            //! Registers a file in the help engine.
            /*!
              \note Registered files will only be saved between sessions when QtilitiesCoreApplication::qtilitiesSettingsEnabled() is enabled.
              */
            void registerFile(const QString &file, bool initialize_after_change = true);
            //! Gets the registered files in the help engine.
            /*!
              \note Registered files will only be saved between sessions when QtilitiesCoreApplication::qtilitiesSettingsEnabled() is enabled.
              */
            QStringList registeredFiles() const;
            //! Unregister files in the help engine.
            /*!
              \note Registered files will only be saved between sessions when QtilitiesCoreApplication::qtilitiesSettingsEnabled() is enabled.
              */
            void unregisterFiles(const QStringList& files, bool initialize_after_change = true);
            //! Unregisters a file in the help engine.
            /*!
              \note Registered files will only be saved between sessions when QtilitiesCoreApplication::qtilitiesSettingsEnabled() is enabled.
              */
            void unregisterFile(const QString& file, bool initialize_after_change = true);
            //! Writes the help settings.
            void writeSettings();
            //! Reads the help settings.
            void readSettings(bool initialize_after_change = true);

            //! Formats file names into the format used by the help manager.
            static QString formatFileName(const QString& file_name);

            //! Request a help page to be shown in any help browsers connected to the help manager.
            /*!
              When the %Qtilities help plugin is loaded in your application, this function will load the requested page in the help plugin and
              if \p ensure_visible is true, the application will switch to the help mode provided by the help plugin.
              */
            void requestUrlDisplay(const QUrl& url, bool ensure_visible = true);

        private slots:
            //! Logs warning messages from the help engine in the logger.
            void logMessage(const QString& message);

        signals:
            //! Signal which is emitted when the files registered in the engine is changed.
            void registeredFilesChanged(const QStringList& files);
            //! Signal used to forward help display requests to any help browsers connected to the help manager.
            void forwardRequestUrlDisplay(const QUrl& url, bool ensure_visible = true);
            //! Home page changed signal.
            /*!
              <i>This signal was added in %Qtilities v1.3.</i>
              */
            void homePageChanged(const QUrl& url);

        private:
            //! Unregister all files.
            void unregisterAllNamespaces();

            HelpManagerPrivateData* d;
        };
    }
}

#endif // HELP_MANAGER_H
