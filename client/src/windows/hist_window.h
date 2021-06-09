#pragma once

#include <tuple>

#include <QWidget>
#include <QChart>
#include <QBarSeries>
#include <QChartView>
#include <QAbstractSeries>
#include <QValueAxis>
#include <QPair>

#include <atomic>

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

	const int getMaxBins() const
	{
		return max_n_bins;
	}

protected:
	void mouseMoveEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
	void mousePressEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
	void mouseReleaseEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
	void mouseDoubleClickEvent(QMouseEvent* event) Q_DECL_OVERRIDE;

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
	void adjustGraphAxisRange(bool auto_adj);

signals:
	void changeAxisBinSig(int nbins);
	void incrementBinValueSig(int idx);
	void changeSelectionSig(int bin, int count);

private:
	qreal barMaximum();

private:
	QtCharts::QBarSeries* bar_series;
	QtCharts::QChart* chart;
	QPointF lastMousePos;

	unsigned max_n_bins;
	unsigned max_bin_h;

	int selected_bin = 0;

	QtCharts::QValueAxis* axis_x;
	QtCharts::QValueAxis* axis_y;

	QRubberBand* rb;
	QPoint origin_rb;
	bool mouse_holding = false;

	QColor gcolor;

	std::thread* nt = nullptr;
	std::thread* update_auto_axis = nullptr;
	std::atomic<bool> auto_adjust_on = false;

	QGraphicsRectItem* selected_bar_rect = nullptr;
};
