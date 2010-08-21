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

namespace Qtilities {
    //! Namespace containing all the classes which forms part of the CoreGui Module.
    namespace CoreGui {
        //! Namespace containing constants used inside the CoreGui Module.
        namespace Constants {
            //! %Factory tag for naming policy subject filters.
            const char * const FACTORY_TAG_NAMING_POLICY_FILTER     = "Naming Policy Filter";
            //! The default file name used to save session shortcut mappings by the Qtilities::CoreGui::ActionManager class.
            const char * const FILE_SHORTCUT_MAPPING                = "shortcut_mapping.smf";
            //! Context used to register observer widgets in the Qtilities::Core::ContextManager class.
            const char * const CONTEXT_OBSERVER_WIDGET              = "Context.ObserverWidget";
            //! Context used to register Qtilities::CoreGui::WidgetLoggerEngineFrontend classes in the Qtilities::Core::ContextManager class.
            const char * const CONTEXT_LOGGER_WIDGET                = "Context.LoggerWidget";
        }

        //! Namespace containing available icons which forms part of the CoreGui Module.
        namespace Icons {
            //! An icon used for observers.
            const char * const ICON_OBSERVER          = ":/icons/observer_default.png";
            //! Icon used in observer widgets for action to push down into an observer hierachy in the current window.
            const char * const ICON_PUSH_DOWN_CURRENT = ":/icons/push_down_current_16x16.png";
            //! Icon used in observer widgets for action to push down into an observer hierachy in a new window.
            const char * const ICON_PUSH_DOWN_NEW     = ":/icons/push_down_new_16x16.png";
            //! Icon used in observer widgets for action to push up in an observer hierachy in the current window.
            const char * const ICON_PUSH_UP_CURRENT   = ":/icons/push_up_current_16x16.png";
            //! Icon used in observer widgets for action to push up in an observer hierachy in a new window.
            const char * const ICON_PUSH_UP_NEW       = ":/icons/push_up_new_16x16.png";
            //! Icon used for action to remove a single object.
            const char * const ICON_REMOVE_ONE        = ":/icons/remove_one_16x16.png";
            //! Icon used for action to remove all objects.
            const char * const ICON_REMOVE_ALL        = ":/icons/remove_all_16x16.png";
            //! Icon used for action to delete a single object.
            const char * const ICON_DELETE_ONE        = ":/icons/delete_one_16x16.png";
            //! Icon used for action to delete all objects.
            const char * const ICON_DELETE_ALL        = ":/icons/delete_all_16x16.png";
            //! Icon used for action to search objects.
            const char * const ICON_MAGNIFY           = ":/icons/magnify_16x16.png";
            //! Icon used for new action.
            const char * const ICON_NEW               = ":/icons/new_16x16.png";
            //! Zoom in icon.
            const char * const ICON_MAGNIFY_PLUS      = ":/icons/magnify_plus_16x16.png";
            //! Zoom out icon.
            const char * const ICON_MAGNIFY_MINUS     = ":/icons/magnify_minus_16x16.png";
            //! Icon used to indicate a split, or duplication operation.
            const char * const ICON_SPLIT             = ":/icons/split_16x16.png";
            //! Icon used to switch between observer widget modes.
            const char * const ICON_SWITCH_VIEW       = ":/icons/switch_view_16x16.png";
            //! Refresh icon.
            const char * const ICON_REFRESH           = ":/icons/refresh_16x16.png";
            //! Success icon.
            const char * const ICON_SUCCESS           = ":/icons/success.png";
            //! Error icon.
            const char * const ICON_ERROR             = ":/icons/error.png";
            //! Icon used for observer access column in observer widgets.
            const char * const ICON_ACCESS            = ":/icons/access_22x22.png";
            //! Icon used for child count column in observer widgets.
            const char * const ICON_CHILD_COUNT       = ":/icons/child_count_22x22.png";
            //! Icon used for object type column in observer widgets.
            const char * const ICON_TYPE_INFO         = ":/icons/type_info_22x22.png";
            //! Icon used to indicate that an observer has locked access.
            const char * const ICON_LOCKED            = ":/icons/locked_22x22.png";
            //! Icon used to indicate that an observer has read only access.
            const char * const ICON_READ_ONLY         = ":/icons/read_only_22x22.png";
            //! Icon used for search options button in search box widget.
            const char * const ICON_SEARCH_OPTIONS    = ":/icons/search_options_22x22.png";
            //! Icon used for an object property.
            const char * const ICON_PROPERTY          = ":/icons/property_icon_16x16.png";
            //! Icon used for an object method (slot).
            const char * const ICON_METHOD            = ":/icons/method_icon_16x16.png";
            //! Icon used for an object event (signal).
            const char * const ICON_EVENT             = ":/icons/event_icon_16x16.png";
            //! Icon used for managers in %Qtilities.
            const char * const ICON_MANAGER_16x16     = ":/icons/manager_icon_16x16.png";
            const char * const ICON_FOLDER_16X16      = ":/icons/folder_icon_16x16.png";
            //! Folder icon, used for categories in observer widgets in tree mode.
            const char * const ICON_EXPORT_16x16      = ":/icons/export_16x16.png";
            //! Print icon.
            const char * const ICON_PRINT_24x24       = ":/icons/print_24x24.png";
            //! Clear icon.
            const char * const ICON_CLEAR_24x24       = ":/icons/clear_24x24.png";
            //! Save icon.
            const char * const ICON_SAVE_24x24        = ":/icons/save_24x24.png";
            //! Icon used for dynamic side widget remove action.
            const char * const ICON_VIEW_REMOVE_16x16   = ":/icons/view_remove_16x16.png";
            //! Icon used for dynamic side widget new action.
            const char * const ICON_VIEW_NEW_16x16      = ":/icons/view_new_16x16.png";

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
            const char * const MENU_SELECTION_MERGE_ITEMS           = "Selection.MergeItems";
            const char * const MENU_SELECTION_SPLIT_ITEMS           = "Selection.SplitItems";
            const char * const MENU_SELECTION_IMPORT                = "Selection.Import";
            const char * const MENU_SELECTION_EXPORT                = "Selection.Export";

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
