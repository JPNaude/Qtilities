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

#ifndef QtilitiesCoreGuiConstants_H
#define QtilitiesCoreGuiConstants_H

#include "QtilitiesCoreGui_global.h"

namespace Qtilities {
    //! The possible display modes of widgets containing observer widgets.
    /*!
      \sa Qtilities::CoreGui::ObserverWidget, Qtilities::CoreGui::ConfigurationWidget
      */
    enum DisplayMode {
        TableView,      /*!< Table view mode. */
        TreeView        /*!< Tree view mode. */
    };

    //! Namespace containing all the classes which forms part of the CoreGui Module.
    namespace CoreGui {
        //! Namespace containing constants used inside the CoreGui Module.
        namespace Constants {
            //! %Factory tag for naming policy subject filters.
            const char * const FACTORY_TAG_NAMING_POLICY_FILTER     = "Naming Policy Filter";
            //! %Factory tag for tree nodes.
            const char * const FACTORY_TAG_TREE_NODE                = "Tree Node";
            //! %Factory tag for tree items.
            const char * const FACTORY_TAG_TREE_ITEM                = "Tree Item";
            //! %Factory tag for tree file items.
            const char * const FACTORY_TAG_TREE_FILE_ITEM           = "Tree File Item";
            //! The default file name used to save session shortcut mappings by the Qtilities::CoreGui::ActionManager class.
            const char * const FILE_SHORTCUT_MAPPING                = "shortcut_mapping.smf";
        }

        //! Namespace containing available icons which forms part of the CoreGui Module.
        namespace Icons {
            //! Icon used for shortcuts config page.
            const char * const ICON_CONFIG_SHORTCUTS_48x48        = ":/icons/config_shortcuts_48x48.png";
            //! Icon used for logging config page.
            const char * const ICON_CONFIG_LOGGING_48x48          = ":/icons/config_logging_48x48.png";
            //! Icon used for plugins config page.
            const char * const ICON_CONFIG_PLUGINS_48x48          = ":/icons/config_plugins_48x48.png";
            //! Icon used for project management config page.
            const char * const ICON_CONFIG_PROJECTS_48x48         = ":/icons/config_projects_48x48.png";

