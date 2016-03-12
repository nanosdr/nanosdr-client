/*
 * Nanosdr Client
 *
 * Copyright (c) 2016 Alexandru Csete OZ9AEC.
 * All rights reserved.
 *
 * This software is licensed under the terms and conditions of the
 * Simplified BSD License, see legal/license-bsd.txt for details.
 */
#include <QDebug>

#include "server_dialog.h"
#include "ui_server_dialog.h"

#define SRV_IDX_NANOSDR     0
#define SRV_IDX_RFSPACE     1

ServerDialog::ServerDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ServerDialog)
{
    ui->setupUi(this);

    // add example to server combo
    ui->serverCombo->addItem("example: mysdr.domain.com:42000");
}

ServerDialog::~ServerDialog()
{
    delete ui;
}

/* Test connection to server */
void ServerDialog::on_testButton_clicked()
{
    int     type_idx = ui->serverType->currentIndex();

    if (type_idx == SRV_IDX_NANOSDR)
    {
        qDebug() << "Test connection: Nanosdr (not implemented)";
    }
    else if (type_idx == SRV_IDX_RFSPACE)
    {
        qDebug() << "Test connection: RFSpace (not implemented)";
    }
    else
    {
        qDebug() << "Unsupported server type";
    }
}
