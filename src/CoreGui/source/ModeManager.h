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

#ifndef MODEWIDGET_H
#define MODEWIDGET_H

#include "QtilitiesCoreGuiConstants.h"
#include "QtilitiesCoreGui_global.h"
#include "IMode.h"

#include <QListWidget>

namespace Ui {
    class ModeManager;
}

class QListWidgetItem;

namespace Qtilities {
    namespace CoreGui {
        using namespace Qtilities::CoreGui::Interfaces;

        /*!
          \struct ModeManagerData
          \brief The ModeManagerData class stores private data used by the ModeManager class.
         */
        struct ModeManagerData;

        /*!
        \class ModeManager
        \brief The ModeManager allows management of application modes.

        A mode is any object implementing the Qtilities::CoreGui::Interfaces::IMode interface. Through this interface different application modes
        can be exposed and this class is responsible to manage all of these modes. The ModeManager class is intended to be used with Qtilities::CoreGui::QtilitiesMainWindow
        which responds to the changeCentralWidget() signal and displays the mode in the main window. The ModeManager class is a widget which will display a
        extended QListWidget with a list containing all the registered modes. The list widget can either display the modes in a left to right, or a top to bottom
        layout depending on the \p orientation parameter of the ModeManager constructor.

        Modes can be easily added using the addMode() or addModes() functions. To get a list of all current modes use the modes() function.

        At any given time a single mode must be active where the active mode will be selected in the mode list widget. To get the ID of the active mode
        use the activeMode() function. It is possible to change the active mode at any time through the setActiveMode() function.

        It is possible to specify the order in which modes must appear to the user, or to change it at any time through the setPreferredModeOrder() functions.
        In some cases it might be needed to disable one or more modes and this can be achieved through the setDisabledModes() function.

        By default the list widget is styled to look like a set of application modes, rather than a normal list view. The default stylesheet that is applied
        to the list is constructed as follows:

\code
QString stylesheet = "";
// Give the view a colored background:
stylesheet += "QListView { background-color: #FFFFFF; border-style: none; }";
// The text underneath the unselected items:
stylesheet += "QListView::item::text { font-weight: bold; border-style: none; color: black }";
// The text underneath the selected item:
stylesheet += "QListView::item:selected:active { font-weight: bold; border-style: none; color: white }";
stylesheet += "QListView::item:selected:!active { font-weight: bold; border-style: none; color: white }";
// Hover effect:
stylesheet += "QListView::item:hover { background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 white, stop: 0.4 #EEEEEE, stop:1 #CCCCCC); }";
// Selected item gradient:
stylesheet += "QListView::item:selected:active { background: #CCCCCC; }";
stylesheet += "QListView::item:selected:!active { background: #CCCCCC; }";
// The padding of items in the list:
if (d->orientation == Qt::Horizontal)
    stylesheet += "QListView::item { padding: 5px 1px 5px 1px;}";
else
    stylesheet += "QListView::item { padding: 5px 0px 5px 0px;}";
modeListWidget()->setStyleSheet(stylesheet);
\endcode

        It is possible to change and customize the style of the list through the modeListWidget() access function to the list widget.
          */
        class QTILITIES_CORE_GUI_SHARED_EXPORT ModeManager : public QObject {
            Q_OBJECT

        public:
            //! Default constructor
            /*!
              \param orientation The orientation of the mode list widget. When \p Qt::Horizontal the list will be a top to bottom list and when
              \p Qt::Vertical the list will be a left to right list.
              */
            ModeManager(Qt::Orientation orientation, QObject *parent = 0);
            ~ModeManager();

            // ----------------------------------
            // QListWidget access functions
            // ----------------------------------
            QListWidget* modeListWidget();

            // ----------------------------------
            // Functions related to adding/removing modes
            // ----------------------------------
            //! Adds a mode to the mode widget.
            /*!
              This function can be called at any time and will cause the mode list to be refreshed.
              */
            void addMode(IMode* mode, bool initialize_mode = true, bool refresh_list = true);
            //! Adds a list of modes to the main window.
            /*!
              This function can be called at any time and will cause the mode list to be refreshed.
              */
            void addModes(QList<IMode*> modes, bool initialize_modes = true);
            //! Adds a list of modes to the main window. This call will attempt to cast each object in the list to IMode* and add the successfull interfaces to the main window.
            /*!
              This function can be called at any time and will cause the mode list to be refreshed.
              */
            void addModes(QList<QObject*> modes, bool initialize_modes = true);
            //! A list of the modes in this mode widget.
            /*!
              \sa addMode(), addModes()
              */
            QList<IMode*> modes() const;

            // ----------------------------------
            // Functions related to mode activity
            // ----------------------------------
            //! Returns the active mode interface.
            IMode* activeModeIFace() const;
            //! Returns the active mode ID.
            int activeModeID() const;
            //! Returns the active mode name.
            QString activeModeName() const;
        public slots:
            //! Slot through which a new mode can be set by specifying the mode ID.
            /*!
              \param mode_id The mode ID of the mode. \sa IMode::modeID().

              \note If \p mode_id appears in the list of disabled modes, this function does nothing.
              */
            void setActiveMode(int mode_id);
            //! Slot through which a new mode can be set by specifying the mode name.
            /*!
              \param mode_name The name of the mode. \sa IMode::text().

              \note If \p mode_id appears in the list of disabled modes, this function does nothing.
              */
            void setActiveMode(const QString& mode_name);
            //! Slot through which a new mode can be set by specifying the mode interface.
            /*!
              \note If \p mode_id appears in the list of disabled modes, this function does nothing.
              */
            void setActiveMode(IMode* mode_iface);

