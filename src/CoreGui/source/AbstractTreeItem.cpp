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

#include "AbstractTreeItem.h"
#include "TreeNode.h"

#include <QDomElement>

using namespace Qtilities::Core;

Qtilities::CoreGui::AbstractTreeItem::AbstractTreeItem() {
    baseItemData = new AbstractTreeItemData;

}

Qtilities::CoreGui::AbstractTreeItem::~AbstractTreeItem() {
    delete baseItemData;
}

QString Qtilities::CoreGui::AbstractTreeItem::getName(TreeNode* parent) const {
    if (!parent)
        return objectBase()->objectName();
    else {
        if (parent->contains(objectBase()))
            return parent->subjectNameInContext(objectBase());
        else
            return QString();
    }
}

void Qtilities::CoreGui::AbstractTreeItem::setName(const QString& new_name, TreeNode* parent) {

}

IExportable::Result Qtilities::CoreGui::AbstractTreeItem::saveFormattingToXML(QDomDocument* doc, QDomElement* object_node) const {
    QDomElement formatting_data = doc->createElement("Formatting");

    //if (hasAlignment())
    //    formatting_data.setAttribute("Alignment",(int) getAlignment());
    if (hasBackgroundRole())
        formatting_data.setAttribute("BackgroundColor",getBackgroundRole().color().name());
    if (hasForegroundRole())
        formatting_data.setAttribute("ForegroundColor",getForegroundRole().color().name());
    if (hasSizeHint()) {
        QDomElement size_data = doc->createElement("Size");
        formatting_data.appendChild(size_data);
        QSize size = getSizeHint();
        formatting_data.setAttribute("Width",size.width());
        formatting_data.setAttribute("Height",size.height());
    }
    if (hasStatusTip())
        formatting_data.setAttribute("StatusTip",getStatusTip());
    if (hasToolTip())
        formatting_data.setAttribute("ToolTip",getToolTip());
    if (hasWhatsThis())
        formatting_data.setAttribute("WhatsThis",getWhatsThis());
    if (hasFont()) {
        QDomElement font_data = doc->createElement("Font");
        formatting_data.appendChild(font_data);
        QFont font = getFont();
        font_data.setAttribute("Family",font.family());
        font_data.setAttribute("PointSize",font.pointSize());
        font_data.setAttribute("Weigth",font.weight());
        if (font.bold())
            font_data.setAttribute("Bold","True");
        else
            font_data.setAttribute("Bold","False");
        if (font.italic())
            font_data.setAttribute("Italic","True");
        else
            font_data.setAttribute("Italic","False");
    }

    if (formatting_data.attributes().count() > 0 || formatting_data.childNodes().count() > 0)
        object_node->appendChild(formatting_data);

    return IExportable::Complete;
}

IExportable::Result Qtilities::CoreGui::AbstractTreeItem::loadFormattingFromXML(QDomDocument* doc, QDomElement* object_node) {
    // First get all attributes:
    //if (object_node->hasAttribute("Alignment")) {
    //    setAlignment((Qt::Alignment) (object_node->attribute("Alignment").toInt()));
    //}
    if (object_node->hasAttribute("BackgroundColor")) {
        QString color_name = object_node->attribute("BackgroundColor");
        setBackgroundRole(QBrush(QColor(color_name)));
    }
    if (object_node->hasAttribute("ForegroundColor")) {
        QString color_name = object_node->attribute("ForegroundColor");
        setForegroundRole(QBrush(QColor(color_name)));
    }
    if (object_node->hasAttribute("StatusTip")) {
        setStatusTip(object_node->attribute("StatusTip"));
    }
    if (object_node->hasAttribute("ToolTip")) {
        setStatusTip(object_node->attribute("ToolTip"));
    }
    if (object_node->hasAttribute("WhatsThis")) {
        setStatusTip(object_node->attribute("WhatsThis"));
    }

    // Next get all child nodes:
    QDomNodeList childNodes = object_node->childNodes();
    for(int i = 0; i < childNodes.count(); i++)
    {
        QDomNode childNode = childNodes.item(i);
        QDomElement childElement = childNode.toElement();

        if (childElement.isNull())
            continue;

        if (childElement.tagName() == "Size") {
            int width = childElement.attribute("Width").toInt();
            int height = childElement.attribute("Height").toInt();
            QSize size(width,height);
            setSizeHint(size);
            continue;
        }

        if (childElement.tagName() == "Font") {
            if (childElement.hasAttribute("Family")) {
                QString family = childElement.attribute("Family");
                int point_size = -1;
                int weight = -1;
                bool italic = false;

                if (childElement.hasAttribute("PointSize"))
                    point_size = childElement.attribute("PointSize").toInt();
                if (childElement.hasAttribute("Weigth"))
                    weight = childElement.attribute("Weigth").toInt();
                if (childElement.hasAttribute("Italic")) {
                    if (childElement.attribute("Italic") == "True")
                        italic = true;
                    else
                        italic = false;
                }

                QFont font(family,point_size,weight,italic);
                if (childElement.hasAttribute("Bold")) {
                    if (childElement.attribute("Bold") == "True")
                        font.setBold(true);
                }
                setFont(font);
            }
            continue;
        }
    }

    return IExportable::Complete;
}

