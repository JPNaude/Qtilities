/****************************************************************************
**
** Copyright (c) 2009-2013, Jaco Naudé
**
** This file is part of Qtilities. For licensing information, please
** see http://jpnaude.github.io/Qtilities/page_licensing.html
**
****************************************************************************/

#include "ConfigurationWidget.h"
#include "ui_ConfigurationWidget.h"
#include "IConfigPage.h"
#include "ObserverWidget.h"
#include "GroupedConfigPage.h"
#include "QtilitiesApplication.h"
#include "QtilitiesMainWindow.h"

#include <Logger>

#include <QTreeWidgetItem>
#include <QBoxLayout>
#include <QDesktopWidget>
#include <QPushButton>
#include <QStatusBar>

using namespace Qtilities::CoreGui::Interfaces;
struct Qtilities::CoreGui::ConfigurationWidgetPrivateData {
    ConfigurationWidgetPrivateData() : config_pages_obs(QObject::tr("Application Settings")),
        activity_filter(0),
        active_widget(0),
        apply_all_pages(true),
        apply_all_pages_visible(true),
        categorized_display(false) {}

    // --------------------------------------------
    // Used by SideTableView and SideTreeView
    // --------------------------------------------
    //! Observer widget which is used to display the categories of the config pages.
    ObserverWidget          obs_widget;

    // --------------------------------------------
    // Used by ModeWidget Modes
    // --------------------------------------------
    //! The mode widget.
    QtilitiesMainWindow*    mode_widget;
    //! Store all the wrappers.
    QList<IMode*>           mode_wrappers;

    // --------------------------------------------
    // Shared Members
    // --------------------------------------------
    //! The configuration pages observer.
    Observer                config_pages_obs;
    //! Indicates if the page have been initialized.
    bool                    initialized;
    //! The activity policy filter for the pages observer.
    ActivityPolicyFilter*   activity_filter;
    //! Keeps track of the active widget.
    QPointer<QWidget>       active_widget;
    //! The display mode of this widget.
    ConfigurationWidget::DisplayMode display_mode;
    //! The apply all pages setting for this widget.
    bool                    apply_all_pages;
    //! During initialization all pages are checked to see if they support Apply. This bool stores if any pages actually did support it.
    bool                    apply_all_pages_visible;
    //! Indicates if this widget uses a categorized display.
    bool                    categorized_display;
    //! Indicates if icons must be used when categorized display is enabled.
    bool                    categorized_display_use_tab_icons;
};

Qtilities::CoreGui::ConfigurationWidget::ConfigurationWidget(Qtilities::DisplayMode display_mode, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConfigurationWidget)
{
    d = new ConfigurationWidgetPrivateData;
    if (display_mode == Qtilities::TreeView)
        d->display_mode = DisplayLeftItemViewTree;
    else if (display_mode == Qtilities::TableView)
        d->display_mode = DisplayLeftItemViewTable;
    sharedConstruct();
}

Qtilities::CoreGui::ConfigurationWidget::ConfigurationWidget(ConfigurationWidget::DisplayMode display_mode, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConfigurationWidget)
{
    d = new ConfigurationWidgetPrivateData;
    d->display_mode = display_mode;
    sharedConstruct();
}

Qtilities::CoreGui::ConfigurationWidget::~ConfigurationWidget() {
    delete ui;
    if (d->activity_filter)
        d->activity_filter->disconnect(this);
    delete d;
}

Qtilities::CoreGui::ModeManager *Qtilities::CoreGui::ConfigurationWidget::modeWidgetModeManager() {
    if (d->display_mode & DisplayModeWidgetViews) {
        if (d->mode_widget) {
            return d->mode_widget->modeManager();
        }
    }

    return 0;
}

void Qtilities::CoreGui::ConfigurationWidget::showEvent(QShowEvent* event) {
    QWidget::showEvent(event);
    QApplication::processEvents(); // Required for rect() to be updated before running the repositioning code below.

    // Put the widget in the center of the screen again after being resized:
    QRect qrect = QApplication::desktop()->availableGeometry(this);
    move(qrect.center() - rect().center());
}

