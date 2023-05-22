#pragma once

#include "../autogen_macros.h"

#include <QWidget>
#include <QTabWidget>
#include <DCSsimu/dcs_gui_new.h>

UI_AUTOGEN(SimulationWindow)

class SimulationWindow : public QWidget
{
	Q_OBJECT
public:
	SimulationWindow(QWidget* parent = nullptr);
	~SimulationWindow();

private:
    Ui::SimulationWindow* ui;
    GUISettingsWindow settings_tab;
};
