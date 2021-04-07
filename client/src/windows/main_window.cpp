#include "main_window.h"

#include <QLabel>
#include <QToolButton>
#include <QProgressBar>
#include <QMenuBar>
#include <QStatusBar>


#include <DCS_Core/include/DCS_ModuleCore.h>
#include <DCS_EngineControl/include/DCS_ModuleEngineControl.h>
#include <DCS_Utils/include/DCS_ModuleUtils.h>

#include "log_window.h"
#include "hist_window.h"
#include "histsett_window.h"
#include "test_window.h"
#include "connect_window.h"

MainWindow::MainWindow(QApplication* app, QWidget *parent) : QMainWindow(parent)
{
    dock_manager = new ads::CDockManager(this);

    ads::CDockManager::setConfigFlag(ads::CDockManager::FloatingContainerHasWidgetIcon, true);
	ads::CDockManager::setConfigFlag(ads::CDockManager::DockAreaDynamicTabsMenuButtonVisibility, true);

	// Disable style sheet, use own
	dock_manager->setStyleSheet("");

    // Create a dock widget with the title Label 1 and set the created label
    // as the dock widget content
    ads::CDockWidget* log_dock = new ads::CDockWidget("Log");
	log_dock->setWidget(new LogWindow());
	log_dock->setIcon(QIcon(":/png/log_window.png"));
	log_dock->toggleViewAction()->setIcon(QIcon(":/png/log_window.png"));

	ads::CDockWidget* hist_dock = new ads::CDockWidget("Spectrum Graph");
	HistWindow* graph_window = new HistWindow();
	hist_dock->setWidget(graph_window);
	hist_dock->setIcon(QIcon(":/png/spectrum_window.png"));
	hist_dock->toggleViewAction()->setIcon(QIcon(":/png/spectrum_window.png"));

	ads::CDockWidget* hist_sett_dock = new ads::CDockWidget("Graph Settings");
	hist_sett_dock->setWidget(new HistSettingsWindow(graph_window));

	ads::CDockWidget* test_dock = new ads::CDockWidget("Debug Window");
	test_dock->setWidget(new TestWindow(app));
	test_dock->setIcon(QIcon(":/png/debug_window.png"));
	test_dock->toggleViewAction()->setIcon(QIcon(":/png/debug_window.png"));

	ads::CDockWidget* connect_dock = new ads::CDockWidget("Remote Control");
	connect_dock->setWidget(new ConnectWindow());
	connect_dock->setIcon(QIcon(":/png/remote_cont_window.png"));
	connect_dock->toggleViewAction()->setIcon(QIcon(":/png/remote_cont_window.png"));

	auto view_menu = menuBar()->addMenu("View");

	auto opt_menu = menuBar()->addMenu("Options");
	opt_menu->setIcon(QIcon());
	opt_menu->addAction("Settings");

	status_bar = new QStatusBar(this);
	status_bar->setSizeGripEnabled(true);

	status_bar->showMessage("Ready.");

	setStatusBar(status_bar);

	view_menu->addAction(log_dock->toggleViewAction());
	view_menu->addAction(hist_dock->toggleViewAction());
	view_menu->addAction(hist_sett_dock->toggleViewAction());
	view_menu->addAction(test_dock->toggleViewAction());
	view_menu->addAction(connect_dock->toggleViewAction());

    // Add the dock widget to the top dock widget area
    dock_manager->addDockWidget(ads::TopDockWidgetArea, log_dock);
    dock_manager->addDockWidget(ads::TopDockWidgetArea, hist_dock);
    dock_manager->addDockWidget(ads::RightDockWidgetArea, hist_sett_dock);
	auto left_area = dock_manager->addDockWidget(ads::LeftDockWidgetArea, test_dock);
	dock_manager->addDockWidgetTabToArea(connect_dock, left_area);
	//dock_manager->addDockWidget(ads::LeftDockWidgetArea, connect_dock, left_area);
}

MainWindow::~MainWindow()
{
	delete dock_manager;
}

