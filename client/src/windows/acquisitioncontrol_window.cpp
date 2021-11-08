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
        emit workingChanged(true);

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

        // BUG : Only one task can be executed at a given time due to API restrictions with channels for now
        DCS::DAQ::ChannelRef ref = DCS::DAQ::ChannelRef::Default; // TODO : Allow the user to choose this value

        unsigned char buffer[4096];
        auto size = DCS::Registry::SVParams::GetDataFromParams(buffer, SV_CALL_DCS_DAQ_GetADCMaxInternalClock);
        auto max_clk_data = DCS::Network::Message::SendSync(DCS::Network::Message::Operation::REQUEST, buffer, size);
        DCS::f64 max_clk = *(DCS::f64*)max_clk_data.ptr;
        double rate = max_clk; // TODO : Allow user to select rate

        for(auto task : channels.keys())
        {
            int ch = channels[task];
            QString channel_name = "PXI_Slot2/ai" + QString::number(ch);
            QString task_name = "Task_" + task;
            DCS::Utils::BasicString tname;
            DCS::Utils::BasicString cname;
            memcpy(tname.buffer, task_name.toStdString().c_str(), task_name.toStdString().size() + 1);
            memcpy(cname.buffer, channel_name.toStdString().c_str(), channel_name.toStdString().size() + 1);

            DCS::DAQ::ChannelLimits lim; // TODO : Allow the user to choose this value
            lim.min = -10.0;
            lim.max =  10.0;

            size = DCS::Registry::SVParams::GetDataFromParams(buffer, SV_CALL_DCS_DAQ_NewAIVChannel, tname, cname, ref, lim);
            DCS::Network::Message::SendSync(DCS::Network::Message::Operation::REQUEST, buffer, size);

            if(task == "DCS")
            {
                size = DCS::Registry::SetupEvent(buffer, SV_EVT_DCS_DAQ_DCSCountEvent, [](DCS::u8* data, DCS::u8* userData) {
                    
                    LOG_DEBUG("GOT DCS Event");

                }, nullptr);
                DCS::Network::Message::SendAsync(DCS::Network::Message::Operation::EVT_SUB, buffer, size);
            }
            else if(task == "MCA")
            {
                size = DCS::Registry::SetupEvent(buffer, SV_EVT_DCS_DAQ_MCACountEvent, [](DCS::u8* data, DCS::u8* userData) {
                    emit ((AcquisitionControlWindow*)userData)->eventMCA(*(DCS::DAQ::MCACountEventData*)data);
                }, (DCS::u8*)this);
                DCS::Network::Message::SendAsync(DCS::Network::Message::Operation::EVT_SUB, buffer, size);
            }
        }
        
        size = DCS::Registry::SVParams::GetDataFromParams(buffer, SV_CALL_DCS_DAQ_StartAIAcquisition, rate);
        DCS::Network::Message::SendAsync(DCS::Network::Message::Operation::REQUEST, buffer, size);

    });

    (void)connect(ui->pushButton_2, &QPushButton::clicked, this, [&]() {
        ui->pushButton_2->setToolTip("No Task running.");
        ui->pushButton_3->setToolTip("No Task running.");
        ui->pushButton_2->setEnabled(false);
        ui->pushButton_3->setEnabled(false);

        working = false;
        emit workingChanged(false);

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

        unsigned char buffer[4096];
        auto size = DCS::Registry::SVParams::GetDataFromParams(buffer, SV_CALL_DCS_DAQ_StopAIAcquisition);
        DCS::Network::Message::SendSync(DCS::Network::Message::Operation::REQUEST, buffer, size);

        for(auto task : channels.keys())
        {
            int ch = channels[task];
            QString task_name = "Task_" + task;
            DCS::Utils::BasicString tname;
            memcpy(tname.buffer, task_name.toStdString().c_str(), task_name.toStdString().size() + 1);

            size = DCS::Registry::SVParams::GetDataFromParams(buffer, SV_CALL_DCS_DAQ_DeleteAIVChannel, tname);
            DCS::Network::Message::SendSync(DCS::Network::Message::Operation::REQUEST, buffer, size);

            if(task == "DCS")
            {
                size = DCS::Registry::RemoveEvent(buffer, SV_EVT_DCS_DAQ_DCSCountEvent);
                DCS::Network::Message::SendAsync(DCS::Network::Message::Operation::EVT_UNSUB, buffer, size);
            }
            else if(task == "MCA")
            {
                size = DCS::Registry::RemoveEvent(buffer, SV_EVT_DCS_DAQ_MCACountEvent);
                DCS::Network::Message::SendAsync(DCS::Network::Message::Operation::EVT_UNSUB, buffer, size);
            }
        }
        
    });

    (void)connect(this, &AcquisitionControlWindow::workingChanged, sdw, &SpectralDisplayWindow::disableWhenWorking);
}

AcquisitionControlWindow::~AcquisitionControlWindow()
{
    
}

