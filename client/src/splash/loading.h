#pragma once

#include "../autogen_macros.h"

#include <QWidget>
#include <QMovie>
#include <QSplashScreen>
#include <QLayout>
#include <QLabel>
#include <QFont>
#include <QWindow>

#include <DCS_Utils/include/DCS_ModuleUtils.h>

class LoadingSplash : public QSplashScreen
{
	Q_OBJECT

private:
	LoadingSplash(QWidget* parent = nullptr) : QSplashScreen(parent->window()->windowHandle()->screen())
    {
        (void)connect(this, &LoadingSplash::changeDisplayMessage, this, &LoadingSplash::setWorkingStatus);

        setWindowModality(Qt::WindowModal);
        this->installEventFilter(this);

        QPixmap pixmap(":/png/splash.png");
        setPixmap(pixmap);
        
        auto layout = new QVBoxLayout();
        setLayout(layout);

        QFont f_title("Arial", 15, QFont::Bold);
        QFont f_status("Arial", 10);


        QMovie* movie = new QMovie(":/gif/loader.gif");
        QLabel* processLabel = new QLabel(this);
        processLabel->setMovie(movie);

        QLabel* title = new QLabel("Hang Tight", this);
        title->setFont(f_title);
        layout->addWidget(title);
        layout->setAlignment(title, Qt::AlignHCenter);

        status = new QLabel("", this);
        status->setFont(f_status);
        layout->addWidget(status);
        layout->setAlignment(status, Qt::AlignHCenter);

        layout->addWidget(processLabel);
        layout->setAlignment(processLabel, Qt::AlignHCenter);

        movie->start();
    }

    ~LoadingSplash()
    {
        this->removeEventFilter(this);
    }

public:
    static LoadingSplash* Start(QWidget* parent = nullptr)
    {
        if(ls == nullptr)
        {
            ls = new LoadingSplash(parent);
            ls->show();
            return ls;
        }
        else
        {
            LOG_WARNING("Trying to display LoadingSplash multiple times is not allowed.");
            return nullptr;
        }
    }

public slots:
    void setWorkingStatus(const QString& value)
    {
        if(ls)
            ls->status->setText(value);
    }

    void finishExec()
    {
        if(ls)
        {
            ls->close();
            delete ls;
            ls = nullptr;
        }
    }

signals:
    void changeDisplayMessage(QString);

protected:
    bool eventFilter(QObject *target, QEvent *event) override
    {
        if(event->type() == QEvent::MouseButtonPress || event->type() == QEvent::MouseButtonDblClick)
            return true;
        return false;
    }

private:
    inline static LoadingSplash* ls = nullptr;
    QLabel* status;
};
