/****************************************************************************
**
** Copyright (c) 2009-2011, Jaco Naude
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
            const char * const qti_def_FACTORY_TAG_NAMING_FILTER     = "qti.def.NamingFilter";
            //! %Factory tag for tree nodes.
            const char * const qti_def_FACTORY_TAG_TREE_NODE         = "qti.def.FactoryTag.TreeNode";
            //! %Factory tag for tree items.
            const char * const qti_def_FACTORY_TAG_TREE_ITEM         = "qti.def.FactoryTag.TreeItem";
            //! %Factory tag for tree file items.
            const char * const qti_def_FACTORY_TAG_TREE_FILE_ITEM    = "qti.def.FactoryTag.TreeFileItem";
            //! The default file name used to save session shortcut mappings by the Qtilities::CoreGui::ActionManager class.
            const char * const qti_def_PATH_SHORTCUTS_FILE           = "shortcut_mapping.smf";
        }

        //! Namespace containing available icons which forms part of the CoreGui Module.
        namespace Icons {
            //! Icon used for shortcuts config page.
            const char * const qti_icon_CONFIG_SHORTCUTS_48x48        = ":/icons/config_shortcuts_48x48.png";
            //! Icon used for logging config page.
            const char * const qti_icon_CONFIG_LOGGING_48x48          = ":/icons/config_logging_48x48.png";
            //! Icon used for plugins config page.
            const char * const qti_icon_CONFIG_PLUGINS_48x48          = ":/icons/config_plugins_48x48.png";
            //! Icon used for project management config page.
            const char * const qti_icon_CONFIG_PROJECTS_48x48         = ":/icons/config_projects_48x48.png";
            //! Icon used for code editor config page.
            const char * const qti_icon_CONFIG_CODE_EDITOR_48x48      = ":/icons/config_code_editor_48x48.png";

            //! Icon used in observer widgets for action to push down into an observer hierachy in the current window.
            const char * const qti_icon_PUSH_DOWN_CURRENT_16x16       = ":/icons/down_16x16.png";
            //! Icon used in observer widgets for action to push down into an observer hierachy in a new window.
            const char * const qti_icon_PUSH_DOWN_NEW_16x16           = ":/icons/down_new_16x16.png";
            //! Icon used in observer widgets for action to push up in an observer hierachy in the current window.
            const char * const qti_icon_PUSH_UP_CURRENT_16x16         = ":/icons/up_16x16.png";
            //! Icon used in observer widgets for action to push up in an observer hierachy in a new window.
            const char * const qti_icon_PUSH_UP_NEW_16x16             = ":/icons/up_new_16x16.png";
            //! Icon used for action to remove a single object.
            const char * const qti_icon_REMOVE_ONE_16x16              = ":/icons/delete_red_16x16.png";
            //! Icon used for action to remove all objects.
            const char * const qti_icon_REMOVE_ALL_16x16              = ":/icons/delete_all_red_16x16.png";
            //! Icon used for action to delete a single object.
            const char * const qti_icon_DELETE_ONE_16x16              = ":/icons/delete_black_16x16.png";
            //! Icon used for action to delete all objects.
            const char * const qti_icon_DELETE_ALL_16x16              = ":/icons/delete_all_black_16x16.png";
            //! Icon used for new action.
            const char * const qti_icon_NEW_16x16                     = ":/icons/new_16x16.png";
            //! Zoom in icon.
            const char * const qti_icon_MAGNIFY_PLUS_16x16            = ":/icons/magnify_plus_16x16.png";
            //! Zoom out icon.
            const char * const qti_icon_MAGNIFY_MINUS_16x16           = ":/icons/magnify_minus_16x16.png";
            //! Icon used to indicate a split, or duplication operation.
            const char * const qti_icon_SPLIT_16x16                   = ":/icons/split_16x16.png";
            //! Icon used to used to switch to tree mode in an observer widget.
            const char * const qti_icon_TREE_16x16                    = ":/icons/tree_16x16.png";
            //! Icon used to used to switch to table mode in an observer widget.
            const char * const qti_icon_TABLE_16x16                   = ":/icons/table_16x16.png";
            //! Refresh icon.
            const char * const qti_icon_REFRESH_16x16                 = ":/icons/refresh_16x16.png";
            //! Information icon.
            const char * const qti_icon_INFO_16x16                    = ":/icons/info_16x16.png";
            //! Success icon.
            const char * const qti_icon_SUCCESS_16x16                 = ":/icons/success_16x16.png";
            //! Error icon.
            const char * const qti_icon_ERROR_16x16                   = ":/icons/error_16x16.png";
            //! Warning icon.
            const char * const qti_icon_WARNING_16x16                 = ":/icons/warning_16x16.png";
            //! Information icon.
            const char * const qti_icon_INFO_12x12                    = ":/icons/info_12x12.png";
            //! Success icon.
            const char * const qti_icon_SUCCESS_12x12                 = ":/icons/success_12x12.png";
            //! Error icon.
            const char * const qti_icon_ERROR_12x12                   = ":/icons/error_12x12.png";
            //! Warning icon.
            const char * const qti_icon_WARNING_12x12                 = ":/icons/warning_12x12.png";
            //! Icon used for observer access column in observer widgets.
            const char * const qti_icon_ACCESS_16x16                  = ":/icons/access_16x16.png";
            //! Icon used for child count column in observer widgets.
            const char * const qti_icon_CHILD_COUNT_22x22             = ":/icons/child_count_22x22.png";
            //! Icon used for object type column in observer widgets.
            const char * const qti_icon_TYPE_INFO_22x22                = ":/icons/type_info_22x22.png";
            //! Icon used to indicate that an observer has locked access.
            const char * const qti_icon_LOCKED_16x16                  = ":/icons/locked_16x16.png";
            //! Icon used to indicate that an observer has read only access.
            const char * const qti_icon_READ_ONLY_16x16               = ":/icons/read_only_16x16.png";
            //! Icon used for search options button in search box widget.
            const char * const qti_icon_SEARCH_OPTIONS_22x22           = ":/icons/search_options_22x22.png";
            //! Icon used for an object property.
            const char * const qti_icon_PROPERTY_16x16                 = ":/icons/property_icon_16x16.png";
            //! Icon used for an object method (slot).
            const char * const qti_icon_METHOD_16x16                   = ":/icons/method_icon_16x16.png";
            //! Icon used for an object event (signal).
            const char * const qti_icon_EVENT_16x16                    = ":/icons/event_icon_16x16.png";
            //! Icon used for managers in %Qtilities.
            const char * const qti_icon_MANAGER_16x16                  = ":/icons/manager_icon_16x16.png";
            //! Folder icon, used for categories in observer widgets in tree mode.
            const char * const qti_icon_FOLDER_16X16                   = ":/icons/folder_icon_16x16.png";
            //! Icon used for dynamic side widget remove action.
            const char * const qti_icon_VIEW_REMOVE_16x16              = ":/icons/view_remove_16x16.png";
            //! Icon used for dynamic side widget new action.
            const char * const qti_icon_VIEW_NEW_16x16                 = ":/icons/view_new_16x16.png";

            // --------------------------------
            // Edit Icons
            // --------------------------------
            //! Copy icon (16x16).
            const char * const qti_icon_EDIT_COPY_16x16         = ":/icons/edit_copy_16x16.png";
            //! Copy icon (22x22).
            const char * const qti_icon_EDIT_COPY_22x22         = ":/icons/edit_copy_22x22.png";
            //! Cut icon (16x16).
            const char * const qti_icon_EDIT_CUT_16x16          = ":/icons/edit_cut_16x16.png";
            //! Cut icon (22x22).
            const char * const qti_icon_EDIT_CUT_22x22          = ":/icons/edit_cut_22x22.png";
            //! Paste icon (16x16).
            const char * const qti_icon_EDIT_PASTE_16x16        = ":/icons/edit_paste_16x16.png";
            //! Paste icon (22x22).
            const char * const qti_icon_EDIT_PASTE_22x22        = ":/icons/edit_paste_22x22.png";
            //! Undo icon (16x16).
            const char * const qti_icon_EDIT_UNDO_16x16         = ":/icons/undo_16x16.png";
            //! Undo icon (22x22).
            const char * const qti_icon_EDIT_UNDO_22x22         = ":/icons/undo_22x22.png";
            //! Redo icon (16x16).
            const char * const qti_icon_EDIT_REDO_16x16         = ":/icons/redo_16x16.png";
            //! Redo icon (22x22).
            const char * const qti_icon_EDIT_REDO_22x22         = ":/icons/redo_22x22.png";
            //! Clear icon (16x16).
            const char * const qti_icon_EDIT_CLEAR_16x16        = ":/icons/clear_16x16.png";
            //! Clear icon (22x22).
            const char * const qti_icon_EDIT_CLEAR_22x22        = ":/icons/clear_22x22.png";
            //! Select All icon (16x16).
            const char * const qti_icon_EDIT_SELECT_ALL_16x16   = ":/icons/select_all_16x16.png";
            //! Select All icon (22x22).
            const char * const qti_icon_EDIT_SELECT_ALL_22x22   = ":/icons/select_all_22x22.png";
            //! Find icon (16x16).
            const char * const qti_icon_FIND_16x16              = ":/icons/find_16x16.png";
            //! Find icon (22x22).
            const char * const qti_icon_FIND_22x22              = ":/icons/find_22x22.png";

            // --------------------------------
            // File Actions
            // --------------------------------
            //! Print icon (16x16).
            const char * const qti_icon_PRINT_16x16             = ":/icons/print_16x16.png";
            //! Print icon (22x22).
            const char * const qti_icon_PRINT_22x22             = ":/icons/print_22x22.png";
            //! Print PDF icon (16x16).
            const char * const qti_icon_PRINT_PDF_16x16         = ":/icons/print_pdf_16x16.png";
            //! Print PDF icon (22x22).
            const char * const qti_icon_PRINT_PDF_22x22         = ":/icons/print_pdf_22x22.png";
            //! Print Preview icon (16x16).
            const char * const qti_icon_PRINT_PREVIEW_16x16     = ":/icons/print_preview_16x16.png";
            //! Print Preview icon (22x22).
            const char * const qti_icon_PRINT_PREVIEW_22x22     = ":/icons/print_preview_22x22.png";
            //! File Open icon (16x16).
            const char * const qti_icon_FILE_OPEN_16x16         = ":/icons/file_open_16x16.png";
            //! File Open icon (22x22).
            const char * const qti_icon_FILE_OPEN_22x22         = ":/icons/file_open_22x22.png";
            //! File Save icon (16x16).
            const char * const qti_icon_FILE_SAVE_16x16         = ":/icons/file_save_16x16.png";
            //! File Save icon (22x22).
            const char * const qti_icon_FILE_SAVE_22x22         = ":/icons/file_save_22x22.png";
            //! File SaveAs icon (16x16).
            const char * const qti_icon_FILE_SAVEAS_16x16       = ":/icons/file_saveas_16x16.png";
            //! File SaveAs icon (22x22).
            const char * const qti_icon_FILE_SAVEAS_22x22       = ":/icons/file_saveas_22x22.png";

            const char * const qti_icon_QTILITIES_SYMBOL_16x16              = ":/icons/qtilities_symbol_16x16.png";
            const char * const qti_icon_QTILITIES_SYMBOL_22x22              = ":/icons/qtilities_symbol_22x22.png";
            const char * const qti_icon_QTILITIES_SYMBOL_24x24              = ":/icons/qtilities_symbol_24x24.png";
            const char * const qti_icon_QTILITIES_SYMBOL_32x32              = ":/icons/qtilities_symbol_32x32.png";
            const char * const qti_icon_QTILITIES_SYMBOL_48x48              = ":/icons/qtilities_symbol_48x48.png";
            const char * const qti_icon_QTILITIES_SYMBOL_64x64              = ":/icons/qtilities_symbol_64x64.png";
            const char * const qti_icon_QTILITIES_SYMBOL_128x128            = ":/icons/qtilities_symbol_128x128.png";
            const char * const qti_icon_QTILITIES_SYMBOL_WHITE_16x16        = ":/icons/qtilities_symbol_white_16x16.png";
            const char * const qti_icon_QTILITIES_SYMBOL_WHITE_22x22        = ":/icons/qtilities_symbol_white_22x22.png";
            const char * const qti_icon_QTILITIES_SYMBOL_WHITE_24x24        = ":/icons/qtilities_symbol_white_24x24.png";
            const char * const qti_icon_QTILITIES_SYMBOL_WHITE_32x32        = ":/icons/qtilities_symbol_white_32x32.png";
            const char * const qti_icon_QTILITIES_SYMBOL_WHITE_48x48        = ":/icons/qtilities_symbol_white_48x48.png";
            const char * const qti_icon_QTILITIES_SYMBOL_WHITE_64x64        = ":/icons/qtilities_symbol_white_64x64.png";
            const char * const qti_icon_QTILITIES_SYMBOL_WHITE_128x128      = ":/icons/qtilities_symbol_white_128x128.png";
        }

        //! Namespace containing available images which forms part of the CoreGui Module.
        namespace Images {
            const char * const QTILITIES_LOGO_BT_300x300                = ":/images/blueontrans_300x300.png";
        }

        //! Namespace containing available actions which forms part of the CoreGui Module.
        namespace Actions {
            // Standard actions & containers
            const char * const qti_action_MENUBAR_STANDARD                = "MainWindowMenuBar";
            const char * const qti_action_FILE                            = "File";
            const char * const qti_action_FILE_NEW                        = "File.New";
            const char * const qti_action_FILE_OPEN                       = "File.Open";
            const char * const qti_action_FILE_SETTINGS                   = "File.Settings";
            const char * const qti_action_FILE_SAVE_AS                    = "File.SaveAs";
            const char * const qti_action_FILE_SAVE                       = "File.Save";
            const char * const qti_action_FILE_PRINT                      = "File.Print";
            const char * const qti_action_FILE_PRINT_PREVIEW              = "File.PrintPreview";
            const char * const qti_action_FILE_PRINT_PDF                  = "File.PrintPDF";
            const char * const qti_action_FILE_EXIT                       = "File.Exit";
            const char * const qti_action_HELP                            = "Help";
            const char * const qti_action_HELP_ABOUT_QTILITIES            = "General.AboutQtilities";
            const char * const qti_action_HELP_ABOUT                      = "General.About";

            // Observer widget actions & containers
            const char * const qti_action_CONTEXT                         = "Context";
            const char * const qti_action_CONTEXT_HIERARCHY               = "Hierarchy";
            const char * const qti_action_CONTEXT_REFRESH_VIEW            = "Context.RefreshView";
            const char * const qti_action_CONTEXT_NEW_ITEM                = "Context.NewItem";
            const char * const qti_action_CONTEXT_SWITCH_VIEW             = "Context.SwitchView";
            const char * const qti_action_CONTEXT_REMOVE_ALL              = "Context.RemoveAll";
            const char * const qti_action_CONTEXT_REMOVE_ITEM             = "Context.RemoveItem";
            const char * const qti_action_CONTEXT_DELETE_ALL              = "Context.DeleteAll";
            const char * const qti_action_CONTEXT_HIERARCHY_UP            = "Context.Hierarchy.Up";
            const char * const qti_action_CONTEXT_HIERARCHY_UP_NEW        = "Context.Hierarchy.UpNew";
            const char * const qti_action_CONTEXT_HIERARCHY_DOWN          = "Context.Hierarchy.Down";
            const char * const qti_action_CONTEXT_HIERARCHY_DOWN_NEW      = "Context.Hierarchy.DownNew";
            const char * const qti_action_CONTEXT_HIERARCHY_EXPAND        = "Context.Hierarchy.Expand";
            const char * const qti_action_CONTEXT_HIERARCHY_COLLAPSE      = "Context.Hierarchy.Collapse";
            const char * const qti_action_SELECTION                       = "Selection";
            const char * const qti_action_SELECTION_DELETE                = "Selection.Delete";

            // ObserverScopeWidget actions
            const char * const qti_action_SELECTION_SCOPE_ADD             = "Selection.Scope.Add";
            const char * const qti_action_SELECTION_SCOPE_REMOVE_SELECTED = "Selection.Scope.RemoveFromCurrentScope";
            const char * const qti_action_SELECTION_SCOPE_REMOVE_OTHERS   = "Selection.Scope.RemoveOthers";
            const char * const qti_action_SELECTION_SCOPE_DUPLICATE       = "Selection.Scope.Duplicate";

            // Edit menu and actions
            const char * const qti_action_EDIT                            = "Edit";
            const char * const qti_action_EDIT_UNDO                       = "Edit.Undo";
            const char * const qti_action_EDIT_REDO                       = "Edit.Redo";
            const char * const qti_action_EDIT_COPY                       = "Edit.Copy";
            const char * const qti_action_EDIT_CUT                        = "Edit.Cut";
            const char * const qti_action_EDIT_PASTE                      = "Edit.Paste";
            const char * const qti_action_EDIT_CLEAR                      = "Edit.Clear";
            const char * const qti_action_EDIT_SELECT_ALL                 = "Edit.SelectAll";
            const char * const qti_action_EDIT_FIND                       = "Edit.Find";

            // View menu and actions
            const char * const qti_action_VIEW                             = "View";
            const char * const qti_action_VIEW_ACTIVATE_NEXT               = "View.ActivateNext";
            const char * const qti_action_VIEW_ACTIVATE_PREV               = "View.ActivatePrev";
            const char * const qti_action_VIEW_CLOSE_ALL                   = "View.CloseAll";
            const char * const qti_action_VIEW_CLOSE_ACTIVE                = "View.CloseActive";
            const char * const qti_action_VIEW_CASCADE                     = "View.Cascade";
            const char * const qti_action_VIEW_TILE                        = "View.Tile";

            // About menu and actions
            const char * const qti_action_ABOUT                            = "About";
            const char * const qti_action_ABOUT_QTILITIES                  = "About.Qtilities";
        }
    }
}

#endif // QtilitiesCoreGuiConstants_H
