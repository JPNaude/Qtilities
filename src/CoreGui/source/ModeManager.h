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

#ifndef MODEWIDGET_H
#define MODEWIDGET_H

#include "QtilitiesCoreGuiConstants.h"
#include "QtilitiesCoreGui_global.h"
#include "IMode.h"

#include <QListWidget>
#include <QStack>

namespace Ui {
    class ModeManager;
}

class QListWidgetItem;

namespace Qtilities {
    namespace CoreGui {
        using namespace Qtilities::CoreGui::Interfaces;

        /*!
          \struct ModeManagerPrivateData
          \brief The ModeManagerPrivateData class stores private data used by the ModeManager class.
         */
        struct ModeManagerPrivateData;

        /*!
        \class ModeManager
        \brief The ModeManager allows management of application modes.

        A mode is any object implementing the Qtilities::CoreGui::Interfaces::IMode interface. Through this interface different application modes can be exposed and this class is responsible to manage all of these modes. The ModeManager class is intended to be used with Qtilities::CoreGui::QtilitiesMainWindow
        which responds to the changeCentralWidget() signal and displays the mode in the main window. The ModeManager class is a widget which will display a extended QListWidget with a list containing all the registered modes. The list widget can either display the modes in a left to right, or a top to bottom
        layout depending on the \p orientation parameter of the ModeManager constructor.

        Modes can be easily added using the addMode() or addModes() functions. To get a list of all current modes use the modes() function.

        At any given time a single mode must be active where the active mode will be selected in the mode list widget. To get the ID of the active mode use the activeMode() function. It is possible to change the active mode at any time through the setActiveMode() function.

        It is possible to specify the order in which modes must appear to the user, or to change it at any time through the setPreferredModeOrder() functions. In some cases it might be needed to disable one or more modes and this can be achieved through the setDisabledModes() function.

        By default the list widget is styled to look like a set of application modes, rather than a normal list view. The default stylesheet that is applied to the list is constructed as follows:

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
ModeListWidget()->setStyleSheet(stylesheet);
\endcode

        It is possible to change and customize the style of the list through the ModeListWidget() access function to the list widget.

          */
        class QTILITIES_CORE_GUI_SHARED_EXPORT ModeManager : public QObject {
            Q_OBJECT

        public:
            //! Default constructor
            /*!
              \param orientation The orientation of the mode list widget. When \p Qt::Horizontal the list will be a top to bottom list and when
              \p Qt::Vertical the list will be a left to right list.
              */
            ModeManager(int manager_id, Qt::Orientation orientation, QObject *parent = 0);
            ~ModeManager();

            // ----------------------------------
            // QListWidget access functions
            // ----------------------------------           
            //! Sets the minimum items size of mode items in the list widget.
            /*!
              \param size The minimum size. Dimensions < 0 will be ignored.

              \sa setMaximumItemSize(), maximumItemSize(), minimumItemSize()

              <i>This function was added in %Qtilities v1.1.</i>
              */
            void setMinimumItemSize(QSize size);
            //! Sets the maximum items size of mode items in the list widget.
            /*!
              \returns The maximum size. Dimensions < 0 will be ignored.

              \sa setMinimumItemSize(), maximumItemSize(), minimumItemSize()

              <i>This function was added in %Qtilities v1.1.</i>
              */
            void setMaximumItemSize(QSize size);
            //! Gets the minimum items size of mode items in the list widget.
            /*!
              \param size The minimum size. Dimensions < 0 will be ignored.

              \sa setMaximumItemSize(), setMinimumItemSize(), maximumItemSize();

              <i>This function was added in %Qtilities v1.1.</i>
              */
            QSize minimumItemSize() const;
            //! Gets the maximum items size of mode items in the list widget.
            /*!
              \returns The maximum size. Dimensions < 0 will be ignored.

              \sa minimumItemSize(), setMinimumItemSize(), setMaximumItemSize()

              <i>This function was added in %Qtilities v1.1.</i>
              */
            QSize maximumItemSize() const;

            //! Gets access to the QListWidget which is used to represent the modes.
            QListWidget* modeListWidget();