void Qtilities::CoreGui::ConfigurationWidget::sharedConstruct() {
    ui->setupUi(this);
    ui->widgetTopModeWidgetHolder->setVisible(false);
    setWindowTitle(QApplication::applicationName() + tr(" Settings"));

    d->initialized = false;

    if (d->display_mode & DisplayModeWidgetViews) {
        ui->widgetTopModeWidgetHolder->setVisible(true);
        ui->widgetObsWidgetHolder->setVisible(false);

        QtilitiesMainWindow::ModeLayout mode_layout;
        if (d->display_mode == DisplayTopModeWidgetView)
            mode_layout = QtilitiesMainWindow::ModesTop;
        else if (d->display_mode == DisplayRightModeWidgetView)
            mode_layout = QtilitiesMainWindow::ModesRight;
        else if (d->display_mode == DisplayBottomModeWidgetView)
            mode_layout = QtilitiesMainWindow::ModesBottom;
        else if (d->display_mode == DisplayLeftModeWidgetView)
            mode_layout = QtilitiesMainWindow::ModesLeft;
        d->mode_widget = new QtilitiesMainWindow(mode_layout);
        d->mode_widget->hideTaskSummaryWidget();
        d->mode_widget->modeManager()->setMinimumItemSize(QSize(128,48));

        // Layout and placement of observer widget:
        if (ui->widgetTopModeWidgetHolder->layout())
            delete ui->widgetTopModeWidgetHolder->layout();

        QHBoxLayout* layout = new QHBoxLayout(ui->widgetTopModeWidgetHolder);
        layout->addWidget(d->mode_widget);
        layout->setMargin(0);
        if (d->mode_widget->statusBar())
            d->mode_widget->statusBar()->hide();
    } else if (d->display_mode & DisplayLeftItemViews) {
        ui->widgetTopModeWidgetHolder->setVisible(false);
        ui->widgetObsWidgetHolder->setVisible(true);

        // Layout and placement of observer widget:
        if (ui->configPagesWidgetHolder->layout())
            delete ui->configPagesWidgetHolder->layout();

        QHBoxLayout* layout = new QHBoxLayout(ui->configPagesWidgetHolder);
        layout->addWidget(&d->obs_widget);
        layout->setMargin(0);
    }

    // Put the widget in the center of the screen:
    QRect qrect = QApplication::desktop()->availableGeometry(this);
    move(qrect.center() - rect().center());

    setAttribute(Qt::WA_QuitOnClose,false);

    // Hide the help button if needed:
    #ifndef QTILITIES_NO_HELP
    ui->buttonBox->button(QDialogButtonBox::Help)->setVisible(false);
    #endif
}

