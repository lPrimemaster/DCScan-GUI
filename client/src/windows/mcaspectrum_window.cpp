#include "mcaspectrum_window.h"

using namespace QtCharts;

MCASpectrumWindow::MCASpectrumWindow(QWidget* parent) : QChartView(parent)
{
    bar_series = new QBarSeries(this);
    set = new QBarSet("Data");

    // TODO : Add this to ctor later
	max_n_bins = 8096;
	max_bin_h = 100;

    for(int i = 0; i < (int)max_n_bins; i++)
		set->append(0.0);

    bar_series->append(set);
    bar_series->setBarWidth(bar_series->count());

    setRenderHint(QPainter::Antialiasing);
    chart()->addSeries(bar_series);

    axis_x = new QValueAxis();
	axis_y = new QValueAxis();

	axis_x->setRange(0, max_n_bins);
	axis_y->setRange(0, max_bin_h);

	chart()->addAxis(axis_x, Qt::AlignBottom);
	chart()->addAxis(axis_y, Qt::AlignLeft);

	bar_series->attachAxis(axis_x);
	bar_series->attachAxis(axis_y);

    chart()->legend()->hide();

    rb = new QRubberBand(QRubberBand::Rectangle, this);

    connect_window = dynamic_cast<MainWindow*>(parent)->GetWindow<ConnectWindow>("Remote Control");
    (void)connect(connect_window, &ConnectWindow::connectionChanged, this, [&](bool status) {
        if(status)
        {
            // Connection on
            unsigned char buffer[1024];
            auto size_written = DCS::Registry::SVParams::GetDataFromParams(buffer,
                SV_CALL_DCS_DAQ_GetMCANumChannels
            );

            auto data = DCS::Network::Message::SendSync(DCS::Network::Message::Operation::REQUEST, buffer, size_written);
            auto mca_channels = *(DCS::u16*)data.ptr;

            axis_x->setRange(0, mca_channels);

            set->setColor(QColor("red"));
            bar_series->setBarWidth(1.01);
			set->setBorderColor(Qt::transparent);

            for(int i = 0; i < mca_channels; i++)
                set->replace(i, 0.0);
        }
        else
        {
            // Connection off
        }
    }, Qt::QueuedConnection);

	acq_window = dynamic_cast<MainWindow*>(parent)->GetWindow<AcquisitionControlWindow>("Acquisition Control");
    (void)connect(acq_window, &AcquisitionControlWindow::eventMCA, this, [&](DCS::DAQ::MCACountEventData data) {
		// if(data.count < 2)
		// {
		for(int i = 0; i < data.count; i++)
		{
			set->replace(data.bins[i], set->at(data.bins[i]) + 1);
		}
		LOG_MESSAGE("Counts this frame: %d", data.count);
		// }
    }, Qt::QueuedConnection);
}

MCASpectrumWindow::~MCASpectrumWindow()
{
	
}

void MCASpectrumWindow::mouseMoveEvent(QMouseEvent* event)
{
	lastMousePos = event->localPos();

	if (mouse_holding)
	{
		rb->setGeometry(QRect(origin_rb, event->pos()).normalized());
	}

	QChartView::mouseMoveEvent(event);
}

void MCASpectrumWindow::mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::MouseButton::LeftButton)
	{
		mouse_holding = true;

		origin_rb = event->pos();

		rb->setGeometry(QRect(origin_rb, QSize()));
		rb->show();
	}

	QChartView::mousePressEvent(event);
}

void MCASpectrumWindow::mouseReleaseEvent(QMouseEvent* event)
{
	if (event->button() == Qt::MouseButton::LeftButton)
	{
		mouse_holding = false;
		rb->hide();

		QRectF zoom_region = rb->geometry();

		if (zoom_region.width() < 1.0f || zoom_region.height() < 1.0f)
			return;

		// Can't unpack directly =[ (using C++11)
		auto axismm = getAxisNewMinMax(zoom_region);

		//auto min_x = std::get<0>(axismm);
		//auto max_x = std::get<1>(axismm);
		auto min_y = std::get<2>(axismm);
		//auto max_y = std::get<3>(axismm);

		if (min_y < 0.0)
		{
			zoom_region = zoom_region.translated(0.0, getChartCoordFromSeriesCoord(QPointF(0.0, 0.0), bar_series).y() - zoom_region.bottomLeft().y());
		}

		LOG_DEBUG("Zooming Chart -- RECT[%lf, %lf]", zoom_region.topLeft().x(), zoom_region.topLeft().y());
		chart()->zoomIn(zoom_region);

		//axis_x->setRange(min_x, max_x);
		//axis_y->setRange(min_y, max_y);
	}
	else if (event->button() == Qt::MouseButton::RightButton)
	{
		LOG_DEBUG("Reset chart zoom");
		resetZoom();
	}

	QChartView::mouseReleaseEvent(event);
}

void MCASpectrumWindow::mouseDoubleClickEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton)
    {
		auto ngi = qgraphicsitem_cast<QGraphicsRectItem*>(this->itemAt(lastMousePos.toPoint()));

		if((ngi == nullptr || selected_bar_rect != ngi) && selected_bar_rect != nullptr)
		{
			QBrush b = selected_bar_rect->brush();
			b.setColor(gcolor);
			selected_bar_rect->setBrush(b);
			selected_bar_rect->update();
			selected_bar_rect = nullptr;
			selected_bin = -1;
			//emit changeSelectionSig(selected_bin, 0);
		}

		selected_bar_rect = ngi;
		
		if(selected_bar_rect != nullptr)
		{
			selected_bin = std::round(getSeriesCoordFromChartCoord(lastMousePos, bar_series).x());
			selected_bin = selected_bin > (int)max_n_bins ? max_n_bins : selected_bin < 0 ? 0 : selected_bin;
			int current_val = static_cast<int>(bar_series->barSets()[0]->at(selected_bin));

			QBrush b = selected_bar_rect->brush();
			b.setColor(Qt::red);
			selected_bar_rect->setBrush(b);
			selected_bar_rect->update();

			//emit changeSelectionSig(selected_bin, current_val);
		}
    }

	QChartView::mouseDoubleClickEvent(event);
}

QPointF MCASpectrumWindow::getSeriesCoordFromChartCoord(const QPointF& chartPos, QAbstractSeries* series) const
{
	auto const chartItemPos = chart()->mapFromScene(chartPos);
	auto const valueGivenSeries = chart()->mapToValue(chartItemPos, series);
	return valueGivenSeries;
}

QPointF MCASpectrumWindow::getChartCoordFromSeriesCoord(const QPointF& seriesPos, QAbstractSeries* series) const
{
	QPointF ret = chart()->mapToPosition(seriesPos, series);
	ret = chart()->mapFromScene(ret);
	return ret;
}

std::tuple<qreal, qreal, qreal, qreal> MCASpectrumWindow::getAxisNewMinMax(const QRectF& rect) const
{
	return std::make_tuple<qreal, qreal, qreal, qreal>(
		getSeriesCoordFromChartCoord(rect.topLeft(), bar_series).x(),    //min x
		getSeriesCoordFromChartCoord(rect.topRight(), bar_series).x(),   //max x
		getSeriesCoordFromChartCoord(rect.bottomLeft(), bar_series).y(), //min y
		getSeriesCoordFromChartCoord(rect.topLeft(), bar_series).y()     //max y
		);
}

void MCASpectrumWindow::resetZoom()
{
	chart()->zoomOut();
	chart()->zoomReset();
}