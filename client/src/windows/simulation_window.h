#pragma once

#include "../autogen_macros.h"

#include <QWidget>
#include <QTabWidget>
#include <QLabel>

#ifdef DCSSIMU_FOUND
#include <DCSsimu/dcs_gui_new.h>
#endif

UI_AUTOGEN(SimulationWindow)

class QHBoxLayout;

class SimulationWindow : public QWidget
{
	Q_OBJECT
public:
	SimulationWindow(QWidget* parent = nullptr);
	~SimulationWindow();

private:
    Ui::SimulationWindow* ui;
#ifdef DCSSIMU_FOUND
    GUISettingsWindow settings_tab;
#else
	QHBoxLayout* message_layout;
#endif
};
