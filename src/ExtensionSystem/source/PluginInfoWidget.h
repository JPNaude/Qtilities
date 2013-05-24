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

#ifndef PLUGININFOWIDGET_H
#define PLUGININFOWIDGET_H

#include <QWidget>
#include "IPlugin.h"

namespace Ui {
    class PluginInfoWidget;
}
using namespace Qtilities::ExtensionSystem::Interfaces;

namespace Qtilities {
    namespace ExtensionSystem {

        /*!
          \class PluginInfoWidget
          \brief A widget that shows information about a plugin.

          <i>This class was added in %Qtilities v0.2.</i>
         */
        class PluginInfoWidget : public QWidget
        {
            Q_OBJECT

        public:
            explicit PluginInfoWidget(IPlugin* plugin, QWidget *parent = 0);
            ~PluginInfoWidget();

        protected:
            void changeEvent(QEvent *e);

        private:
            Ui::PluginInfoWidget *ui;
        };
    }
}

#endif // PLUGININFOWIDGET_H
