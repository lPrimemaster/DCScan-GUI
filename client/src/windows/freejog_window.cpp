#include "freejog_window.h"
#include "./ui_freejog_window.h"

FreejogWindow::FreejogWindow(QWidget* parent) : ui(new Ui::FreejogWindow)
{
    auto connect_window = dynamic_cast<MainWindow*>(parent)->GetWindow<ConnectWindow>("Remote Control");
    (void)connect(connect_window, SIGNAL(connectionChanged(bool)), this, SLOT(enableFreejog(bool)), Qt::QueuedConnection);
    ui->setupUi(this);
    ui->warning_msg->setText("Free motion is disabled.\nConnect to server to enable.");
    enableFreejog(false);

    (void)connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(moveEngine1To()));
    (void)connect(ui->pushButton_2, SIGNAL(clicked()), this, SLOT(moveEngine2To()));

    (void)connect(this, SIGNAL(enableMotionSignal(int, bool)), this, SLOT(enableMotion(int, bool)));

    (void)connect(ui->spinBox, &QSpinBox::editingFinished, this, [=](){ moveTiltTo(0, 1, ui->spinBox); });
    (void)connect(ui->spinBox_2, &QSpinBox::editingFinished, this, [=](){ moveTiltTo(0, 2, ui->spinBox_2); });
    (void)connect(ui->spinBox_3, &QSpinBox::editingFinished, this, [=](){ moveTiltTo(1, 1, ui->spinBox_3); });
    (void)connect(ui->spinBox_4, &QSpinBox::editingFinished, this, [=](){ moveTiltTo(1, 2, ui->spinBox_4); });

    (void)connect(ui->doubleSpinBox_3, SIGNAL(editingFinished()), this, SLOT(updatePIDParams()));
    (void)connect(ui->doubleSpinBox_4, SIGNAL(editingFinished()), this, SLOT(updatePIDParams()));
    (void)connect(ui->doubleSpinBox_5, SIGNAL(editingFinished()), this, SLOT(updatePIDParams()));
}

FreejogWindow::~FreejogWindow()
{
    
}

void FreejogWindow::updatePIDParams()
{
    DCS::f64 Kp = ui->doubleSpinBox_3->value();
    DCS::f64 Ki = ui->doubleSpinBox_4->value();
    DCS::f64 Kd = ui->doubleSpinBox_5->value();

    unsigned char buffer[4096];
    auto size_written = DCS::Registry::SVParams::GetDataFromParams(buffer,
            SV_CALL_DCS_Control_SetPIDControlVariables,
            DCS::Control::UnitTarget::XPSRLD4,
            DCS::Utils::BasicString{ "Group4" },
            DCS::i8(2), Kp, Ki, Kd
        );
    DCS::Network::Message::SendSync(DCS::Network::Message::Operation::REQUEST, buffer, size_written);

    // size_written = DCS::Registry::SVParams::GetDataFromParams(buffer,
    //         SV_CALL_DCS_Control_SetPIDControlVariables,
    //         DCS::Control::UnitTarget::XPSRLD4,
    //         DCS::Utils::BasicString{ "Group2" },
    //         DCS::i8(4), Kp, Ki, Kd
    //     );
    // DCS::Network::Message::SendSync(DCS::Network::Message::Operation::REQUEST, buffer, size_written);
}