void Qtilities::CoreGui::ConfigurationWidget::initialize(QList<IConfigPage*> config_pages, QList<IGroupedConfigPageInfoProvider*> grouped_page_info_providers) {
    if (!d->initialized) {
        // Add an activity policy filter to the config pages observer:
        d->activity_filter = new ActivityPolicyFilter();
        d->activity_filter->setActivityPolicy(ActivityPolicyFilter::UniqueActivity);
        d->activity_filter->setMinimumActivityPolicy(ActivityPolicyFilter::ProhibitNoneActive);
        d->activity_filter->setNewSubjectActivityPolicy(ActivityPolicyFilter::SetNewInactive);
        connect(d->activity_filter,SIGNAL(activeSubjectsChanged(QList<QObject*>,QList<QObject*>)),SLOT(handleActiveItemChanges(QList<QObject*>)));
        d->config_pages_obs.installSubjectFilter(d->activity_filter);

        if (d->display_mode & DisplayLeftItemViews) {
            d->config_pages_obs.useDisplayHints();
            d->config_pages_obs.displayHints()->setActivityControlHint(ObserverHints::FollowSelection);
            d->config_pages_obs.displayHints()->setActivityDisplayHint(ObserverHints::NoActivityDisplay);
            d->config_pages_obs.displayHints()->setHierarchicalDisplayHint(ObserverHints::CategorizedHierarchy);
            d->config_pages_obs.displayHints()->setActionHints(ObserverHints::ActionFindItem);
        }
    }

    QApplication::setOverrideCursor(Qt::BusyCursor);
    d->apply_all_pages_visible = false;

    // Figure out what the maximum sizes are in all pages.
    int max_width = -1;
    int max_height = -1;

    QList<IConfigPage*> uncategorized_pages;
    QList<IConfigPage*> initialized_pages;

    d->config_pages_obs.startProcessingCycle();
    if (d->categorized_display) {
        QList<QtilitiesCategory>                    categories;
        QMap<IConfigPage*,QtilitiesCategory>        config_page_category_map;
        QMap<QtilitiesCategory,GroupedConfigPage*>  grouped_config_pages;
        // Add all the pages to the config pages observer:
        for (int i = 0; i < config_pages.count(); ++i) {
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
                if (config_page->supportsApply())
                    d->apply_all_pages_visible = true;
                config_page->configPageInitialize();
                initialized_pages << config_page;
            }
        }

        // Construct grouped pages for all categories:
        for (int i = 0; i < categories.count(); ++i) {
            // Check if there is already a grouped page for this category (in case initialize() is called twice):
            if (!grouped_config_pages.contains(categories.at(i))) {
                GroupedConfigPage* grouped_page = new GroupedConfigPage(categories.at(i));
                grouped_page->setUseTabIcons(d->categorized_display_use_tab_icons);
                grouped_config_pages[categories.at(i)] = grouped_page;
                grouped_page->setApplyAll(d->apply_all_pages);
                connect(grouped_page,SIGNAL(appliedPage(IConfigPage*)),SIGNAL(appliedPage(IConfigPage*)),Qt::UniqueConnection);
                connect(grouped_page,SIGNAL(activeGroupedPageChanged(IConfigPage*)),SLOT(handleActiveGroupedPageChanged(IConfigPage*)));
                uncategorized_pages << grouped_page;

                // Look if there is an information provider for this page:
                for (int g = 0; g < grouped_page_info_providers.count(); g++) {
                    if (grouped_page_info_providers.at(g)->isProviderForCategory(categories.at(i))) {
                        grouped_page->setConfigPageIcon(grouped_page_info_providers.at(g)->groupedConfigPageIcon(categories.at(i)));
                        grouped_page->setPageOrder(grouped_page_info_providers.at(g)->pageOrderForCategory(categories.at(i)));
                        break;
                    }
                }
            }
        }

        // Now add all categorized pages to the grouped pages:
        QMapIterator<IConfigPage*,QtilitiesCategory> itr(config_page_category_map);
        while (itr.hasNext()) {
            itr.next();
            if (grouped_config_pages.contains(itr.value())) {
                GroupedConfigPage* group_page_for_category = grouped_config_pages[itr.value()];
                group_page_for_category->addConfigPage(itr.key());

                QWidget* page_widget = itr.key()->configPageWidget();
                if (page_widget) {
                    if (page_widget->size().width() > max_width)
                        max_width = page_widget->sizeHint().width();
                    if (page_widget->size().height() > max_height)
                        max_height = page_widget->sizeHint().height();
                }
            } else {
                qWarning() << Q_FUNC_INFO << "Could not find grouped category page for category " << itr.value().toString();
            }
        }

        // Finally construct tabs in all grouped pages:
        itr.toFront();
        while (itr.hasNext()) {
            itr.next();
            if (grouped_config_pages.contains(itr.value())) {
                GroupedConfigPage* group_page_for_category = grouped_config_pages[itr.value()];
                group_page_for_category->constructTabWidget();
            } else {
                qWarning() << Q_FUNC_INFO << "Could not find grouped category page for category " << itr.value().toString();
            }
        }
    } else {
        uncategorized_pages = config_pages;
    }

    // Wrap all config pages in ConfigPageModeWrapper if needed:
    for (int i = 0; i < d->mode_wrappers.count(); i++) {
        delete d->mode_wrappers.at(i);
    }
    d->mode_wrappers.clear();

    // Add all the pages to the config pages observer:
    for (int i = 0; i < uncategorized_pages.count(); ++i) {
        IConfigPage* config_page = uncategorized_pages.at(i);
        if (config_page) {
            if (config_page->configPageWidget()) {
                if (d->config_pages_obs.contains(uncategorized_pages.at(i)->objectBase())) {
                    if (config_page->configPageWidget()->size().width() > max_width)
                        max_width = config_page->configPageWidget()->sizeHint().width();
                    if (config_page->configPageWidget()->size().height() > max_height)
                        max_height = config_page->configPageWidget()->sizeHint().height();
                    continue;
                }

                // Set the object name to the page title:
                config_page->objectBase()->setObjectName(config_page->configPageTitle());
                if (!initialized_pages.contains(config_page))
                    config_page->configPageInitialize();

                addPageCategoryProperty(config_page);
                addPageIconProperty(config_page);

                if (config_page->supportsApply())
                    d->apply_all_pages_visible = true;

                if (d->config_pages_obs.attachSubject(config_page->objectBase())) {
                    if (config_page->configPageWidget()->size().width() > max_width)
                        max_width = config_page->configPageWidget()->sizeHint().width();
                    if (config_page->configPageWidget()->size().height() > max_height)
                        max_height = config_page->configPageWidget()->sizeHint().height();
                }

                if (d->display_mode & DisplayModeWidgetViews) {
                    if (config_page->configPageWidget()->layout()) {
                        if (d->categorized_display && !config_page->configPageCategory().isEmpty()) {
                            if (d->display_mode == DisplayTopModeWidgetView) {
                                config_page->configPageWidget()->layout()->setContentsMargins(9,3,9,0);
                            } else if (d->display_mode == DisplayRightModeWidgetView) {
                                config_page->configPageWidget()->layout()->setContentsMargins(9,9,3,0);
                            } else if (d->display_mode == DisplayBottomModeWidgetView) {
                                config_page->configPageWidget()->layout()->setContentsMargins(9,0,9,3);
                            } else if (d->display_mode == DisplayLeftModeWidgetView) {
                                config_page->configPageWidget()->layout()->setContentsMargins(3,9,0,0);
                            }
                        } else {
                            if (d->display_mode == DisplayTopModeWidgetView) {
                                config_page->configPageWidget()->layout()->setContentsMargins(9,3,9,0);
                            } else if (d->display_mode == DisplayRightModeWidgetView) {
                                config_page->configPageWidget()->layout()->setContentsMargins(9,9,3,0);
                            } else if (d->display_mode == DisplayBottomModeWidgetView) {
                                config_page->configPageWidget()->layout()->setContentsMargins(6,0,9,3);
                            } else if (d->display_mode == DisplayLeftModeWidgetView) {
                                config_page->configPageWidget()->layout()->setContentsMargins(3,9,9,0);
                            }
                        }
                    }
                    d->mode_wrappers.append(new ConfigPageModeWrapper(config_page));
                }
            } else {
                LOG_DEBUG("Found configuration page \"" + config_page->configPageTitle() + "\" without a valid configuration widget. This page will not be shown.");
            }
        }
    }

    d->config_pages_obs.endProcessingCycle();

    // Check if the max sizes are bigger than the current size. If so we need to resize:
    int new_width = size().width();
    int new_heigth = size().height();
    if (max_width > size().width())
        new_width = max_width;
