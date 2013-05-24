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
        \struct TreeFileItemPrivateData
        \brief Structure used by TreeFileItem to store private data.
          */
        struct TreeFileItemPrivateData {
            TreeFileItemPrivateData() : instanceFactoryInfo(qti_def_FACTORY_QTILITIES,qti_def_FACTORY_TAG_TREE_FILE_ITEM,QString()),
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
          - It is possible to format the tree item depending on the file. You can for example color files which does not exist red and files that does exist green etc.

          \note This class is meant to be used where the tree item will only be attached to a single observer context since setFile() does not update any qti_prop_ALIAS_MAP. You can
                however reimplement setFile() and setFileForce() in order to implement this functionality if needed.

          <i>This class was added in %Qtilities v0.2.</i>
        */
        class QTILITIES_CORE_GUI_SHARED_EXPORT TreeFileItem : public TreeItemBase
        {
            Q_OBJECT
            Q_INTERFACES(Qtilities::Core::Interfaces::IExportable)
            Q_ENUMS(PathDisplay)

        public:
            //! This enumeration provides the possible ways that tree file items can be displayed.
            /*!
              The default is DisplayFileName.
              */
            enum PathDisplay {
                DisplayFileName,        /*!< Display fileName(). */
                DisplayFilePath,        /*!< Display filePath(). */
                DisplayActualFilePath   /*!< Display actualFilePath(). */
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
            //! Event filter which catches qti_prop_NAME property changes on this object.
            bool eventFilter(QObject *object, QEvent *event);

            //! Sets the PathDisplay used for this tree file item.
            inline void setPathDisplay(PathDisplay path_display) { d_path_display = path_display; }
            //! Gets the PathDisplay used for this tree file item.
            inline PathDisplay pathDisplay() const { return d_path_display; }

            //! Sets the file path of this tree file item and only update the internal QFileInfo when the change was validated by any subject filters.
            /*!
              Does the same as QFileInfo::setFile() except that it also sets the correct property on the object needed to display it in an ObserverWidget.

              This function will check if there is a qti_prop_NAME property on this object and set it. If it does not exist it will
              just set objectName(). Note that this does not set the names in the qti_prop_ALIAS_MAP property if it exists.

              Important: If the qti_prop_NAME property exists this function will set the property, but not the path on the internal QFileInfo. When a QtilitiesPropertyChangeEvent
              is recieved it will update the internal QFileInfo. This is usefull in cases where the name change can be rejected (for example when a NamingPolicyFilter detects a duplicate). Thus
              the internal QFileInfo will only be changed when the new paths are valid. The important thing to remember here is that in order for a QtilitiesPropertyChangeEvent to be
              delivered, the observer to which the TreeFileItem is attached needs to have delivery of QtilitiesPropertyChangeEvent events enabled.

              An alternative is to call setFileForce() which will update the internal QFileInfo without waiting for a QtilitiesPropertyChangeEvent. Make sure you understand when to use
              setFileForce() since the internal QFileInfo can get out of sync with the object name if the object name property is rejected by any subject filters. However if you are sure
              that it won't be rejected, setFileForce() should be used instead of setFile().

              \param file_path The new file path.
              \param relative_to_path The relative to path to use.
              \param broadcast Indicates if the file model must broadcast that it was changed. This also hold for the modification state status of the file model.

              \sa setFileForce()
              */
            virtual void setFile(const QString& file_path, const QString& relative_to_path = QString(), bool broadcast = true);
            //! Sets the file path of this tree file item and update the internal QFileInfo without waiting for the change to be validated.
            /*!
              Does the same as setFile() except that it updates the internal QFileInfo immediately.

              This function will check if there is a qti_prop_NAME property on this object and set it. If it does not exist it will
              just set objectName(). Note that this does not set the names in the qti_prop_ALIAS_MAP property if it exists.

              Important: See the documentation of setFile() in order to understand when to use setFile() and when to use setFileForce().

              \param file_path The new file path.
              \param relative_to_path The relative to path to use.
              \param broadcast Indicates if the file model must broadcast that it was changed. This also hold for the modification state status of the file model.

              \sa setFile()

              <i>This function was added in %Qtilities v1.1.</i>
              */
            virtual void setFileForce(const QString& file_path, const QString& relative_to_path = QString(), bool broadcast = true);

            //! See QFileInfo::isRelative().
            bool isRelative() const;
            //! See QFileInfo::isAbsolute().
            bool isAbsolute() const;

            //! See QtilitiesFileInfo::hasRelativeToPath().
            bool hasRelativeToPath() const;
            //! See QtilitiesFileInfo::setRelativeToPath().
            void setRelativeToPath(const QString& path);
            //! See QtilitiesFileInfo::relativeToPath().
            virtual QString relativeToPath() const;

            //! See QFileInfo::path().
            virtual QString path() const;
            //! See QFileInfo::filePath().
            virtual QString filePath() const;
            //! See QFileInfo::setFilePath();
            virtual void setFilePath(const QString& new_file_path) ;
            //! See QtilitiesFileInfo::absoluteToRelativePath().
            virtual QString absoluteToRelativePath() const;
            //! See QtilitiesFileInfo::absoluteToRelativeFilePath().
            virtual QString absoluteToRelativeFilePath() const;

            //! See QFileInfo::fileName().
            QString fileName() const;
            //! Reimplementation of QtilitiesFileInfo::setFileName() which sets the modification state to true if needed.
            void setFileName(const QString& new_file_name);
            //! See QFileInfo::baseName().
            QString baseName() const;
            //! See QFileInfo::completeBaseName().
            QString completeBaseName() const;
            //! See QFileInfo::suffix().
            QString suffix() const;
            //! See QFileInfo::completeSuffix().
            QString completeSuffix() const;

            //! See QtilitiesFileInfo::actualPath().
            virtual QString actualPath() const;
            //! See QtilitiesFileInfo::actualFilePath().
            virtual QString actualFilePath() const;

            //! Returns true if the file exists, false otherwise. Note that the file path used to check is actualFilePath().
            virtual bool exists() const;

            //! Extended access to file info object.
            QtilitiesFileInfo fileInfo() const;

            // --------------------------------
            // Factory Interface Implementation
            // --------------------------------
            static FactoryItem<QObject, TreeFileItem> factory;

            // --------------------------------
            // IExportable Implementation
            // --------------------------------
            ExportModeFlags supportedFormats() const;
            InstanceFactoryInfo instanceFactoryInfo() const;
            virtual IExportable::ExportResultFlags exportXml(QDomDocument* doc, QDomElement* object_node) const;
            virtual IExportable::ExportResultFlags importXml(QDomDocument* doc, QDomElement* object_node, QList<QPointer<QObject> >& import_list);

        signals:
            //! Signal which is emitted when the file path of this tree file item changes.
            /*!
              \param new_file_path Equal to the new filePath().
              */
            void filePathChanged(const QString& new_file_path);

        protected:
            void setFactoryData(InstanceFactoryInfo instanceFactoryInfo);
            TreeFileItemPrivateData* treeFileItemBase;

        private:
            //! Internal function to get the displayed name according to the PathDisplay type.
            /*!
                \param file_path By default (thus by passing a QString()) this function will return the display name of the current file. When file_path contains a path, this function will return the display name based on this object's PathDisplay configuration.
              */
            QString displayName(const QString& file_path = QString());

            PathDisplay d_path_display;
            QString d_queued_file_path;
            QString d_queued_relative_to_path;
        };
    }
}

#endif //  TREE_FILE_ITEM_H
