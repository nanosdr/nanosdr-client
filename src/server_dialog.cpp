/*
 * Nanosdr Client
 *
 * Copyright (c) 2016 Alexandru Csete OZ9AEC.
 * All rights reserved.
 *
 * This software is licensed under the terms and conditions of the
 * Simplified BSD License, see legal/license-bsd.txt for details.
 */
#include "server_dialog.h"
#include "ui_server_dialog.h"

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
