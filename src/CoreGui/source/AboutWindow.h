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

#ifndef ABOUTWINDOW_H
#define ABOUTWINDOW_H

#include "QtilitiesCoreGui_global.h"

#include <QWidget>

namespace Ui {
    class AboutWindow;
}

namespace Qtilities {
    namespace CoreGui {
        /*!
        \class AboutWindow
        \brief About widget which displays information about the application.

        Below is an example of the about window displayed when calling the Qtilities::CoreGui::QtilitiesCoreGui::aboutQtilities() slot.

        \image html about_window.jpg "About Window"
        \image latex about_window.eps "About Window" width=5in
          */
        class QTILITIES_CORE_GUI_SHARED_EXPORT AboutWindow : public QWidget {
            Q_OBJECT
        public:
            AboutWindow(QWidget *parent = 0);
            ~AboutWindow();

            //! Sets the copyright string to be displayed.
            void setCopyright(const QString& copyright);
            //! Sets the logo to be displayed.
            void setLogo(const QPixmap& pixmap);
            //! Sets the string to be displayed in the version label.
            void setVersionString(const QString& version_string);
            //! Sets the string to be displayed as an extended description. When an empty string is set, the extended description label will not be visible.
            void setExtendedDescription(const QString& extended_description);
            //! Sets the website address to be displayed. If no displayed name is specified, the url will be used.
            void setWebsite(const QString& url, const QString& displayed_name = QString());

        protected:
            void changeEvent(QEvent *e);

        private:
            Ui::AboutWindow *ui;
        };
    }
}

#endif // ABOUTWINDOW_H
