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

#ifndef SIDEWIDGETWRAPPER_H
#define SIDEWIDGETWRAPPER_H

#include "QtilitiesCoreGui_global.h"
#include "ISideViewerWidget.h"

#include <QMainWindow>
#include <QMap>
#include <QString>

namespace Ui {
    class DynamicSideWidgetWrapper;
}

namespace Qtilities {
    namespace CoreGui {
        using namespace Qtilities::CoreGui::Interfaces;

        /*!
          \struct DynamicSideWidgetWrapperPrivateData
          \brief The DynamicSideWidgetWrapperPrivateData class stores private data used by the DynamicSideWidgetWrapper class.
         */
        struct DynamicSideWidgetWrapperPrivateData;

        /*!
        \class DynamicSideWidgetWrapper
        \brief A wrapper for side viewer widgets.
          */
        class QTILITIES_CORE_GUI_SHARED_EXPORT DynamicSideWidgetWrapper : public QWidget {
            Q_OBJECT
        public:
            //! Constructs a dynamic side widget wrapper widget.
            /*!
              \param text_iface_map The string-interface map which must be shown in this widget.
              \param current_text The current widget that is shown.
              \param is_exclusive Indicates if this wrapper is part of an exclusive side widget viewer.
              \param parent The parent of this widget.
              */
            DynamicSideWidgetWrapper(QMap<QString, ISideViewerWidget*> text_iface_map, const QString& current_text, const bool is_exclusive, QWidget* parent = 0);
            ~DynamicSideWidgetWrapper();

            //! Static function to access the style this widget uses for its combo boxes.
            /*!
             * <i>This function  was added in %Qtilities v1.2.</i>
             */
            static QLatin1String comboBoxStyle();

            //! Function which returns the current text in the combo box.
            QString currentText() const;
            //! Returns the current widget displayed in this wrapper.
            /*!
             * \return The current widget displayed in this wrapper.
             *
             * <i>This function  was added in %Qtilities v1.2.</i>
             */
            QWidget* currentWidget() const;

        public slots:
            void handleCurrentIndexChanged(const QString& text);
            //! This function expects a map of widgets which can still be produced.
            /*!
              \note The current widget should not be in map list.
              */
            void updateAvailableWidgets(QMap<QString, ISideViewerWidget*> text_iface_map);
            void close();

        signals:
            void currentTextChanged(const QString& text);
            void newSideWidgetRequest();
            void aboutToBeDestroyed(QWidget* widget);

        private:
            //! Refreshes the new widget action's state.
            void refreshNewWidgetAction();

            Ui::DynamicSideWidgetWrapper *ui;
            DynamicSideWidgetWrapperPrivateData* d;
        };
    }
}

#endif // SIDEWIDGETWRAPPER_H
