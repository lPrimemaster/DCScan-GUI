#include "simulation_window.h"
#include "./ui_simulation_window.h"

SimulationWindow::SimulationWindow(QWidget* parent) : ui(new Ui::SimulationWindow), settings_tab(this)
{
	ui->setupUi(this);
    ui->tabWidget->insertTab(0, &settings_tab, "Simulation Settings");
	ui->tabWidget->insertTab(1, settings_tab.getSimulationWindow(), "Simulation Plots");
}

SimulationWindow::~SimulationWindow()
{
	delete ui;
}
