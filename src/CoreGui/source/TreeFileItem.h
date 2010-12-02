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

#ifndef TREE_FILE_ITEM_H
#define TREE_FILE_ITEM_H

#include "QtilitiesCoreGui_global.h"
#include "TreeItem.h"
#include "QtilitiesCoreGuiConstants.h"

namespace Qtilities {
    namespace CoreGui {
        using namespace Qtilities::Core;
        using namespace Qtilities::CoreGui::Constants;

        /*!
        \struct TreeFileItemData
        \brief Structure used by TreeFileItem to store private data.
          */
        struct TreeFileItemData {
            TreeFileItemData() : file_path(QString()),
                instanceFactoryInfo(FACTORY_QTILITIES,FACTORY_TAG_TREE_FILE_ITEM,QString()) { }

            QString file_path;
            InstanceFactoryInfo instanceFactoryInfo;
        };

        /*!
          \class TreeFileItem
          \brief The TreeFileItem class is an item in a tree which can be used to easily represent files.

          The TreeFileItem tree item is derived from TreeItem can provided functionality to represent a file
          in a tree. This functionality includes:
          - A QFile representing the file is stored.
          - Importing/exporting of the file path is done automatically.
          - It is possible to format the tree item depending on the file. You can for example color files which does not exist
          red and one that does exist green etc.

          <b>Important:</b> This object depends on Qtilities property change event being delivered to it. Thus if you attach it to
          an observer, you must enable property changed events by calling Qtilities::Core::Observer::toggleQtilitiesPropertyChangeEvents() on
          the observer.

          \note This class is meant to be used where the tree item will only be attached to a single observer context.

          <i>This class was added in %Qtilities v0.2.</i>
        */
        class QTILITIES_CORE_GUI_SHARED_EXPORT TreeFileItem : public TreeItemBase
        {
            Q_OBJECT
            Q_INTERFACES(Qtilities::Core::Interfaces::IExportable)

        public:
            TreeFileItem(const QString& file_name = QString(), QObject* parent = 0);
            virtual ~TreeFileItem();

            //! Sets the file name of this file model.
            /*!
              This function will check if there is an OBJECT_NAME property on this object and set it. If it does not exist it will
              just set objectName(). Note that this does not set the names in the INSTANCE_NAMES property if it exists.

              \param file_name The new file name.
              \param broadcast Indicates if the file model must broadcast that it was changed. This also hold for the modification state status of the file model.
              */
            void setFileName(const QString& file_name, bool broadcast = true);
            //! Returns the file name of the file represented by this file model.
            QString fileName() const;
            //! Returns true if the file exists, false otherwise.
            virtual bool exists() const;
            //! Returns the file extension of the file represented by this file model.
            QString fileExtension() const;

            //! Returns the file path of the specified file name.
            static QString strippedPath(const QString &fullFileName) {
                QFileInfo file_info(fullFileName);
                return file_info.path();
            }

            //! Returns the file name stripped from the file path. Thus, only the file name.
            static QString strippedName(const QString &fullFileName) {
                QString stripped_name = QFileInfo(fullFileName).fileName();
                if (stripped_name.endsWith("\""))
                    stripped_name.chop(1);

                return stripped_name;
            }

            //! Returns the file type stripped from the file path and file name. Thus, only the file extension.
            static QString strippedFileExtension(const QString &fullFileName) {
                QFileInfo file_info(fullFileName);
                QString extension = file_info.fileName().split(".").last();
                if (extension.endsWith("\""))
                    extension.chop(1);

                return extension;
            }

            // --------------------------------
            // Factory Interface Implemenation
            // --------------------------------
            static FactoryItem<QObject, TreeFileItem> factory;

            // --------------------------------
            // IObjectBase Implemenation
            // --------------------------------
            QObject* objectBase() { return this; }
            const QObject* objectBase() const { return this; }

            // --------------------------------
            // IExportable Implementation
            // --------------------------------
            ExportModeFlags supportedFormats() const;
            InstanceFactoryInfo instanceFactoryInfo() const;
            IExportable::Result exportBinary(QDataStream& stream, QList<QVariant> params = QList<QVariant>()) const;
            IExportable::Result importBinary(QDataStream& stream, QList<QPointer<QObject> >& import_list, QList<QVariant> params = QList<QVariant>());
            virtual Result exportXML(QDomDocument* doc, QDomElement* object_node, QList<QVariant> params = QList<QVariant>()) const;
            virtual Result importXML(QDomDocument* doc, QDomElement* object_node, QList<QPointer<QObject> >& import_list, QList<QVariant> params = QList<QVariant>());

        signals:
            //! Signal which is emitted when the file name of this tree file item changes:
            void fileNameChanged(const QString& new_file_name);

        protected:
            void setFactoryData(InstanceFactoryInfo instanceFactoryInfo);
            TreeFileItemData* treeFileItemBase;
        };
    }
}

#endif //  TREE_FILE_ITEM_H
