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

#ifndef ACTIONMANAGER_H
#define ACTIONMANAGER_H

#include <QObject>
#include <QAction>
#include <QShortcut>
#include <QMainWindow>

#include "QtilitiesCoreGui_global.h"
#include "IActionManager.h"

namespace Qtilities {
    namespace CoreGui {
        using namespace Qtilities::CoreGui::Interfaces;

        /*!
        \struct ActionManagerPrivateData
        \brief A structure storing private data in the ActionManager class.
          */
        struct ActionManagerPrivateData;

        /*!
        \class ActionManager
        \brief A class which represents an action manager.

        For more information see the \ref page_action_management article.
          */
        class QTILITIES_CORE_GUI_SHARED_EXPORT ActionManager : public IActionManager
        {
            Q_OBJECT
            Q_INTERFACES(Qtilities::CoreGui::Interfaces::IActionManager)

        public:
            ActionManager(QObject* parent = 0);
            ~ActionManager();

            // --------------------------------
            // IActionManager Implementation
            // --------------------------------
            ActionContainer* createMenu(const QString &id,
                                        bool& existed);
            ActionContainer* menu(const QString &id);
            ActionContainer* createMenuBar(const QString &id,
                                           bool& existed);
            ActionContainer* menuBar(const QString &id);
            Command *registerAction(const QString &id,
                                    QAction *action,
                                    const QList<int> &context = QList<int>());
            Command *registerActionPlaceHolder(const QString &id,
                                               const QString& user_text,
                                               const QKeySequence& shortcut = QKeySequence(),
                                               const QList<int> &context = QList<int>(),
                                               const QIcon& icon = QIcon());
            Command* registerShortcut(const QString &id,
                                      const QString& user_text,
                                      QShortcut *shortcut,
                                      const QList<int> &active_contexts = QList<int>());
            Command* command(const QString &id) const;
            ActionContainer *actionContainer(const QString &id) const;

            void restoreDefaultShortcuts();
            bool saveShortcutMapping(const QString& file_name,
                                     Qtilities::ExportVersion version = Qtilities::Qtilities_Latest);
            bool loadShortcutMapping(const QString& file_name);
            QWidget* commandEditor();
            QList<Command*> commandsWithKeySequence(QKeySequence shortcut);
            Core::Observer* commandObserver();
            Core::Observer* actionContainerObserver();

        public slots:
            void unregisterCommandsForContext(int context);
            void handleContextChanged(QList<int> new_contexts);

        private:
            ActionManagerPrivateData* d;
            static bool showed_warning;
        };
    }
}

#endif // ACTIONMANAGER_H
