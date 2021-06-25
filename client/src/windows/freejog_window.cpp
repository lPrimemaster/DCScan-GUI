#include "freejog_window.h"
#include "./ui_freejog_window.h"

FreejogWindow::FreejogWindow(QWidget* parent) : ui(new Ui::FreejogWindow)
{
    auto connect_window = dynamic_cast<MainWindow*>(parent)->GetWindow<ConnectWindow>("Remote Control");
    (void)connect(connect_window, SIGNAL(connectionChanged(bool)), this, SLOT(enableFreejog(bool)), Qt::QueuedConnection);
    ui->setupUi(this);
    ui->warning_msg->setText("Free motion is disabled.\nConnect to server to enable.");
    enableFreejog(false);

    (void)connect(ui->t1_slider, SIGNAL(valueChanged(int)), this, SLOT(moveEngine1Free(int)));
    (void)connect(ui->t2_slider, SIGNAL(valueChanged(int)), this, SLOT(moveEngine2Free(int)));

    (void)connect(ui->doubleSpinBox, SIGNAL(editingFinished()), this, SLOT(moveEngine1To()));
    (void)connect(ui->doubleSpinBox_2, SIGNAL(editingFinished()), this, SLOT(moveEngine2To()));

    (void)connect(ui->t1_slider, SIGNAL(sliderReleased()), this, SLOT(resetSlider1()));
    (void)connect(ui->t2_slider, SIGNAL(sliderReleased()), this, SLOT(resetSlider2()));

    (void)connect(ui->doubleSpinBox_3, SIGNAL(editingFinished()), this, SLOT(updateVel()));
    (void)connect(ui->doubleSpinBox_4, SIGNAL(editingFinished()), this, SLOT(updateAcc()));
}

FreejogWindow::~FreejogWindow()
{
    
}

void FreejogWindow::updateAcc()
{
    double acc = ui->doubleSpinBox_4->value();
    QString values = 
         "1AU" + QString::number(acc) + 
        ";1AC" + QString::number(acc) +
        ";2AU" + QString::number(acc) + 
        ";2AC" + QString::number(acc) + ";";

    DCS::Utils::BasicString str;
    memcpy(str.buffer, values.toStdString().c_str(), values.toStdString().size() + 1);
    LOG_DEBUG("Updating acceleration: %s.", str.buffer);

    unsigned char buffer[4096];
    auto size_written = DCS::Registry::SVParams::GetDataFromParams(buffer,
        SV_CALL_DCS_Control_IssueGenericCommand,
        DCS::Control::UnitTarget::ESP301,
        str
    );

    DCS::Network::Message::SendAsync(DCS::Network::Message::Operation::REQUEST, buffer, size_written);
}

void FreejogWindow::updateVel()
{
    double vel = ui->doubleSpinBox_3->value();
    QString values = 
         "1VU" + QString::number(vel) + 
        ";2VU" + QString::number(vel) + ";";

    DCS::Utils::BasicString str;
    memcpy(str.buffer, values.toStdString().c_str(), values.toStdString().size() + 1);
    LOG_DEBUG("Updating velocity: %s.", str.buffer);

    unsigned char buffer[4096];
    auto size_written = DCS::Registry::SVParams::GetDataFromParams(buffer,
        SV_CALL_DCS_Control_IssueGenericCommand,
        DCS::Control::UnitTarget::ESP301,
        str
    );

    DCS::Network::Message::SendAsync(DCS::Network::Message::Operation::REQUEST, buffer, size_written);
}

