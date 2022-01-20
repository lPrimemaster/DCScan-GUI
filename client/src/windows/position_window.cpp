#include "position_window.h"
#include "./ui_position_window.h"

PositionWindow::PositionWindow(QWidget* parent) : ui(new Ui::PositionWindow)
{
    connect_window = dynamic_cast<MainWindow*>(parent)->GetWindow<ConnectWindow>("Remote Control");


    ui->setupUi(this);
    (void)connect(ui->doubleSpinBox, SIGNAL(editingFinished()), this, SLOT(resetTimer()));
    
    timer = new QTimer(this);
    (void)connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(2000);
}

PositionWindow::~PositionWindow()
{
    timer->stop();
}

void PositionWindow::update()
{
    // Check if connected to the network
    // TODO : Make this non-blocking instead with SendAsync and a QThread
    if(connect_window->isNetworkConnected())
    {
#if 0
        // Ask for engines position
        unsigned char buffer[1024];
        auto size_written = DCS::Registry::SVParams::GetDataFromParams(buffer,
                SV_CALL_DCS_Control_IssueGenericCommandResponse,
                DCS::Control::UnitTarget::ESP301,
                DCS::Utils::BasicString{ "1TP?" }
            );
        
        auto pos1 = DCS::Network::Message::SendSync(DCS::Network::Message::Operation::REQUEST, buffer, size_written);

        size_written = DCS::Registry::SVParams::GetDataFromParams(buffer,
                SV_CALL_DCS_Control_IssueGenericCommandResponse,
                DCS::Control::UnitTarget::ESP301,
                DCS::Utils::BasicString{ "2TP?" }
            );
        
        auto pos2 = DCS::Network::Message::SendSync(DCS::Network::Message::Operation::REQUEST, buffer, size_written);

        // Ask for engines velocity
        size_written = DCS::Registry::SVParams::GetDataFromParams(buffer,
                SV_CALL_DCS_Control_IssueGenericCommandResponse,
                DCS::Control::UnitTarget::ESP301,
                DCS::Utils::BasicString{ "1TV?" }
            );
        
        auto vel1 = DCS::Network::Message::SendSync(DCS::Network::Message::Operation::REQUEST, buffer, size_written);

        size_written = DCS::Registry::SVParams::GetDataFromParams(buffer,
                SV_CALL_DCS_Control_IssueGenericCommandResponse,
                DCS::Control::UnitTarget::ESP301,
                DCS::Utils::BasicString{ "2TV?" }
            );
        
        auto vel2 = DCS::Network::Message::SendSync(DCS::Network::Message::Operation::REQUEST, buffer, size_written);

        // Ask for step engines position
        // TODO : Check the connections and the motion directions
        size_written = DCS::Registry::SVParams::GetDataFromParams(buffer,
                SV_CALL_DCS_Control_IssueGenericCommandResponse,
                DCS::Control::UnitTarget::PMC8742,
                DCS::Utils::BasicString{ "0>1TP?" }
            );
        
        auto stepX1 = DCS::Network::Message::SendSync(DCS::Network::Message::Operation::REQUEST, buffer, size_written);

        size_written = DCS::Registry::SVParams::GetDataFromParams(buffer,
                SV_CALL_DCS_Control_IssueGenericCommandResponse,
                DCS::Control::UnitTarget::PMC8742,
                DCS::Utils::BasicString{ "0>2TP?" }
            );
        
        auto stepY1 = DCS::Network::Message::SendSync(DCS::Network::Message::Operation::REQUEST, buffer, size_written);

        size_written = DCS::Registry::SVParams::GetDataFromParams(buffer,
                SV_CALL_DCS_Control_IssueGenericCommandResponse,
                DCS::Control::UnitTarget::PMC8742,
                DCS::Utils::BasicString{ "1>1TP?" }
            );
        
        auto stepX2 = DCS::Network::Message::SendSync(DCS::Network::Message::Operation::REQUEST, buffer, size_written);

        size_written = DCS::Registry::SVParams::GetDataFromParams(buffer,
                SV_CALL_DCS_Control_IssueGenericCommandResponse,
                DCS::Control::UnitTarget::PMC8742,
                DCS::Utils::BasicString{ "1>2TP?" }
            );
        
        auto stepY2 = DCS::Network::Message::SendSync(DCS::Network::Message::Operation::REQUEST, buffer, size_written);

        ui->doubleSpinBox_2->setValue(atof((*(DCS::Utils::BasicString*)pos1.ptr).buffer));
        ui->doubleSpinBox_3->setValue(atof((*(DCS::Utils::BasicString*)pos2.ptr).buffer));
        ui->doubleSpinBox_4->setValue(atof((*(DCS::Utils::BasicString*)vel1.ptr).buffer));
        ui->doubleSpinBox_5->setValue(atof((*(DCS::Utils::BasicString*)vel2.ptr).buffer));

        ui->spinBox_6->setValue(atoi((*(DCS::Utils::BasicString*)stepX1.ptr).buffer));
        ui->spinBox_8->setValue(atoi((*(DCS::Utils::BasicString*)stepY1.ptr).buffer));
        ui->spinBox_7->setValue(atoi((*(DCS::Utils::BasicString*)stepX2.ptr).buffer));
        ui->spinBox_9->setValue(atoi((*(DCS::Utils::BasicString*)stepY2.ptr).buffer));
#elif 0
        // Ask for engines position
        unsigned char buffer[1024];
        auto size_written = DCS::Registry::SVParams::GetDataFromParams(buffer,
                SV_CALL_DCS_Control_IssueGenericCommandResponse,
                DCS::Control::UnitTarget::XPSRLD4,
                DCS::Utils::BasicString{ "GroupPositionCurrentGet(Group1.Pos, double*)" }
            );
        
        auto pos1 = DCS::Network::Message::SendSync(DCS::Network::Message::Operation::REQUEST, buffer, size_written);
        QString val1 = (*(DCS::Utils::BasicString*)pos1.ptr).buffer;
        ui->doubleSpinBox_2->setValue(val1.split(',').at(1).toDouble());
#else
        // Ask for encoder position
        unsigned char buffer[1024];
        auto size_written = DCS::Registry::SVParams::GetDataFromParams(buffer,
                SV_CALL_DCS_ENC_InspectLastEncoderValues
            );
        
        auto enc = DCS::Network::Message::SendSync(DCS::Network::Message::Operation::REQUEST, buffer, size_written);
        DCS::ENC::EncoderData data = (*(DCS::ENC::EncoderData*)enc.ptr);
        ui->doubleSpinBox_2->setDecimals(8);
        ui->doubleSpinBox_2->setValue(data.axis[0].calpos);
        // ui->doubleSpinBox_3->setValue(data.axis[1].calpos);
#endif
    }
}

void PositionWindow::resetTimer()
{
    float val = ui->doubleSpinBox->value();
    if(val > 0.0f)
    {
        int timeout = (int)round((1 / val) * 1000);
        LOG_DEBUG("Reseting timer. dt = %d", timeout);
        timer->start();
        timer->setInterval(timeout);
    }
    else
    {
        timer->stop();
    }
}