            // ----------------------------------
            // Functions related to preferred mode order
            // ----------------------------------
        public:
            //! Sets the preferred order of modes in the mode widget using the names of the modes.
            /*!
              A QStringList with the names of the modes in the order in which they should appear. The first item in the list will appear first, the last item last.
              If a mode exists which does not appear in the order list, it will be placed after all modes which appear in the list. In the same way,
              if a mode name is specified in the preferred list which does not appear in the list of modes it will be placed after all modes which appear in the list.

              <b>Important:</b> The modes listed in \p preferred_order must be registered in the mode manager for this function to work. If the modes are not there yet,
              you must use the QList<int> parameter alternative of this function.

              \note This function will automatically refresh the view if modes are already present at the time the function is called.
              */
            void setPreferredModeOrder(const QStringList& preferred_order);
            //! Sets the preferred order of modes in the mode widget using the unique IDs of the modes.
            /*!
              An integer list with the ids of the modes in the order in which they should appear. The first item in the list will appear first, the last item last.
              If a mode exists which does not appear in the order list, it will be placed after all modes which appear in the list. In the same way,
              if a mode name is specified in the preferred list which does not appear in the list of modes it will be placed after all modes which appear in the list.

              \note This function will automatically refresh the view if modes are already present at the time the function is called.
              */
            void setPreferredModeOrder(QList<int> preferred_order);
            //! Sets the preferred order of modes in the mode widget using the IMode interfaces of the modes.
            /*!
              A QStringList with the names of the modes in the order in which they should appear. The first item in the list will appear first, the last item last.
              If a mode exists which does not appear in the order list, it will be placed after all modes which appear in the list. In the same way,
              if a mode name is specified in the preferred list which does not appear in the list of modes it will be placed after all modes which appear in the list.

              <b>Important:</b> The modes listed in \p preferred_order must be registered in the mode manager for this function to work. If the modes are not there yet,
              you must use the QList<int> parameter alternative of this function.

              \note This function will automatically refresh the view if modes are already present at the time the function is called.
              */
            void setPreferredModeOrder(QList<IMode*> preferred_order);
            //! Gets the preferred mode order's mode names where the first items in the list will appear first.
            /*!
              \sa setPreferredModeOrder()
              */
            QStringList preferredModeOrderNames() const;
            //! Gets the preferred mode order's mode interfaces where the first items in the list will appear first.
            /*!
              \sa setPreferredModeOrder()
              */
            QList<IMode*> preferredModeOrderIFaces() const;
            //! Gets the preferred mode order's mode IDs where the first items in the list will appear first.
            /*!
              \sa setPreferredModeOrder()
              */
            QList<int> preferredModeOrderIDs() const;

            // ----------------------------------
            // Functions related to mode enabling/disabling
            // ----------------------------------
            //! Sets the modes which should be disabled using the mode names.
            /*!
              To enable all modes, just pass an empty QStringList as the \p disabled_modes parameter.

              \sa disabledModeNames()
              */
            void setDisabledModes(const QStringList& disabled_modes);
            //! Sets the modes which should be disabled using the unique mode IDs.
            /*!
              To enable all modes, just pass an empty list as the \p disabled_modes parameter.

              \sa disabledModeNames()
              */
            void setDisabledModes(QList<int> disabled_modes);
            //! Sets the modes which should be disabled using the IMode interfaces.
            /*!
              To enable all modes, just pass an empty list as the \p disabled_modes parameter.

              \sa disabledModeNames()
              */
            void setDisabledModes(QList<IMode*> disabled_modes);
            //! Gets the mode names which are disabled.
            /*!
              \sa setDisabledModes()
              */
            QStringList disabledModeNames() const;
            //! Gets the mode interfaces which are disabled.
            /*!
              \sa setDisabledModes()
              */
            QList<IMode*> disabledModeIFaces() const;
            //! Gets the mode ids which are disabled.
            /*!
              \sa setDisabledModes()
              */
            QList<int> disabledModeIDs() const;
            //! Checks if a mode is valid, that is it a mode with the same ID is not yet registered in the mode manager.
            bool isValidModeID(int mode_id) const;

        private slots:
            //! Handles selection changes in the top to bottom mode widget.
            void handleModeListCurrentItemChanged(QListWidgetItem * item);
            //! Handle mode shortcut activation.
            void handleModeShortcutActivated();

        signals:
            //! This signal is emitted with the new active mode widget as the \p new_central_widget parameter as soon as the active mode changes.
            void changeCentralWidget(QWidget* new_central_widget);

        private:
            //! This function that should be used to add mode items to the list. This will take mode ordering into account.
            void refreshList();
            //! Converts a list of mode names to the corresponding list of mode ids.
            /*!
              \note If the name_list constains multiple entries, only the first instance will be converted.
              */
            QList<int> modeNamesToIDs(QStringList name_list) const;
            //! Converts a list of mode interfaces to the corresponding list of mode ids.
            /*!
              \note If the imode_list constains multiple entries, only the first instance will be converted.
              */
            QList<int> modeIFacesToIDs(QList<IMode*> imode_list) const;
            //! Converts a list of mode IDs to a list of corresponding mode names.
            QStringList modeIDsToNames(QList<int> mode_ids) const;
            //! Converts a list of mode interfaces to a list of corresponding mode interfaces.
            QList<IMode*> modeIDsToIFaces(QList<int> mode_ids) const;

            ModeManagerData* d;
        };
    }
}

#endif // MODEWIDGET_H
