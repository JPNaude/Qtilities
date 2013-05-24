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

#ifndef ABOUTWINDOW_H
#define ABOUTWINDOW_H

#include <QWidget>

namespace Ui {
    class qti_private_AboutWindow;
}

namespace Qtilities {
    namespace CoreGui {
        /*!
        \class qti_private_AboutWindow
        \brief About %Qtilities window.
          */
        class qti_private_AboutWindow : public QWidget {
            Q_OBJECT
        public:
            qti_private_AboutWindow(QWidget *parent = 0);
            ~qti_private_AboutWindow();

        protected:
            void changeEvent(QEvent *e);

        private:
            Ui::qti_private_AboutWindow *ui;
        };
    }
}

#endif // ABOUTWINDOW_H