void FreejogWindow::moveEngine1To()
{
#if 0
    if(ui->doubleSpinBox_3->value() > 0.0f && ui->doubleSpinBox_4->value() > 0.0f)
    {
        QString cmd = "1VA" + QString::number(ui->doubleSpinBox_3->value()) + ";";
        unsigned char buffer[4096];

        cmd += "1PA" + QString::number(ui->doubleSpinBox->value()) + ";"; // Move to desired position

        DCS::Utils::BasicString str;
        memcpy(str.buffer, cmd.toLatin1().constData(), cmd.toLatin1().size());

        auto size_written = DCS::Registry::SVParams::GetDataFromParams(buffer,
            SV_CALL_DCS_Control_IssueGenericCommand,
            DCS::Control::UnitTarget::ESP301,
            str
        );

        DCS::Network::Message::SendAsync(DCS::Network::Message::Operation::REQUEST, buffer, size_written);
    }
    else
    {
        LOG_ERROR("Cannot move engine 1: acceleration or velocity value is zero.");
    }
#else
        unsigned char buffer[4096];

        auto size_written = DCS::Registry::SVParams::GetDataFromParams(buffer,
                SV_CALL_DCS_Control_MoveAbsolutePID,
                DCS::Control::UnitTarget::XPSRLD4,
                DCS::Utils::BasicString{ "Group4" },
                ui->doubleSpinBox->value()
            );
        DCS::Network::Message::SendAsync(DCS::Network::Message::Operation::REQUEST, buffer, size_written);
#endif
}

void FreejogWindow::moveEngine2To()
{
    if(ui->doubleSpinBox_3->value() > 0.0f && ui->doubleSpinBox_4->value() > 0.0f)
    {
#if 0
        QString cmd = "2VA" + QString::number(ui->doubleSpinBox_3->value()) + ";";
        unsigned char buffer[4096];

        cmd += "2PA" + QString::number(ui->doubleSpinBox->value()) + ";"; // Move to desired position

        DCS::Utils::BasicString str;
        memcpy(str.buffer, cmd.toLatin1().constData(), cmd.toLatin1().size());

        auto size_written = DCS::Registry::SVParams::GetDataFromParams(buffer,
            SV_CALL_DCS_Control_IssueGenericCommand,
            DCS::Control::UnitTarget::ESP301,
            str
        );
    
        DCS::Network::Message::SendAsync(DCS::Network::Message::Operation::REQUEST, buffer, size_written);
#else
        // unsigned char buffer[4096];

        // auto size_written = DCS::Registry::SVParams::GetDataFromParams(buffer,
        //         SV_CALL_DCS_Control_MoveAbsolutePID,
        //         DCS::Control::UnitTarget::XPSRLD4,
        //         DCS::Utils::BasicString{ "Group4" },
        //         ui->doubleSpinBox->value()
        //     );
        // DCS::Network::Message::SendAsync(DCS::Network::Message::Operation::REQUEST, buffer, size_written);
#endif
    }
    else
    {
        LOG_ERROR("Cannot move engine 2: acceleration or velocity value is zero.");
    }
}

void FreejogWindow::moveTiltTo(int stage, int axis, QSpinBox* spin_obj)
{
    QString cmd = QString::number(stage) + ">" + QString::number(axis) + "PA" + QString::number(spin_obj->value()) + ";";
    unsigned char buffer[4096];

    DCS::Utils::BasicString str;
    memcpy(str.buffer, cmd.toLatin1().constData(), cmd.toLatin1().size());

    auto size_written = DCS::Registry::SVParams::GetDataFromParams(buffer,
        SV_CALL_DCS_Control_IssueGenericCommand,
        DCS::Control::UnitTarget::PMC8742,
        str
    );

    DCS::Network::Message::SendAsync(DCS::Network::Message::Operation::REQUEST, buffer, size_written);
}

void FreejogWindow::enableMotion(int stage, bool enable)
{
    if(stage == 0)
    {
        ui->pushButton->setEnabled(enable);
        ui->pushButton->setText(enable ? "Move" : "Working");
    }
    else if(stage == 1)
    {
        ui->pushButton_2->setEnabled(enable);
        ui->pushButton_2->setText(enable ? "Move" : "Working");
    }
}

