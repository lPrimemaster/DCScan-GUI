#include "position_window.h"
#include "./ui_position_window.h"

PositionWindow::PositionWindow(QWidget* parent) : ui(new Ui::PositionWindow), encoder_period(0), max_graph_points(0), graphx_span(10)
{
    connect_window = dynamic_cast<MainWindow*>(parent)->GetWindow<ConnectWindow>("Remote Control");

    ui->setupUi(this);
    (void)connect(ui->doubleSpinBox, SIGNAL(editingFinished()), this, SLOT(resetTimer()));
    (void)connect(ui->spinBox, SIGNAL(editingFinished()), this, SLOT(resetGraphPeriod()));

    (void)connect(this, SIGNAL(appendToGraphs()), this, SLOT(drawGraphsRolling()), Qt::QueuedConnection);

    series1 = new QLineSeries();
    series2 = new QLineSeries();

    QVector<QPointF> points(1000);
    for(int i = 0; i < points.size(); i++)
    {
        points[i].setX(i);
        points[i].setY(0);
    }

    // TODO: Fetch the theme colors instead
    series1->setColor(QColor("#ed7e00"));
    series2->setColor(QColor("#ed7e00"));

    series1->replace(points);
    series2->replace(points);
    ui->angle1_plot->chart()->addSeries(series1);
    ui->angle2_plot->chart()->addSeries(series2);
    ui->angle1_plot->chart()->legend()->hide();
    ui->angle2_plot->chart()->legend()->hide();

    axis_x1 = new QValueAxis();
	axis_y1 = new QValueAxis();
	axis_x1->setRange(-graphx_span, 0);
	axis_y1->setRange(42.424, 42.425);
    axis_x1->setTitleText(QString::fromUtf8("Time passed (Δt)"));
    axis_y1->setTitleText(QString::fromUtf8("Encoder (°)"));

    axis_x2 = new QValueAxis();
	axis_y2 = new QValueAxis();
    axis_x2->setRange(-graphx_span, 0);
	axis_y2->setRange(0.0001, 0.0002);
    axis_x2->setTitleText(QString::fromUtf8("Time passed (Δt)"));
    axis_y2->setTitleText(QString::fromUtf8("Encoder (°)"));

	ui->angle1_plot->chart()->addAxis(axis_x1, Qt::AlignBottom);
	ui->angle1_plot->chart()->addAxis(axis_y1, Qt::AlignLeft);
    
    ui->angle2_plot->chart()->addAxis(axis_x2, Qt::AlignBottom);
	ui->angle2_plot->chart()->addAxis(axis_y2, Qt::AlignLeft);

	series1->attachAxis(axis_x1);
	series1->attachAxis(axis_y1);
    
	series2->attachAxis(axis_x2);
	series2->attachAxis(axis_y2);
    
    timer = new QTimer(this);
    (void)connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(2000);

    (void)connect(connect_window, &ConnectWindow::connectionChanged, this, [&](bool status) {
        // Check if we have a connection before setting up these
        if(status)
        {
            unsigned char buffer[1024];
            auto size_written = DCS::Registry::SVParams::GetDataFromParams(buffer,
                    SV_CALL_DCS_ENC_GetTriggerPeriod
                );
            encoder_period = *(DCS::u32*)DCS::Network::Message::SendSync(DCS::Network::Message::Operation::REQUEST, buffer, size_written).ptr;

            float val = ui->doubleSpinBox->value();
            max_graph_points = (int)round(val * graphx_span) + 2;
        }
        else
        {
            // Clear the graphs
            series1->clear();
            series2->clear();
        }
    }, Qt::QueuedConnection);
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

        ui->doubleSpinBox_2->setDecimals(6);
        ui->doubleSpinBox_2->setValue(data.axis[1].calpos); // Axis X12
        ui->doubleSpinBox_3->setDecimals(6);
        ui->doubleSpinBox_3->setValue(data.axis[3].calpos); // Axis X14

        QPointF p1(0, data.axis[1].calpos);
        QPointF p2(0, data.axis[3].calpos);

        insertRollingData(points1, p1);
        insertRollingData(points2, p2);

        auto [miny1, maxy1] = std::minmax_element(points1.begin(), points1.end(), [](QPointF a, QPointF b) -> bool { return a.y() < b.y(); });
        auto [miny2, maxy2] = std::minmax_element(points2.begin(), points2.end(), [](QPointF a, QPointF b) -> bool { return a.y() < b.y(); });

        axis_y1->setRange(miny1->y(), maxy1->y());
	    axis_y2->setRange(miny2->y(), maxy2->y());

        emit appendToGraphs();
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

        max_graph_points = (int)round(val * graphx_span) + 2;
    }
    else
    {
        timer->stop();
    }
}

void PositionWindow::resetGraphPeriod()
{
    graphx_span = ui->spinBox->value();
    float refresh_rate = ui->doubleSpinBox->value();
    max_graph_points = (int)round(refresh_rate * graphx_span) + 2;

    points1.clear();
    points2.clear();

    axis_x1->setRange(-graphx_span, 0);
    axis_x2->setRange(-graphx_span, 0);
}

void PositionWindow::drawGraphsRolling()
{
    series1->replace(points1);
    series2->replace(points2);
}

void PositionWindow::insertRollingData(QVector<QPointF>& vector, const QPointF& data)
{
    int interval = timer->interval();
    if(vector.size() >= max_graph_points)
    {
        vector.removeFirst();
    }

    for(int i = 0; i < vector.size(); i++)
    {
        vector[i].setX(vector[i].x() - (interval / 1000.0f));
    }
    vector.append(data);
}
