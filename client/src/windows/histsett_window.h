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
#include <QCheckBox>

#include <DCS_Core/include/DCS_ModuleCore.h>
#include <DCS_EngineControl/include/DCS_ModuleEngineControl.h>
#include <DCS_Utils/include/DCS_ModuleUtils.h>

class HistWindow;

namespace QtUtils
{
	inline static QIcon CreateBasicColorIcon(const QString& color_name)
	{
		QPixmap pixmap(100, 100);
		pixmap.fill(QColor(color_name));
		return QIcon(pixmap);
	}

	inline static QIcon CreateBasicColorIcon(const char* color_name)
	{
		QPixmap pixmap(100, 100);
		pixmap.fill(QColor(color_name));
		return QIcon(pixmap);
	}

	inline static QIcon CreateBasicColorIcon(const QColor& color)
	{
		QPixmap pixmap(100, 100);
		pixmap.fill(color);
		return QIcon(pixmap);
	}
}

struct GraphSettings
{
	int maximum_bins = 0;
	QColor bins_color;
	int bins_color_idx = 0;
};

class HistSettingsWindow : public QWidget
{
	Q_OBJECT

public:
	HistSettingsWindow(QWidget* parent = nullptr);
	~HistSettingsWindow() = default;

public slots:
	void updateAllSettingsValue();
	void resetAllSettingsValue();

	void updateStatusAcqRunning(bool state);
	void updateGraphView(bool auto_adj);

	void updateSelection(int bin, int count);

signals:
	void settingsChangedSig(GraphSettings settings);
	void axisViewAdjustSig(bool auto_adj);

protected:
	void hideEvent(QHideEvent* event) Q_DECL_OVERRIDE;

private:
	struct SettingsWidgets
	{
		QSpinBox* max_bin_spinBox = nullptr;
		QComboBox* bin_color_comboBox = nullptr;
		QLabel* bin_selected = nullptr;
		QLabel* bin_count = nullptr;
		QLabel* bin_time = nullptr;
		QLabel* bin_angle = nullptr;
	}widgets;

	GraphSettings settings;

private:
	QHBoxLayout* tab_bot;
	QVBoxLayout* parent_layout;
	QCheckBox* auto_reset_barGraph_window;

	QLabel* warning_label;
};
