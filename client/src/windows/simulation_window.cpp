#include "simulation_window.h"
#include "./ui_simulation_window.h"
#include <QHBoxLayout>
#include <QVBoxLayout>

SimulationWindow::SimulationWindow(QWidget* parent) : ui(new Ui::SimulationWindow)
#ifdef DCSSIMU_FOUND
	, settings_tab(this)
#endif
{
#ifdef DCSSIMU_FOUND
	ui->setupUi(this);
    ui->tabWidget->insertTab(0, &settings_tab, "Simulation Settings");
	ui->tabWidget->insertTab(1, settings_tab.getSimulationWindow(), "Simulation Plots");
#else
	QLabel* message = new QLabel("Simulation package is missing.\nPlease install it from the DCSimu repository.", this);
	QLabel* icon = new QLabel(this);
	icon->setPixmap(QPixmap(":/png/warning.png"));
	icon->setEnabled(false);
	QVBoxLayout* window_layout = new QVBoxLayout(this);
	message_layout = new QHBoxLayout();
	message_layout->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
	message_layout->addWidget(icon);
	QFont font;
	font.setPointSize(12);
	message->setFont(font);
	message->setEnabled(false);
	message_layout->addWidget(message);
	message_layout->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
	window_layout->addLayout(message_layout);
#endif
}

SimulationWindow::~SimulationWindow()
{
#ifndef DCSSIMU_FOUND
	delete message_layout;
#endif
	delete ui;
}
