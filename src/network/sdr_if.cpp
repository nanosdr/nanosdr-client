/*
 * Nanosdr Client
 *
 * Copyright (c) 2016 Alexandru Csete OZ9AEC.
 * All rights reserved.
 *
 * This software is licensed under the terms and conditions of the
 * Simplified BSD License, see legal/license-bsd.txt for details.
 */
#include <QThread>

#include "sdr_if.h"

SdrIf::SdrIf()
{
    worker_thread = new QThread(this);
    this->moveToThread(worker_thread);
    //connect(worker_thread, SIGNAL(started()), this, SLOT(threadStarted()));
    //connect(worker_thread, SIGNAL(finished()), this, SLOT(threadFinished()));
    worker_thread->start();
}

SdrIf::~SdrIf()
{
    worker_thread->quit();
    worker_thread->wait();      // FIXME: Use finite timeout?
    delete worker_thread;
}

int SdrIf::setup(quint8 iftype, QString host, quint16 port)
{
    if (iftype != SDR_IF_NANOSDR && iftype != SDR_IF_RFSPACE)
        return SDR_IF_EINVAL;
    if (host.isEmpty() || !port)
        return SDR_IF_EINVAL;


    return SDR_IF_OK;
}

int SdrIf::startInterface()
{

    return SDR_IF_OK;
}

int SdrIf::stopInterface()
{
    return SDR_IF_OK;
}
