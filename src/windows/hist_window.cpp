#include "hist_window.h"

#include <QBarSet>
#include <QRubberBand>

#include <QRandomGenerator>

using namespace QtCharts;

HistWindow::HistWindow(QWidget* parent) : QChartView(parent)
{
	bar_series = new QBarSeries();
	chart = new QChart();

	setChart(chart);
	
	setRenderHint(QPainter::Antialiasing);

	QBarSet* set0 = new QBarSet("Data");

	// TODO : Add this to ctor later
	max_n_bins = 1000;
	max_bin_h = 100;

	for(int i = 0; i < 1000; i++)
		set0->append(i);
	
	bar_series->append(set0);
	bar_series->setBarWidth(bar_series->count());
	
	(void)connect(this, &HistWindow::incrementBinValueSig, this, &HistWindow::updateBin);

	axis_x = new QValueAxis();
	axis_y = new QValueAxis();

	axis_x->setRange(0, 1000);
	axis_y->setRange(0, 1000);

	chart->addAxis(axis_x, Qt::AlignBottom);
	chart->addAxis(axis_y, Qt::AlignLeft);

	chart->addSeries(bar_series);

	bar_series->attachAxis(axis_x);
	bar_series->attachAxis(axis_y);

	chart->legend()->hide();

	rb = new QRubberBand(QRubberBand::Rectangle, this);
}

void HistWindow::mouseMoveEvent(QMouseEvent* event)
{
	lastMousePos = event->localPos();

	if (mouse_holding)
	{
		rb->setGeometry(QRect(origin_rb, event->pos()).normalized());
	}

	QChartView::mouseMoveEvent(event);
}

void HistWindow::mousePressEvent(QMouseEvent* event)
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

void HistWindow::mouseReleaseEvent(QMouseEvent* event)
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

		DCS::Utils::Logger::Debug("Zooming Chart -- RECT[%lf, %lf]", zoom_region.topLeft().x(), zoom_region.topLeft().y());
		chart->zoomIn(zoom_region);

		//axis_x->setRange(min_x, max_x);
		//axis_y->setRange(min_y, max_y);
	}
	else if (event->button() == Qt::MouseButton::RightButton)
	{
		DCS::Utils::Logger::Debug("Reset chart zoom");
		resetZoom();
	}

	QChartView::mouseReleaseEvent(event);
}

HistWindow::~HistWindow()
{
	delete chart;
	if (nt)
	{
		nt->join();
		delete nt;
	}
}

QPointF HistWindow::getSeriesCoordFromChartCoord(const QPointF& chartPos, QAbstractSeries* series) const
{
	auto const chartItemPos = chart->mapFromScene(chartPos);
	auto const valueGivenSeries = chart->mapToValue(chartItemPos, series);
	return valueGivenSeries;
}

QPointF HistWindow::getChartCoordFromSeriesCoord(const QPointF& seriesPos, QAbstractSeries* series) const
{
	QPointF ret = chart->mapToPosition(seriesPos, series);
	ret = chart->mapFromScene(ret);
	return ret;
}

std::tuple<qreal, qreal, qreal, qreal> HistWindow::getAxisNewMinMax(const QRectF& rect) const
{
	return std::make_tuple<qreal, qreal, qreal, qreal>(
		getSeriesCoordFromChartCoord(rect.topLeft(), bar_series).x(),    //min x
		getSeriesCoordFromChartCoord(rect.topRight(), bar_series).x(),   //max x
		getSeriesCoordFromChartCoord(rect.bottomLeft(), bar_series).y(), //min y
		getSeriesCoordFromChartCoord(rect.topLeft(), bar_series).y()     //max y
		);
}

void HistWindow::resetZoom()
{
	chart->zoomOut();
	chart->zoomReset();
}

void HistWindow::updateAllSettings(GraphSettings settings)
{
	// Change Bins / Axis
	if(settings.maximum_bins != max_n_bins)
		setAxisBinRange(settings.maximum_bins);

	// Change bars color - Assume only one set at all times
	bar_series->barSets()[0]->setColor(settings.bins_color);
}

void HistWindow::updateBin(int idx)
{
	auto s = bar_series->barSets()[0];

	if (idx >= s->count())
	{
		DCS::Utils::Logger::Error("Trying to update QBarSeries with invalid index. Ignoring...");
	}
	else
	{
		s->replace(idx, s->at(idx) + 1);
	}
}

void HistWindow::adjustGraphAxisRange()
{
	qreal max_y = barMaximum();
	int max_x = bar_series->barSets()[0]->count();

	axis_x->setRange(0, max_x);
	axis_y->setRange(0, max_y);
}

qreal HistWindow::barMaximum()
{
	auto s = bar_series->barSets()[0];
	qreal max = 0.0;
	for (int i = 0; i < s->count(); i++)
	{
		qreal v = s->at(i);
		if (v > max)
			max = v;
	}
	return max;
}

void HistWindow::setAxisBinRange(int nbins)
{
	max_n_bins = nbins;
	//max_bin_h = 100;

	if (bar_series)
	{
		// Removes and deletes
		chart->removeAllSeries();

		bar_series = new QBarSeries();

		QBarSet* set = new QBarSet("Data");

		axis_x->setRange(0.0, max_n_bins);

		std::vector<qreal> vec(max_n_bins, 0.0);
		QList<qreal> zero_l;
		zero_l.reserve(max_n_bins);
		std::copy(vec.begin(), vec.end(), std::back_inserter(zero_l));
		set->append(zero_l);
		bar_series->append(set);
		bar_series->setBarWidth(bar_series->count());

		if (nt) { nt->join(); delete nt; }
		nt = new std::thread([&]() -> void {

			std::random_device rd{};
			std::mt19937 gen{ rd() };
			std::normal_distribution<> d{50, 2};

			for (int i = 0; i < 10000; i++)
			{
				int v = (int)std::round(d(gen));
				emit incrementBinValueSig(v);
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}
			});

		chart->addSeries(bar_series);
		bar_series->attachAxis(axis_x);
		bar_series->attachAxis(axis_y);

		DCS::Utils::Logger::Debug("Bar set reset successful.");
	}
	else
	{
		DCS::Utils::Logger::Error("Failed to remove bar set from graph on reset.");
	}

	resetZoom();
}
