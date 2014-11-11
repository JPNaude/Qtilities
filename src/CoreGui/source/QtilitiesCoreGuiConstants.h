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

#ifndef QtilitiesCoreGuiConstants_H
#define QtilitiesCoreGuiConstants_H

#include "QtilitiesCoreGui_global.h"

namespace Qtilities {
    //! The possible display modes of widgets containing observer widgets.
    /*!
      \sa Qtilities::CoreGui::ObserverWidget
      */
    enum DisplayMode {
        TableView,      /*!< Table view mode. */
        TreeView        /*!< Tree view mode. */
    };

    //! Namespace containing all the classes which forms part of the CoreGui Module.
    /*!
    To use this module, add the following to your .pro %file:
    \code
    QTILITIES += coregui;
    include(Qtilities_Path/src/Qtilities.pri)
    \endcode

    To include all files in this module:
    \code
    #include <QtilitiesCoreGui>
    using namespace QtilitiesCoreGui;
    \endcode

    For more information about the modules in %Qtilities, see \ref page_modules_overview.
    */
    namespace CoreGui {
        //! Namespace containing constants used inside the CoreGui Module.
        namespace Constants {
            //! %Factory tag for naming policy subject filters.
            const char * const qti_def_FACTORY_TAG_NAMING_FILTER     = "qti.def.FactoryTag.NamingFilter";
            //! %Factory tag for tree nodes.
            const char * const qti_def_FACTORY_TAG_TREE_NODE         = "qti.def.FactoryTag.TreeNode";
            //! %Factory tag for tree items.
            const char * const qti_def_FACTORY_TAG_TREE_ITEM         = "qti.def.FactoryTag.TreeItem";
            //! %Factory tag for tree file items.
            const char * const qti_def_FACTORY_TAG_TREE_FILE_ITEM    = "qti.def.FactoryTag.TreeFileItem";
            //! The default file name used to save session shortcut mappings by the Qtilities::CoreGui::ActionManager class.
            const char * const qti_def_PATH_SHORTCUTS_FILE           = "shortcut_mapping.smf";
            //! The mode manager ID of the default mode manager constructed in QtilitiesMainWindow.
            const int qti_def_DEFAULT_MODE_MANAGER                   = 1;

            //! The default category for config pages in %Qtilities.
            const char * const qti_config_page_DEFAULT_CAT           = "General";
            //! The name of the config page for logging.
            const char * const qti_config_page_LOGGING               = "Logging";
            //! The name of the config page for projects.
            const char * const qti_config_page_PROJECTS              = "Projects";
            //! The name of the config page for the help system.
            const char * const qti_config_page_HELP                  = "Help";
            //! The name of the config page for code editors.
            const char * const qti_config_page_CODE_EDITORS          = "Editor Options";
            //! The name of the config page for plugins.
            const char * const qti_config_page_PLUGINS               = "Plugins";
        }

        //! Namespace containing available icons which forms part of the CoreGui Module.
        namespace Icons {
            //! Icon used to debug selections in observer widgets.
            const char * const qti_icon_DEBUG_16x16                   = ":/qtilities/coregui/icons/debug_16x16.png";
            //! Icon used to debug selections in observer widgets.
            const char * const qti_icon_DEBUG_48x48                   = ":/qtilities/coregui/icons/debug_48x48.png";
            //! Icon used for shortcuts config page.
            const char * const qti_icon_CONFIG_SHORTCUTS_48x48        = ":/qtilities/coregui/icons/config_shortcuts_48x48.png";
            //! Icon used for logging config page.
            const char * const qti_icon_CONFIG_LOGGING_48x48          = ":/qtilities/coregui/icons/config_logging_48x48.png";
            //! Icon used for plugins config page.
            const char * const qti_icon_CONFIG_PLUGINS_48x48          = ":/qtilities/coregui/icons/config_plugins_48x48.png";
            //! Icon used for project management config page.
            const char * const qti_icon_CONFIG_PROJECTS_48x48         = ":/qtilities/coregui/icons/config_projects_48x48.png";
            //! Icon used for code editor config page.
            const char * const qti_icon_CONFIG_CODE_EDITOR_48x48      = ":/qtilities/coregui/icons/config_code_editor_48x48.png";