// This shall only be called if a connections is established, or an error will be raised
void FreejogWindow::enableFreejog(bool e)
{
    if(!e)
    {
        ui->warning_msg->setText("Free motion is disabled.\nConnect to server to enable.");
    }
    else
    {
#if 0
        // Get default values for engines acc and vel
        unsigned char buffer[1024];

        auto size_written = DCS::Registry::SVParams::GetDataFromParams(buffer,
                SV_CALL_DCS_Control_IssueGenericCommandResponse,
                DCS::Control::UnitTarget::ESP301,
                DCS::Utils::BasicString{ "1VU?" }
            );
        
        auto max_vel = DCS::Network::Message::SendSync(DCS::Network::Message::Operation::REQUEST, buffer, size_written);

        size_written = DCS::Registry::SVParams::GetDataFromParams(buffer,
                SV_CALL_DCS_Control_IssueGenericCommandResponse,
                DCS::Control::UnitTarget::ESP301,
                DCS::Utils::BasicString{ "1AU?" }
            );
        
        auto max_acc = DCS::Network::Message::SendSync(DCS::Network::Message::Operation::REQUEST, buffer, size_written);

        ui->doubleSpinBox_3->setValue(atof((*(DCS::Utils::BasicString*)max_vel.ptr).buffer));
        ui->doubleSpinBox_4->setValue(atof((*(DCS::Utils::BasicString*)max_acc.ptr).buffer));
#else
        // Get default values for engines acc and vel
        unsigned char buffer[1024];

        auto size_written = DCS::Registry::SVParams::GetDataFromParams(buffer,
                SV_CALL_DCS_Control_IssueGenericCommand,
                DCS::Control::UnitTarget::XPSRLD4,
                DCS::Utils::BasicString{ "GroupKill(Group4)" }
            );
        DCS::Network::Message::SendSync(DCS::Network::Message::Operation::REQUEST, buffer, size_written);

        size_written = DCS::Registry::SVParams::GetDataFromParams(buffer,
                SV_CALL_DCS_Control_IssueGenericCommand,
                DCS::Control::UnitTarget::XPSRLD4,
                DCS::Utils::BasicString{ "GroupInitialize(Group4)" }
            );
        DCS::Network::Message::SendSync(DCS::Network::Message::Operation::REQUEST, buffer, size_written);

        size_written = DCS::Registry::SVParams::GetDataFromParams(buffer,
                SV_CALL_DCS_Control_IssueGenericCommand,
                DCS::Control::UnitTarget::XPSRLD4,
                DCS::Utils::BasicString{ "GroupHomeSearch(Group4)" }
            );
        DCS::Network::Message::SendSync(DCS::Network::Message::Operation::REQUEST, buffer, size_written);

        size_written = DCS::Registry::SVParams::GetDataFromParams(buffer,
                SV_CALL_DCS_Control_SetPIDControlVariables,
                DCS::Control::UnitTarget::XPSRLD4,
                DCS::Utils::BasicString{ "Group4" },
                DCS::i8(2), -0.9, 0.0, 0.0
            );
        DCS::Network::Message::SendSync(DCS::Network::Message::Operation::REQUEST, buffer, size_written);

        size_written = DCS::Registry::SetupEvent(buffer, SV_EVT_DCS_Control_MoveAbsolutePIDChanged, [](DCS::u8* data, DCS::u8* userData) {
            DCS::Control::PIDStatusGroup* status = (DCS::Control::PIDStatusGroup*)data;

            if (QString(status->group.buffer).contains("Group4"))
            {
                emit ((FreejogWindow*)userData)->enableMotionSignal(0, status->status == DCS::Control::PIDStatus::READY);
            }
        }, (DCS::u8*)this);
        DCS::Network::Message::SendAsync(DCS::Network::Message::Operation::EVT_SUB, buffer, size_written);
#endif
    }

    is_enabled = e;

    ui->warning_msg->setVisible(!e);
    ui->warning_sign->setVisible(!e);

    ui->pushButton->setEnabled(e);
    ui->pushButton_2->setEnabled(e);

    ui->doubleSpinBox->setEnabled(e);
    ui->doubleSpinBox_2->setEnabled(e);
    ui->doubleSpinBox_3->setEnabled(e);
    ui->doubleSpinBox_4->setEnabled(e);
    ui->doubleSpinBox_5->setEnabled(e);

    ui->spinBox->setEnabled(e);
    ui->spinBox_2->setEnabled(e);
    ui->spinBox_3->setEnabled(e);
    ui->spinBox_4->setEnabled(e);
}