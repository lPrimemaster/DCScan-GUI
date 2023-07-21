#include "acquisitioncontrol_window.h"
#include "./ui_acquisitioncontrol_window.h"

#include "../widgets/dnd_outputconfig.h"

AcquisitionControlWindow::AcquisitionControlWindow(QWidget* parent) : ui(new Ui::AcquisitionControlWindow)
{
    ui->setupUi(this);

    cmw = dynamic_cast<MainWindow*>(parent)->GetWindow<ChannelManagerWindow>("Channel Manager");
    auto connect_window = dynamic_cast<MainWindow*>(parent)->GetWindow<ConnectWindow>("Remote Control");

    (void)connect(connect_window, &ConnectWindow::connectionChanged, this, [&](bool status) {
        if(status)
        {
            if(currentEventValid())
            {
                ui->lineEdit->setText("Ready.");
                ui->lineEdit->setStyleSheet("color: green;");
                ui->pushButton->setEnabled(true);
            }
            else
            {
                ui->lineEdit->setText("Invalid detection mode event configuration.");
                ui->lineEdit->setStyleSheet("color: orange;");
                ui->pushButton->setEnabled(false);
            }
            ui->pushButton_2->setEnabled(false);
            ui->pushButton_3->setEnabled(false);
        }
        else
        {
            ui->lineEdit->setText("No Connection.");
            ui->lineEdit->setStyleSheet("color: red;");
            ui->pushButton->setEnabled(false);
            ui->pushButton_2->setEnabled(false);
            ui->pushButton_3->setEnabled(false);
        }

    }, Qt::QueuedConnection);

    (void)connect(cmw, &ChannelManagerWindow::eventValueChanged, this, [this](){
        if(currentEventValid())
        {
            ui->lineEdit->setText("Ready.");
            ui->lineEdit->setStyleSheet("color: green;");
            ui->pushButton->setEnabled(true);
        }
        else
        {
            ui->lineEdit->setText("Invalid detection mode event configuration.");
            ui->lineEdit->setStyleSheet("color: orange;");
            ui->pushButton->setEnabled(false);
        }
        ui->pushButton_2->setEnabled(false);
        ui->pushButton_3->setEnabled(false);
    });

    (void)connect(ui->radioButton_2, &QRadioButton::toggled, this, [this](){
        if(currentEventValid())
        {
            ui->lineEdit->setText("Ready.");
            ui->lineEdit->setStyleSheet("color: green;");
            ui->pushButton->setEnabled(true);
        }
        else
        {
            ui->lineEdit->setText("Invalid detection mode event configuration.");
            ui->lineEdit->setStyleSheet("color: orange;");
            ui->pushButton->setEnabled(false);
        }
        ui->pushButton_2->setEnabled(false);
        ui->pushButton_3->setEnabled(false);
    });

    (void)connect(ui->pushButton, &QPushButton::clicked, this, [this]() {
        cmw->setEnabled(false);
        ui->lineEdit->setText("Running.");
        ui->lineEdit->setStyleSheet("color: blue;");
        ui->pushButton->setEnabled(true);
        ui->groupBox->setEnabled(false);
        ui->groupBox_2->setEnabled(false);

        if(ui->radioButton_2->isChecked()) // Use counter events to trigger everything
        {
            DCS::Utils::BasicString ci_hardware_name;
            strcpy(ci_hardware_name.buffer, cmw->getCounterTimebaseHardwareChannel().toStdString().c_str());
            unsigned char buffer[4096];
            auto size = DCS::Registry::SVParams::GetDataFromParams(buffer,
                SV_CALL_DCS_DAQ_StartCIAcquisition,
                ci_hardware_name,
                DCS::Utils::BasicString{ "NONE" },
                DCS::Utils::BasicString{ "NONE" },
                cmw->getCounterTimebaseRate()
            );
            DCS::Network::Message::SendSync(DCS::Network::Message::Operation::REQUEST, buffer, size);

            size = DCS::Registry::SetupEvent(buffer, SV_EVT_DCS_DAQ_DCSCountEvent, [](DCS::u8* data, DCS::u8* userData) {
                DCS::DAQ::EventData* edata = (DCS::DAQ::EventData*)data;
                AcquisitionControlWindow* window = (AcquisitionControlWindow*)userData;
                
                LOG_DEBUG("counts: %d", edata->counts.num_detected);
                LOG_DEBUG("c1 ange: %lf", edata->angle_c1);

            }, (DCS::u8*)this);
            DCS::Network::Message::SendAsync(DCS::Network::Message::Operation::EVT_SUB, buffer, size);
        }
        else // Use voltage events to trigger everything
        {
            DCS::Utils::BasicString ai_hardware_name;
            strcpy(ai_hardware_name.buffer, cmw->getVoltageTimebaseHardwareChannel().toStdString().c_str());
            unsigned char buffer[4096];
            auto size = DCS::Registry::SVParams::GetDataFromParams(buffer,
                SV_CALL_DCS_DAQ_StartAIAcquisition,
                ai_hardware_name,
                cmw->getVoltageTimebaseRate()
            );
            DCS::Network::Message::SendSync(DCS::Network::Message::Operation::REQUEST, buffer, size);
        }

        ui->pushButton->setEnabled(false);
        ui->pushButton_2->setEnabled(true);

        // TODO : Add callbacks
    });

    (void)connect(ui->pushButton_2, &QPushButton::clicked, this, [this]() {
        cmw->setEnabled(true);
        ui->lineEdit->setText("Ready.");
        ui->lineEdit->setStyleSheet("color: green;");
        ui->pushButton->setEnabled(true);
        ui->groupBox->setEnabled(true);
        ui->groupBox_2->setEnabled(true);

        if(ui->radioButton_2->isChecked())
        {
            unsigned char buffer[4096];
            auto size = DCS::Registry::SVParams::GetDataFromParams(buffer, SV_CALL_DCS_DAQ_StopCIAcquisition);
            DCS::Network::Message::SendSync(DCS::Network::Message::Operation::REQUEST, buffer, size);

            size = DCS::Registry::RemoveEvent(buffer, SV_EVT_DCS_DAQ_DCSCountEvent);
            DCS::Network::Message::SendAsync(DCS::Network::Message::Operation::EVT_UNSUB, buffer, size);
        }
        else
        {
            unsigned char buffer[4096];
            auto size = DCS::Registry::SVParams::GetDataFromParams(buffer, SV_CALL_DCS_DAQ_StopAIAcquisition);
            DCS::Network::Message::SendSync(DCS::Network::Message::Operation::REQUEST, buffer, size);
        }

        ui->pushButton->setEnabled(true);
        ui->pushButton_2->setEnabled(false);

        // TODO : Remove callbacks
    });

    (void)connect(ui->pushButton_5, &QPushButton::clicked, this, &AcquisitionControlWindow::addNewListModeField);
}