            //! Icon used in observer widgets for action to push down into an observer hierachy in the current window.
            const char * const qti_icon_PUSH_DOWN_CURRENT_16x16       = ":/qtilities/coregui/icons/down_16x16.png";
            //! Icon used in observer widgets for action to push down into an observer hierachy in a new window.
            const char * const qti_icon_PUSH_DOWN_NEW_16x16           = ":/qtilities/coregui/icons/down_new_16x16.png";
            //! Icon used in observer widgets for action to push up in an observer hierachy in the current window.
            const char * const qti_icon_PUSH_UP_CURRENT_16x16         = ":/qtilities/coregui/icons/up_16x16.png";
            //! Icon used in observer widgets for action to push up in an observer hierachy in a new window.
            const char * const qti_icon_PUSH_UP_NEW_16x16             = ":/qtilities/coregui/icons/up_new_16x16.png";
            //! Icon used for action to remove a single object.
            const char * const qti_icon_REMOVE_ONE_16x16              = ":/qtilities/coregui/icons/delete_red_16x16.png";
            //! Icon used for action to remove all objects.
            const char * const qti_icon_REMOVE_ALL_16x16              = ":/qtilities/coregui/icons/delete_all_red_16x16.png";
            //! Icon used for action to delete a single object.
            const char * const qti_icon_DELETE_ONE_16x16              = ":/qtilities/coregui/icons/delete_black_16x16.png";
            //! Icon used for action to delete all objects.
            const char * const qti_icon_DELETE_ALL_16x16              = ":/qtilities/coregui/icons/delete_all_black_16x16.png";
            //! Icon used for new action.
            const char * const qti_icon_NEW_16x16                     = ":/qtilities/coregui/icons/new_16x16.png";
            //! Zoom in icon.
            const char * const qti_icon_MAGNIFY_PLUS_16x16            = ":/qtilities/coregui/icons/magnify_plus_16x16.png";
            //! Zoom out icon.
            const char * const qti_icon_MAGNIFY_MINUS_16x16           = ":/qtilities/coregui/icons/magnify_minus_16x16.png";
            //! Icon used to indicate a split, or duplication operation.
            const char * const qti_icon_SPLIT_16x16                   = ":/qtilities/coregui/icons/split_16x16.png";
            //! Icon used to used to switch to tree mode in an observer widget.
            const char * const qti_icon_TREE_16x16                    = ":/qtilities/coregui/icons/tree_16x16.png";
            //! Icon used to used to switch to table mode in an observer widget.
            const char * const qti_icon_TABLE_16x16                   = ":/qtilities/coregui/icons/table_16x16.png";
            //! Refresh icon.
            const char * const qti_icon_REFRESH_16x16                 = ":/qtilities/coregui/icons/refresh_16x16.png";
            //! Information icon.
            const char * const qti_icon_INFO_16x16                    = ":/qtilities/coregui/icons/info_16x16.png";
            //! Success icon.
            const char * const qti_icon_SUCCESS_16x16                 = ":/qtilities/coregui/icons/success_16x16.png";
            //! Error icon.
            const char * const qti_icon_ERROR_16x16                   = ":/qtilities/coregui/icons/error_16x16.png";
            //! Warning icon.
            const char * const qti_icon_WARNING_16x16                 = ":/qtilities/coregui/icons/warning_16x16.png";
            //! Information icon.
            const char * const qti_icon_INFO_12x12                    = ":/qtilities/coregui/icons/info_12x12.png";
            //! Success icon.
            const char * const qti_icon_SUCCESS_12x12                 = ":/qtilities/coregui/icons/success_12x12.png";
            //! Error icon.
            const char * const qti_icon_ERROR_12x12                   = ":/qtilities/coregui/icons/error_12x12.png";
            //! Warning icon.
            const char * const qti_icon_WARNING_12x12                 = ":/qtilities/coregui/icons/warning_12x12.png";
            //! Icon used for observer access column in observer widgets.
            const char * const qti_icon_ACCESS_16x16                  = ":/qtilities/coregui/icons/access_16x16.png";
            //! Broom icon.
            const char * const qti_icon_BROOM_16x16                   = ":/qtilities/coregui/icons/broom_16x16.png";
            //! Text wrap icon.
            const char * const qti_icon_LINE_WRAP_16x16               = ":/qtilities/coregui/icons/line_wrap_24x24.png";
            //! Icon used for child count column in observer widgets.
            const char * const qti_icon_CHILD_COUNT_22x22             = ":/qtilities/coregui/icons/child_count_22x22.png";
            //! Icon used for object type column in observer widgets.
            const char * const qti_icon_TYPE_INFO_22x22               = ":/qtilities/coregui/icons/type_info_22x22.png";
            //! Icon used to indicate that an observer has locked access.
            const char * const qti_icon_LOCKED_16x16                  = ":/qtilities/coregui/icons/locked_16x16.png";
            //! Icon used to indicate that an observer has read only access.
            const char * const qti_icon_READ_ONLY_16x16               = ":/qtilities/coregui/icons/read_only_16x16.png";
            //! Icon used for search options button in search box widget.
            const char * const qti_icon_SEARCH_OPTIONS_22x22          = ":/qtilities/coregui/icons/search_options_22x22.png";
            //! Icon used for an object property.
            const char * const qti_icon_PROPERTY_16x16                = ":/qtilities/coregui/icons/property_icon_16x16.png";
            //! Icon used for an object method (slot).
            const char * const qti_icon_METHOD_16x16                  = ":/qtilities/coregui/icons/method_icon_16x16.png";
            //! Icon used for an object event (signal).
            const char * const qti_icon_EVENT_16x16                   = ":/qtilities/coregui/icons/event_icon_16x16.png";
            //! Folder icon, used for categories in observer widgets in tree mode.
            const char * const qti_icon_FOLDER_16X16                  = ":/qtilities/coregui/icons/folder_icon_16x16.png";
            //! Icon used for dynamic side widget remove action.
            const char * const qti_icon_VIEW_REMOVE_16x16             = ":/qtilities/coregui/icons/view_remove_16x16.png";
            //! Icon used for dynamic side widget new action.
            const char * const qti_icon_VIEW_NEW_16x16                = ":/qtilities/coregui/icons/view_new_16x16.png";

