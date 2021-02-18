#pragma once

#include <tuple>

#include <QWidget>
#include <QChart>
#include <QBarSeries>
#include <QChartView>
#include <QAbstractSeries>
#include <QValueAxis>
#include <QPair>

#include <DCS_Core/include/DCS_ModuleCore.h>
#include <DCS_EngineControl/include/DCS_ModuleEngineControl.h>
#include <DCS_Utils/include/DCS_ModuleUtils.h>

#include "histsett_window.h"


class HistWindow : public QtCharts::QChartView
{
	Q_OBJECT

public:
	HistWindow(QWidget* parent = nullptr);

	~HistWindow();

protected:
	void mouseMoveEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
	void mousePressEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
	void mouseReleaseEvent(QMouseEvent* event) Q_DECL_OVERRIDE;

private:
	QPointF getSeriesCoordFromChartCoord(const QPointF& chartPos, QtCharts::QAbstractSeries* series) const;
	QPointF getChartCoordFromSeriesCoord(const QPointF& seriesPos, QtCharts::QAbstractSeries* series) const;

	std::tuple<qreal, qreal, qreal, qreal> getAxisNewMinMax(const QRectF& rect) const;

	void updateBins();
	void resetZoom();
	
public slots:
	void setAxisBinRange(int nbins);
	void updateAllSettings(GraphSettings settings);
	void updateBin(int idx);
	void adjustGraphAxisRange();

signals:
	void changeAxisBinSig(int nbins);
	void incrementBinValueSig(int idx);


private:
	qreal barMaximum();

private:
	QtCharts::QBarSeries* bar_series;
	QtCharts::QChart* chart;
	QPointF lastMousePos;

	unsigned max_n_bins;
	unsigned max_bin_h;

	QtCharts::QValueAxis* axis_x;
	QtCharts::QValueAxis* axis_y;

	QRubberBand* rb;
	QPoint origin_rb;
	bool mouse_holding = false;

	std::thread* nt = nullptr;
};