AcquisitionControlWindow::~AcquisitionControlWindow()
{
    
}

void AcquisitionControlWindow::addNewListModeField()
{
    DragNDropOutputConfig* item = new DragNDropOutputConfig(this);
    item->setComboOptions(QList<QString>(AVAILABLE_OUT_VARS.cbegin(), AVAILABLE_OUT_VARS.cend()));
    (void)connect(item->getButtonDel(),  &QPushButton::clicked, this, [this, item]() { deleteListModeField(item); });
    (void)connect(item->getButtonUp(),   &QPushButton::clicked, this, [this, item]() { swapField(item, -1); });
    (void)connect(item->getButtonDown(), &QPushButton::clicked, this, [this, item]() { swapField(item, +1); });
    QVBoxLayout* layout = dynamic_cast<QVBoxLayout*>(ui->scrollAreaWidgetContents->layout());
    item->setPropertyLabel(QString("Column ") + QString::number(layout->count() - 1));
    layout->insertWidget(layout->count() - 1, item); // -1 for the end size
}

void AcquisitionControlWindow::deleteListModeField(DragNDropOutputConfig* item)
{
    QVBoxLayout* layout = dynamic_cast<QVBoxLayout*>(ui->scrollAreaWidgetContents->layout());
    int index = layout->indexOf(item);
    QLayoutItem* layout_item = layout->itemAt(index);
    layout->removeItem(layout_item);
    delete layout_item;
    item->deleteLater();

    // Recalculate indices below
    for(int i = index; i < layout->count() - 1; i++)
    {
        dynamic_cast<DragNDropOutputConfig*>(layout->itemAt(i)->widget())->setPropertyLabel(QString("Column ") + QString::number(i));
    }
}

void AcquisitionControlWindow::swapField(DragNDropOutputConfig* item, int direction)
{
    QVBoxLayout* layout = dynamic_cast<QVBoxLayout*>(ui->scrollAreaWidgetContents->layout());
    int index = layout->indexOf(item);
    int other_index = index + direction;

    if(other_index >= 0 && other_index < layout->count() - 1) // -1 for the spacer from the ui as the last item
    {
        QLayoutItem* litem_0 = layout->itemAt(index);
        QLayoutItem* litem_1 = layout->itemAt(other_index);
        layout->removeItem(litem_0);
        layout->removeItem(litem_1);

        // Swap the column numbers
        item->setPropertyLabel(QString("Column ") + QString::number(other_index));
        dynamic_cast<DragNDropOutputConfig*>(litem_1->widget())->setPropertyLabel(QString("Column ") + QString::number(index));
        
        // Swap the values
        if(other_index < index)
        {
            layout->insertItem(other_index, litem_0);
            layout->insertItem(index, litem_1);
        }
        else
        {
            layout->insertItem(index, litem_1);
            layout->insertItem(other_index, litem_0);
        }
    }
}

bool AcquisitionControlWindow::currentEventValid()
{
    return ui->radioButton_2->isChecked() ? cmw->getCounterEventValid() : cmw->getVoltageEventValid();
}