            // --------------------------------
            // Edit Icons
            // --------------------------------
            //! Copy icon (16x16).
            const char * const qti_icon_EDIT_COPY_16x16         = ":/qtilities/coregui/icons/edit_copy_16x16.png";
            //! Copy icon (22x22).
            const char * const qti_icon_EDIT_COPY_22x22         = ":/qtilities/coregui/icons/edit_copy_22x22.png";
            //! Cut icon (16x16).
            const char * const qti_icon_EDIT_CUT_16x16          = ":/qtilities/coregui/icons/edit_cut_16x16.png";
            //! Cut icon (22x22).
            const char * const qti_icon_EDIT_CUT_22x22          = ":/qtilities/coregui/icons/edit_cut_22x22.png";
            //! Paste icon (16x16).
            const char * const qti_icon_EDIT_PASTE_16x16        = ":/qtilities/coregui/icons/edit_paste_16x16.png";
            //! Paste icon (22x22).
            const char * const qti_icon_EDIT_PASTE_22x22        = ":/qtilities/coregui/icons/edit_paste_22x22.png";
            //! Undo icon (16x16).
            const char * const qti_icon_EDIT_UNDO_16x16         = ":/qtilities/coregui/icons/undo_16x16.png";
            //! Undo icon (22x22).
            const char * const qti_icon_EDIT_UNDO_22x22         = ":/qtilities/coregui/icons/undo_22x22.png";
            //! Redo icon (16x16).
            const char * const qti_icon_EDIT_REDO_16x16         = ":/qtilities/coregui/icons/redo_16x16.png";
            //! Redo icon (22x22).
            const char * const qti_icon_EDIT_REDO_22x22         = ":/qtilities/coregui/icons/redo_22x22.png";
            //! Clear icon (16x16).
            const char * const qti_icon_EDIT_CLEAR_16x16        = ":/qtilities/coregui/icons/clear_16x16.png";
            //! Clear icon (22x22).
            const char * const qti_icon_EDIT_CLEAR_22x22        = ":/qtilities/coregui/icons/clear_22x22.png";
            //! Select All icon (16x16).
            const char * const qti_icon_EDIT_SELECT_ALL_16x16   = ":/qtilities/coregui/icons/select_all_16x16.png";
            //! Select All icon (22x22).
            const char * const qti_icon_EDIT_SELECT_ALL_22x22   = ":/qtilities/coregui/icons/select_all_22x22.png";
            //! Find icon (16x16).
            const char * const qti_icon_FIND_16x16              = ":/qtilities/coregui/icons/find_16x16.png";
            //! Find icon (22x22).
            const char * const qti_icon_FIND_22x22              = ":/qtilities/coregui/icons/find_22x22.png";

