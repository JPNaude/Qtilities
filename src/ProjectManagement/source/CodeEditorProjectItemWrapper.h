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
          \struct CodeEditorProjectItemWrapperPrivateData
          \brief The CodeEditorProjectItemWrapperPrivateData struct stores private data used by the CodeEditorProjectItemWrapper class.
         */
        struct CodeEditorProjectItemWrapperPrivateData;

        /*!
        \class CodeEditorProjectItemWrapper
        \brief This class wraps Qtilities::CoreGui::CodeEditorWidget as a project item.

        Using CodeEditorProjectItemWrapper you can easily make any Qtilities::CoreGui::CodeEditorWidget part of a project. For example:

\code
CodeEditorWidget* code_editor = new CodeEditorWidget();
CodeEditorProjectItemWrapper* project_item = new CodeEditorProjectItemWrapper(code_editor);
OBJECT_MANAGER->registerObject(project_item,QtilitiesCategory("Core::Project Items (IProjectItem)","::"));
\endcode

        Make sure you register the project item in the global object pool before initializing the project manager.

        <i>This class was added in %Qtilities v1.0.</i>
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
            bool closeProjectItem(ITask *task = 0);

            // --------------------------------
            // IExportable Implementation
            // --------------------------------
            ExportModeFlags supportedFormats() const;
            InstanceFactoryInfo instanceFactoryInfo() const;
            virtual IExportable::ExportResultFlags exportBinary(QDataStream& stream ) const;
            virtual IExportable::ExportResultFlags importBinary(QDataStream& stream, QList<QPointer<QObject> >& import_list);
            virtual IExportable::ExportResultFlags exportXml(QDomDocument* doc, QDomElement* object_node) const;
            virtual IExportable::ExportResultFlags importXml(QDomDocument* doc, QDomElement* object_node, QList<QPointer<QObject> >& import_list);

            // --------------------------------
            // IModificationNotifier Implementation
            // --------------------------------
            bool isModified() const;
        public slots:
            void setModificationState(bool new_state, IModificationNotifier::NotificationTargets = IModificationNotifier::NotifyListeners, bool force_notifications = false);
        signals:
            void modificationStateChanged(bool is_modified) const;

        public:
            // --------------------------------
            // IObjectBase Implementation
            // --------------------------------
            QObject* objectBase() { return this; }
            const QObject* objectBase() const { return this; }

        private:
            CodeEditorProjectItemWrapperPrivateData* d;
        };
    }
}

#endif // CODE_EDITOR_PROJECT_ITEM_WRAPPER_H