            // ----------------------------------
            // Functions related to adding/removing modes
            // ----------------------------------
            //! Refreshes the list of modes in this mode manager by doing a search through the global object pool.
            /*!
              This function will search the global object pool and automatically add all found modes. A debug message with information about the found pages will be created. All found modes will automatically be initialized.

              More modes can be added at a later stage using the addMode() and addModes() functions. A list of modes can be found using modes().

              \note This function only refreshes all modes, it does not populate the mode list widget. To do that, call initialize().
              */
            void refreshModeList();
            //! Initializes the mode widget by displaying all current modes.
            /*!
              If there are no modes present in the mode manager, refreshModeList() will be called first. However if modes
              are present this step will be skipped.

              Initialization will update the list widget displaying modes.

              \sa refreshModeList()
              */
            void initialize();
            //! Adds a mode to the mode widget.
            /*!
              This function can be called at any time and will cause the mode list to be refreshed.

              \param mode The mode to be added.
              \param initialize_mode When true, the mode will be initialized and its context will be added to the context manager if it is not empty.
              \param refresh_list When true, the mode list widget will be refreshed.

              \note The mode will only be added if it specifies this manager as one of its manager IDs.
              */
            void addMode(IMode* mode, bool initialize_mode = true, bool refresh_list = true);
            //! Adds a list of modes to the main window.
            /*!
              This function can be called at any time and will cause the mode list to be refreshed.

              \param modes A list of modes to be added.
              \param initialize_mode When true, each mode will be initialized and its context will be added to the context manager if it is not empty.
              \param refresh_list When true, the mode list widget will be refreshed.

              \note Only modes which specify this manager as one of their manager IDs will be added.
              */
            void addModes(QList<IMode*> modes, bool initialize_modes = true, bool refresh_list = true);
            //! Adds a list of modes to the main window. This call will attempt to cast each object in the list to IMode* and add the Successful interfaces to the main window.
            /*!
              This function can be called at any time and will cause the mode list to be refreshed.

              \param modes A list of modes to be added.
              \param initialize_mode When true, each mode will be initialized and its context will be added to the context manager if it is not empty.
              \param refresh_list When true, the mode list widget will be refreshed.

              \note Only modes which specify this manager as one of their manager IDs will be added.
              */
            void addModes(QList<QObject*> modes, bool initialize_modes = true, bool refresh_list = true);
            //! A list of the modes in this mode widget.
            /*!
              \sa addMode(), addModes()
              */
            QList<IMode*> modes() const;
            //! Function which returns a string representation of the shortcut assigned to a specific mode.
            QString modeShortcut(int mode_id) const;
            //! Returns an unique ID for this mode manager.
            /*!
              The Qtilities::CoreGui::Interfaces::IMode interface allows you to specify the mode manager in which the mode must be shown. This allows you to have multiple modes in your application.

              The mode ID for the manager can be set using setManagerID() or in the constructor. By default the manager ID will be -1 and no modes will be assigned to it.

              \note Manager IDs available for user managers range from 100 - 999. The default manager created by Qtilities::CoreGui::QtilitiesMainWindow uses the mode defined by qti_def_DEFAULT_MODE_MANAGER.

              \sa setManagerID().
              */
            int managerID() const;
            //! Sets the unique ID of this mode manager.
            /*!
              \sa managerID()
              */
            void setManagerID(int manager_id);

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
            //! Slot which gets notified when a mode's icon changed.
            /*!
             * <i>This function was added in %Qtilities v1.5.</i>
             */
            void handleModeIconChanged();
            //! Slot through which a new mode can be set by specifying the mode ID.
            /*!
              \param mode_id The mode ID of the mode. \sa IMode::modeID().
              \param refresh_list When true, the mode list widget will be refreshed.

              \note If \p mode_id appears in the list of disabled modes, this function does nothing.
              */
            void setActiveMode(int mode_id, bool refresh_list = false);
            //! Slot through which a new mode can be set by specifying the mode name.
            /*!
              \param mode_name The name of the mode. \sa IMode::text().
              \param refresh_list When true, the mode list widget will be refreshed.

              \note If \p mode_id appears in the list of disabled modes, this function does nothing.
              */
            void setActiveMode(const QString& mode_name, bool refresh_list = false);
            //! Slot through which a new mode can be set by specifying the mode interface.
            /*!
              \note If \p mode_id appears in the list of disabled modes, this function does nothing.
              \param refresh_list When true, the mode list widget will be refreshed.
              */
            void setActiveMode(IMode* mode_iface, bool refresh_list = false);

