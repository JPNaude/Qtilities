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

#ifndef TREE_FILE_ITEM_H
#define TREE_FILE_ITEM_H

#include "QtilitiesCoreGui_global.h"
#include "TreeItem.h"
#include "QtilitiesCoreGuiConstants.h"

#include <QtilitiesFileInfo>

namespace Qtilities {
    namespace CoreGui {
        using namespace Qtilities::Core;
        using namespace Qtilities::CoreGui::Constants;

        /*!
        \struct TreeFileItemData
        \brief Structure used by TreeFileItem to store private data.
          */
        struct TreeFileItemData {
            TreeFileItemData() : instanceFactoryInfo(FACTORY_QTILITIES,FACTORY_TAG_TREE_FILE_ITEM,QString()),
                ignore_events(false) { }

            QtilitiesFileInfo file_info;
            InstanceFactoryInfo instanceFactoryInfo;
            bool ignore_events;
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

          \note This class is meant to be used where the tree item will only be attached to a single observer context.

          <i>This class was added in %Qtilities v0.2.</i>
        */
        class QTILITIES_CORE_GUI_SHARED_EXPORT TreeFileItem : public TreeItemBase
        {
            Q_OBJECT
            Q_INTERFACES(Qtilities::Core::Interfaces::IExportable)

        public:
            //! This enumeration provides the possible ways that tree file items can be displayed.
            /*!
              The default is DisplayFileName.
              */
            enum PathDisplay {
                DisplayFileName,    /*!< Display fileName(). */
                DisplayFilePath,    /*!< Display filePath(). */
            };

            //! Constructs a TreeFileItem object.
            /*!
              \param file_path The file path of the file.
              \param relative_to_path The relative to path for the file. See Qtilities::Core::QtilitiesFileInfo::relativeToPath() for more information.
              \param path_display The preferred way that this file item must be displayed.
              \param parent The parent of the item.
              */
            TreeFileItem(const QString& file_path = QString(), const QString& relative_to_path = QString(), PathDisplay path_display = DisplayFileName,  QObject* parent = 0);
            virtual ~TreeFileItem();
            //! Event filter which catches OBJECT_NAME property changes on this object.
            bool eventFilter(QObject *object, QEvent *event);

            //! Sets the PathDisplay used for this tree file item.
            inline void setPathDisplay(PathDisplay path_display) { d_path_display = path_display; }
            //! Gets the PathDisplay used for this tree file item.
            inline PathDisplay pathDisplay() const { return d_path_display; }

            //! Sets the file path of this tree file item.
            /*!
              Does the same as QFileModel::setFile() except that is also sets the correct property on the object needed to display it in an ObserverWidget.

              This function will check if there is an OBJECT_NAME property on this object and set it. If it does not exist it will
              just set objectName(). Note that this does not set the names in the INSTANCE_NAMES property if it exists.

              \param file_path The new file path.
              \param relative_to_path The relative to path to use.
              \param broadcast Indicates if the file model must broadcast that it was changed. This also hold for the modification state status of the file model.
              */
            virtual void setFile(const QString& file_path, const QString& relative_to_path = QString(), bool broadcast = true);

            //! See QFileModel::isRelative().
            bool isRelative() const;
            //! See QFileModel::isAbsolute().
            bool isAbsolute() const;

            //! See QtilitiesFileInfo::hasRelativeToPath().
            bool hasRelativeToPath() const;
            //! See QtilitiesFileInfo::setRelativeToPath().
            void setRelativeToPath(const QString& path);
            //! See QtilitiesFileInfo::relativeToPath().
            QString relativeToPath() const;

            //! See QFileModel::path().
            virtual QString path() const;
            //! See QFileModel::path().
            virtual QString filePath() const;
            //! See QtilitiesFileInfo::absoluteToRelativePath().
            virtual QString absoluteToRelativePath() const;
            //! See QtilitiesFileInfo::absoluteToRelativeFilePath().
            virtual QString absoluteToRelativeFilePath() const;

            //! See QFileModel::fileName().
            QString fileName() const;
            //! Reimplementation of QtilitiesFileInfo::setFileName() which sets the modification state to true if needed.
            void setFileName(const QString& new_file_name);
            //! See QFileModel::baseName().
            QString baseName() const;
            //! See QFileModel::completeBaseName().
            QString completeBaseName() const;
            //! See QFileModel::suffix().
            QString suffix() const;
            //! See QFileModel::completeSuffix().
            QString completeSuffix() const;

            //! See QtilitiesFileInfo::actualPath().
            QString actualPath() const;
            //! See QtilitiesFileInfo::actualFilePath().
            QString actualFilePath() const;

            //! Returns true if the file exists, false otherwise.
            virtual bool exists() const;

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
            virtual Result exportXML(QDomDocument* doc, QDomElement* object_node, QList<QVariant> params = QList<QVariant>()) const;
            virtual Result importXML(QDomDocument* doc, QDomElement* object_node, QList<QPointer<QObject> >& import_list, QList<QVariant> params = QList<QVariant>());

        signals:
            //! Signal which is emitted when the file name of this tree file item changes.
            /*!
              \param new_file_name Equal to the new filePath().
              */
            void fileNameChanged(const QString& new_file_name);

        protected:
            void setFactoryData(InstanceFactoryInfo instanceFactoryInfo);
            TreeFileItemData* treeFileItemBase;

        private:
            //! Internal function to get the displayed name according to the PathDisplay type.
            QString displayName();
            //! Internal function to set the displayed name according to the PathDisplay type.
            void setDisplayName(const QString& new_display_name);

            PathDisplay d_path_display;
        };
    }
}

#endif //  TREE_FILE_ITEM_H
