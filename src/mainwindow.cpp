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

#include "network/sdr_if.h"
#include "server_dialog.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    serverDialog(0)
{
    ui->setupUi(this);

    sdr_if = new SdrIf();
    connect(sdr_if, SIGNAL(sdrifStateChanged(sdrif_state_t)),
            this, SLOT(newSdrifState(sdrif_state_t)));

    this->statusBar()->showMessage(tr("Idle"));
}

MainWindow::~MainWindow()
{
    delete sdr_if;
    delete serverDialog;
    delete ui;
}

void MainWindow::newSdrifState(sdrif_state_t new_state)
{
    QString     message;

    switch (new_state)
    {
    case SDRIF_ST_IDLE:
        message = tr("Idle");
        break;
    case SDRIF_ST_ERROR:
        message = tr("Interface error");
        break;
    case SDRIF_ST_CONNECTING:
        message = tr("Connecting to server...");
        break;
    case SDRIF_ST_CONNECTED:
        message = tr("Connected");
        ui->actionStartSdr->setText(tr("Stop SDR"));
        ui->actionStartSdr->setToolTip(tr("Stop streaming from the SDR server"));
        break;
    case SDRIF_ST_DISCONNECTING:
        message = tr("Disconnecting from server...");
        break;
    case SDRIF_ST_DISCONNECTED:
        message = tr("Disconnected");
        ui->actionStartSdr->setText(tr("Start SDR"));
        ui->actionStartSdr->setToolTip(tr("Start streaming from the SDR server"));
        break;
    default:
        break;
    }

    // FIXME: Use a permanent widget instead
    if (!message.isEmpty())
        this->statusBar()->showMessage(message);
}

/* Menu item: SDR -> New connection */
void MainWindow::on_actionNew_triggered()
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

void MainWindow::on_actionStartSdr_triggered()
{
    if (sdr_if->state() == SDRIF_ST_CONNECTED)
        sdr_if->stopInterface();
    else
        sdr_if->startInterface();
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

/* The server dialog has been closed with "OK" button.
 * Setup network interface using current settings.
 */
void MainWindow::serverDialog_accepted()
{
    QString     host = serverDialog->getHost();
    quint16     port = serverDialog->getPort();
    quint8      type = serverDialog->getType();
    int         retval;

    qDebug() << __func__
             << "\n    Server type:" << type
             << "\n    Server host:" << host
             << "\n    Server port:" << port;

    retval = sdr_if->setup(type, host, port);
    if (retval == SDRIF_OK)
        return;

    // show an error dialog and reopen server dialog
    QString         message;

    if (retval == SDRIF_ETYPE)
        message = tr("Invalid host type %1. This is a bug.").arg(type);
    else if (retval == SDRIF_EHOST)
        message = tr("Invalid host name or IP address: %1").
                arg(host.isEmpty() ? "<null>" : host);
    else if (retval == SDRIF_EPORT)
        message = tr("Invalid port number: %1").arg(port);
    else
        message = tr("An unknown error has occurred. Please review the server settings.");

    QMessageBox::critical(this, tr("Server setup error"), message,
                          QMessageBox::Ok);

    on_actionNew_triggered();
}