            // ----------------------------------
            // Functions related to preferred mode order
            // ----------------------------------
        public:
            //! Sets the preferred order of modes in the mode widget using the names of the modes.
            /*!
              A QStringList with the names of the modes in the order in which they should appear. The first item in the list will appear first, the last item last.
              If a mode exists which does not appear in the order list, it will be placed after all modes which appear in the list. In the same way,
              if a mode name is specified in the preferred list which does not appear in the list of modes it will be placed after all modes which appear in the list.

              <b>Important:</b> The modes listed in \p preferred_order must be registered in the mode manager for this function to work. If the modes are not there yet, you must use the QList<int> parameter alternative of this function.

              \note This function will automatically refresh the view if modes are already present at the time the function is called.
              */
            void setPreferredModeOrder(const QStringList& preferred_order, bool refresh_list = true);
            //! Sets the preferred order of modes in the mode widget using the unique IDs of the modes.
            /*!
              An integer list with the ids of the modes in the order in which they should appear. The first item in the list will appear first, the last item last.
              If a mode exists which does not appear in the order list, it will be placed after all modes which appear in the list. In the same way,
              if a mode name is specified in the preferred list which does not appear in the list of modes it will be placed after all modes which appear in the list.

              \note This function will automatically refresh the view if modes are already present at the time the function is called.
              */
            void setPreferredModeOrder(QList<int> preferred_order, bool refresh_list = true);
            //! Sets the preferred order of modes in the mode widget using the IMode interfaces of the modes.
            /*!
              A QStringList with the names of the modes in the order in which they should appear. The first item in the list will appear first, the last item last.
              If a mode exists which does not appear in the order list, it will be placed after all modes which appear in the list. In the same way,
              if a mode name is specified in the preferred list which does not appear in the list of modes it will be placed after all modes which appear in the list.

              <b>Important:</b> The modes listed in \p preferred_order must be registered in the mode manager for this function to work. If the modes are not there yet, you must use the QList<int> parameter alternative of this function.

              \note This function will automatically refresh the view if modes are already present at the time the function is called.
              */
            void setPreferredModeOrder(QList<IMode*> preferred_order, bool refresh_list = true);
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
            //! Checks if a mode is registered in the mode manager.
            bool containsModeId(int mode_id) const;
            //! Gets if the mode manager registers application wide shortcuts for the modes shown.
            /*!
              True by default.

              \sa setRegisterModeShortcuts()
              */
            bool registerModeShortcuts() const;
            //! Sets if the mode manager registers application wide shortcuts for the modes shown.
            /*!
              True by default.

              \note This function must be called before initializing the design the first time.

              \sa registerModeShortcuts()
              */
            void setRegisterModeShortcuts(bool register_shortcuts);

            //! Returns a QStack storing the ids of previously active modes.
            /*!
             * <i>This function was added in %Qtilities v1.2.</i>
             */
            QStack<int> previousModeIDs() const;
            //! Convenience function which returns an action which can be placed in a menu to allow switching back to the previous mode.
            /*!
             * This action will be disabled when there is no valid previous mode.
             *
             * <i>This function was added in %Qtilities v1.2.</i>
             */
            QAction* switchToPreviousModeAction();

        public slots:
            //! Switches back to the mode that was previously active before the current mode was activated.
            /*!
             * <i>This function was added in %Qtilities v1.2.</i>
             */
            bool switchToPreviousMode();

        private slots:
            //! Handles selection changes in the top to bottom mode widget.
            void handleModeListCurrentItemChanged(QListWidgetItem * new_item, QListWidgetItem * old_item);
            //! Handle mode shortcut activation.
            void handleModeShortcutActivated();

        signals:
            //! This signal is emitted with the new active mode widget as the \p new_central_widget parameter as soon as the active mode changes.
            void changeCentralWidget(QWidget* new_central_widget);
            //! This signal is emitted when the number sizes of items in the mode list changes.
            void modeListItemSizesChanged();
            //! Signal which is emitted when the active mode of the application changed.
            /*!
             * \param new_mode_id The id of the new active mode. -1 if no mode is active.
             * \param old_mode_id The id of the old mode that was active. -1 if no mode was active before the change.
             *
             * <i>This function was added in %Qtilities v1.2.</i>
             */
            void activeModeChanged(int new_mode_id, int old_mode_id);

        private:
            //! This function that should be used to add mode items to the list. This will take mode ordering into account.
            void refreshList();
            //! Converts a list of mode names to the corresponding list of mode ids.
            /*!
              \note If the name_list contains multiple entries, only the first instance will be converted.
              */
            QList<int> modeNamesToIDs(QStringList name_list) const;
            //! Converts a list of mode interfaces to the corresponding list of mode ids.
            /*!
              \note If the imode_list contains multiple entries, only the first instance will be converted.
              */
            QList<int> modeIFacesToIDs(QList<IMode*> imode_list) const;
            //! Converts a list of mode IDs to a list of corresponding mode names.
            QStringList modeIDsToNames(QList<int> mode_ids) const;
            //! Converts a list of mode interfaces to a list of corresponding mode interfaces.
            QList<IMode*> modeIDsToIFaces(QList<int> mode_ids) const;
            //! Gets the QListWidgetItem corresponding to a specific mode_id.
            QListWidgetItem* listWidgetItemForID(int id) const;
            //! Update the previous mode switch to action.
            void updateSwitchToPreviousModeAction();

            ModeManagerPrivateData* d;
        };
    }
}

#endif // MODEWIDGET_H
