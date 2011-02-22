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

#ifndef STRING_LIST_WIDGET_H
#define STRING_LIST_WIDGET_H

#include "QtilitiesCoreGui_global.h"
#include "IMode.h"
#include "ModeManager.h"

#include <QResizeEvent>
#include <QMainWindow>
#include <Logger>

namespace Ui
{
    class StringListWidget;
}

namespace Qtilities {
    namespace CoreGui {
        using namespace Qtilities::CoreGui::Interfaces;
        using namespace Qtilities::Logging;

        /*!
        \struct StringListWidgetPrivateData
        \brief A structure storing private data in the StringListWidget class.
          */
        struct StringListWidgetPrivateData;

        /*!
        \class StringListWidget
        \brief A widget which allows easy editing of a QStringList.

        <i>This class was added in %Qtilities v0.3.</i>
          */
        class QTILITIES_CORE_GUI_SHARED_EXPORT StringListWidget : public QMainWindow
        {
            Q_OBJECT

        public:
            StringListWidget(const QStringList& string_list = QStringList(), const QString& string_type = QString(), QWidget * parent = 0, Qt::WindowFlags flags = 0);
            ~StringListWidget();

            //! Returns the current list of strings.
            QStringList stringList() const;
            //! Sets the current list of string.
            void setStringList(const QStringList& string_list);

            //! Returns the string type.
            QString stringType() const;
            //! Sets the string type.
            void setStringType(const QString& string_type);

        signals:
            //! Signal emitted as soon as the list of string changed.
            void stringListChanged(const QStringList& string_list);

        private slots:
            void handleAddString();
            void handleRemoveString();

        private:
            Ui::StringListWidget *ui;
            StringListWidgetPrivateData* d;
        };
    }
}
#endif // STRING_LIST_WIDGET_H
