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

#ifndef SIDE_WIDGET_FILE_SYSTEM_H
#define SIDE_WIDGET_FILE_SYSTEM_H

#include <Factory>

#include <QObject>
#include <QtGui>

namespace Ui {
    class SideWidgetFileSystem;
}

namespace Qtilities {
    namespace Examples {
        namespace MainWindow {
            using namespace Qtilities::Core;

            /*!
            \struct SideWidgetFileSystemData
            \brief Structure used by the SideWidgetFileSystem class to store private data.
              */
            struct SideWidgetFileSystemData;

            /*!
            \class SideWidgetFileSystem
            \brief A widget which provides access to the file system as a side widget.
              */
            class SideWidgetFileSystem : public QWidget
            {
                Q_OBJECT

            public:
                explicit SideWidgetFileSystem(QWidget *parent = 0);
                virtual ~SideWidgetFileSystem() {}

                // --------------------------------
                // Factory Interface Implemenation
                // --------------------------------
                static FactoryItem<QWidget, SideWidgetFileSystem> factory;

            private slots:
                void handleRootPathChanged(const QString& newPath);
                void handleBtnBrowse();
                void handleDoubleClicked(const QModelIndex& index);

            signals:
                void requestEditor(const QString& file_name);

            protected:
                Ui::SideWidgetFileSystem *m_ui;
                SideWidgetFileSystemData* d;
            };
        }
    }
}

#endif // SCRIPTINGFILESYSTEMWIDGET_H
