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

#include "SearchBoxWidget.h"
#include "ui_SearchBoxWidget.h"
#include "QtilitiesCoreGuiConstants.h"

#include <QMenu>
#include <QAction>

using namespace Qtilities::CoreGui::Constants;
using namespace Qtilities::CoreGui::Icons;

struct Qtilities::CoreGui::SearchBoxWidgetData {
    QMenu* searchOptionsMenu;
    QAction* searchOptionCaseSensitive;
    QAction* searchOptionWholeWordsOnly;
    QAction* searchOptionRegEx;

    SearchBoxWidget::ButtonFlags button_flags;
    SearchBoxWidget::SearchOptions search_options;
    SearchBoxWidget::WidgetMode widget_mode;
};

Qtilities::CoreGui::SearchBoxWidget::SearchBoxWidget(SearchOptions search_options, WidgetMode mode, ButtonFlags buttons, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SearchBoxWidget)
{
    ui->setupUi(this);
    d = new SearchBoxWidgetData;

    setWindowTitle(tr("Search Box Widget"));

    // Connect ui signals to own signals which will be monitored by this widget's parent
    connect(ui->btnClose,SIGNAL(clicked()),SIGNAL(btnClose_clicked()));
    connect(ui->btnFindPrevious,SIGNAL(clicked()),SIGNAL(btnFindPrevious_clicked()));
    connect(ui->btnFindNext,SIGNAL(clicked()),SIGNAL(btnFindNext_clicked()));
    connect(ui->btnReplacePrevious,SIGNAL(clicked()),SIGNAL(btnReplacePrevious_clicked()));
    connect(ui->btnReplaceNext,SIGNAL(clicked()),SIGNAL(btnReplaceNext_clicked()));

    // Construct the search options pop-up menu
    d->searchOptionsMenu = new QMenu("Search Options");
    ui->btnSearchOptions->setIcon(QIcon(ICON_SEARCH_OPTIONS_22x22));
    ui->btnSearchOptions->setIconSize(QSize(16,16));
    d->searchOptionCaseSensitive = new QAction("Case Sensitive",0);
    d->searchOptionCaseSensitive->setCheckable(true);
    d->searchOptionsMenu->addAction(d->searchOptionCaseSensitive);
    connect(d->searchOptionCaseSensitive,SIGNAL(triggered()),SIGNAL(searchOptionsChanged()));
    d->searchOptionWholeWordsOnly = new QAction("Whole Words Only",0);
    d->searchOptionsMenu->addAction(d->searchOptionWholeWordsOnly);
    d->searchOptionWholeWordsOnly->setCheckable(true);
    d->searchOptionRegEx = new QAction("Use Regular Expression",0);
    d->searchOptionsMenu->addAction(d->searchOptionRegEx);
    d->searchOptionRegEx->setCheckable(true);
    ui->btnSearchOptions->setPopupMode(QToolButton::InstantPopup);
    ui->btnSearchOptions->setMenu(d->searchOptionsMenu);

    // Inspect the search options and button flags:
    setButtonFlags(buttons);
    setSearchOptions(search_options);
    setWidgetMode(mode);
}

Qtilities::CoreGui::SearchBoxWidget::~SearchBoxWidget()
{
    delete d;
    delete ui;
}

QString Qtilities::CoreGui::SearchBoxWidget::currentSearchString() const {
    return ui->txtSearchString->text();
}

void Qtilities::CoreGui::SearchBoxWidget::setCurrentSearchString(const QString& search_string) {
    ui->txtSearchString->setText(search_string);
}

QString Qtilities::CoreGui::SearchBoxWidget::currentReplaceString() const {
    return ui->txtReplaceString->text();
}

void Qtilities::CoreGui::SearchBoxWidget::setCurrentReplaceString(const QString& replace_string) {
    ui->txtReplaceString->setText(replace_string);
}

void Qtilities::CoreGui::SearchBoxWidget::changeEvent(QEvent *e)
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

void Qtilities::CoreGui::SearchBoxWidget::on_txtSearchString_textChanged(const QString & text) {
    emit searchStringChanged(text);
}