            // --------------------------------
            // File Actions
            // --------------------------------
            //! Print icon (16x16).
            const char * const qti_icon_PRINT_16x16             = ":/qtilities/coregui/icons/print_16x16.png";
            //! Print icon (22x22).
            const char * const qti_icon_PRINT_22x22             = ":/qtilities/coregui/icons/print_22x22.png";
            //! Print PDF icon (16x16).
            const char * const qti_icon_PRINT_PDF_16x16         = ":/qtilities/coregui/icons/print_pdf_16x16.png";
            //! Print PDF icon (22x22).
            const char * const qti_icon_PRINT_PDF_22x22         = ":/qtilities/coregui/icons/print_pdf_22x22.png";
            //! Print Preview icon (16x16).
            const char * const qti_icon_PRINT_PREVIEW_16x16     = ":/qtilities/coregui/icons/print_preview_16x16.png";
            //! Print Preview icon (22x22).
            const char * const qti_icon_PRINT_PREVIEW_22x22     = ":/qtilities/coregui/icons/print_preview_22x22.png";
            //! File Open icon (16x16).
            const char * const qti_icon_FILE_OPEN_16x16         = ":/qtilities/coregui/icons/file_open_16x16.png";
            //! File Open icon (22x22).
            const char * const qti_icon_FILE_OPEN_22x22         = ":/qtilities/coregui/icons/file_open_22x22.png";
            //! File Save icon (16x16).
            const char * const qti_icon_FILE_SAVE_16x16         = ":/qtilities/coregui/icons/file_save_16x16.png";
            //! File Save icon (22x22).
            const char * const qti_icon_FILE_SAVE_22x22         = ":/qtilities/coregui/icons/file_save_22x22.png";
            //! File Save As icon (16x16).
            const char * const qti_icon_FILE_SAVEAS_16x16       = ":/qtilities/coregui/icons/file_saveas_16x16.png";
            //! File Save As icon (22x22).
            const char * const qti_icon_FILE_SAVEAS_22x22       = ":/qtilities/coregui/icons/file_saveas_22x22.png";
            //! File Save All icon (16x16).
            const char * const qti_icon_FILE_SAVE_ALL_16x16     = ":/qtilities/coregui/icons/file_save_all_16x16.png";
            //! File Save All icon (22x22).
            const char * const qti_icon_FILE_SAVE_ALL_22x22     = ":/qtilities/coregui/icons/file_save_all_22x22.png";

            // --------------------------------
            // Task Icons
            // --------------------------------
            //! Task Not Started Icon.
            const char * const qti_icon_TASK_NOT_STARTED_22x22                  = ":/qtilities/coregui/icons/task_not_started_22x22.png";
            //! Task Pause Icon.
            const char * const qti_icon_TASK_PAUSE_22x22                        = ":/qtilities/coregui/icons/task_pause_22x22.png";
            //! Task Resume Icon.
            const char * const qti_icon_TASK_RESUME_22x22                       = ":/qtilities/coregui/icons/task_resume_22x22.png";
            //! Task Stop Icon.
            const char * const qti_icon_TASK_STOP_22x22                         = ":/qtilities/coregui/icons/task_stop_22x22.png";
            //! Task Start Icon.
            const char * const qti_icon_TASK_START_22x22                        = ":/qtilities/coregui/icons/task_start_22x22.png";
            //! Task Busy Icon.
            const char * const qti_icon_TASK_BUSY_22x22                         = ":/qtilities/coregui/icons/task_busy_22x22.png";
            //! Task Busy With Warnings Icon.
            const char * const qti_icon_TASK_BUSY_WITH_WARNINGS_22x22           = ":/qtilities/coregui/icons/task_busy_with_warnings_22x22.png";
            //! Task Busy With Errors Icon.
            const char * const qti_icon_TASK_BUSY_WITH_ERRORS_22x22             = ":/qtilities/coregui/icons/task_busy_with_errors_22x22.png";
            //! Task Completed Icon.
            const char * const qti_icon_TASK_DONE_22x22                         = ":/qtilities/coregui/icons/task_done_22x22.png";
            //! Task Completed With Warnings Icon.
            const char * const qti_icon_TASK_DONE_WITH_WARNINGS_22x22           = ":/qtilities/coregui/icons/task_done_with_warnings_22x22.png";
            //! Task Completed With Errors Icon.
            const char * const qti_icon_TASK_DONE_WITH_ERRORS_22x22             = ":/qtilities/coregui/icons/task_done_with_errors_22x22.png";
            //! Task Failed Icon.
            const char * const qti_icon_TASK_FAILED_22x22                       = ":/qtilities/coregui/icons/task_failed_22x22.png";