//    if (max_height > size().height())
//        new_heigth = max_height;
    resize(new_width,new_heigth);

    // Note: If its not yet show, the showEvent() override will reposition it.
    QApplication::processEvents(); // Process in order for rect() to be updated before we move to center of the screen again.
    QRect qrect = QApplication::desktop()->availableGeometry(this);
    move(qrect.center() - rect().center());

    if (d->display_mode & DisplayModeWidgetViews) {
        disconnect(d->mode_widget->modeManager(),SIGNAL(activeModeChanged(int,int)),this,SLOT(handleModeWidgetActiveModeChanged()));
        d->mode_widget->modeManager()->addModes(d->mode_wrappers);
        connect(d->mode_widget->modeManager(),SIGNAL(activeModeChanged(int,int)),this,SLOT(handleModeWidgetActiveModeChanged()),Qt::UniqueConnection);
    } else if (d->display_mode & DisplayLeftItemViews) {
        // Set up the observer widget:
        if (!d->initialized) {
            if (d->display_mode == DisplayLeftItemViewTable)
                d->obs_widget.setDisplayMode(TableView);
            else if (d->display_mode == DisplayLeftItemViewTree)
                d->obs_widget.setDisplayMode(TreeView);
            d->obs_widget.setObserverContext(&d->config_pages_obs);
            d->obs_widget.initialize();
            if (!d->obs_widget.searchBoxWidget()) {
                d->obs_widget.toggleSearchBox();
                d->obs_widget.searchBoxWidget()->setButtonFlags(SearchBoxWidget::NoButtons);
            }
        }

        d->obs_widget.resizeTableViewRows(22);

        // Set up the table or tree view:
        if (d->obs_widget.tableView() && d->display_mode == DisplayLeftItemViewTable) {
            d->obs_widget.tableView()->horizontalHeader()->hide();
            d->obs_widget.tableView()->sortByColumn(1,Qt::AscendingOrder);
        } else if (d->obs_widget.treeView() && d->display_mode == DisplayLeftItemViewTree) {
            //d->config_pages_obs_widget.treeView()->setRootIsDecorated(false);
            d->obs_widget.treeView()->sortByColumn(1,Qt::AscendingOrder);
        }
    }

    d->initialized = true;

    QApplication::restoreOverrideCursor();
}

