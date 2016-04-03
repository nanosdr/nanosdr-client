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
#include <QString>
#include <QStringList>

#include "network/sdr_if.h"
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
    ui->serverCombo->addItem("localhost:42000");
}

ServerDialog::~ServerDialog()
{
    delete ui;
}

/* Test connection to server */
void ServerDialog::on_testButton_clicked()
{
    SdrIf       test_if;

    test_if.setup(SDRIF_NANOSDR, getHost(), getPort());
    if (test_if.testInterface())
        qDebug("Interface test SUCCESS!");
    else
        qDebug("Interface test ERROR!");
/*
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
*/
}

QString ServerDialog::getHost() const
{
    QStringList     list = ui->serverCombo->currentText().split(':');

    if (list.size() != 2)
        return 0;

    return list.at(0);
}

quint16 ServerDialog::getPort() const
{
    QStringList     list = ui->serverCombo->currentText().split(':');

    if (list.size() != 2)
        return 0;

    bool        conv_ok;
    quint16     port = list.at(1).toUInt(&conv_ok, 10);

    return conv_ok ? port : 0;
}
