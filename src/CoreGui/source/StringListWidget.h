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

#ifndef STRING_LIST_WIDGET_H
#define STRING_LIST_WIDGET_H

#include "QtilitiesCoreGui_global.h"

#include <QMainWindow>
#include <QToolBar>
#include <QListView>

namespace Ui
{
    class StringListWidget;
}
class QStringListModel;

namespace Qtilities {
    namespace CoreGui {
        /*!
        \struct StringListWidgetPrivateData
        \brief A structure storing private data in the StringListWidget class.
          */
        struct StringListWidgetPrivateData;

        /*!
        \class StringListWidget
        \brief A widget which allows easy editing of a QStringList.

        The StringListWidget class is a simple widget which can be used to display a QStringList. You can give your string list a type using setStringType() and the list of strings to be displayed is set using setStringList().

        The widget is a QMainWindow with a toolbar at the top containing actions to remove and add strings. To customize the operation of the widget the string list model can be accessed using stringListModel() and the view can be accessed through listView().

        Below is an image of the string list widget with the string type set to "Architecture".

        \image html class_stringlistwidget_screenshot.jpg "The String List Widget"

        <i>This class was added in %Qtilities v1.0.</i>
          */
        class QTILITIES_CORE_GUI_SHARED_EXPORT StringListWidget : public QMainWindow
        {
            Q_OBJECT
            Q_ENUMS(ListType)

        public:
            //! The possible types of items which are listed in the string list widget.
            enum ListType {
                PlainStrings         = 0, /*!< Plain strings. */
                FilePaths            = 1, /*!< File paths. */
                Directories          = 2  /*!< Directories. */
            };           

            StringListWidget(const QStringList& string_list = QStringList(), Qt::ToolBarArea toolbar_area = Qt::TopToolBarArea, QWidget * parent = 0, Qt::WindowFlags flags = 0);
            ~StringListWidget();

        public:
            //! Gets the current list of strings.
            QStringList stringList() const;
            //! Sets the current list of string.
            void setStringList(const QStringList& string_list);     

            //! Gets the current list of non-removable only strings.
            QStringList nonRemovableStringList() const;
            //! Sets the current list of non-removable only strings.
            void setNonRemovableStringList(const QStringList& string_list);

            //! Sets if this list is read only.
            void setReadOnly(bool read_only);
            //! Gets if this list is read only.
            bool readOnly() const;

            //! Gets the string type.
            QString stringType() const;
            //! Sets the string type.
            void setStringType(const QString& string_type);

            //! Gets the type of items in the list.
            ListType listType() const;
            //! Sets the type of items in the list.
            void setListType(const ListType& list_type);

            //! Gets the file open dialog path to be used for FilePaths and Directories ListType.
            QString fileOpenDialogPath() const;
            //! Sets the file open dialog path to be used for FilePaths and Directories ListType.
            void setFileOpenDialogPath(const QString& open_dialog_path);

            //! Gets the file open dialog filter to be used for FilePaths ListType.
            QString fileOpenDialogFilter() const;
            //! Sets the file open dialog filter to be used for FilePaths ListType.
            void setFileOpenDialogFilter(const QString& open_dialog_filter);

            //! Returns the list view used to display the strings.
            QListView* listView();
            //! Returns the QStringListModel used as the model in the list view.
            QStringListModel* stringListModel();

            //! Sets if files and folders must be opened when double clicked.
            /*!
              Only applicable for FilePaths and Directories list types, see listType().

              \sa openOnDoubleClick()

              <i>This function was added in %Qtilities v1.1.</i>
              */
            void setOpenOnDoubleClick(bool open_on_double_click);
            //! Gets if files and folder must be opened when double clicked.
            /*!
              False by default.

              \sa setOpenOnDoubleClick()

              <i>This function was added in %Qtilities v1.1.</i>
              */
            bool openOnDoubleClick() const;

        signals:
            //! Signal emitted as soon as the list of string changed.
            void stringListChanged(const QStringList& string_list);
            //! Signal emitted when the selection changes.
            void selectionChanged();

        private slots:
            void handleAddString();
            void handleRemoveString();
            void handleDoubleClick(QModelIndex index);

        private:
            Ui::StringListWidget *ui;
            StringListWidgetPrivateData* d;
        };
    }
}
#endif // STRING_LIST_WIDGET_H
