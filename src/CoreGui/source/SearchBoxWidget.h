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
#include <QMenu>
#include <QTextEdit>
#include <QPlainTextEdit>

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
          and connected to those widgets. It supports both a search only mode and a search and replace mode set using the setWidgetMode()
          function. The buttons shown are set using setButtonFlags() and the search options available to the user is set using
          setSearchOptions().

          The widget in search and replace mode with all buttons enabled is shown below:
          \image html search_box_widget.jpg "Search Box Widget"
          \image latex search_box_widget.eps "Search Box Widget" width=\textwidth

          By default, the widget can be embedded anywhere as shown in the example below. When used in this way, the widget
          emits signals such as searchStringChanged(), searchOptionsChanged(), btnFindNext_clicked() etc. A signal is emitted for each button
          that is pressed.

          Below is an example where the search box widget is embedded at the bottom of an observer widget:
          \image html observer_widget_searching.jpg "Observer Widget Search Demonstration"
          \image latex observer_widget_searching.eps "Observer Widget Search Demonstration" width=4in

          The widget can also be used directly on a QTextEdit window by calling the setTextEdit() function. Alternatively it can
          be used on a QPlainTextEdit by calling the setPlainTextEditor() functions. When calling both these functions, the one that was
          called last will be used and at any time you can check what the widget target is using widgetTarget().

          When using the search box widget with text edit and plain text edit targets, the buttons (except the HideButton) will not trigger
          their signals and the search string related signals will not trigger either. The widget will handle these buttons directly on the
          specified text edit. The behavior of the replace buttons will also change. They will only be enabled when text is selected in the text
          editor.
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
            //! An enumeration which is used to indicate the target on which the search and place operations must be performed.
            enum WidgetTarget {
                ExternalTarget,         /*!< External target. Will emit needed signals for external target to connect to. */
                TextEdit,               /*!< Text edit set using setTextEdit(). */
                PlainTextEdit           /*!< Plain text edit set using setPlainTextEdit(). */
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
            SearchBoxWidget(SearchOptions search_options = AllSearchOptions, WidgetMode mode = SearchOnly, ButtonFlags buttons = AllButtons, QWidget *parent = 0);
            ~SearchBoxWidget();

            //! Gets the current search string in the search text box.
            QString currentSearchString() const;
            //! Sets the current replace string in the replace text box.
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
            //! Sets the text editor on which this widget must operate.
            /*!
              The widget can also be used directly on a QTextEdit window by calling the setTextEdit() function. When using the
              search box widget in this way, the buttons (except the HideButton) will not trigger their signals and the search string
              related signals will not trigger either. The widget will handle these buttons directly on the specified text edit.

              \sa setPlainTextEditor();
              */
            void setTextEditor(QTextEdit* textEdit);
            //! Returns a reference to the text editor on which this widget performs its operations.
            QTextEdit* textEditor() const;
            //! Sets the text editor on which this widget must operate.
            /*!
              The widget can also be used directly on a QPlainTextEdit window by calling the setTextEdit() function. When using the
              search box widget in this way, the buttons (except the HideButton) will not trigger their signals and the search string
              related signals will not trigger either. The widget will handle these buttons directly on the specified text edit.

              \sa setTextEditor();
              */
            void setPlainTextEditor(QPlainTextEdit* plainTextEdit);
            //! Returns a reference to the plain text editor on which this widget performs its operations.
            QPlainTextEdit* plainTextEditor() const;
            //! Current widget target.
            WidgetTarget widgetTarget() const;

            //! Function to set the button flags of the widget.
            void setButtonFlags(ButtonFlags button_flags);
            //! Function to get the button flags of the widget.
            ButtonFlags buttonFlags() const;
            //! Function to set the search options of the widget.
            void setSearchOptions(SearchOptions search_options);
            //! Function to get the search options of the widget.
            SearchOptions searchOptions() const;
            //! Function to set the WidgetMode of the widget.
            void setWidgetMode(WidgetMode widget_mode);
            //! Function to get the WidgetMode of the widget.
            WidgetMode widgetMode() const;
            //! Function providing access to the search options menu.
            /*!
              This reference can be used to add menu items to the search options menu.
              */
            QMenu* searchOptionsMenu();
            //! Function to set the message string displayed in the search box.
            void setMessage(const QString& message);
            //! Function which returns the QTextDocument::FindFlags for the current search options.
            QTextDocument::FindFlags findFlags() const;

        public slots:
            //! Sets the current search string in the search text box.
            void setCurrentSearchString(const QString& search_string);
            //! Gets the current replace string in the replace text box.
            void setCurrentReplaceString(const QString& replace_string);

        protected:
            void changeEvent(QEvent *e);

        private slots:
            void handleSearchStringChanged(const QString& string);
            void handleReplaceStringChanged(const QString& string);
            void handleOptionsChanged();
            void handleClose();
            void handleFindNext();
            void handleFindPrevious();
            void handleReplaceNext();
            void handleReplacePrevious();
            void handleReplaceAll();

        signals:
            //! Signal emitted when the search string changes with the new string passed as the paramater.
            void searchStringChanged(const QString& string);
            //! Signal emitted when the replace string changes with the new string passed as the paramater.
            void replaceStringChanged(const QString& string);
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
            //! Indicates that the replace all button was clicked.
            void btnReplaceAll_clicked();

        private:
            Ui::SearchBoxWidget* ui;
            SearchBoxWidgetData* d;
        };

        Q_DECLARE_OPERATORS_FOR_FLAGS(SearchBoxWidget::ButtonFlags)
        Q_DECLARE_OPERATORS_FOR_FLAGS(SearchBoxWidget::SearchOptions)
    }
}

#endif // SEARCHBOXWIDGET_H
