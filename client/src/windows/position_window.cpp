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

        ui->doubleSpinBox_2->setValue(atof((*(DCS::Utils::BasicString*)pos1.ptr).buffer));
        ui->doubleSpinBox_3->setValue(atof((*(DCS::Utils::BasicString*)pos2.ptr).buffer));
    }
}

void PositionWindow::resetTimer()
{
    int timeout = (int)round((1 / ui->doubleSpinBox->value()) * 1000);
    LOG_DEBUG("Reseting timer. dt = %d", timeout);
    timer->setInterval(timeout);
}