// TODO : Refactor this to avoid writting unecessary LOC's
void FreejogWindow::moveEngine1Free(int val)
{
    if(ui->doubleSpinBox_3->value() > 0.0f && ui->doubleSpinBox_4->value() > 0.0f)
    {
        QString cmd = "1VA" + QString::number((ui->doubleSpinBox_3->value() / 5.0) * abs(val)) + ";";
        unsigned char buffer[4096];

        if(val > 0)
        {
            cmd += "1PR+360;"; // Move 360 deg relative to pos

            DCS::Utils::BasicString str;
            memcpy(str.buffer, cmd.toLatin1().constData(), cmd.toLatin1().size());

            auto size_written = DCS::Registry::SVParams::GetDataFromParams(buffer,
                SV_CALL_DCS_Control_IssueGenericCommand,
                DCS::Control::UnitTarget::ESP301,
                str
            );
        
            DCS::Network::Message::SendAsync(DCS::Network::Message::Operation::REQUEST, buffer, size_written);
        }
        else if(val == 0)
        {
            cmd = "1ST;";

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
            cmd += "1PR-360;"; // Move 360 deg relative to pos

            DCS::Utils::BasicString str;
            memcpy(str.buffer, cmd.toLatin1().constData(), cmd.toLatin1().size());

            auto size_written = DCS::Registry::SVParams::GetDataFromParams(buffer,
                SV_CALL_DCS_Control_IssueGenericCommand,
                DCS::Control::UnitTarget::ESP301,
                str
            );
        
            DCS::Network::Message::SendAsync(DCS::Network::Message::Operation::REQUEST, buffer, size_written);
        }
    }
    else
    {
        LOG_ERROR("Cannot move engine 1: acceleration or velocity value is zero.");
    }
}

void FreejogWindow::moveEngine2Free(int val)
{
    if(ui->doubleSpinBox_3->value() > 0.0f && ui->doubleSpinBox_4->value() > 0.0f)
    {
        QString cmd = "2VA" + QString::number((ui->doubleSpinBox_3->value() / 5.0) * abs(val)) + ";";
        unsigned char buffer[4096];

        if(val > 0)
        {
            cmd += "2PR+360;"; // Move 360 deg relative to pos

            DCS::Utils::BasicString str;
            memcpy(str.buffer, cmd.toLatin1().constData(), cmd.toLatin1().size());

            auto size_written = DCS::Registry::SVParams::GetDataFromParams(buffer,
                SV_CALL_DCS_Control_IssueGenericCommand,
                DCS::Control::UnitTarget::ESP301,
                str
            );
        
            DCS::Network::Message::SendAsync(DCS::Network::Message::Operation::REQUEST, buffer, size_written);
        }
        else if(val == 0)
        {
            cmd = "2ST;";

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
            cmd += "2PR-360;"; // Move 360 deg relative to pos

            DCS::Utils::BasicString str;
            memcpy(str.buffer, cmd.toLatin1().constData(), cmd.toLatin1().size());

            auto size_written = DCS::Registry::SVParams::GetDataFromParams(buffer,
                SV_CALL_DCS_Control_IssueGenericCommand,
                DCS::Control::UnitTarget::ESP301,
                str
            );
        
            DCS::Network::Message::SendAsync(DCS::Network::Message::Operation::REQUEST, buffer, size_written);
        }
    }
    else
    {
        LOG_ERROR("Cannot move engine 2: acceleration or velocity value is zero.");
    }
}

void FreejogWindow::moveEngine1To()
{
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
}

void FreejogWindow::moveEngine2To()
{
    if(ui->doubleSpinBox_3->value() > 0.0f && ui->doubleSpinBox_4->value() > 0.0f)
    {
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
    }
    else
    {
        LOG_ERROR("Cannot move engine 2: acceleration or velocity value is zero.");
    }
}

void FreejogWindow::resetSlider1()
{
    ui->t1_slider->setValue(0);
}

void FreejogWindow::resetSlider2()
{
    ui->t2_slider->setValue(0);
}

void FreejogWindow::enableFreejog(bool e)
{
    if(!e)
    {
        ui->warning_msg->setText("Free motion is disabled.\nConnect to server to enable.");
    }

    is_enabled = e;

    ui->warning_msg->setVisible(!e);
    ui->warning_sign->setVisible(!e);

    ui->doubleSpinBox->setEnabled(e);
    ui->doubleSpinBox_2->setEnabled(e);
    ui->doubleSpinBox_3->setEnabled(e);
    ui->doubleSpinBox_4->setEnabled(e);

    ui->spinBox->setEnabled(e);
    ui->spinBox_2->setEnabled(e);
    ui->spinBox_3->setEnabled(e);
    ui->spinBox_4->setEnabled(e);

    ui->t1_slider->setEnabled(e);
    ui->t2_slider->setEnabled(e);
}