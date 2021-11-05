#pragma once

#include <QTabWidget>
#include <QWidget>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpinBox>
#include <QComboBox>
#include <QLineEdit>
#include <QLabel>
#include <QTimer>
#include <QBarSeries>
#include <QBarSet>
#include <QValueAxis>
#include <QChartView>

#include <DCS_Core/include/DCS_ModuleCore.h>
#include <DCS_EngineControl/include/DCS_ModuleEngineControl.h>
#include <DCS_Utils/include/DCS_ModuleUtils.h>
#include <DCS_Network/include/DCS_ModuleNetwork.h>

#include "main_window.h"
#include "connect_window.h"
#include "acquisitioncontrol_window.h"

// TODO : Make a base class for this and the DCS spectra, its simply easier...
class MCASpectrumWindow : public QtCharts::QChartView
{
	Q_OBJECT

public:
	MCASpectrumWindow(QWidget* parent = nullptr);
	~MCASpectrumWindow();

protected:
	void mouseMoveEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
	void mousePressEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
	void mouseReleaseEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
	void mouseDoubleClickEvent(QMouseEvent* event) Q_DECL_OVERRIDE;

private:
	QPointF getSeriesCoordFromChartCoord(const QPointF& chartPos, QtCharts::QAbstractSeries* series) const;
	QPointF getChartCoordFromSeriesCoord(const QPointF& seriesPos, QtCharts::QAbstractSeries* series) const;

	std::tuple<qreal, qreal, qreal, qreal> getAxisNewMinMax(const QRectF& rect) const;

	//void updateBins();
	void resetZoom();

private:
	QtCharts::QBarSeries* bar_series;
	QtCharts::QBarSet* set;
	ConnectWindow* connect_window;
	AcquisitionControlWindow* acq_window;

	QtCharts::QValueAxis* axis_x;
	QtCharts::QValueAxis* axis_y;

	QPointF lastMousePos;

	unsigned max_n_bins;
	unsigned max_bin_h;

	int selected_bin = 0;

	QRubberBand* rb;
	QPoint origin_rb;
	bool mouse_holding = false;

	QColor gcolor;

	// std::thread* nt = nullptr;
	// std::thread* update_auto_axis = nullptr;
	// std::atomic<bool> auto_adjust_on = false;

	QGraphicsRectItem* selected_bar_rect = nullptr;
};