            const char * const qti_icon_QTILITIES_SYMBOL_16x16              = ":/qtilities/coregui/icons/qtilities_symbol_16x16.png";
            const char * const qti_icon_QTILITIES_SYMBOL_22x22              = ":/qtilities/coregui/icons/qtilities_symbol_22x22.png";
            const char * const qti_icon_QTILITIES_SYMBOL_24x24              = ":/qtilities/coregui/icons/qtilities_symbol_24x24.png";
            const char * const qti_icon_QTILITIES_SYMBOL_32x32              = ":/qtilities/coregui/icons/qtilities_symbol_32x32.png";
            const char * const qti_icon_QTILITIES_SYMBOL_48x48              = ":/qtilities/coregui/icons/qtilities_symbol_48x48.png";
            const char * const qti_icon_QTILITIES_SYMBOL_64x64              = ":/qtilities/coregui/icons/qtilities_symbol_64x64.png";
            const char * const qti_icon_QTILITIES_SYMBOL_128x128            = ":/qtilities/coregui/icons/qtilities_symbol_128x128.png";
            const char * const qti_icon_QTILITIES_SYMBOL_WHITE_16x16        = ":/qtilities/coregui/icons/qtilities_symbol_white_16x16.png";
            const char * const qti_icon_QTILITIES_SYMBOL_WHITE_22x22        = ":/qtilities/coregui/icons/qtilities_symbol_white_22x22.png";
            const char * const qti_icon_QTILITIES_SYMBOL_WHITE_24x24        = ":/qtilities/coregui/icons/qtilities_symbol_white_24x24.png";
            const char * const qti_icon_QTILITIES_SYMBOL_WHITE_32x32        = ":/qtilities/coregui/icons/qtilities_symbol_white_32x32.png";
            const char * const qti_icon_QTILITIES_SYMBOL_WHITE_48x48        = ":/qtilities/coregui/icons/qtilities_symbol_white_48x48.png";
            const char * const qti_icon_QTILITIES_SYMBOL_WHITE_64x64        = ":/qtilities/coregui/icons/qtilities_symbol_white_64x64.png";
            const char * const qti_icon_QTILITIES_SYMBOL_WHITE_128x128      = ":/qtilities/coregui/icons/qtilities_symbol_white_128x128.png";
        }

        //! Namespace containing available images which forms part of the CoreGui Module.
        namespace Images {
            const char * const QTILITIES_LOGO_BT_300x300                = ":/qtilities/coregui/images/blueontrans_300x300.png";
        }

        //! Namespace containing available actions which forms part of the CoreGui Module.
        namespace Actions {
            // Standard actions & containers
            const char * const qti_action_MENUBAR_STANDARD                = "MainWindowMenuBar";
            const char * const qti_action_FILE                            = "&File";
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
            const char * const qti_action_CONTEXT                         = "&Context";
            const char * const qti_action_CONTEXT_HIERARCHY               = "&Hierarchy";
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

            // Edit menu and actions
            const char * const qti_action_EDIT                            = "&Edit";
            const char * const qti_action_EDIT_UNDO                       = "Edit.Undo";
            const char * const qti_action_EDIT_REDO                       = "Edit.Redo";
            const char * const qti_action_EDIT_COPY                       = "Edit.Copy";
            const char * const qti_action_EDIT_CUT                        = "Edit.Cut";
            const char * const qti_action_EDIT_PASTE                      = "Edit.Paste";
            const char * const qti_action_EDIT_LINE_WRAP                  = "Edit.LineWrap";
            const char * const qti_action_EDIT_CLEAR                      = "Edit.Clear";
            const char * const qti_action_EDIT_SELECT_ALL                 = "Edit.SelectAll";
            const char * const qti_action_EDIT_FIND                       = "Edit.Find";

            // View menu and actions
            const char * const qti_action_VIEW                             = "&View";
            const char * const qti_action_VIEW_ACTIVATE_NEXT               = "View.ActivateNext";
            const char * const qti_action_VIEW_ACTIVATE_PREV               = "View.ActivatePrev";
            const char * const qti_action_VIEW_CLOSE_ALL                   = "View.CloseAll";
            const char * const qti_action_VIEW_CLOSE_ACTIVE                = "View.CloseActive";
            const char * const qti_action_VIEW_CASCADE                     = "View.Cascade";
            const char * const qti_action_VIEW_TILE                        = "View.Tile";
            const char * const qti_action_VIEW_SWITCH_TO_PREVIOUS_MODE     = "View.SwitchToPreviousMode";

            // About menu and actions
            const char * const qti_action_ABOUT                            = "&About";
            const char * const qti_action_ABOUT_QTILITIES                  = "About.Qtilities";

            // Project actions
            const char * const qti_action_PROJECTS_NEW                      = "Projects.New";
            const char * const qti_action_PROJECTS_OPEN                     = "Projects.Open";
            const char * const qti_action_PROJECTS_CLOSE                    = "Projects.Close";
            const char * const qti_action_PROJECTS_SAVE                     = "Projects.Save";
            const char * const qti_action_PROJECTS_SAVE_AS                  = "Projects.SaveAs";
            const char * const qti_action_PROJECTS_RECENT                   = "Projects.Recent";
        }
    }
}

#endif // QtilitiesCoreGuiConstants_H
