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

#include "GenericPropertyPathEditorListWrapper.h"
#include "ui_GenericPropertyPathEditorListWrapper.h"

#ifdef QTILITIES_PROPERTY_BROWSER

namespace Qtilities {
namespace CoreGui {

GenericPropertyPathEditorListWrapper::GenericPropertyPathEditorListWrapper(StringListWidget* list_widget, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GenericPropertyPathEditorListWrapper)
{
    ui->setupUi(this);

    if (ui->listHolder->layout())
        delete ui->listHolder->layout();

    QHBoxLayout* layout = new QHBoxLayout(ui->listHolder);
    layout->setMargin(0);
    layout->addWidget(list_widget);
}

GenericPropertyPathEditorListWrapper::~GenericPropertyPathEditorListWrapper() {
    delete ui;
}

void GenericPropertyPathEditorListWrapper::on_buttonBox_accepted() {
    QDialog::accept();
}

void GenericPropertyPathEditorListWrapper::on_buttonBox_rejected() {
    QDialog::reject();
}

}
}

#endif