void Qtilities::CoreGui::ConfigurationWidget::initialize(QList<QObject*> object_list) {
    if (object_list.isEmpty()) {
        object_list = OBJECT_MANAGER->registeredInterfaces("com.Qtilities.CoreGui.IConfigPage/1.0");
        object_list.append(OBJECT_MANAGER->registeredInterfaces("com.Qtilities.CoreGui.IGroupedConfigPageInfoProvider/1.0"));
        LOG_DEBUG(QString("%1 config pages and grouped config page info providers page(s) found.").arg(object_list.count()));
    }

    QList<IConfigPage*> config_ifaces;
    QList<IGroupedConfigPageInfoProvider*> grouped_config_page_info_providers;
    for (int i = 0; i < object_list.count(); ++i) {
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
    for (int i = 0; i < d->config_pages_obs.subjectCount(); ++i) {
        // Check if its just a normal config page:
        IConfigPage* config_page = qobject_cast<IConfigPage*> (d->config_pages_obs.subjectAt(i));
        if (config_page){
            if (config_page->configPageTitle() == page_name)
                return true;
        }
        // Check if its a grouped config page:
        GroupedConfigPage* grouped_config_page = qobject_cast<GroupedConfigPage*> (d->config_pages_obs.subjectAt(i));
        if (grouped_config_page){
            if (grouped_config_page->hasConfigPage(page_name))
                return true;
        }
    }

    return false;
}

IConfigPage* Qtilities::CoreGui::ConfigurationWidget::getPage(const QString& page_name) const {
    for (int i = 0; i < d->config_pages_obs.subjectCount(); ++i) {
        // Check if its just a normal config page:
        IConfigPage* config_page = qobject_cast<IConfigPage*> (d->config_pages_obs.subjectAt(i));
        if (config_page){
            if (config_page->configPageTitle() == page_name)
                return config_page;
        }
        // Check if its a grouped config page:
        GroupedConfigPage* grouped_config_page = qobject_cast<GroupedConfigPage*> (d->config_pages_obs.subjectAt(i));
        if (grouped_config_page){
            if (grouped_config_page->hasConfigPage(page_name))
                return grouped_config_page->getConfigPage(page_name);
        }
    }

    return 0;
}

void Qtilities::CoreGui::ConfigurationWidget::setCategorizedTabDisplay(bool enabled, bool use_tab_icons) {
    d->categorized_display = enabled;
    d->categorized_display_use_tab_icons = use_tab_icons;
}

bool Qtilities::CoreGui::ConfigurationWidget::categorizedTabDisplay() const {
    return d->categorized_display;
}

void Qtilities::CoreGui::ConfigurationWidget::handleModeWidgetActiveModeChanged() {
    IMode* mode = d->mode_widget->modeManager()->activeModeIFace();
    Q_ASSERT(mode);
    ConfigPageModeWrapper* wrapper = qobject_cast<ConfigPageModeWrapper*> (mode->objectBase());
    Q_ASSERT(wrapper);
    Q_ASSERT(wrapper->configPage());
    d->activity_filter->setActiveSubject(wrapper->configPage()->objectBase());
}

void Qtilities::CoreGui::ConfigurationWidget::handleActiveItemChanges(QList<QObject*> active_pages) {
    if (active_pages.count() != 1)
        return;

    IConfigPage* config_page = qobject_cast<IConfigPage*> (active_pages.front());
    if (config_page) {
        // Hide the current widget
        if (d->active_widget)
            d->active_widget->hide();

        if (d->apply_all_pages) {
            ui->buttonBox->button(QDialogButtonBox::Apply)->setVisible(d->apply_all_pages_visible);
            ui->buttonBox->button(QDialogButtonBox::Cancel)->setVisible(d->apply_all_pages_visible);
            ui->buttonBox->button(QDialogButtonBox::Ok)->setVisible(d->apply_all_pages_visible);
            ui->buttonBox->button(QDialogButtonBox::Close)->setVisible(!d->apply_all_pages_visible);
        } else {
            ui->buttonBox->button(QDialogButtonBox::Apply)->setVisible(config_page->supportsApply());
            ui->buttonBox->button(QDialogButtonBox::Cancel)->setVisible(true);
            ui->buttonBox->button(QDialogButtonBox::Ok)->setVisible(true);
            ui->buttonBox->button(QDialogButtonBox::Close)->setVisible(true);
        }

        #ifdef QTILITIES_NO_HELP
        ui->buttonBox->button(QDialogButtonBox::Help)->setVisible(!config_page->configPageHelpID().isEmpty());
        #endif

        ui->buttonBox->button(QDialogButtonBox::RestoreDefaults)->setVisible(config_page->supportsRestoreDefaults());

        d->active_widget = config_page->configPageWidget();
        if (d->display_mode & DisplayLeftItemViews) {
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

            layout->addWidget(d->active_widget);
            if (d->active_widget) {
                d->active_widget->show();
    //            if (d->active_widget->layout())
    //                d->active_widget->layout()->setMargin(0);
            } else {
                d->active_widget = new QLabel(tr("Failed to find configuartion page for selected item."));
                d->active_widget->show();
            }
        } else if (d->display_mode & DisplayModeWidgetViews) {
            disconnect(d->mode_widget->modeManager(),SIGNAL(activeModeChanged(int,int)),this,SLOT(handleModeWidgetActiveModeChanged()));
            d->mode_widget->modeManager()->setActiveMode(config_page->configPageTitle());
            connect(d->mode_widget->modeManager(),SIGNAL(activeModeChanged(int,int)),this,SLOT(handleModeWidgetActiveModeChanged()),Qt::UniqueConnection);
        }
    }
}

void Qtilities::CoreGui::ConfigurationWidget::handleActiveGroupedPageChanged(IConfigPage *new_active_grouped_page) {
    // First check if the grouped page is the active page:
    if (d->activity_filter->activeSubjects().count() != 1)
        return;

    if (d->activity_filter->activeSubjects().front() != sender())
        return;

    if (d->apply_all_pages) {
        ui->buttonBox->button(QDialogButtonBox::Apply)->setVisible(d->apply_all_pages_visible);
        ui->buttonBox->button(QDialogButtonBox::Cancel)->setVisible(d->apply_all_pages_visible);
        ui->buttonBox->button(QDialogButtonBox::Ok)->setVisible(d->apply_all_pages_visible);
        ui->buttonBox->button(QDialogButtonBox::Close)->setVisible(!d->apply_all_pages_visible);
    } else {
        ui->buttonBox->button(QDialogButtonBox::Apply)->setVisible(new_active_grouped_page->supportsApply());
        ui->buttonBox->button(QDialogButtonBox::Cancel)->setVisible(true);
        ui->buttonBox->button(QDialogButtonBox::Ok)->setVisible(true);
        ui->buttonBox->button(QDialogButtonBox::Close)->setVisible(true);
    }
    #ifdef QTILITIES_NO_HELP
    ui->buttonBox->button(QDialogButtonBox::Help)->setVisible(!new_active_grouped_page->configPageHelpID().isEmpty());
    #endif
    ui->buttonBox->button(QDialogButtonBox::RestoreDefaults)->setVisible(new_active_grouped_page->supportsRestoreDefaults());
}

void Qtilities::CoreGui::ConfigurationWidget::changeEvent(QEvent *e) {
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
    if (!d->initialized) {
        LOG_DEBUG(QString("%1 - called before initialize() was called."));
        return;
    }

    QObject* page = d->config_pages_obs.subjectReference(active_page_name);
    if (page) {
        QList<QObject*> active_pages;
        active_pages << page;
        d->activity_filter->setActiveSubjects(active_pages);
        return;
    } else {
        // Check if its in a grouped config page:
        for (int i = 0; i < d->config_pages_obs.subjectCount(); ++i) {
            GroupedConfigPage* grouped_config_page = qobject_cast<GroupedConfigPage*> (d->config_pages_obs.subjectAt(i));
            if (grouped_config_page){
                if (grouped_config_page->hasConfigPage(active_page_name)) {
                    QList<QObject*> active_pages;
                    active_pages << grouped_config_page;
                    d->activity_filter->setActiveSubjects(active_pages);

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
    if (ui->buttonBox->button(QDialogButtonBox::Apply) == button || ui->buttonBox->button(QDialogButtonBox::Ok) == button) {
        if (d->apply_all_pages) {
            for (int i = 0; i < d->config_pages_obs.subjectCount(); ++i) {
                IConfigPage* config_page = qobject_cast<IConfigPage*> (d->config_pages_obs.subjectAt(i));
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

        if (ui->buttonBox->button(QDialogButtonBox::Ok) == button)
            close();
    } else if (ui->buttonBox->button(QDialogButtonBox::RestoreDefaults) == button) {
        if (d->activity_filter->activeSubjects().count() == 1) {
            IConfigPage* config_page = qobject_cast<IConfigPage*> (d->activity_filter->activeSubjects().front());
            config_page->configPageRestoreDefaults();
        }
    } else if (ui->buttonBox->button(QDialogButtonBox::Cancel) == button || ui->buttonBox->button(QDialogButtonBox::Close) == button) {
        close();
    } else if (ui->buttonBox->button(QDialogButtonBox::Help) == button) {
        #ifndef QTILITIES_NO_HELP
        if (activePageIFace()) {
            if (!activePageIFace()->configPageHelpID().isEmpty())
                HELP_MANAGER->requestUrlDisplay(QUrl(activePageIFace()->configPageHelpID()));
        }
        #endif
    }
}

void Qtilities::CoreGui::ConfigurationWidget::addPageIconProperty(IConfigPage *config_page) {
    if (d->display_mode & DisplayLeftItemViews) {
        if (!config_page)
            return;

        if (config_page->configPageIcon().isNull())
            return;

        SharedProperty icon_property(qti_prop_DECORATION,config_page->configPageIcon());
        ObjectManager::setSharedProperty(config_page->objectBase(),icon_property);
    }
}

void Qtilities::CoreGui::ConfigurationWidget::addPageCategoryProperty(IConfigPage *config_page) {
    if (d->display_mode & DisplayLeftItemViews) {
        if (!config_page)
            return;

        if (config_page->configPageCategory().isEmpty())
            return;

        if (ObjectManager::propertyExists(config_page->objectBase(),qti_prop_CATEGORY_MAP)) {
            MultiContextProperty category_property = ObjectManager::getMultiContextProperty(config_page->objectBase(),qti_prop_CATEGORY_MAP);
            if (category_property.setValue(qVariantFromValue(config_page->configPageCategory()),d->config_pages_obs.observerID()))
                ObjectManager::setMultiContextProperty(config_page->objectBase(),category_property);
        } else {
            MultiContextProperty category_property(qti_prop_CATEGORY_MAP);
            if (category_property.setValue(qVariantFromValue(config_page->configPageCategory()),d->config_pages_obs.observerID()))
                ObjectManager::setMultiContextProperty(config_page->objectBase(),category_property);
        }
    }
}
