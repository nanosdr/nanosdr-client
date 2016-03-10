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
#include <QMessageBox>

#include "server_dialog.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    serverDialog(0)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete serverDialog;
    delete ui;
}

/* Menu item: SDR -> New connection */
void MainWindow::on_actionConnect_triggered()
{
    if (!serverDialog)
    {
        serverDialog = new ServerDialog(this);
        connect(serverDialog, SIGNAL(accepted()), this,
                SLOT(serverDialog_accepted()));
    }

    serverDialog->show();
    serverDialog->raise();
    serverDialog->activateWindow();
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this, tr("About Nanosdr Client"),
        tr("<p>This is Nanosdr Client 0.0</p>"
          "<p>Copyright (c) 2016 Alexandru Csete OZ9AEC.</p>"));
}

void MainWindow::on_actionAboutQt_triggered()
{
    QMessageBox::aboutQt(this, tr("About Qt"));
}

/* The server dialog has been closed with "OK" button */
void MainWindow::serverDialog_accepted()
{
    qDebug() << __func__;
}
