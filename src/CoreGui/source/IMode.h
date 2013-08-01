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

#ifndef IMODE_H
#define IMODE_H

#include "QtilitiesCoreGui_global.h"
#include "QtilitiesCoreGuiConstants.h"

#include <IObjectBase>
#include <IContext>

#include <QWidget>
#include <QIcon>

namespace Qtilities {
    namespace CoreGui {
        namespace Interfaces {
            using namespace Qtilities::Core::Interfaces;

            /*!
            \class IMode
            \brief Used by the ModeManager to communicate with child modes.
              */
            class QTILITIES_CORE_GUI_SHARED_EXPORT IMode: virtual public IObjectBase, public IContext
            {

            public:               
                IMode() : d_mode_id(-1) {
                    d_target_manager_ids << Qtilities::CoreGui::Constants::qti_def_DEFAULT_MODE_MANAGER;
                }
                virtual ~IMode() {}

                //! The main window's central widget for the mode.
                /*!
                  This widget must have a proper layout in order to be displayed correctly.
                  */
                virtual QWidget* modeWidget() = 0;
                //! This function is called in the initializeDependencies() function, thus all objects implementing interfaces in which a mode might be interested will be present in the global object pool.
                virtual void initializeMode() = 0;
                //! An icon for the mode.
                /*!
                  The expected mode icon size is 48x48.

                  \sa setModeIcon()
                  */
                virtual QIcon modeIcon() const = 0;
                //! A function which can be implemented by modes if they allow their icon to be changed. By default this function does nothing and returns false.
                /*!
                  The expected mode icon size is 48x48.

                  \note When implementing this function, also implement modeIconChanged() as a signal and emit it in the function implementation.

                  \sa modeIcon()
                  */
                virtual bool setModeIcon(QIcon icon) { Q_UNUSED(icon) return false; }
                //! The name of the mode which is the text used to represent it.
                virtual QString modeName() const = 0;
                //! Function which can be implemented as a signal in subclasses of IMode to notify the mode manager(s) in which the mode is present that the modeIcon() changed.
                /*!
                 * By default this function does nothing.
                 *
                 * \note To make use of this function in a mode implementation, reimplement it as a signal.
                 *
                 * <i>This function was added in %Qtilities v1.5.</i>
                 */
                virtual void modeIconChanged() {}
                //! Returns a context string for the context associated with this mode.
                /*!
                  By default no context will be associated with a mode.
                  */
                virtual QString contextString() const { return QString(); }
                //! Returns a help ID for this context.
                virtual QString contextHelpId() const { return QString(); }
                //! Returns a unique ID for this mode.
                /*!
                    A mode ID is an unique number which is associated with a mode in an application. This allows modes with the same name to be added if desired. From the developer's perspective you don't have to reimplement this function in your interface implementation. In that case the Qtilities::CoreGui::ModeManager class will assign an unique mode ID when the mode is added to the mode manager and a debug message will be printed with information about the newly assigned mode ID.

                    It is however desired to specify your own mode ID in some cases, more specifically if you use the Qtilities::CoreGui::DynamicSideWidgetViewer widget in your application and you want dynamic side viewer widgets to appear only in specific application modes. To achieve this
                    your modeID() must appear in the list of Qtilities::CoreGui::Interfaces::ISideViewerWidget::destinationModes() for a specific side viewer widget
                    implementation.

                    \note Mode IDs available for user modes range from 100 - 999. The mode manager starts to auto-assign unique mode IDs from 1000 onwards for modes which does not have their own IDs.
                  */
                virtual int modeID() const { return d_mode_id; }
                //! Sets the mode ID for this mode.
                /*!
                  When your IMode implementation does not reimplement modeID(), it will automatically return the mode ID which was assigned by the ModeManager.
                  If you want to use your own mode ID you must reimplement modeID() and return your mode there, or set it using this function.
                  */
                inline void setModeID(int mode_id) { d_mode_id = mode_id; }
                //! This function is called when the mode is about to be activated. That is, when the user clicks on the mode and it is going to become the active mode.
                /*!
                  By default this function does nothing. Modes that require this functionality must reimplement this virtual function.

                  \sa justActivated()
                  */
                virtual void aboutToBeActivated() {}
                //! This function is called after a mode was activated. That is, when the user clicked on the mode and it became the active mode.
                /*!
                  By default this function does nothing. Modes that require this functionality must reimplement this virtual function.

                  \sa aboutToBeActivated()
                  */
                virtual void justActivated() {}

                //! Gets the list of mode managers in which this mode must appear.
                /*!
                  The default implementation returns qti_def_DEFAULT_MODE_MANAGER which is used by the mode manager constructed by QtilitiesMainWindow.

                  \sa setTargetManagerIDs()
                  */
                QList<int> targetManagerIDs() const {
                    return d_target_manager_ids;
                }
                //! Sets the list of mode managers in which this mode must appear.
                /*!
                  \sa targetManagerIDs()
                  */
                void setTargetManagerIDs(QList<int> new_target_manager_ids) {
                    d_target_manager_ids = new_target_manager_ids;
                }

                //! Returns the supported locations of extension widgets in this mode.
                /*!
                 * The default implementation returns 0 which is equal to no locations.
                 *
                 * \returns (int) QtilitiesMainWindow::ModeLayoutFlags representing the supported extension widget locations.
                 *
                 * <i>This function was added in %Qtilities v1.3.</i>
                 */
                int supportedExtensionWidgetLocations() const {
                    return 0;
                }

            private:
                int d_mode_id;
                QList<int> d_target_manager_ids;
            };
        }
    }
}

Q_DECLARE_INTERFACE(Qtilities::CoreGui::Interfaces::IMode,"com.Qtilities.CoreGui.IMode/1.0")

#endif // IMODE_H
