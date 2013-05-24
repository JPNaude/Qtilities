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

#ifndef TREE_ITEM_H
#define TREE_ITEM_H

#include "QtilitiesCoreGui_global.h"
#include "TreeItemBase.h"

#include <IModificationNotifier>
#include <IExportable>

namespace Qtilities {
    namespace CoreGui {
        using namespace Qtilities::Core;

        /*!
        \struct TreeItemPrivateData
        \brief Structure used by TreeItem to store private data.
          */
        struct TreeItemPrivateData;

        /*!
          \class TreeItem
          \brief The TreeItem class is an item in a tree. It can be attached to a TreeNode.

          <i>This class was added in %Qtilities v0.2.</i>
        */
        class QTILITIES_CORE_GUI_SHARED_EXPORT TreeItem : public TreeItemBase
        {
            Q_OBJECT
            Q_INTERFACES(Qtilities::Core::Interfaces::IExportable)
            Q_PROPERTY(QFont Font READ getFont WRITE setFont)
            Q_PROPERTY(QColor ForegroundRole READ getForegroundColor WRITE setForegroundColor)
            Q_PROPERTY(QColor BackgroundRole READ getBackgroundColor WRITE setBackgroundColor)
            Q_PROPERTY(QSize Size READ getSizeHint WRITE setSizeHint)
            Q_PROPERTY(QString StatusTip READ getStatusTip WRITE setStatusTip)
            Q_PROPERTY(QString ToolTip READ getToolTip WRITE setToolTip)
            Q_PROPERTY(QString WhatsThis READ getWhatsThis WRITE setWhatsThis)
            Q_PROPERTY(QString Category READ getCategoryString WRITE setCategoryString)

        public:
            TreeItem(const QString& name = QString(), QObject* parent = 0);
            virtual ~TreeItem();

            // --------------------------------
            // Factory Interface Implementation
            // --------------------------------
            static FactoryItem<QObject, TreeItem> factory;

            // --------------------------------
            // IExportable Implementation
            // --------------------------------
            ExportModeFlags supportedFormats() const;
            InstanceFactoryInfo instanceFactoryInfo() const;
            IExportable::ExportResultFlags exportBinary(QDataStream& stream ) const;
            IExportable::ExportResultFlags importBinary(QDataStream& stream, QList<QPointer<QObject> >& import_list);
            IExportable::ExportResultFlags exportXml(QDomDocument* doc, QDomElement* object_node) const;
            IExportable::ExportResultFlags importXml(QDomDocument* doc, QDomElement* object_node, QList<QPointer<QObject> >& import_list);

        protected:
            TreeItemPrivateData* d;
        };
    }
}

#endif //  TREE_ITEM_H