            //! Icon used in observer widgets for action to push down into an observer hierachy in the current window.
            const char * const ICON_PUSH_DOWN_CURRENT_16x16       = ":/icons/push_down_current_16x16.png";
            //! Icon used in observer widgets for action to push down into an observer hierachy in a new window.
            const char * const ICON_PUSH_DOWN_NEW_16x16           = ":/icons/push_down_new_16x16.png";
            //! Icon used in observer widgets for action to push up in an observer hierachy in the current window.
            const char * const ICON_PUSH_UP_CURRENT_16x16         = ":/icons/push_up_current_16x16.png";
            //! Icon used in observer widgets for action to push up in an observer hierachy in a new window.
            const char * const ICON_PUSH_UP_NEW_16x16             = ":/icons/push_up_new_16x16.png";
            //! Icon used for action to remove a single object.
            const char * const ICON_REMOVE_ONE_16x16              = ":/icons/remove_one_16x16.png";
            //! Icon used for action to remove all objects.
            const char * const ICON_REMOVE_ALL_16x16              = ":/icons/remove_all_16x16.png";
            //! Icon used for action to delete a single object.
            const char * const ICON_DELETE_ONE_16x16              = ":/icons/delete_one_16x16.png";
            //! Icon used for action to delete all objects.
            const char * const ICON_DELETE_ALL_16x16              = ":/icons/delete_all_16x16.png";
            //! Icon used for new action.
            const char * const ICON_NEW_16x16                     = ":/icons/new_16x16.png";
            //! Zoom in icon.
            const char * const ICON_MAGNIFY_PLUS_16x16            = ":/icons/magnify_plus_16x16.png";
            //! Zoom out icon.
            const char * const ICON_MAGNIFY_MINUS_16x16           = ":/icons/magnify_minus_16x16.png";
            //! Icon used to indicate a split, or duplication operation.
            const char * const ICON_SPLIT_16x16                   = ":/icons/split_16x16.png";
            //! Icon used to switch between observer widget modes.
            const char * const ICON_SWITCH_VIEW_16x16             = ":/icons/switch_view_16x16.png";
            //! Refresh icon.
            const char * const ICON_REFRESH_16x16                 = ":/icons/refresh_16x16.png";
            //! Success icon.
            const char * const ICON_SUCCESS_16x16                 = ":/icons/success_16x16.png";
            //! Error icon.
            const char * const ICON_ERROR_16x16                   = ":/icons/error_16x16.png";
            //! Error icon.
            const char * const ICON_WARNING_16x16                 = ":/icons/warning_16x16.png";
            //! Icon used for observer access column in observer widgets.
            const char * const ICON_ACCESS_22x22                  = ":/icons/access_22x22.png";
            //! Icon used for child count column in observer widgets.
            const char * const ICON_CHILD_COUNT_22x22             = ":/icons/child_count_22x22.png";
            //! Icon used for object type column in observer widgets.
            const char * const ICON_TYPE_INFO_22x22                = ":/icons/type_info_22x22.png";
            //! Icon used to indicate that an observer has locked access.
            const char * const ICON_LOCKED_22x22                  = ":/icons/locked_22x22.png";
            //! Icon used to indicate that an observer has read only access.
            const char * const ICON_READ_ONLY_22x22               = ":/icons/read_only_22x22.png";
            //! Icon used for search options button in search box widget.
            const char * const ICON_SEARCH_OPTIONS_22x22           = ":/icons/search_options_22x22.png";
            //! Icon used for an object property.
            const char * const ICON_PROPERTY_16x16                 = ":/icons/property_icon_16x16.png";
            //! Icon used for an object method (slot).
            const char * const ICON_METHOD_16x16                   = ":/icons/method_icon_16x16.png";
            //! Icon used for an object event (signal).
            const char * const ICON_EVENT_16x16                    = ":/icons/event_icon_16x16.png";
            //! Icon used for managers in %Qtilities.
            const char * const ICON_MANAGER_16x16                  = ":/icons/manager_icon_16x16.png";
            //! Folder icon, used for categories in observer widgets in tree mode.
            const char * const ICON_FOLDER_16X16                   = ":/icons/folder_icon_16x16.png";
            //! Icon used for dynamic side widget remove action.
            const char * const ICON_VIEW_REMOVE_16x16              = ":/icons/view_remove_16x16.png";
            //! Icon used for dynamic side widget new action.
            const char * const ICON_VIEW_NEW_16x16                 = ":/icons/view_new_16x16.png";

            // --------------------------------
            // Edit Icons
            // --------------------------------
            //! Copy icon (16x16).
            const char * const ICON_EDIT_COPY_16x16         = ":/icons/edit_copy_16x16.png";
            //! Copy icon (22x22).
            const char * const ICON_EDIT_COPY_22x22         = ":/icons/edit_copy_22x22.png";
            //! Cut icon (16x16).
            const char * const ICON_EDIT_CUT_16x16          = ":/icons/edit_cut_16x16.png";
            //! Cut icon (22x22).
            const char * const ICON_EDIT_CUT_22x22          = ":/icons/edit_cut_22x22.png";
            //! Paste icon (16x16).
            const char * const ICON_EDIT_PASTE_16x16        = ":/icons/edit_paste_16x16.png";
            //! Paste icon (22x22).
            const char * const ICON_EDIT_PASTE_22x22        = ":/icons/edit_paste_22x22.png";
            //! Undo icon (16x16).
            const char * const ICON_EDIT_UNDO_16x16         = ":/icons/undo_16x16.png";
            //! Undo icon (22x22).
            const char * const ICON_EDIT_UNDO_22x22         = ":/icons/undo_22x22.png";
            //! Redo icon (16x16).
            const char * const ICON_EDIT_REDO_16x16         = ":/icons/redo_16x16.png";
            //! Redo icon (22x22).
            const char * const ICON_EDIT_REDO_22x22         = ":/icons/redo_22x22.png";
            //! Clear icon (16x16).
            const char * const ICON_EDIT_CLEAR_16x16        = ":/icons/clear_16x16.png";
            //! Clear icon (22x22).
            const char * const ICON_EDIT_CLEAR_22x22        = ":/icons/clear_22x22.png";
            //! Select All icon (16x16).
            const char * const ICON_EDIT_SELECT_ALL_16x16   = ":/icons/select_all_16x16.png";
            //! Select All icon (22x22).
            const char * const ICON_EDIT_SELECT_ALL_22x22   = ":/icons/select_all_22x22.png";
            //! Find icon (16x16).
            const char * const ICON_FIND_16x16              = ":/icons/find_16x16.png";
            //! Find icon (22x22).
            const char * const ICON_FIND_22x22              = ":/icons/find_22x22.png";

