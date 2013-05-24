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

#ifndef ICLIPBOARD
#define ICLIPBOARD

#include "QtilitiesCoreGui_global.h"

#include <IObjectBase>

#include <QObject>
#include <QMimeData>

namespace Qtilities {
    namespace CoreGui {
        class ActionContainer;
        class Command;
        using namespace Qtilities::Core::Interfaces;

        namespace Interfaces {
/*!
\class IClipboard
\brief Interface used to communicate with the clipboard manager.

The goal of the clipboard manager is to register backends (associated with the standard context) for the
Qtilities::CoreGui::Actions::qti_action_EDIT_COPY, Qtilities::CoreGui::Actions::qti_action_EDIT_CUT and
Qtilities::CoreGui::Actions::qti_action_EDIT_PASTE action placeholders if they exists. This allows control
over disabling and enabling these three actions in %Qtilities applications. For example, the paste action
should only be enabled if something exists in the the clipboard. Also, when you perform a paste operation,
the paste action must become disabled again. The clipboard manager provides this functionality.

To use the %Qtilities clipboard in your applications, you must register the copy and cut actions in your main
function and then initialize the clipboard as shown below:

\code
// Create the menu bar and menus in the menu bar:
bool existed;
ActionContainer* menu_bar = ACTION_MANAGER->createMenuBar(qti_action_MENUBAR_STANDARD,existed);
ActionContainer* edit_menu = ACTION_MANAGER->createMenu(qti_action_EDIT,existed);
menu_bar->addMenu(edit_menu);

// Get the standard context:
QList<int> std_context;
std_context.push_front(CONTEXT_MANAGER->contextID(qti_def_CONTEXT_STANDARD));

// Register action placeholders for the copy, cut and paste actions:
Command* command = ACTION_MANAGER->registerActionPlaceHolder(qti_action_EDIT_COPY,QObject::tr("Copy"),QKeySequence(QKeySequence::Copy));
edit_menu->addAction(command);
command = ACTION_MANAGER->registerActionPlaceHolder(qti_action_EDIT_CUT,QObject::tr("Cut"),QKeySequence(QKeySequence::Cut));
edit_menu->addAction(command);

// We want to use paste operations in this application, thus initialize the clipboard.
// It is important to do this after registering the copy, cut and paste action holders above.
// The initialization will register backends for these actions.
CLIPBOARD_MANAGER->initialize();
\endcode

The operation and intended usage of the clipboard manager is best shown using an example, thus we look at
a shortened version of the copy, cut and paste action handlers taken from the Qtilities::CoreGui::ObserverWidget class' sources.

First we look at the observer widget's copy action handler implementation:
\code
ObserverMimeData *mimeData = new ObserverMimeData(selectedObjects(),current_observer_ID);
QApplication::clipboard()->setMimeData(mimeData);
CLIPBOARD_MANAGER->setClipboardOrigin(IClipboard::CopyAction);
\endcode

Next we look at the observer widget's cut action handler implementation:
\code
ObserverMimeData *mimeData = new ObserverMimeData(selectedObjects(),current_observer_ID);
QApplication::clipboard()->setMimeData(mimeData);
CLIPBOARD_MANAGER->setClipboardOrigin(IClipboard::CutAction);
\endcode

Finaly we look at the observer widget's paste action handler implementation:
\code
const ObserverMimeData* observer_mime_data = qobject_cast<const ObserverMimeData*> (QApplication::clipboard()->mimeData());
if (observer_mime_data) {
    if (current_observer->canAttach(const_cast<ObserverMimeData*> (observer_mime_data)) == Qtilities::Core::Observer::Allowed) {
        // Now check the proposed action of the event.
        if (CLIPBOARD_MANAGER->clipboardOrigin() == IClipboard::CutAction) {
            OBJECT_MANAGER->moveSubjects(observer_mime_data->subjectList(),observer_mime_data->sourceID(),current_observer_ID);
            // Accept the data, thus the paste action will be disabled and the clipboard cleared.
            CLIPBOARD_MANAGER->acceptMimeData();
        } else if (CLIPBOARD_MANAGER->clipboardOrigin() == IClipboard::CopyAction) {
            // Attempt to copy the objects
            // For now we discard objects that cause problems during attachment and detachment
            for (int i = 0; i < observer_mime_data->subjectList().count(); i++) {
                // Attach to destination
                current_observer->attachSubject(observer_mime_data->subjectList().at(i));
            }
            // Accept the data, thus the paste action will be disabled and the clipboard cleared.
            CLIPBOARD_MANAGER->acceptMimeData();
        }
    }
}
\endcode
*/
            class QTILITIES_CORE_GUI_SHARED_EXPORT IClipboard : public QObject, virtual public IObjectBase
            {
                Q_OBJECT
                Q_ENUMS(ClipboardOrigin)

            public:
                IClipboard(QObject* parent = 0) : QObject(parent) {}
                virtual ~IClipboard() {}

                //! Enumeration which is used to indicate if a copy or a cut operation caused the clipboard to change.
                /*!
                  \sa setClipboardOrigin(), clipboardOrigin()
                  */
                enum ClipboardOrigin {
                    CopyAction,         /*!< A copy action changed the clipboard. */
                    CutAction,          /*!< A cut action changed the clipboard. */
                    Unspecified         /*!< An unspecified action changed the clipboard. */
                };

                //! Gives information about the data which was placed in the clipboard.
                virtual ClipboardOrigin clipboardOrigin() = 0;
                //! Call this function after a copy or cut action call to indicate what the paste action should do.
                virtual void setClipboardOrigin(ClipboardOrigin) = 0;
                //! Initializes the actions associated with the clipboard manager.
                virtual void initialize() = 0;
                //! Call this function if you accepted the mime data and want to remove the data from the clipboard. It will disable the paste action.
                virtual void acceptMimeData() = 0;

                virtual void setMimeData(QMimeData* mimeData) = 0;
                virtual QMimeData* mimeData() const = 0;
                virtual void clearMimeData() = 0;

            public slots:
                virtual void handleClipboardChanged() = 0;

            signals:
                void filled();
                void cleared();
            };
        }
    }
}

Q_DECLARE_INTERFACE(Qtilities::CoreGui::Interfaces::IClipboard,"com.Qtilities.ObjManagement.IClipboard/1.0")

#endif // ICLIPBOARD

