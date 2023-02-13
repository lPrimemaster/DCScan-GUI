#include "clinometer_window.h"
#include "./ui_clinometer_window.h"

ClinometerWindow::ClinometerWindow(QWidget* parent) : ui(new Ui::ClinometerWindow)
{
    ui->setupUi(this);

    auto connect_window = dynamic_cast<MainWindow*>(parent)->GetWindow<ConnectWindow>("Remote Control");
    (void)connect(connect_window, SIGNAL(connectionChanged(bool)), ui->pushButton, SLOT(setEnabled(bool)), Qt::QueuedConnection);

    (void)connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(requestAIStart()));
    (void)connect(ui->pushButton_2, SIGNAL(clicked()), this, SLOT(requestAIStop()));

    ui->pushButton->setEnabled(false);
    ui->pushButton_2->setEnabled(false);
}

ClinometerWindow::~ClinometerWindow()
{

}

void ClinometerWindow::requestAIStart()
{
    // Channel names
    QString axis0 = "Dev1/ai0";
    QString axis1 = "Dev1/ai1";

    // Read mode
    DCS::DAQ::ChannelRef ref = DCS::DAQ::ChannelRef::SingleEnded;

    // Sample rate in samples per sec
    double rate = 1000;

    DCS::Utils::BasicString cname0 = { "Axis0" };
    DCS::Utils::BasicString str0;
    memcpy(str0.buffer, axis0.toStdString().c_str(), axis0.toStdString().size() + 1);

    DCS::Utils::BasicString cname1 = { "Axis1" };
    DCS::Utils::BasicString str1;
    memcpy(str1.buffer, axis1.toStdString().c_str(), axis1.toStdString().size() + 1);

    DCS::DAQ::ChannelLimits l = { 0.0, 5.0 };

    unsigned char buffer[4096];

    // Generate the ai0 channel
    auto size = DCS::Registry::SVParams::GetDataFromParams(buffer, SV_CALL_DCS_DAQ_NewAIVChannel, cname0, str0, ref, l);
    DCS::Network::Message::SendSync(DCS::Network::Message::Operation::REQUEST, buffer, size);

    // Generate the ai1 channel
    size = DCS::Registry::SVParams::GetDataFromParams(buffer, SV_CALL_DCS_DAQ_NewAIVChannel, cname1, str1, ref, l);
    DCS::Network::Message::SendSync(DCS::Network::Message::Operation::REQUEST, buffer, size);

    size = DCS::Registry::SetupEvent(buffer, SV_EVT_DCS_DAQ_ClinometerEvent, [](DCS::u8* data, DCS::u8* userData) {
        // Output vindo do ClinometerEvent() do servidor
        DCS::DAQ::ClinometerEventData evt_data = *(DCS::DAQ::ClinometerEventData*)data;
        
        // TODO

    }, (DCS::u8*)this);
    DCS::Network::Message::SendAsync(DCS::Network::Message::Operation::EVT_SUB, buffer, size);

    size = DCS::Registry::SVParams::GetDataFromParams(buffer, SV_CALL_DCS_DAQ_StartAIAcquisition, rate);
    DCS::Network::Message::SendAsync(DCS::Network::Message::Operation::REQUEST, buffer, size);

    ui->pushButton->setEnabled(false);
    ui->pushButton_2->setEnabled(true);
}

void ClinometerWindow::requestAIStop()
{
    unsigned char buffer[4096];
    auto size = DCS::Registry::SVParams::GetDataFromParams(buffer, SV_CALL_DCS_DAQ_StopAIAcquisition);
    DCS::Network::Message::SendSync(DCS::Network::Message::Operation::REQUEST, buffer, size);

    size = DCS::Registry::SVParams::GetDataFromParams<DCS::Utils::BasicString>(buffer, SV_CALL_DCS_DAQ_DeleteAIVChannel, { "Axis0" });
    DCS::Network::Message::SendAsync(DCS::Network::Message::Operation::REQUEST, buffer, size);

    size = DCS::Registry::SVParams::GetDataFromParams<DCS::Utils::BasicString>(buffer, SV_CALL_DCS_DAQ_DeleteAIVChannel, { "Axis1" });
    DCS::Network::Message::SendAsync(DCS::Network::Message::Operation::REQUEST, buffer, size);

    size = DCS::Registry::RemoveEvent(buffer, SV_EVT_DCS_DAQ_ClinometerEvent);
    DCS::Network::Message::SendAsync(DCS::Network::Message::Operation::EVT_UNSUB, buffer, size);

    ui->pushButton->setEnabled(true);
    ui->pushButton_2->setEnabled(false);
}

