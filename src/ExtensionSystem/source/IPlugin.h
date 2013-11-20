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

#ifndef IPLUGIN_H
#define IPLUGIN_H

#include "ExtensionSystem_global.h"

#include <IObjectBase>
#include <QtilitiesCategory>
#include <VersionInformation>

#include <QObject>
#include <QList>
#include <QStringList>

namespace Qtilities {
    namespace ExtensionSystem {
        using namespace Qtilities::Core;
        using namespace Qtilities::Core::Interfaces;

        //! Namespace containing available interfaces which forms part of the ExtensionSystem Module.
        namespace Interfaces {
            /*!
            \class IPlugin
            \brief Interface used to communicate with plugins.
              */
            class EXTENSION_SYSTEM_SHARED_EXPORT IPlugin : virtual public IObjectBase
            {
            public:
                IPlugin() {
                    d_state = Functional;
                }
                virtual ~IPlugin() {}

                // -----------------------------------
                // Plugin State Functions
                // -----------------------------------
                //! The possible states in which a plugin can be.
                enum PluginState {
                    Functional = 0,             /*!< The plugin is fully functional and no errors were reported during initialization and dependency initialization. */
                    IncompatibleState = 1,      /*!< The plugin is loaded, but indicated that it is incompatible with the current version of the application it was loaded in. See errorMsg() for a list of error messages. */
                    ErrorState = 2,             /*!< The plugin is loaded, but errors occured. See errorMsg() for a list of error messages. */
                    InActive = 4                /*!< The plugin was loaded but not initialized. \sa ExtensionSystemCore::setInactivePlugins().  */
                };
                Q_DECLARE_FLAGS(PluginStateFlags, PluginState)
                Q_FLAGS(PluginStateFlags)

                //! Function which returns a string associated with a the plugin's state.
                QString pluginStateString() const {
                    if (d_state == Functional)
                        return QObject::tr("Functional");
                    else if (d_state == InActive)
                        return QObject::tr("Inactive");
                    else {
                        QString combined_error_str;
                        if (d_state == ErrorState)
                            combined_error_str = QObject::tr("Error State");
                        if (d_state == IncompatibleState)
                            combined_error_str = QObject::tr("Incompatible");
                        if (d_state & ErrorState && d_state & IncompatibleState)
                            combined_error_str = QObject::tr("Error State, Incompatible");
                        return combined_error_str;
                    }

                    return QString();
                }
                //! Adds a PluginState the PluginStateFlags of the plugin.
                void addPluginState(PluginState state) { d_state = d_state | state; }
                //! Gets the plugin state.
                PluginStateFlags pluginState() const { return d_state; }
                //! Sets the plugin file name.
                void setPluginFileName(const QString& file_name) { d_file_name = file_name; }
                //! Gets the plugin file name.
                QString pluginFileName() const { return d_file_name; }
                //! Adds a error message to list of error associated with the plugin.
                void addErrorMessage(const QString& error_string) { d_error_strings << error_string; }
                //! Adds error messages to list of error associated with the plugin.
                void addErrorMessages(const QStringList& error_strings) { d_error_strings << error_strings; }
                //! Gets the error messages associated with the plugin.
                QStringList errorMessages() const { return d_error_strings; }
                //! Indicates if there are errors messages associated with this plugin.
                bool hasErrors() const { return (d_error_strings.count() > 0); }

                //! This function is called when the plugin is loaded.
                /*!
                    Use this implementation to register objects in the global object pool. Typically one plugin
                    will look for objects in other plugins which implements a specific interface. These objects,
                    the ones implementing the interfaces, must be added to the global object pool in this function.

                    \param arguments A list of possible arguments to be sent to the plugin during initialization.
                    \param error_strings Plugins can add error/warning strings to this list when they detect errors.
                    \returns True if the plugin was successfully initialized, false otherwise.
                    */
                virtual bool initialize(const QStringList &arguments, QStringList *error_strings) = 0;
                //! This function is called when all the plugins in the system were loaded and initialized.
                /*!
                    If you have an object which looks for specific interfaces in the global object pool, do the search
                    here.

                    \param error_strings Plugins can add error/warning strings to this list when they detect errors.
                    \returns True if dependancies were successfully initialized, false otherwise.
                    */
                virtual bool initializeDependencies(QStringList *error_strings) = 0;
                //! This function is called before a plugin is unloaded in the system.
                /*!
                    If your plugin needs to do something before it is unloaded (save settings for example), do it in here.
                    */
                virtual void finalize() { }

                // -----------------------------------
                // Plugin Information Functions
                // -----------------------------------
                //! The name of the plugin.
                virtual QString pluginName() const = 0;
                //! The category of the plugin.
                virtual QtilitiesCategory pluginCategory() const = 0;
                //! The version information of the plugin.
                /*!
                  If your plugin does not depend on the application it is used in, you can return an empty list of supported versions (this is the default) to let the extension system know not to check the compatibility of your plugin.

                  Here is an example implementation of this function:

\code
// Create a version information object with the version of the plugin:
VersionInformation version_info(1,0,1);

// Next add a compatible application version:
VersionNumber compatible_version(1,0,0);
// Add it as a compatible version to our version information for this plugin:
version_info << compatible_version;

// Return the version information object:
return version_info;
\endcode
                  */
                virtual VersionInformation pluginVersionInformation() const = 0;
                //! The name of the plugin's publisher.
                virtual QString pluginPublisher() const = 0;
                //! The website of the plugin's publisher.
                virtual QString pluginPublisherWebsite() const = 0;
                //! The contact details (in the form of an email address) of the plugin's publisher.
                virtual QString pluginPublisherContact() const = 0;
                //! A description for the plugin.
                virtual QString pluginDescription() const = 0;
                //! The copyright information of the plugin.
                virtual QString pluginCopyright() const = 0;
                //! The licensing details of the plugin.
                virtual QString pluginLicense() const = 0;

            private:
                PluginStateFlags d_state;
                QStringList d_error_strings;
                QString d_file_name;
            };
        }
    }
}

Q_DECLARE_INTERFACE(Qtilities::ExtensionSystem::Interfaces::IPlugin,"com.Qtilities.ExtensionSystem.IPlugin/1.0");

#endif // IPLUGIN_H
