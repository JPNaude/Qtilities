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

#ifndef DYNAMICWIDGETVIEWER_H
#define DYNAMICWIDGETVIEWER_H

#include "ISideViewerWidget.h"
#include "QtilitiesCoreGui_global.h"

#include <QWidget>
#include <QMap>
#include <QString>

namespace Ui {
    class DynamicSideWidgetViewer;
}

namespace Qtilities {
    namespace CoreGui {
        using namespace Qtilities::CoreGui::Interfaces;

        /*!
          \struct DynamicSideWidgetViewerPrivateData
          \brief The DynamicSideWidgetViewerPrivateData class stores private data used by the DynamicSideWidgetViewer class.
         */
        struct DynamicSideWidgetViewerPrivateData;

        /*!
        \class DynamicSideWidgetViewer
        \brief The widget which can display dynamic side widgets (widgets implementing the ISideViewerWidget interface).
          */
        class QTILITIES_CORE_GUI_SHARED_EXPORT DynamicSideWidgetViewer : public QWidget {
            Q_OBJECT
        public:
            DynamicSideWidgetViewer(int mode_destination, QWidget *parent = 0);
            ~DynamicSideWidgetViewer();

            //! Adds a widget to the bottom of the dynamic viewer.
            /*!
              This functions gets a map with name-interface pairs of all the side viewer widgets found in the global
              object pool. The function will automatically filter the widgets depending on the destination mode assigned
              to this widget in its constructor.

              \param text_iface_map A map with the name-interface pairs of side viewer widgets in the global object pool.
              \param is_exclusive When true, the side viewer widgets are exclusive which means that they can only appear once. If a widget is
              already active, it will not appear in the combo box to be switched to again.
              \param widget_order A list containing the names of side widget you would like to show first (at the top) in your side widget viewer.
              */
            void setIFaceMap(QMap<QString, ISideViewerWidget*> text_iface_map, bool is_exclusive = false, const QStringList& widget_order = QStringList());
            //! Indicates if this widget handles dynamic widgets in an exclusive way.
            /*!
              \sa setIFaceMap();
              */
            bool isExclusive() const;
            //! Returns a list of all widget names available in this widget.
            QStringList widgetNames() const;
            //! Returns a list of all side viewer widget interfaces displayed in this viewer.
            /*!
             * \return A list of all side viewer widget interfaces displayed in this viewer.
             *
             * <i>This function  was added in %Qtilities v1.2.</i>
             */
            QList<ISideViewerWidget*> sideViewerWidgetInterfaces() const;
            //! Returns a list of all side viewer widgets displayed in this viewer.
            /*!
             * \return A list of all side viewer widgets displayed in this viewer.
             *
             * <i>This function  was added in %Qtilities v1.2.</i>
             */
            QList<QWidget*> sideViewerWidgets() const;

            //! Specifies side widgets which should be hidden.
            /*!
             * Calling this function will hide the specified widgets and also remove it from all combo boxes. All
             * widgets not part of this list will be handled as normal.
             *
             * <i>This function  was added in %Qtilities v1.3.</i>
             */
            void setHiddenSideWidgets(const QStringList& widget_names);
            //! Gets the side widgets which are hidden.
            /*!
             * <i>This function  was added in %Qtilities v1.3.</i>
             */
            QStringList hiddenSideWidgets() const;

        public slots:
            //! Handles the deletion of side widgets.
            void handleSideWidgetDestroyed(QWidget* widget);
            //! Handles requests for new side widgets.
            void handleNewSideWidgetRequest();
        private slots:
            //! Function which updates the items in all the combo boxes of active wrappers in this side widget viewer.
            /*!
              \param exclude_text When set, all boxes except the box with the exclude text will be updated.
              */
            void updateWrapperComboBoxes(const QString& exclude_text = QString());

        signals:
            //! Signal indicating that this side widget viewer must be hidden.
            void toggleVisibility(bool toggle);

        protected:
            void changeEvent(QEvent *e);

        private:
            //! Gets a QStringList of all active wrapper names.
            QStringList activeWrapperNames() const;

            Ui::DynamicSideWidgetViewer *ui;
            DynamicSideWidgetViewerPrivateData* d;
        };
    }
}

#endif // DYNAMICWIDGETVIEWER_H