            // --------------------------------
            // File Actions
            // --------------------------------
            //! Print icon (16x16).
            const char * const ICON_PRINT_16x16             = ":/icons/print_16x16.png";
            //! Print icon (22x22).
            const char * const ICON_PRINT_22x22             = ":/icons/print_22x22.png";
            //! Print PDF icon (16x16).
            const char * const ICON_PRINT_PDF_16x16         = ":/icons/print_pdf_16x16.png";
            //! Print PDF icon (22x22).
            const char * const ICON_PRINT_PDF_22x22         = ":/icons/print_pdf_22x22.png";
            //! Print Preview icon (16x16).
            const char * const ICON_PRINT_PREVIEW_16x16     = ":/icons/print_preview_16x16.png";
            //! Print Preview icon (22x22).
            const char * const ICON_PRINT_PREVIEW_22x22     = ":/icons/print_preview_22x22.png";
            //! File Open icon (16x16).
            const char * const ICON_FILE_OPEN_16x16         = ":/icons/file_open_16x16.png";
            //! File Open icon (22x22).
            const char * const ICON_FILE_OPEN_22x22         = ":/icons/file_open_22x22.png";
            //! File Save icon (16x16).
            const char * const ICON_FILE_SAVE_16x16         = ":/icons/file_save_16x16.png";
            //! File Save icon (22x22).
            const char * const ICON_FILE_SAVE_22x22         = ":/icons/file_save_22x22.png";
            //! File SaveAs icon (16x16).
            const char * const ICON_FILE_SAVEAS_16x16       = ":/icons/file_saveas_16x16.png";
            //! File SaveAs icon (22x22).
            const char * const ICON_FILE_SAVEAS_22x22       = ":/icons/file_saveas_22x22.png";

            const char * const ICON_QTILITIES_SYMBOL_16x16              = ":/icons/qtilities_symbol_16x16.png";
            const char * const ICON_QTILITIES_SYMBOL_22x22              = ":/icons/qtilities_symbol_22x22.png";
            const char * const ICON_QTILITIES_SYMBOL_24x24              = ":/icons/qtilities_symbol_24x24.png";
            const char * const ICON_QTILITIES_SYMBOL_32x32              = ":/icons/qtilities_symbol_32x32.png";
            const char * const ICON_QTILITIES_SYMBOL_48x48              = ":/icons/qtilities_symbol_48x48.png";
            const char * const ICON_QTILITIES_SYMBOL_64x64              = ":/icons/qtilities_symbol_64x64.png";
            const char * const ICON_QTILITIES_SYMBOL_128x128            = ":/icons/qtilities_symbol_128x128.png";
            const char * const ICON_QTILITIES_SYMBOL_WHITE_16x16        = ":/icons/qtilities_symbol_white_16x16.png";
            const char * const ICON_QTILITIES_SYMBOL_WHITE_22x22        = ":/icons/qtilities_symbol_white_22x22.png";
            const char * const ICON_QTILITIES_SYMBOL_WHITE_24x24        = ":/icons/qtilities_symbol_white_24x24.png";
            const char * const ICON_QTILITIES_SYMBOL_WHITE_32x32        = ":/icons/qtilities_symbol_white_32x32.png";
            const char * const ICON_QTILITIES_SYMBOL_WHITE_48x48        = ":/icons/qtilities_symbol_white_48x48.png";
            const char * const ICON_QTILITIES_SYMBOL_WHITE_64x64        = ":/icons/qtilities_symbol_white_64x64.png";
            const char * const ICON_QTILITIES_SYMBOL_WHITE_128x128      = ":/icons/qtilities_symbol_white_128x128.png";
        }

        //! Namespace containing available images which forms part of the CoreGui Module.
        namespace Images {
            const char * const QTILITIES_LOGO_BT_300x300                = ":/images/blueontrans_300x300.png";
        }

