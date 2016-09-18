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

#ifndef GENERIC_PROPERTY_PATH_EDITOR_H
#define GENERIC_PROPERTY_PATH_EDITOR_H

#ifdef QTILITIES_PROPERTY_BROWSER
#include <GenericProperty.h>
using namespace Qtilities::Core;

#include "StringListWidget.h"

#include <QWidget>

namespace Ui {
class GenericPropertyPathEditor;
}

namespace Qtilities {
    namespace CoreGui {
        class GenericPropertyPathEditor : public QWidget
        {
            Q_OBJECT

        public:
            explicit GenericPropertyPathEditor(GenericProperty::PropertyType property_type, QWidget *parent = 0);
            ~GenericPropertyPathEditor();

            void setText(const QString& text);
            void setItemFilter(const QString& file_filter);
            void setEditable(bool editable);
            void setPropertyName(const QString& name);
            Q_DECL_DEPRECATED void setListSeperatorBackend(const QString& list_seperator);
            void setListSeparatorBackend(const QString& list_separator);
            void setDefaultOpenPath(const QString& default_open_path);
            void setCurrentValues(const QStringList& current_values);

        private slots:
            void on_btnBrowse_clicked();
            void handleValueChanged(const QString& value);

        signals:
            void valueChanged(const QString& value);

        private:
            Ui::GenericPropertyPathEditor *ui;
            QString d_file_filter;
            QString d_default_open_path;
            GenericProperty::PropertyType d_property_type;
            QString d_list_separator_backend;
            QStringList d_current_values;
            bool d_editable;
            QString d_property_name;
        };
    }
}

#endif // QTILITIES_PROPERTY_BROWSER
#endif // GENERIC_PROPERTY_PATH_EDITOR_H
