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

#ifndef GENERIC_PROPERTY_PATH_EDITOR_LIST_WRAPPER_H
#define GENERIC_PROPERTY_PATH_EDITOR_LIST_WRAPPER_H

#ifdef QTILITIES_PROPERTY_BROWSER
#include <StringListWidget>
using namespace Qtilities::CoreGui;

#include <QDialog>

namespace Ui {
class GenericPropertyPathEditorListWrapper;
}

namespace Qtilities {
    namespace CoreGui {
        class GenericPropertyPathEditorListWrapper : public QDialog
        {
            Q_OBJECT

        public:
            explicit GenericPropertyPathEditorListWrapper(StringListWidget* list_widget, QWidget *parent = 0);
            ~GenericPropertyPathEditorListWrapper();

        private slots:
            void on_buttonBox_accepted();
            void on_buttonBox_rejected();

        private:
            Ui::GenericPropertyPathEditorListWrapper *ui;
        };
    }
}

#endif // QTILITIES_PROPERTY_BROWSER
#endif // GENERIC_PROPERTY_PATH_EDITOR_LIST_WRAPPER_H
