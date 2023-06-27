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
    series3 = new QLineSeries();
    series4 = new QLineSeries();

    QVector<QPointF> points(1000);
    for(int i = 0; i < points.size(); i++)
    {
        points[i].setX(i);
        points[i].setY(0);
    }

    // TODO: Fetch the theme colors instead
    series1->setColor(QColor("#ed7e00"));
    series2->setColor(QColor("#ed7e00"));
    series3->setColor(QColor("#ed7e00"));
    series4->setColor(QColor("#ed7e00"));

    series1->replace(points);
    series2->replace(points);
    series3->replace(points);
    series4->replace(points);

    ui->angle1_plot->chart()->addSeries(series1);
    ui->angle2_plot->chart()->addSeries(series2);
    ui->angle3_plot->chart()->addSeries(series3);
    ui->angle4_plot->chart()->addSeries(series4);
    ui->angle1_plot->chart()->legend()->hide();
    ui->angle2_plot->chart()->legend()->hide();
    ui->angle3_plot->chart()->legend()->hide();
    ui->angle4_plot->chart()->legend()->hide();

    axis_x1 = new QValueAxis();
	axis_y1 = new QValueAxis();
	axis_x1->setRange(-graphx_span, 0);
	axis_y1->setRange(0.0001, 0.0002);
    axis_x1->setTitleText(QString::fromUtf8("Time passed (Δt)"));
    axis_y1->setTitleText(QString::fromUtf8("Encoder (°)"));

    axis_x2 = new QValueAxis();
	axis_y2 = new QValueAxis();
    axis_x2->setRange(-graphx_span, 0);
	axis_y2->setRange(0.0001, 0.0002);
    axis_x2->setTitleText(QString::fromUtf8("Time passed (Δt)"));
    axis_y2->setTitleText(QString::fromUtf8("Encoder (°)"));

    axis_x3 = new QValueAxis();
	axis_y3 = new QValueAxis();
    axis_x3->setRange(-graphx_span, 0);
	axis_y3->setRange(0.0001, 0.0002);
    axis_x3->setTitleText(QString::fromUtf8("Time passed (Δt)"));
    axis_y3->setTitleText(QString::fromUtf8("Encoder (°)"));

    axis_x4 = new QValueAxis();
	axis_y4 = new QValueAxis();
    axis_x4->setRange(-graphx_span, 0);
	axis_y4->setRange(0.0001, 0.0002);
    axis_x4->setTitleText(QString::fromUtf8("Time passed (Δt)"));
    axis_y4->setTitleText(QString::fromUtf8("Encoder (°)"));

	ui->angle1_plot->chart()->addAxis(axis_x1, Qt::AlignBottom);
	ui->angle1_plot->chart()->addAxis(axis_y1, Qt::AlignLeft);
    
    ui->angle2_plot->chart()->addAxis(axis_x2, Qt::AlignBottom);
	ui->angle2_plot->chart()->addAxis(axis_y2, Qt::AlignLeft);

    ui->angle3_plot->chart()->addAxis(axis_x3, Qt::AlignBottom);
	ui->angle3_plot->chart()->addAxis(axis_y3, Qt::AlignLeft);

    ui->angle4_plot->chart()->addAxis(axis_x4, Qt::AlignBottom);
	ui->angle4_plot->chart()->addAxis(axis_y4, Qt::AlignLeft);

	series1->attachAxis(axis_x1);
	series1->attachAxis(axis_y1);
    
	series2->attachAxis(axis_x2);
	series2->attachAxis(axis_y2);

    series3->attachAxis(axis_x3);
	series3->attachAxis(axis_y3);

    series4->attachAxis(axis_x4);
	series4->attachAxis(axis_y4);
    
    timer = new QTimer(this);
    (void)connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(2000);

    (void)connect(connect_window, &ConnectWindow::connectionChanged, this, [&](bool status) {
        // Check if we have a connection before setting up these
        if(status)
        {
#ifdef DCS_MODULE_ENCODER
            unsigned char buffer[1024];
            auto size_written = DCS::Registry::SVParams::GetDataFromParams(buffer,
                    SV_CALL_DCS_ENC_GetTriggerPeriod
                );
            encoder_period = *(DCS::u32*)DCS::Network::Message::SendSync(DCS::Network::Message::Operation::REQUEST, buffer, size_written).ptr;
#endif
            float val = ui->doubleSpinBox->value();
            max_graph_points = (int)round(val * graphx_span) + 2;
        }
        else
        {
            // Clear the graphs
            series1->clear();
            series2->clear();
            series3->clear();
            series4->clear();
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
#ifdef DCS_MODULE_ENCODER
        // Ask for encoder position
        unsigned char buffer[1024];
        auto size_written = DCS::Registry::SVParams::GetDataFromParams(buffer,
            SV_CALL_DCS_ENC_InspectLastEncoderValues
        );
        auto enc = DCS::Network::Message::SendSync(DCS::Network::Message::Operation::REQUEST, buffer, size_written);
        DCS::ENC::EncoderData data = (*(DCS::ENC::EncoderData*)enc.ptr);

        // From the external encoder
        ui->doubleSpinBox_2->setDecimals(6);
        ui->doubleSpinBox_2->setValue(data.axis[1].calpos); // Crystal 1 (Axis X12)
        ui->doubleSpinBox_3->setDecimals(6);
        ui->doubleSpinBox_3->setValue(data.axis[3].calpos); // Crystal 2 (Axis X14)

        size_written = DCS::Registry::SVParams::GetDataFromParams(buffer,
            SV_CALL_DCS_Control_IssueGenericCommandResponse,
            DCS::Control::UnitTarget::XPSRLD4,
            DCS::Utils::BasicString{ "GroupPositionCurrentGet(Table.Pos, double*)" }
        );
        enc = DCS::Network::Message::SendSync(DCS::Network::Message::Operation::REQUEST, buffer, size_written);
        auto table_response = QString((*(DCS::Utils::BasicString*)enc.ptr).buffer).split(',');
        double table_pos = 0.0;
        if(table_response[0].toInt() == 0)
        {
            size_written = DCS::Registry::SVParams::GetDataFromParams(buffer,
                SV_CALL_DCS_Database_ReadValuef64,
                DCS::Utils::BasicString{ "Geometric_AngleOffsetT" }
            );
            enc = DCS::Network::Message::SendSync(DCS::Network::Message::Operation::REQUEST, buffer, size_written);
            table_pos = table_response[1].toDouble() + *(DCS::f64*)enc.ptr;
        }

        size_written = DCS::Registry::SVParams::GetDataFromParams(buffer,
            SV_CALL_DCS_Control_IssueGenericCommandResponse,
            DCS::Control::UnitTarget::XPSRLD4,
            DCS::Utils::BasicString{ "GroupPositionCurrentGet(Detector.Pos, double*)" }
        );
        enc = DCS::Network::Message::SendSync(DCS::Network::Message::Operation::REQUEST, buffer, size_written);
        auto detector_response = QString((*(DCS::Utils::BasicString*)enc.ptr).buffer).split(',');
        double detector_pos = 0.0;
        if(detector_response[0].toInt() == 0)
        {
            size_written = DCS::Registry::SVParams::GetDataFromParams(buffer,
                SV_CALL_DCS_Database_ReadValuef64,
                DCS::Utils::BasicString{ "Geometric_AngleOffsetD" }
            );
            enc = DCS::Network::Message::SendSync(DCS::Network::Message::Operation::REQUEST, buffer, size_written);
            detector_pos = detector_response[1].toDouble() + *(DCS::f64*)enc.ptr;
        }

        // From the engines encoder
        ui->doubleSpinBox_4->setDecimals(3);
        ui->doubleSpinBox_4->setValue(table_pos);    // Table
        ui->doubleSpinBox_5->setDecimals(3);
        ui->doubleSpinBox_5->setValue(detector_pos); // Detector

        QPointF p1(0, data.axis[1].calpos);
        QPointF p2(0, data.axis[3].calpos);
        QPointF p3(0, table_pos);
        QPointF p4(0, detector_pos);
#else
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


        double double_pos1 = std::atof((*(DCS::Utils::BasicString*)pos1.ptr).buffer);
        double double_pos2 = std::atof((*(DCS::Utils::BasicString*)pos1.ptr).buffer);

        ui->doubleSpinBox_2->setDecimals(6);
        ui->doubleSpinBox_2->setValue(double_pos1); // Direct motor readouts
        ui->doubleSpinBox_3->setDecimals(6);
        ui->doubleSpinBox_3->setValue(double_pos2); // Direct motor readouts

        QPointF p1(0, double_pos1);
        QPointF p2(0, double_pos2);
#endif
        insertRollingData(points1, p1);
        insertRollingData(points2, p2);
        insertRollingData(points3, p3);
        insertRollingData(points4, p4);

        auto [miny1, maxy1] = std::minmax_element(points1.begin(), points1.end(), [](QPointF a, QPointF b) -> bool { return a.y() < b.y(); });
        auto [miny2, maxy2] = std::minmax_element(points2.begin(), points2.end(), [](QPointF a, QPointF b) -> bool { return a.y() < b.y(); });
        auto [miny3, maxy3] = std::minmax_element(points3.begin(), points3.end(), [](QPointF a, QPointF b) -> bool { return a.y() < b.y(); });
        auto [miny4, maxy4] = std::minmax_element(points4.begin(), points4.end(), [](QPointF a, QPointF b) -> bool { return a.y() < b.y(); });

        axis_y1->setRange(miny1->y(), maxy1->y());
        axis_y2->setRange(miny2->y(), maxy2->y());
        axis_y3->setRange(miny3->y() - 0.005, maxy3->y() + 0.005);
        axis_y4->setRange(miny4->y() - 0.005, maxy4->y() + 0.005);

        emit appendToGraphs();
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

    axis_x1->setRange(-graphx_span, 0);
    axis_x2->setRange(-graphx_span, 0);
    axis_x3->setRange(-graphx_span, 0);
    axis_x4->setRange(-graphx_span, 0);
}

void PositionWindow::drawGraphsRolling()
{
    series1->replace(points1);
    series2->replace(points2);
    series3->replace(points3);
    series4->replace(points4);
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
