#pragma once

#include "../autogen_macros.h"

#include <QPushButton>
#include <QWidget>
#include <QComboBox>
#include <QLineEdit>
#include <QLabel>

#include "./ui_newchannel_dialog.h"
#include "../windows/connect_window.h"
#include "../windows/main_window.h"
#include "../windows/channelmanager_window.h"
#include <DCS_Network/include/DCS_ModuleNetwork.h>

UI_AUTOGEN(NewChannelDialog)

class NewChannelDialog : public QDialog
{
	Q_OBJECT

public:
	NewChannelDialog(ChannelManagerWindow* cmw, QWidget* parent) : QDialog(parent), ui(new Ui::NewChannelDialog), cmw(cmw)
    {
        ui->setupUi(this);
        (void)connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(newChannel()));

        auto connect_window = dynamic_cast<MainWindow*>(parent)->GetWindow<ConnectWindow>("Remote Control");
        (void)connect(connect_window, &ConnectWindow::connectionChanged, this, [&](bool status) {
            if(status)
            {
                unsigned char buffer[4096];
                auto size = DCS::Registry::SVParams::GetDataFromParams(buffer, SV_CALL_DCS_DAQ_GetConnectedDevicesAliases);
                auto devices_ptr = DCS::Network::Message::SendSync(DCS::Network::Message::Operation::REQUEST, buffer, size);

                ui->comboBox->clear();

                if(devices_ptr.ptr[0] == '\0')
                {
                    LOG_ERROR("No NI devices detected upon connecting to server.");
                    ui->comboBox->addItem("No Devices");
                    ui->comboBox->setEnabled(false);
                    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
                    ui->lineEdit->setEnabled(false);
                    ui->groupBox_2->setEnabled(false);
                    ui->groupBox->setEnabled(false);
                }
                else
                {
                    QString devices(((DCS::Utils::BasicString*)(devices_ptr.ptr))->buffer);
                    ui->comboBox->addItems(devices.split(','));
                    ui->comboBox->setEnabled(true);
                    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
                    ui->lineEdit->setEnabled(true);
                    ui->groupBox_2->setEnabled(true);
                    ui->groupBox->setEnabled(true);
                }
            }
            else
            {
                ui->comboBox->addItem("No Connection");
                ui->comboBox->setEnabled(false);
                ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
                ui->lineEdit->setEnabled(false);
                ui->groupBox_2->setEnabled(false);
                ui->groupBox->setEnabled(false);
            }
        }, Qt::QueuedConnection);
    }

public slots:
    void newChannel()
    {
        emit cmw->pushRowSig({
            ui->lineEdit_2->text(),
            "/" + ui->comboBox->currentText() + "/" + ui->lineEdit->text(),
            ui->radioButton_4->isChecked() ? "Input" : "Output",
            ui->radioButton->isChecked()    ? "Analog"    :
            (ui->radioButton_2->isChecked() ? "Digital"   :
            (ui->radioButton_3->isChecked() ? "Counter"   :
            (ui->radioButton_7->isChecked() ? "Generator" :
                                              "Internal"))),
            ui->groupBox_3->isEnabled() ? QString::number(ui->spinBox->value()) : "N/A"
        });
    }

signals:
    void newChannelSignal();

private:
	Ui::NewChannelDialog* ui;
    ChannelManagerWindow* cmw;
};
