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

#ifndef SEARCHBOXWIDGET_H
#define SEARCHBOXWIDGET_H

#include <QWidget>

#include "QtilitiesCoreGui_global.h"

namespace Ui {
    class SearchBoxWidget;
}

namespace Qtilities {
    namespace CoreGui {
        /*!
          \struct SearchBoxWidgetData
          \brief The SearchBoxWidgetData class contains private data which is used by a SearchBoxWidget widget.

          \sa Qtilities::CoreGui::SearchBoxWidget
          */
        class SearchBoxWidgetData;

        /*!
          \class Qtilities::CoreGui::SearchBoxWidget
          \brief The SearchBoxWidget class provides a ready to use search/replace widget.

          The search box widget is a generic, parameterizable search box widget. It is usually embedded into other widgets
          and connected to those widgets. It supports both a search only mode, and a search and replace mode.

          The search and replace mode with all buttons enabled is shown below:
          \image html search_box_widget.jpg "Search Box Widget"
          \image latex search_box_widget.eps "Search Box Widget" width=\textwidth

          Below is an example where the search box widget is embedded at the bottom of an observer widget:
          \image html observer_widget_searching.jpg "Observer Widget Search Demonstration"
          \image latex observer_widget_searching.eps "Observer Widget Search Demonstration" width=4in
          \todo
          - Wishlist: Add a search history combo box, maybe use auto-completion?
          */
        class QTILITIES_CORE_GUI_SHARED_EXPORT SearchBoxWidget : public QWidget {
            Q_OBJECT
            Q_ENUMS(WidgetMode)
            Q_ENUMS(ButtonFlag)
            Q_ENUMS(SearchOptions)

        public:
            //! An enumeration which is used to indicate in which mode the widget must be used.
            enum WidgetMode {
                SearchOnly,             /*!< The widget will only show search related items. */
                SearchAndReplace        /*!< The widget will show both show search and replace related items. */
            };
            //! An enumeration which is used to indicate which buttons should be visible in the widget.
            enum ButtonFlag {
                NoButtons = 0,          /*!< No buttons will be visible in the widget. */
                NextButtons = 1,        /*!< The next buttons will be visible in the widget. */
                PreviousButtons = 2,    /*!< The previous buttons will be visible in the widget. */
                HideButton = 4,         /*!< The button to hide the search box will be visible in the widget. */
                AllButtons = NextButtons | PreviousButtons | HideButton
            };
            Q_DECLARE_FLAGS(ButtonFlags, ButtonFlag);
            Q_FLAGS(ButtonFlags);
            //! An enumeration which indicates which search options must be present in the widget.
            enum SearchOption {
                CaseSensitive = 1,      /*!< Indicates that an action to toggle case sensitivity must be present. */
                WholeWordsOnly = 2,     /*!< Indicates that an action to toggle whole words only must be present. */
                RegEx = 4,              /*!< Indicates that an action to toggle regular expressions must be present. */
                AllSearchOptions = CaseSensitive | WholeWordsOnly | RegEx
            };
            Q_DECLARE_FLAGS(SearchOptions, SearchOption);
            Q_FLAGS(SearchOptions);

            //! Constructs a search box widget using the paramaters to customize the look of the widget.
            /*!

              */
            SearchBoxWidget(SearchOptions search_options = AllSearchOptions, WidgetMode mode = SearchOnly, ButtonFlags buttons = AllButtons, QWidget *parent = 0);
            ~SearchBoxWidget();

            //! Returns the current search string in the search text box.
            QString currentSearchString() const;
            //! Returns the current replace string in the replace text box.
            QString currentReplaceString() const;
            //! Indicates if the search string must be handled in a case sensitive way.
            bool caseSensitive() const;
            //! Indicates if the search string must match only whole words.
            bool wholeWordsOnly() const;
            //! Indicates if the search string must be interpreted as a regular expression.
            bool regExpression() const;
            //! Sets the case sensitivity search option.
            void setCaseSensitive(bool toggle);
            //! Sets the whole words only search option.
            void setWholeWordsOnly(bool toggle);
            //! Sets the regular expression search option.
            void setRegExpression(bool toggle);
            //! Sets focus to the search string text editor.
            void setEditorFocus();

        protected:
            void changeEvent(QEvent *e);

        private slots:
            void on_txtSearchString_textChanged(const QString & text);
            void on_txtReplaceString_textChanged(const QString & text);

        signals:
            //! Signal emitted when the search string changes with the new string passed as the paramater.
            void searchStringChanged(const QString & text);
            //! Indicates that the search options changed.
            void searchOptionsChanged();
            //! Indicates that the close button was clicked.
            void btnClose_clicked();
            //! Indicates that the find previous button was clicked.
            void btnFindPrevious_clicked();
            //! Indicates that the find next button was clicked.
            void btnFindNext_clicked();
            //! Indicates that the replace previous button was clicked.
            void btnReplacePrevious_clicked();
            //! Indicates that the replace next button was clicked.
            void btnReplaceNext_clicked();

        private:
            Ui::SearchBoxWidget* ui;
            SearchBoxWidgetData* d;
        };

        Q_DECLARE_OPERATORS_FOR_FLAGS(SearchBoxWidget::ButtonFlags)
        Q_DECLARE_OPERATORS_FOR_FLAGS(SearchBoxWidget::SearchOptions)
    }
}

#endif // SEARCHBOXWIDGET_H
