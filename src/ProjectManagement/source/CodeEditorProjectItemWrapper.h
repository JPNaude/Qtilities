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

#ifndef CODE_EDITOR_PROJECT_ITEM_WRAPPER_H
#define CODE_EDITOR_PROJECT_ITEM_WRAPPER_H

#include "IProjectItem.h"
#include "ProjectManagement_global.h"

#include <QObject>

#include <CodeEditorWidget>

namespace Qtilities {
    namespace ProjectManagement {
        using namespace Qtilities::CoreGui;
        using namespace Qtilities::ProjectManagement::Interfaces;

        /*!
          \struct CodeEditorProjectItemWrapperData
          \brief The CodeEditorProjectItemWrapperData struct stores private data used by the CodeEditorProjectItemWrapper class.
         */
        struct CodeEditorProjectItemWrapperData;

        /*!
        \class CodeEditorProjectItemWrapper
        \brief This class wraps Qtilities::CoreGui::CodeEditorWidget as a project item.
          */
        class PROJECT_MANAGEMENT_SHARED_EXPORT CodeEditorProjectItemWrapper : public QObject, public IProjectItem
        {
            Q_OBJECT
            Q_INTERFACES(Qtilities::ProjectManagement::Interfaces::IProjectItem)

        public:
            explicit CodeEditorProjectItemWrapper(CodeEditorWidget* code_editor = 0, QObject *parent = 0);

            //! Sets the code editor widget for this project item.
            void setCodeEditor(CodeEditorWidget* code_editor);

            // --------------------------------------------
            // IProjectItem Implementation
            // --------------------------------------------
            QString projectItemName() const;
            bool newProjectItem();
            bool loadProjectItem(QDataStream& stream);
            bool saveProjectItem(QDataStream& stream);
            bool closeProjectItem();

            // --------------------------------
            // IExportable Implementation
            // --------------------------------
            ExportModeFlags supportedFormats() const;
            InstanceFactoryInfo instanceFactoryInfo() const;
            virtual IExportable::Result exportBinary(QDataStream& stream, QList<QVariant> params = QList<QVariant>()) const;
            virtual IExportable::Result importBinary(QDataStream& stream, QList<QPointer<QObject> >& import_list, QList<QVariant> params = QList<QVariant>());
            virtual Result exportXML(QDomDocument* doc, QDomElement* object_node, QList<QVariant> params = QList<QVariant>()) const;
            virtual Result importXML(QDomDocument* doc, QDomElement* object_node, QList<QPointer<QObject> >& import_list, QList<QVariant> params = QList<QVariant>());

            // --------------------------------
            // IModificationNotifier Implemenation
            // --------------------------------
            bool isModified() const;
        public slots:
            void setModificationState(bool new_state, IModificationNotifier::NotificationTargets = IModificationNotifier::NotifyListeners);
        signals:
            void modificationStateChanged(bool is_modified) const;

        public:
            // --------------------------------
            // IObjectBase Implemenation
            // --------------------------------
            QObject* objectBase() { return this; }
            const QObject* objectBase() const { return this; }

        private:
            CodeEditorProjectItemWrapperData* d;
        };
    }
}

#endif // CODE_EDITOR_PROJECT_ITEM_WRAPPER_H