void Qtilities::CoreGui::SearchBoxWidget::on_txtReplaceString_textChanged(const QString & text) {
    ui->btnReplaceNext->click();
}

bool Qtilities::CoreGui::SearchBoxWidget::caseSensitive() const {
    if (d->searchOptionCaseSensitive)
        return d->searchOptionCaseSensitive->isChecked();
    else
        return false;
}

bool Qtilities::CoreGui::SearchBoxWidget::wholeWordsOnly() const {
    if (d->searchOptionWholeWordsOnly)
        return d->searchOptionWholeWordsOnly->isChecked();
    else
        return false;
}

bool Qtilities::CoreGui::SearchBoxWidget::regExpression() const {
    if (d->searchOptionRegEx)
        return d->searchOptionRegEx->isChecked();
    else
        return false;
}

void Qtilities::CoreGui::SearchBoxWidget::setCaseSensitive(bool toggle) {
    if (d->searchOptionCaseSensitive)
        return d->searchOptionCaseSensitive->setChecked(toggle);
}

void Qtilities::CoreGui::SearchBoxWidget::setWholeWordsOnly(bool toggle) {
    if (d->searchOptionWholeWordsOnly)
        return d->searchOptionWholeWordsOnly->setChecked(toggle);
}

void Qtilities::CoreGui::SearchBoxWidget::setRegExpression(bool toggle) {
    if (d->searchOptionRegEx)
        return d->searchOptionRegEx->setChecked(toggle);
}

void Qtilities::CoreGui::SearchBoxWidget::setEditorFocus() {
    ui->txtSearchString->setFocus();
}


void Qtilities::CoreGui::SearchBoxWidget::setButtonFlags(ButtonFlags button_flags) {
    d->button_flags = button_flags;
    // Shows buttons according to button flags
    if (d->button_flags & NextButtons) {
        ui->btnFindNext->show();
        ui->btnReplaceNext->show();
    } else {
        ui->btnFindNext->hide();
        ui->btnReplaceNext->hide();
    }
    if (d->button_flags & PreviousButtons) {
        ui->btnFindPrevious->show();
        ui->btnReplacePrevious->show();
    } else {
        ui->btnFindPrevious->hide();
        ui->btnReplacePrevious->hide();
    }
    if (d->button_flags & HideButton) {
        ui->btnClose->show();
    } else {
        ui->btnClose->hide();
    }
}

Qtilities::CoreGui::SearchBoxWidget::ButtonFlags Qtilities::CoreGui::SearchBoxWidget::buttonFlags() const {
    return d->button_flags;
}

void Qtilities::CoreGui::SearchBoxWidget::setSearchOptions(SearchOptions search_options) {
    d->search_options = search_options;
    if (d->search_options & CaseSensitive) {
        d->searchOptionCaseSensitive->setVisible(true);
        d->searchOptionCaseSensitive->setChecked(true);
    } else {
        d->searchOptionCaseSensitive->setVisible(false);
    }

    if (d->search_options & WholeWordsOnly) {
        d->searchOptionWholeWordsOnly->setVisible(true);
        d->searchOptionWholeWordsOnly->setChecked(true);
    } else {
        d->searchOptionWholeWordsOnly->setVisible(false);
    }

    if (d->search_options & RegEx) {
        d->searchOptionRegEx->setVisible(true);
        d->searchOptionRegEx->setChecked(true);
    } else {
        d->searchOptionRegEx->setVisible(false);
    }
}

Qtilities::CoreGui::SearchBoxWidget::SearchOptions Qtilities::CoreGui::SearchBoxWidget::searchOptions() const {
    return d->search_options;
}

void Qtilities::CoreGui::SearchBoxWidget::setWidgetMode(WidgetMode widget_mode) {
    d->widget_mode = widget_mode;
    // Show contents according to selected mode
    if (d->widget_mode == SearchOnly) {
        ui->widgetReplace->hide();
        setFixedHeight(24);
    } else if (d->widget_mode == SearchAndReplace) {
        ui->widgetReplace->show();
        setFixedHeight(52);
    }
}

Qtilities::CoreGui::SearchBoxWidget::WidgetMode Qtilities::CoreGui::SearchBoxWidget::widgetMode() const {
    return d->widget_mode;
}
