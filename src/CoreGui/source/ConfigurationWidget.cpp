/****************************************************************************
**
** Copyright (c) 2009-2012, Jaco Naude
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

#include "ConfigurationWidget.h"
#include "ui_ConfigurationWidget.h"
#include "IConfigPage.h"
#include "ObserverWidget.h"
#include "GroupedConfigPage.h"
#include "QtilitiesApplication.h"

#include <Logger>

#include <QTreeWidgetItem>
#include <QBoxLayout>
#include <QDesktopWidget>
#include <QPushButton>

using namespace Qtilities::CoreGui::Interfaces;

struct Qtilities::CoreGui::ConfigurationWidgetPrivateData {
    ConfigurationWidgetPrivateData() : config_pages(QObject::tr("Application Settings")),
        activity_filter(0),
        active_widget(0),
        apply_all_pages(true),
        categorized_display(false) {}

    //! Observer widget which is used to display the categories of the config pages.
    ObserverWidget          config_pages_widget;
    //! The configuration pages observer.
    Observer                config_pages;
    //! Indicates if the page have been initialized.
    bool                    initialized;
    //! The activity policy filter for the pages observer.
    ActivityPolicyFilter*   activity_filter;
    //! Keeps track of the active widget.
    QPointer<QWidget>       active_widget;
    //! The display mode of this widget.
    Qtilities::DisplayMode  display_mode;
    //! The apply all pages setting for this widget.
    bool                    apply_all_pages;
    //! Indicates if this widgets uses a categorized display.
    bool                    categorized_display;
    //! List of IGroupedConfigPageInfoProvider interfaces found during initilization.
    QList<IGroupedConfigPageInfoProvider> grouped_config_page_providers;
};

Qtilities::CoreGui::ConfigurationWidget::ConfigurationWidget(DisplayMode display_mode, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConfigurationWidget)
{
    ui->setupUi(this);
    d = new ConfigurationWidgetPrivateData;
    d->initialized = false;
    d->display_mode = display_mode;

    // Layout and placement of observer widget:
    if (ui->configPagesWidgetHolder->layout())
        delete ui->configPagesWidgetHolder->layout();

    QHBoxLayout* layout = new QHBoxLayout(ui->configPagesWidgetHolder);
    layout->addWidget(&d->config_pages_widget);
    layout->setMargin(0);

    // Put the widget in the center of the screen:
    QRect qrect = QApplication::desktop()->availableGeometry(this);
    move(qrect.center() - rect().center());

    setAttribute(Qt::WA_QuitOnClose,false);
}

Qtilities::CoreGui::ConfigurationWidget::~ConfigurationWidget() {
    delete ui;
    if (d->activity_filter)
        d->activity_filter->disconnect(this);
    delete d;
}

void Qtilities::CoreGui::ConfigurationWidget::initialize(QList<IConfigPage*> config_pages, QList<IGroupedConfigPageInfoProvider*> grouped_page_info_providers) {
    if (!d->initialized) {
        // Add an activity policy filter to the config pages observer:
        d->activity_filter = new ActivityPolicyFilter();
        d->activity_filter->setActivityPolicy(ActivityPolicyFilter::UniqueActivity);
        d->activity_filter->setMinimumActivityPolicy(ActivityPolicyFilter::ProhibitNoneActive);
        d->activity_filter->setNewSubjectActivityPolicy(ActivityPolicyFilter::SetNewInactive);
        connect(d->activity_filter,SIGNAL(activeSubjectsChanged(QList<QObject*>,QList<QObject*>)),SLOT(handleActiveItemChanges(QList<QObject*>)));
        d->config_pages.installSubjectFilter(d->activity_filter);
        d->config_pages.useDisplayHints();
        d->config_pages.displayHints()->setActivityControlHint(ObserverHints::FollowSelection);
        d->config_pages.displayHints()->setActivityDisplayHint(ObserverHints::NoActivityDisplay);
        d->config_pages.displayHints()->setHierarchicalDisplayHint(ObserverHints::CategorizedHierarchy);
        d->config_pages.displayHints()->setActionHints(ObserverHints::ActionFindItem);
    }

    // Figure out what the maximum sizes are in all pages.
    int max_width = -1;
    int max_height = -1;

    QList<IConfigPage*> uncategorized_pages;

    d->config_pages.startProcessingCycle();
    if (d->categorized_display) {
        QList<QtilitiesCategory>                    categories;
        QMap<IConfigPage*,QtilitiesCategory>        config_page_category_map;
        QMap<QtilitiesCategory,GroupedConfigPage*>  grouped_config_pages;
        // Add all the pages to the config pages observer:
        for (int i = 0; i < config_pages.count(); i++) {
            IConfigPage* config_page = config_pages.at(i);
            if (config_page) {
                QString current_category = config_page->configPageCategory().toString();
                // If it has a category, we need to process it further, if not we just add it:
                if (current_category.isEmpty()) {
                    uncategorized_pages << config_page;
                } else {
                    if (!categories.contains(current_category))
                        categories << current_category;
                    config_page_category_map[config_page] = current_category;
                }
            }
        }

        // Construct grouped pages for all categories:
        for (int i = 0; i < categories.count(); i++) {
            // Check if there is already a grouped page for this category (in case initialize() is called twice):
            if (!grouped_config_pages.contains(categories.at(i))) {
                GroupedConfigPage* grouped_page = new GroupedConfigPage(categories.at(i));
                grouped_config_pages[categories.at(i)] = grouped_page;
                grouped_page->setApplyAll(d->apply_all_pages);
                connect(grouped_page,SIGNAL(appliedPage(IConfigPage*)),SIGNAL(appliedPage(IConfigPage*)),Qt::UniqueConnection);
                connect(grouped_page,SIGNAL(activeGroupedPageChanged(IConfigPage*)),SLOT(handleActiveGroupedPageChanged(IConfigPage*)));
                uncategorized_pages << grouped_page;

                // Look if there is an information provider for this page:
                for (int g = 0; g < grouped_page_info_providers.count(); g++) {
                    if (grouped_page_info_providers.at(g)->isProviderForCategory(categories.at(i))) {
                        grouped_page->setConfigPageIcon(grouped_page_info_providers.at(g)->groupedConfigPageIcon(categories.at(i)));
                        continue;
                    }
                }
            }

        }

        // Now add all categorized pages to the grouped pages:
        for (int i = 0; i < config_page_category_map.count(); i++) {
            GroupedConfigPage* group_page_for_category = grouped_config_pages[config_page_category_map.values().at(i)];
            if (group_page_for_category) {
                group_page_for_category->addConfigPage(config_page_category_map.keys().at(i));

                QWidget* page_widget = config_page_category_map.keys().at(i)->configPageWidget();
                if (page_widget) {
                    if (page_widget->size().width() > max_width)
                        max_width = page_widget->sizeHint().width();
                    if (page_widget->size().height() > max_height)
                        max_height = page_widget->sizeHint().height();
                }
            } else {
                qWarning() << Q_FUNC_INFO << "Could not find grouped category page for category " << config_page_category_map.values().at(i).toString();
            }
        }
    } else {
        uncategorized_pages = config_pages;
    }

    // Add all the pages to the config pages observer:
    for (int i = 0; i < uncategorized_pages.count(); i++) {
        IConfigPage* config_page = uncategorized_pages.at(i);
        if (config_page) {
            if (config_page->configPageWidget()) {
                if (d->config_pages.contains(uncategorized_pages.at(i)->objectBase())) {
                    if (config_page->configPageWidget()->size().width() > max_width)
                        max_width = config_page->configPageWidget()->sizeHint().width();
                    if (config_page->configPageWidget()->size().height() > max_height)
                        max_height = config_page->configPageWidget()->sizeHint().height();
                    continue;
                }

                // Set the object name to the page title:
                config_page->objectBase()->setObjectName(config_page->configPageTitle());

                addPageCategoryProperty(config_page);
                addPageIconProperty(config_page);

                if (d->config_pages.attachSubject(config_page->objectBase())) {
                    if (config_page->configPageWidget()->size().width() > max_width)
                        max_width = config_page->configPageWidget()->sizeHint().width();
                    if (config_page->configPageWidget()->size().height() > max_height)
                        max_height = config_page->configPageWidget()->sizeHint().height();
                }
            } else {
                LOG_DEBUG("Found configuration page \"" + config_page->configPageTitle() + "\" without a valid configuration widget. This page will not be shown.");
            }
        }
    }

    d->config_pages.endProcessingCycle();

    // Check if the max sizes are bigger than the current size. If so we need to resize:
    int new_width = size().width();
    int new_heigth = size().height();
    if (max_width > size().width())
        new_width = max_width;
//    if (max_height > size().height())
//        new_heigth = max_height;
    resize(new_width,new_heigth);

    // Put the widget in the center of the screen:
    QRect qrect = QApplication::desktop()->availableGeometry(this);
    move(qrect.center() - rect().center());

    // Set up the observer widget:
    if (!d->initialized) {
        d->config_pages_widget.setDisplayMode(d->display_mode);
        d->config_pages_widget.setObserverContext(&d->config_pages);
        d->config_pages_widget.initialize();
        if (!d->config_pages_widget.searchBoxWidget()) {
            d->config_pages_widget.toggleSearchBox();
            d->config_pages_widget.searchBoxWidget()->setButtonFlags(SearchBoxWidget::NoButtons);
        }
    }

    d->config_pages_widget.resizeTableViewRows(22);

    // Set up the table or tree view:
    if (d->config_pages_widget.tableView() && d->display_mode == TableView) {
        d->config_pages_widget.tableView()->horizontalHeader()->hide();
        d->config_pages_widget.tableView()->sortByColumn(1,Qt::AscendingOrder);
    } else if (d->config_pages_widget.treeView() && d->display_mode == TreeView) {
        d->config_pages_widget.treeView()->setRootIsDecorated(false);
        d->config_pages_widget.treeView()->sortByColumn(1,Qt::AscendingOrder);
    }

    if (d->config_pages.subjectCount() > 0)
        d->activity_filter->setActiveSubject(d->config_pages.subjectAt(0));

    d->initialized = true;
}

void Qtilities::CoreGui::ConfigurationWidget::initialize(QList<QObject*> object_list) {
    if (object_list.isEmpty()) {
        object_list = OBJECT_MANAGER->registeredInterfaces("com.Qtilities.CoreGui.IConfigPage/1.0");
        object_list.append(OBJECT_MANAGER->registeredInterfaces("com.Qtilities.CoreGui.IGroupedConfigPageInfoProvider/1.0"));
        LOG_DEBUG(QString("%1 config pages and grouped config page info providers page(s) found.").arg(object_list.count()));
    }

    QList<IConfigPage*> config_ifaces;
    QList<IGroupedConfigPageInfoProvider*> grouped_config_page_info_providers;
    for (int i = 0; i < object_list.count(); i++) {
        IConfigPage* config_iface = qobject_cast<IConfigPage*> (object_list.at(i));
        if (config_iface)
            config_ifaces.append(config_iface);
        else {
            IGroupedConfigPageInfoProvider* grouped_page_info_provider = qobject_cast<IGroupedConfigPageInfoProvider*> (object_list.at(i));
            if (grouped_page_info_provider)
                grouped_config_page_info_providers.append(grouped_page_info_provider);
        }
    }

    initialize(config_ifaces,grouped_config_page_info_providers);
}

void Qtilities::CoreGui::ConfigurationWidget::setApplyAllPages(bool apply_all_pages) {
    d->apply_all_pages = apply_all_pages;
}

bool Qtilities::CoreGui::ConfigurationWidget::applyAllPages() const {
    return d->apply_all_pages;
}

bool Qtilities::CoreGui::ConfigurationWidget::hasPage(const QString& page_name) const {
    for (int i = 0; i < d->config_pages.subjectCount(); i++) {
        // Check if its just a normal config page:
        IConfigPage* config_page = qobject_cast<IConfigPage*> (d->config_pages.subjectAt(i));
        if (config_page){
            if (config_page->configPageTitle() == page_name)
                return true;
        }
        // Check if its a grouped config page:
        GroupedConfigPage* grouped_config_page = qobject_cast<GroupedConfigPage*> (d->config_pages.subjectAt(i));
        if (grouped_config_page){
            if (grouped_config_page->hasConfigPage(page_name))
                return true;
        }
    }

    return false;
}

IConfigPage* Qtilities::CoreGui::ConfigurationWidget::getPage(const QString& page_name) const {
    for (int i = 0; i < d->config_pages.subjectCount(); i++) {
        // Check if its just a normal config page:
        IConfigPage* config_page = qobject_cast<IConfigPage*> (d->config_pages.subjectAt(i));
        if (config_page){
            if (config_page->configPageTitle() == page_name)
                return config_page;
        }
        // Check if its a grouped config page:
        GroupedConfigPage* grouped_config_page = qobject_cast<GroupedConfigPage*> (d->config_pages.subjectAt(i));
        if (grouped_config_page){
            if (grouped_config_page->hasConfigPage(page_name))
                return grouped_config_page->getConfigPage(page_name);
        }
    }

    return 0;
}

void Qtilities::CoreGui::ConfigurationWidget::setCategorizedTabDisplay(bool enabled) {
    d->categorized_display = enabled;
}

bool Qtilities::CoreGui::ConfigurationWidget::categorizedTabDisplay() const {
    return d->categorized_display;
}

void Qtilities::CoreGui::ConfigurationWidget::handleActiveItemChanges(QList<QObject*> active_pages) {
    if (active_pages.count() != 1)
        return;

    IConfigPage* config_page = qobject_cast<IConfigPage*> (active_pages.front());
    if (config_page) {
        // Hide the current widget
        if (d->active_widget)
            d->active_widget->hide();

        if (d->apply_all_pages)
            ui->buttonBox->button(QDialogButtonBox::Apply)->setEnabled(true);
        else
            ui->buttonBox->button(QDialogButtonBox::Apply)->setEnabled(config_page->supportsApply());
        ui->buttonBox->button(QDialogButtonBox::Help)->setEnabled(!config_page->configPageHelpID().isEmpty());
        ui->buttonBox->button(QDialogButtonBox::RestoreDefaults)->setEnabled(config_page->supportsRestoreDefaults());
        ui->lblPageHeader->setText(config_page->configPageTitle());
        if (!config_page->configPageIcon().isNull()) {
            ui->lblPageIcon->setPixmap(QPixmap(config_page->configPageIcon().pixmap(32,32)));
            ui->lblPageIcon->setVisible(true);
        } else
            ui->lblPageIcon->setVisible(false);

        if (ui->configWidget->layout())
            delete ui->configWidget->layout();

        // Create new layout with new widget
        QBoxLayout* layout = new QBoxLayout(QBoxLayout::LeftToRight,ui->configWidget);
        layout->setMargin(0);
        d->active_widget = config_page->configPageWidget();
        layout->addWidget(d->active_widget);
        if (d->active_widget) {
            d->active_widget->show();
            if (d->active_widget->layout())
                d->active_widget->layout()->setMargin(0);
        } else {
            d->active_widget = new QLabel(tr("Failed to find configuartion page for selected item."));
            d->active_widget->show();
        }
    }
}

void Qtilities::CoreGui::ConfigurationWidget::handleActiveGroupedPageChanged(IConfigPage *new_active_grouped_page) {
    // First check if the grouped page is the active page:
    if (d->activity_filter->activeSubjects().count() != 1)
        return;

    if (d->activity_filter->activeSubjects().front() != sender())
        return;

    if (d->apply_all_pages)
        ui->buttonBox->button(QDialogButtonBox::Apply)->setEnabled(true);
    else
        ui->buttonBox->button(QDialogButtonBox::Apply)->setEnabled(new_active_grouped_page->supportsApply());
    ui->buttonBox->button(QDialogButtonBox::Help)->setEnabled(!new_active_grouped_page->configPageHelpID().isEmpty());
    ui->buttonBox->button(QDialogButtonBox::RestoreDefaults)->setEnabled(new_active_grouped_page->supportsRestoreDefaults());
}

void Qtilities::CoreGui::ConfigurationWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void Qtilities::CoreGui::ConfigurationWidget::setActivePage(const QString& active_page_name) {
    QObject* page = d->config_pages.subjectReference(active_page_name);
    if (page) {
        QList<QObject*> active_pages;
        active_pages << page;
        d->activity_filter->setActiveSubjects(active_pages);
        d->config_pages_widget.selectObjects(active_pages);
        return;
    } else {
        // Check if its in a grouped config page:
        for (int i = 0; i < d->config_pages.subjectCount(); i++) {
            GroupedConfigPage* grouped_config_page = qobject_cast<GroupedConfigPage*> (d->config_pages.subjectAt(i));
            if (grouped_config_page){
                if (grouped_config_page->hasConfigPage(active_page_name)) {
                    QList<QObject*> active_pages;
                    active_pages << grouped_config_page;
                    d->activity_filter->setActiveSubjects(active_pages);
                    d->config_pages_widget.selectObjects(active_pages);

                    // Now select the correct tab in the grouped config page:
                    grouped_config_page->setActivePage(active_page_name);
                    return;
                }
            }
        }
    }

    LOG_DEBUG("Cannot set active configuration page to \"" + active_page_name + "\". A page with this title does not exist.");
}

QString Qtilities::CoreGui::ConfigurationWidget::activePageName() const {
    if (!d->initialized)
        return QString();

    IConfigPage* config_page = qobject_cast<IConfigPage*> (d->activity_filter->activeSubjects().front());

    // Check if the active config page is a grouped page:
    GroupedConfigPage* grouped_config_page = qobject_cast<GroupedConfigPage*> (config_page->objectBase());
    if (grouped_config_page) {
        return grouped_config_page->activePage()->configPageTitle();
    } else
        return config_page->configPageTitle();
}

Qtilities::CoreGui::Interfaces::IConfigPage* Qtilities::CoreGui::ConfigurationWidget::activePageIFace() const {
    if (!d->initialized)
        return 0;

    IConfigPage* config_page = qobject_cast<IConfigPage*> (d->activity_filter->activeSubjects().front());

    // Check if the active config page is a grouped page:
    GroupedConfigPage* grouped_config_page = qobject_cast<GroupedConfigPage*> (config_page->objectBase());
    if (grouped_config_page) {
        return grouped_config_page->activePage();
    } else
        return config_page;
}

void Qtilities::CoreGui::ConfigurationWidget::on_buttonBox_clicked(QAbstractButton *button) {
    if (ui->buttonBox->button(QDialogButtonBox::Apply) == button) {
        if (d->apply_all_pages) {
            for (int i = 0; i < d->config_pages.subjectCount(); i++) {
                IConfigPage* config_page = qobject_cast<IConfigPage*> (d->config_pages.subjectAt(i));
                if (config_page){
                    config_page->configPageApply();
                    emit appliedPage(config_page);
                }
            }
        } else {
            if (d->activity_filter->activeSubjects().count() == 1) {
                IConfigPage* config_page = qobject_cast<IConfigPage*> (d->activity_filter->activeSubjects().front());
                config_page->configPageApply();
                emit appliedPage(config_page);
            }
        }
    } else if (ui->buttonBox->button(QDialogButtonBox::RestoreDefaults) == button) {
        if (d->activity_filter->activeSubjects().count() == 1) {
            IConfigPage* config_page = qobject_cast<IConfigPage*> (d->activity_filter->activeSubjects().front());
            config_page->configPageRestoreDefaults();
        }
    } else if (ui->buttonBox->button(QDialogButtonBox::Cancel) == button) {
        close();
    } else if (ui->buttonBox->button(QDialogButtonBox::Help) == button) {
        if (activePageIFace()) {
            if (!activePageIFace()->configPageHelpID().isEmpty())
                HELP_MANAGER->requestUrlDisplay(QUrl(activePageIFace()->configPageHelpID()));
        }
    }
}

void Qtilities::CoreGui::ConfigurationWidget::addPageIconProperty(IConfigPage *config_page) {
    if (!config_page)
        return;

    if (config_page->configPageIcon().isNull())
        return;

    SharedProperty icon_property(qti_prop_DECORATION,config_page->configPageIcon());
    ObjectManager::setSharedProperty(config_page->objectBase(),icon_property);
}

void Qtilities::CoreGui::ConfigurationWidget::addPageCategoryProperty(IConfigPage *config_page) {
    if (!config_page)
        return;

    if (config_page->configPageCategory().isEmpty())
        return;

    if (ObjectManager::propertyExists(config_page->objectBase(),qti_prop_CATEGORY_MAP)) {
        MultiContextProperty category_property = ObjectManager::getMultiContextProperty(config_page->objectBase(),qti_prop_CATEGORY_MAP);
        if (category_property.setValue(qVariantFromValue(config_page->configPageCategory()),d->config_pages.observerID()))
            ObjectManager::setMultiContextProperty(config_page->objectBase(),category_property);
    } else {
        MultiContextProperty category_property(qti_prop_CATEGORY_MAP);
        if (category_property.setValue(qVariantFromValue(config_page->configPageCategory()),d->config_pages.observerID()))
            ObjectManager::setMultiContextProperty(config_page->objectBase(),category_property);
    }
}