        //! Namespace containing available actions which forms part of the CoreGui Module.
        namespace Actions {
            // Standard actions & containers
            const char * const MENUBAR_STANDARD                     = "MainWindowMenuBar";
            const char * const MENU_FILE                            = "File";
            const char * const MENU_FILE_NEW                        = "File.New";
            const char * const MENU_FILE_OPEN                       = "File.Open";
            const char * const MENU_FILE_SETTINGS                   = "File.Settings";
            const char * const MENU_FILE_SAVE_AS                    = "File.SaveAs";
            const char * const MENU_FILE_SAVE                       = "File.Save";
            const char * const MENU_FILE_PRINT                      = "File.Print";
            const char * const MENU_FILE_PRINT_PREVIEW              = "File.PrintPreview";
            const char * const MENU_FILE_PRINT_PDF                  = "File.PrintPDF";
            const char * const MENU_FILE_EXIT                       = "File.Exit";
            const char * const MENU_HELP                            = "Help";
            const char * const MENU_HELP_ABOUT_QTILITIES            = "General.AboutQtilities";
            const char * const MENU_HELP_ABOUT                      = "General.About";

            // Observer widget actions & containers
            const char * const MENU_CONTEXT                         = "Context";
            const char * const MENU_CONTEXT_HIERARCHY               = "Hierarchy";
            const char * const MENU_CONTEXT_REFRESH_VIEW            = "Context.RefreshView";
            const char * const MENU_CONTEXT_NEW_ITEM                = "Context.NewItem";
            const char * const MENU_CONTEXT_SWITCH_VIEW             = "Context.SwitchView";
            const char * const MENU_CONTEXT_REMOVE_ALL              = "Context.RemoveAll";
            const char * const MENU_CONTEXT_REMOVE_ITEM             = "Context.RemoveItem";
            const char * const MENU_CONTEXT_DELETE_ALL              = "Context.DeleteAll";
            const char * const MENU_CONTEXT_HIERARCHY_UP            = "Context.Hierarchy.Up";
            const char * const MENU_CONTEXT_HIERARCHY_UP_NEW        = "Context.Hierarchy.UpNew";
            const char * const MENU_CONTEXT_HIERARCHY_DOWN          = "Context.Hierarchy.Down";
            const char * const MENU_CONTEXT_HIERARCHY_DOWN_NEW      = "Context.Hierarchy.DownNew";
            const char * const MENU_CONTEXT_HIERARCHY_EXPAND        = "Context.Hierarchy.Expand";
            const char * const MENU_CONTEXT_HIERARCHY_COLLAPSE      = "Context.Hierarchy.Collapse";
            const char * const MENU_SELECTION                       = "Selection";
            const char * const MENU_SELECTION_DELETE                = "Selection.Delete";

            // ObserverScopeWidget actions
            const char * const SELECTION_SCOPE_ADD                  = "Selection.Scope.Add";
            const char * const SELECTION_SCOPE_REMOVE_SELECTED      = "Selection.Scope.RemoveFromCurrentScope";
            const char * const SELECTION_SCOPE_REMOVE_OTHERS        = "Selection.Scope.RemoveOthers";
            const char * const SELECTION_SCOPE_DUPLICATE            = "Selection.Scope.Duplicate";

            // Edit menu and actions
            const char * const MENU_EDIT                            = "Edit";
            const char * const MENU_EDIT_UNDO                       = "Edit.Undo";
            const char * const MENU_EDIT_REDO                       = "Edit.Redo";
            const char * const MENU_EDIT_COPY                       = "Edit.Copy";
            const char * const MENU_EDIT_CUT                        = "Edit.Cut";
            const char * const MENU_EDIT_PASTE                      = "Edit.Paste";
            const char * const MENU_EDIT_CLEAR                      = "Edit.Clear";
            const char * const MENU_EDIT_SELECT_ALL                 = "Edit.SelectAll";
            const char * const MENU_EDIT_FIND                       = "Edit.Find";

            // View menu and actions
            const char * const MENU_VIEW                             = "View";
            const char * const MENU_VIEW_ACTIVATE_NEXT               = "View.ActivateNext";
            const char * const MENU_VIEW_ACTIVATE_PREV               = "View.ActivatePrev";
            const char * const MENU_VIEW_CLOSE_ALL                   = "View.CloseAll";
            const char * const MENU_VIEW_CLOSE_ACTIVE                = "View.CloseActive";
            const char * const MENU_VIEW_CASCADE                     = "View.Cascade";
            const char * const MENU_VIEW_TILE                        = "View.Tile";

            // About menu and actions
            const char * const MENU_ABOUT                            = "About";
            const char * const MENU_ABOUT_QTILITIES                  = "About.Qtilities";
        }
    }
}

#endif // QtilitiesCoreGuiConstants_H
