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

#ifndef ICLIPBOARD
#define ICLIPBOARD

#include "QtilitiesCoreGui_global.h"

#include <QObject>

namespace Qtilities {
    namespace CoreGui {
        class ActionContainer;
        class Command;

        namespace Interfaces {
/*!
\class IClipboard
\brief Interface used to communicate with the clipboard manager.

The goal of the clipboard manager is to register backends (associated with the standard context) for the
Qtilities::CoreGui::Actions::MENU_EDIT_COPY, Qtilities::CoreGui::Actions::MENU_EDIT_CUT and
Qtilities::CoreGui::Actions::MENU_EDIT_PASTE action placeholders if they exists. This allows control
over disabling and enabling these three actions in %Qtilities applications. For example, the paste action
should only be enabled if something exists in the the clipboard. Also, when you perform a paste operation,
the paste action must become disabled again. The clipboard manager provides this functionality.

The operation and intended usage of the clipboard manager is best shown using an example, thus we look at
a shortened version of the copy, cut and paste action handlers taken from the Qtilities::CoreGui::ObserverWidget class' sources.

First we look at the observer widget's copy action handler implementation:
\code
ObserverMimeData *mimeData = new ObserverMimeData(selectedObjects(),current_observer_ID);
QApplication::clipboard()->setMimeData(mimeData);
QtilitiesCoreGui::instance()->clipboardManager()->setClipboardOrigin(IClipboard::CopyAction);
\endcode

Next we look at the observer widget's cut action handler implementation:
\code
ObserverMimeData *mimeData = new ObserverMimeData(selectedObjects(),current_observer_ID);
QApplication::clipboard()->setMimeData(mimeData);
QtilitiesCoreGui::instance()->clipboardManager()->setClipboardOrigin(IClipboard::CutAction);
\endcode

Finaly we look at the observer widget's paste action handler implementation:
\code
const ObserverMimeData* observer_mime_data = qobject_cast<const ObserverMimeData*> (QApplication::clipboard()->mimeData());
if (observer_mime_data) {
    if (current_observer->canAttach(const_cast<ObserverMimeData*> (observer_mime_data)) == Qtilities::Core::Observer::Allowed) {
        // Now check the proposed action of the event.
        if (QtilitiesCoreGui::instance()->clipboardManager()->clipboardOrigin() == IClipboard::CutAction) {
            QtilitiesCore::instance()->objectManager()->moveSubjects(observer_mime_data->subjectList(),observer_mime_data->sourceID(),current_observer_ID);
            // Accept the data, thus the paste action will be disabled and the clipboard cleared.
            QtilitiesCoreGui::instance()->clipboardManager()->acceptMimeData();
        } else if (QtilitiesCoreGui::instance()->clipboardManager()->clipboardOrigin() == IClipboard::CopyAction) {
            // Attempt to copy the objects
            // For now we discard objects that cause problems during attachment and detachment
            for (int i = 0; i < observer_mime_data->subjectList().count(); i++) {
                // Attach to destination
                current_observer->attachSubject(observer_mime_data->subjectList().at(i));
            }
            // Accept the data, thus the paste action will be disabled and the clipboard cleared.
            QtilitiesCoreGui::instance()->clipboardManager()->acceptMimeData();
        }
    }
}
\endcode
*/
            class QTILITIES_CORE_GUI_SHARED_EXPORT IClipboard : public QObject
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
                //! Call this function if you accepted the mime data. It will disable the paste action.
                virtual void acceptMimeData() = 0;

            public slots:
                virtual void handleClipboardChanged() = 0;

            signals:
                void filled();
                void cleared();
            };
        }
    }
}

Q_DECLARE_INTERFACE(Qtilities::CoreGui::Interfaces::IClipboard,"com.qtilities.ObjManagement.IClipboard/1.0");

#endif // ICLIPBOARD

