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

#ifndef SIDE_WIDGET_FILE_SYSTEM_H
#define SIDE_WIDGET_FILE_SYSTEM_H

#include <Factory>
#include "QtilitiesCoreGui_global.h"

#include <QObject>
#include <QtGui>

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QtWidgets>
#endif

namespace Ui {
    class SideWidgetFileSystem;
}

namespace Qtilities {
    namespace CoreGui {
        using namespace Qtilities::Core;

        /*!
        \struct SideWidgetFileSystemPrivateData
        \brief Structure used by the SideWidgetFileSystem class to store private data.
          */
        struct SideWidgetFileSystemPrivateData;

        /*!
        \class SideWidgetFileSystem
        \brief A widget which provides access to the file system as a side widget.

        A side widget which can be used in the %Qtilities main window architecture. For example:

\code
#define MODE_ID 101

DynamicSideWidgetViewer* side_viewer_widget = new DynamicSideWidgetViewer(MODE_ID);
QMap<QString, ISideViewerWidget*> text_iface_map;
QList<int> file_system_modes;
file_system_modes << MODE_ID;

SideViewerWidgetFactory* factory = new SideViewerWidgetFactory(&SideWidgetFileSystem::factory,"My Label",file_system_modes,QList<int>(),false);
text_iface_map[factory->widgetLabel()] = factory;
connect(factory,SIGNAL(newWidgetCreated(QWidget*)),SLOT(handleNewFileSystemWidget(QWidget*)));

side_viewer_widget->setIFaceMap(text_iface_map,true);
side_viewer_widget->show();
\endcode

        \sa DynamicSideWidgetViewer, SideViewerWidgetFactory

        <i>This class was added in %Qtilities v0.2.</i>
          */
        class QTILITIES_CORE_GUI_SHARED_EXPORT SideWidgetFileSystem : public QWidget
        {
            Q_OBJECT

        public:
            //! Constructs a side widget file system object.
            /*!
              \param start_path The path to use when this widget is constructed. By default it will use QDir::currentPath(), as well as in the case where start_path does not exist.
              \param parent The parent widget.
              */
            explicit SideWidgetFileSystem(const QString& start_path = QString(), QWidget *parent = 0);
            virtual ~SideWidgetFileSystem();
            bool eventFilter(QObject *object, QEvent *event);
            void dragEnterEvent(QDragEnterEvent *event);
            void dropEvent(QDropEvent *event);

            // --------------------------------
            // Factory Interface Implementation
            // --------------------------------
            static FactoryItem<QWidget, SideWidgetFileSystem> factory;

            //! Function to set the path of this side viewer widget.
            void setPath(const QString& path);
            //! Attempts to select the specified path in the view.
            /*!
              <i>This function was added in %Qtilities v1.2.</i>
              */
            void selectPath(const QString& path);
            //! Function to get the path of this side viewer widget.
            QString path() const;
            //! Function to get the path of this side viewer widget.
            /*!
              <i>This function was added in %Qtilities v1.1.</i>
              */
            QString filePath() const;

            //! Function which enables/disables opening of files when double clicking on them.
            /*!
              This is enabled by default.

              <i>This function was added in %Qtilities v1.1.</i>
              */
            void toggleDoubleClickFileOpen(bool open_file);

            //! Reconstructs the QFileSystemModel used inside the widget.
            /*!
              There is known problem with QFileSystemWatcher/QFileSystemModel which causes directories to not be deleted property when they
              are viewed. Thus, thus function will destroy and reconstruct the model with the path pointed to QApplication::applicationDirPath().
              */
            void releasePath();

        private slots:
            void handleRootPathChanged(const QString& newPath);
            void handleBtnBrowse();
            void handleDoubleClicked(const QModelIndex& index);
            void on_btnCdUp_clicked();
            void on_txtCurrentPath_editingFinished();
            void handleClicked(const QModelIndex& index);
            void on_treeView_doubleClicked(QModelIndex index);

        signals:
            void requestEditor(const QString& file_name);

        protected:
            Ui::SideWidgetFileSystem *ui;
            SideWidgetFileSystemPrivateData* d;
        };
    }
}

#endif // SCRIPTINGFILESYSTEMWIDGET_H
