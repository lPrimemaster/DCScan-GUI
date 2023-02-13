#include "channelviz_window.h"
#include "./ui_channelviz_window.h"

ChannelvizWindow::ChannelvizWindow(QWidget* parent) : ui(new Ui::ChannelvizWindow)
{
    ui->setupUi(this);

    auto connect_window = dynamic_cast<MainWindow*>(parent)->GetWindow<ConnectWindow>("Remote Control");

    (void)connect(this, SIGNAL(gotBufferSamples(QVector<QPointF>)), this, SLOT(drawGraphFull(QVector<QPointF>)), Qt::QueuedConnection);

    // This makes the device combo box update based on the connection status
    // TODO: Add a polling rate for new devices (assume PnP NI devices) 
    (void)connect(connect_window, &ConnectWindow::connectionChanged, this, [&](bool status) {
        if(status)
        {
            unsigned char buffer[4096];
            auto size = DCS::Registry::SVParams::GetDataFromParams(buffer, SV_CALL_DCS_DAQ_GetConnectedDevicesAliases);
            auto devices_ptr = DCS::Network::Message::SendSync(DCS::Network::Message::Operation::REQUEST, buffer, size);

            ui->comboBox_9->clear();

            if(devices_ptr.ptr[0] == '\0')
            {
                LOG_ERROR("No NI devices detected upon connecting to server.");
                ui->comboBox_9->addItem("No Devices");
                ui->comboBox_9->setEnabled(false);
                ui->pushButton->setEnabled(false);
            }
            else
            {
                QString devices(((DCS::Utils::BasicString*)(devices_ptr.ptr))->buffer);
                ui->comboBox_9->addItems(devices.split(','));
                ui->comboBox_9->setEnabled(true);
                ui->pushButton->setEnabled(true);
            }
        }
        else
        {
            ui->comboBox_9->clear();
            ui->comboBox_9->addItem("No Connection");
            ui->comboBox_9->setEnabled(false);
            ui->pushButton->setEnabled(false);
        }
    }, Qt::QueuedConnection);

    (void)connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(requestAIStart()));
    (void)connect(ui->pushButton_2, SIGNAL(clicked()), this, SLOT(requestAIStop()));
    (void)connect(ui->pushButton_3, SIGNAL(clicked()), this, SLOT(clearGraph()));

    ui->pushButton->setEnabled(false);
    ui->pushButton_2->setEnabled(false);

    series = new QLineSeries();

    QVector<QPointF> points(1000);
    for(int i = 0; i < points.size(); i++)
    {
        points[i].setX(i);
        points[i].setY(0);
    }

    series->replace(points);
    ui->plot->chart()->addSeries(series);
    ui->plot->chart()->legend()->hide();

    QValueAxis* axis_x = new QValueAxis();
	QValueAxis* axis_y = new QValueAxis();

	axis_x->setRange(0, 1000);
	axis_y->setRange(-10, 10);

	ui->plot->chart()->addAxis(axis_x, Qt::AlignBottom);
	ui->plot->chart()->addAxis(axis_y, Qt::AlignLeft);

	series->attachAxis(axis_x);
	series->attachAxis(axis_y);
}

ChannelvizWindow::~ChannelvizWindow()
{

}

void ChannelvizWindow::requestAIStart()
{
    // No need to check for connection, since button is disabled when disconnected
<<<<<<< HEAD
    QString channel = ui->comboBox_9->currentText() + "/" + ui->comboBox->currentText();
=======

    // BUG : Make the PXI SLOT name an option!
    QString channel = "Dev1/" + ui->comboBox->currentText();
>>>>>>> e9af327... Suspicious crashing.
    DCS::DAQ::ChannelRef ref;

    switch (ui->comboBox_3->currentIndex())
    {
        case 0: ref = DCS::DAQ::ChannelRef::Default; break;
        case 1: ref = DCS::DAQ::ChannelRef::SingleEnded; break;
        case 2: ref = DCS::DAQ::ChannelRef::NoRefSingleEnded; break;
        case 3: ref = DCS::DAQ::ChannelRef::Differential; break;
        case 4: ref = DCS::DAQ::ChannelRef::PseudoDifferential; break;
        default: LOG_ERROR("ChannelViz combobox unexpected index."); break;
    }

    double rate = ui->doubleSpinBox->value() * 1000;

    DCS::Utils::BasicString cname = { "T_ChannelViz" };
    DCS::Utils::BasicString str;
    memcpy(str.buffer, channel.toStdString().c_str(), channel.toStdString().size() + 1);

    unsigned char buffer[4096];
    DCS::DAQ::ChannelLimits l; // NOTE : Use max limits always, maybe change this later (?)
    l.min = -10.0;
    l.max =  10.0;

    auto size = DCS::Registry::SVParams::GetDataFromParams(buffer, SV_CALL_DCS_DAQ_NewAIVChannel, cname, str, ref, l);

    DCS::Network::Message::SendSync(DCS::Network::Message::Operation::REQUEST, buffer, size);

    size = DCS::Registry::SetupEvent(buffer, SV_EVT_DCS_DAQ_VoltageEvent, [](DCS::u8* data, DCS::u8* userData) {
            DCS::f64* fdata = (DCS::f64*)data;

            ChannelvizWindow* window = (ChannelvizWindow*)userData;

            // NOTE : Allocate QVector buffer every time??
            QVector<QPointF> points;
            points.reserve(1000);
            for(int i = 0 ; i < 1000; i++)
            {
                points.append(QPointF(i, fdata[i]));
            }

            emit window->gotBufferSamples(points);

        }, (DCS::u8*)this);

    DCS::Network::Message::SendAsync(DCS::Network::Message::Operation::EVT_SUB, buffer, size);
    
    size = DCS::Registry::SVParams::GetDataFromParams(buffer, SV_CALL_DCS_DAQ_StartAIAcquisition, rate);

    DCS::Network::Message::SendAsync(DCS::Network::Message::Operation::REQUEST, buffer, size);

    ui->pushButton->setEnabled(false);
    ui->pushButton_2->setEnabled(true);
}

void ChannelvizWindow::requestAIStop()
{
    unsigned char buffer[4096];
    auto size = DCS::Registry::SVParams::GetDataFromParams(buffer, SV_CALL_DCS_DAQ_StopAIAcquisition);

    DCS::Network::Message::SendSync(DCS::Network::Message::Operation::REQUEST, buffer, size);

    size = DCS::Registry::SVParams::GetDataFromParams<DCS::Utils::BasicString>(buffer, SV_CALL_DCS_DAQ_DeleteAIVChannel, { "T_ChannelViz" });

    DCS::Network::Message::SendAsync(DCS::Network::Message::Operation::REQUEST, buffer, size);

    size = DCS::Registry::RemoveEvent(buffer, SV_EVT_DCS_DAQ_VoltageEvent);

    DCS::Network::Message::SendAsync(DCS::Network::Message::Operation::EVT_UNSUB, buffer, size);

    ui->pushButton->setEnabled(true);
    ui->pushButton_2->setEnabled(false);
}

void ChannelvizWindow::clearGraph()
{
    series->clear();
}

void ChannelvizWindow::drawGraphFull(QVector<QPointF> points)
{
    series->replace(points);
}