void Qtilities::CoreGui::AbstractTreeItem::setCategory(const QtilitiesCategory& category, TreeNode* tree_node) {
    if (!category.isValid())
        return;

    QObject* obj = objectBase();
    if (obj && tree_node) {
        if (Observer::propertyExists(obj,OBJECT_CATEGORY)) {
            ObserverProperty category_property = Observer::getObserverProperty(obj,OBJECT_CATEGORY);
            category_property.setValue(qVariantFromValue(category),tree_node->observerID());
            Observer::setObserverProperty(obj,category_property);
        } else {
            ObserverProperty category_property(OBJECT_CATEGORY);
            category_property.setValue(qVariantFromValue(category),tree_node->observerID());
            Observer::setObserverProperty(obj,category_property);
        }
    }
}

Qtilities::Core::QtilitiesCategory Qtilities::CoreGui::AbstractTreeItem::getCategory(TreeNode* tree_node) const {
    const QObject* obj = objectBase();
    if (obj && tree_node) {
        QVariant category_variant = tree_node->getObserverPropertyValue(obj,OBJECT_CATEGORY);
        if (category_variant.isValid()) {
            return category_variant.value<QtilitiesCategory>();
        }
    }

    return QtilitiesCategory();
}

bool Qtilities::CoreGui::AbstractTreeItem::hasCategory() const {
    return Observer::propertyExists(objectBase(),OBJECT_CATEGORY);
}

void Qtilities::CoreGui::AbstractTreeItem::setToolTip(const QString& tooltip) {
    QObject* obj = objectBase();
    if (obj) {
        SharedObserverProperty property(tooltip,OBJECT_ROLE_TOOLTIP);
        property.setIsExportable(true);
        Observer::setSharedProperty(obj,property);
    }
}

QString Qtilities::CoreGui::AbstractTreeItem::getToolTip() const {
    const QObject* obj = objectBase();
    if (obj)
        return Observer::getSharedProperty(obj,OBJECT_ROLE_TOOLTIP).value().toString();

    return QString();
}

bool Qtilities::CoreGui::AbstractTreeItem::hasToolTip() const {
    return Observer::propertyExists(objectBase(),OBJECT_ROLE_TOOLTIP);
}

void Qtilities::CoreGui::AbstractTreeItem::setIcon(const QIcon& icon) {
    QObject* obj = objectBase();
    if (obj) {
        SharedObserverProperty property(icon,OBJECT_ROLE_DECORATION);
        property.setIsExportable(true);
        Observer::setSharedProperty(obj,property);
    }
}

QIcon Qtilities::CoreGui::AbstractTreeItem::getIcon() const {
    const QObject* obj = objectBase();
    if (obj) {
        QVariant variant = Observer::getSharedProperty(obj,OBJECT_ROLE_TOOLTIP).value();
        return variant.value<QIcon>();
    }

    return QIcon();
}

bool Qtilities::CoreGui::AbstractTreeItem::hasIcon() const {
    return Observer::propertyExists(objectBase(),OBJECT_ROLE_TOOLTIP);
}

void Qtilities::CoreGui::AbstractTreeItem::setWhatsThis(const QString& whats_this) {
    QObject* obj = objectBase();
    if (obj) {
        SharedObserverProperty property(whats_this,OBJECT_ROLE_WHATS_THIS);
        property.setIsExportable(true);
        Observer::setSharedProperty(obj,property);
    }
}

QString Qtilities::CoreGui::AbstractTreeItem::getWhatsThis() const {
    const QObject* obj = objectBase();
    if (obj)
        return Observer::getSharedProperty(obj,OBJECT_ROLE_WHATS_THIS).value().toString();

    return QString();
}

bool Qtilities::CoreGui::AbstractTreeItem::hasWhatsThis() const {
    return Observer::propertyExists(objectBase(),OBJECT_ROLE_WHATS_THIS);
}

void Qtilities::CoreGui::AbstractTreeItem::setStatusTip(const QString& status_tip) {
    QObject* obj = objectBase();
    if (obj) {
        SharedObserverProperty property(status_tip,OBJECT_ROLE_STATUSTIP);
        property.setIsExportable(true);
        Observer::setSharedProperty(obj,property);
    }
}

QString Qtilities::CoreGui::AbstractTreeItem::getStatusTip() const {
    const QObject* obj = objectBase();
    if (obj)
        return Observer::getSharedProperty(obj,OBJECT_ROLE_STATUSTIP).value().toString();

    return QString();
}

bool Qtilities::CoreGui::AbstractTreeItem::hasStatusTip() const {
    return Observer::propertyExists(objectBase(),OBJECT_ROLE_STATUSTIP);
}

void Qtilities::CoreGui::AbstractTreeItem::setSizeHint(const QSize& size) {
    QObject* obj = objectBase();
    if (obj) {
        SharedObserverProperty property(size,OBJECT_ROLE_SIZE_HINT);
        property.setIsExportable(true);
        Observer::setSharedProperty(obj,property);
    }
}

