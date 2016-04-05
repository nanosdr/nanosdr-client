/*
 * Nanosdr Client
 *
 * Copyright (c) 2016 Alexandru Csete OZ9AEC.
 * All rights reserved.
 *
 * This software is licensed under the terms and conditions of the
 * Simplified BSD License, see legal/license-bsd.txt for details.
 */
#pragma once

#include <QMainWindow>

#include "network/sdr_if.h"
#include "server_dialog.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit    MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void        newSdrifState(sdrif_state_t new_state);

private slots:
    void        on_actionNew_triggered();
    void        on_actionStartSdr_triggered();
    void        on_actionAbout_triggered();
    void        on_actionAboutQt_triggered();

    void        serverDialog_accepted();

private:
    Ui::MainWindow *ui;

    SdrIf          *sdr_if;
    ServerDialog   *serverDialog;
};
