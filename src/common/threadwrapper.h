/*
 * Nanosdr Client
 *
 * Copyright (c) 2013 Moe Wheatley
 * Copyright (c) 2016 Alexandru Csete OZ9AEC.
 * All rights reserved.
 *
 * This software is licensed under the terms and conditions of the
 * Simplified BSD License, see legal/license-bsd.txt for details.
 */
#pragma once

#include <QObject>
#include <QThread>

/**
 * @brief Thread wrapper class.
 *
 * This base class is used to create classes that need their own event loop
 * running in their own worker thread.
 *
 * The init() function is called by the worker thread when started to setup
 * all signal-slot connections.
 *
 * The ThreadExit() function is called by the thread after CleanupThread() is
 * called. This can be used in cases where the worker thread must delete its own
 * resources.
 */
class ThreadWrapper : public QObject
{
	Q_OBJECT

public:
    ThreadWrapper()
	{
        tw_thread = new QThread(this);
        this->moveToThread(tw_thread);
        connect(tw_thread, SIGNAL(started()), this, SLOT(threadInit()));
        connect(this, SIGNAL(threadExitSignal()), this, SLOT(threadExit()));
        tw_thread->start();
	}

    virtual     ~ThreadWrapper()
	{
        tw_thread->exit();
        tw_thread->wait();
        delete tw_thread;
	}

    void        cleanupThread()
    {
        emit threadExitSignal();
        tw_thread->wait(10);
    }

signals:
    void        threadExitSignal();

public slots:
    // derived classes must override these
    virtual void    threadInit() = 0;	// Called by new thread when started
    virtual void    threadExit() = 0;

protected:
    QThread     *tw_thread;
};