QSize Qtilities::CoreGui::AbstractTreeItem::getSizeHint() const {
    const QObject* obj = objectBase();
    if (obj)
        return Observer::getSharedProperty(obj,OBJECT_ROLE_SIZE_HINT).value().toSize();

    return QSize();
}

bool Qtilities::CoreGui::AbstractTreeItem::hasSizeHint() const {
    return Observer::propertyExists(objectBase(),OBJECT_ROLE_SIZE_HINT);
}

void Qtilities::CoreGui::AbstractTreeItem::setFont(const QFont& font) {
    QObject* obj = objectBase();
    if (obj) {
        SharedObserverProperty property(font,OBJECT_ROLE_FONT);
        property.setIsExportable(true);
        Observer::setSharedProperty(obj,property);
    }
}

QFont Qtilities::CoreGui::AbstractTreeItem::getFont() const {
    const QObject* obj = objectBase();
    if (obj) {
        QVariant variant = Observer::getSharedProperty(obj,OBJECT_ROLE_FONT).value();
        return variant.value<QFont>();
    }

    return QFont();
}

bool Qtilities::CoreGui::AbstractTreeItem::hasFont() const {
    return Observer::propertyExists(objectBase(),OBJECT_ROLE_FONT);
}

void Qtilities::CoreGui::AbstractTreeItem::setAlignment(const Qt::AlignmentFlag& alignment) {
    QObject* obj = objectBase();
    if (obj) {
        SharedObserverProperty property((int) alignment,OBJECT_ROLE_TEXT_ALIGNMENT);
        property.setIsExportable(true);
        Observer::setSharedProperty(obj,property);
    }
}

Qt::AlignmentFlag Qtilities::CoreGui::AbstractTreeItem::getAlignment() const {
    const QObject* obj = objectBase();
    if (obj) {
        QVariant variant = Observer::getSharedProperty(obj,OBJECT_ROLE_TEXT_ALIGNMENT).value();
        return (Qt::AlignmentFlag) variant.toInt();
    }

    return Qt::AlignLeft;
}

bool Qtilities::CoreGui::AbstractTreeItem::hasAlignment() const {
    return Observer::propertyExists(objectBase(),OBJECT_ROLE_TEXT_ALIGNMENT);
}

void Qtilities::CoreGui::AbstractTreeItem::setForegroundRole(const QBrush& foreground_role) {
    QObject* obj = objectBase();
    if (obj) {
        SharedObserverProperty property(foreground_role,OBJECT_ROLE_FOREGROUND);
        property.setIsExportable(true);
        Observer::setSharedProperty(obj,property);
    }
}

QBrush Qtilities::CoreGui::AbstractTreeItem::getForegroundRole() const {
    const QObject* obj = objectBase();
    if (obj) {
        QVariant variant = Observer::getSharedProperty(obj,OBJECT_ROLE_FOREGROUND).value();
        return variant.value<QBrush>();
    }

    return QBrush();
}

bool Qtilities::CoreGui::AbstractTreeItem::hasForegroundRole() const {
    return Observer::propertyExists(objectBase(),OBJECT_ROLE_FOREGROUND);
}

void Qtilities::CoreGui::AbstractTreeItem::setForegroundColor(const QColor& color) {
    if (hasForegroundRole()) {
        QBrush brush = getForegroundRole();
        brush.setColor(color);
        setForegroundRole(brush);
    } else
        setForegroundRole(QBrush(color));
}

QColor Qtilities::CoreGui::AbstractTreeItem::getForegroundColor() const {
    if (hasForegroundRole()) {
        QBrush brush = getForegroundRole();
        return brush.color();
    } else
        return QColor();
}

void Qtilities::CoreGui::AbstractTreeItem::setBackgroundRole(const QBrush& background_role) {
    QObject* obj = objectBase();
    if (obj) {
        SharedObserverProperty property(background_role,OBJECT_ROLE_BACKGROUND);
        property.setIsExportable(true);
        Observer::setSharedProperty(obj,property);
    }
}

QBrush Qtilities::CoreGui::AbstractTreeItem::getBackgroundRole() const {
    const QObject* obj = objectBase();
    if (obj) {
        QVariant variant = Observer::getSharedProperty(obj,OBJECT_ROLE_BACKGROUND).value();
        return variant.value<QBrush>();
    }

    return QBrush();
}

bool Qtilities::CoreGui::AbstractTreeItem::hasBackgroundRole() const {
    return Observer::propertyExists(objectBase(),OBJECT_ROLE_BACKGROUND);
}

void Qtilities::CoreGui::AbstractTreeItem::setBackgroundColor(const QColor& color) {
    if (hasBackgroundRole()) {
        QBrush brush = getBackgroundRole();
        brush.setColor(color);
        setBackgroundRole(brush);
    } else
        setBackgroundRole(QBrush(color));
}

QColor Qtilities::CoreGui::AbstractTreeItem::getBackgroundColor() const {
    if (hasBackgroundRole()) {
        QBrush brush = getBackgroundRole();
        return brush.color();
    } else
        return QColor(Qt::white);
}
