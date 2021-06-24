#pragma once

#include <QThread>
#include <functional>

#include "../splash/loading.h"


class SimpleThread : public QThread
{
    Q_OBJECT
public:
    SimpleThread(std::function<void()> worker, QObject * parent = nullptr) : QThread(parent)
    {
        w = worker;
    }

private:
    void run() override
    {
        w();
        emit jobDone();
    }
signals:
    void jobDone();

private:
    std::function<void()> w;
};