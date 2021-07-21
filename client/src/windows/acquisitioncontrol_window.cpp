#include "acquisitioncontrol_window.h"
#include "./ui_acquisitioncontrol_window.h"

AcquisitionControlWindow::AcquisitionControlWindow(QWidget* parent) : ui(new Ui::AcquisitionControlWindow)
{
    ui->setupUi(this);

    for(int i = 0; i < ui->listWidget->count(); i++)
    {
        QPixmap pixmap(15, 15);
        pixmap.fill(QColor("red"));
        ui->listWidget->item(i)->setIcon(QIcon(pixmap)); // Set all tasks as disabled on start (icons)
    }

    auto sdw = dynamic_cast<MainWindow*>(parent)->GetWindow<SpectralDisplayWindow>("Acquisition Details");
    auto connect_window = dynamic_cast<MainWindow*>(parent)->GetWindow<ConnectWindow>("Remote Control");
    

    (void)connect(connect_window, &ConnectWindow::connectionChanged, this, [&](bool status) {
        local_net_status = status;

        bool enable_start = !channels.isEmpty() && local_net_status && !working;
        if(enable_start)
        {
            ui->pushButton->setToolTip("Start acquiring data.");
            ui->lineEdit->setText("Ready.");
            ui->lineEdit->setStyleSheet("color: aqua;");
            
        }
        else
        {
            if(channels.isEmpty() && local_net_status)
            {
                ui->pushButton->setToolTip("No Task to run.");
                ui->lineEdit->setText("No Task to run.");
                ui->lineEdit->setStyleSheet("color: rgb(0, 0, 255);");
            }
            else
            {
                ui->pushButton->setToolTip("Not connected.");
                ui->lineEdit->setText("Not connected.");
                ui->lineEdit->setStyleSheet("color: rgb(255, 0, 0);");
            }
        }
        ui->pushButton->setEnabled(enable_start);

    }, Qt::QueuedConnection);

    (void)connect(sdw, &SpectralDisplayWindow::acquisitionEnableScheduleChanged, this, [&](const QString& task, int channel, bool status) {
        if(status)
        {
            QPixmap pixmap(15, 15);
            pixmap.fill(QColor("aqua"));
            auto item = ui->listWidget->item(channel);
            item->setIcon(QIcon(pixmap));
            item->setText("ai" + QString::number(channel) + " (" + task + " Task) [Scheduled]");
            channels.insert(task, channel);
        }
        else
        {
            QPixmap pixmap(15, 15);
            pixmap.fill(QColor("red"));
            auto item = ui->listWidget->item(channels[task]);
            item->setIcon(QIcon(pixmap));
            item->setText("ai" + QString::number(channels[task]) + " (No Task)");
            channels.remove(task);
        }

        bool enable_start = !channels.isEmpty() && local_net_status && !working;
        if(enable_start)
        {
            ui->pushButton->setToolTip("Start acquiring data.");
            ui->lineEdit->setText("Ready.");
            ui->lineEdit->setStyleSheet("color: aqua;");
        }
        else
        {
            if(channels.isEmpty() && local_net_status)
            {
                ui->pushButton->setToolTip("No Task to run.");
                ui->lineEdit->setText("No Task to run.");
                ui->lineEdit->setStyleSheet("color: rgb(0, 0, 255);");
            }
            else
            {
                ui->pushButton->setToolTip("Not connected.");
                ui->lineEdit->setText("Not connected.");
                ui->lineEdit->setStyleSheet("color: rgb(255, 0, 0);");
            }
        }
        ui->pushButton->setEnabled(enable_start);
    });

    ui->pushButton->setEnabled(false);
    ui->pushButton_2->setEnabled(false);
    ui->pushButton_3->setEnabled(false);

    ui->pushButton->setToolTip("No Task to run.");
    ui->pushButton_2->setToolTip("No Task running.");
    ui->pushButton_3->setToolTip("No Task running.");
    ui->pushButton_4->setToolTip("(!Work in progress!)");

    (void)connect(ui->pushButton, &QPushButton::clicked, this, [&]() {
        ui->pushButton_2->setToolTip("");
        ui->pushButton_3->setToolTip("");
        ui->pushButton_2->setEnabled(true);
        ui->pushButton_3->setEnabled(true);

        working = true;
        ui->pushButton->setEnabled(false);
        ui->lineEdit->setText("Working.");
        ui->lineEdit->setStyleSheet("color: green;");

        for(auto task : channels.keys())
        {
            int ch = channels[task];
            QPixmap pixmap(15, 15);
            pixmap.fill(QColor("green"));
            auto item = ui->listWidget->item(ch);
            item->setIcon(QIcon(pixmap));
            item->setText("ai" + QString::number(ch) + " (" + task + " Task) [Working]");
        }

        // TODO : Flag acquisition start to the backend with the tasks inside the channels map.
    });

    (void)connect(ui->pushButton_2, &QPushButton::clicked, this, [&]() {
        ui->pushButton_2->setToolTip("No Task running.");
        ui->pushButton_3->setToolTip("No Task running.");
        ui->pushButton_2->setEnabled(false);
        ui->pushButton_3->setEnabled(false);

        working = false;
        ui->pushButton->setEnabled(true);
        ui->lineEdit->setText("Ready.");
        ui->lineEdit->setStyleSheet("color: aqua;");

        for(auto task : channels.keys())
        {
            int ch = channels[task];
            QPixmap pixmap(15, 15);
            pixmap.fill(QColor("aqua"));
            auto item = ui->listWidget->item(ch);
            item->setIcon(QIcon(pixmap));
            item->setText("ai" + QString::number(ch) + " (" + task + " Task) [Scheduled]");
        }

        // TODO : Flag acquisition stop to the backend.
    });
}

AcquisitionControlWindow::~AcquisitionControlWindow()
{
    
}

