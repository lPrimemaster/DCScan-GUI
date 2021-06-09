#include "main_window.h"

#include <QLabel>
#include <QToolButton>
#include <QProgressBar>
#include <QMenuBar>
#include <QSettings>


#include <DCS_Core/include/DCS_ModuleCore.h>
#include <DCS_EngineControl/include/DCS_ModuleEngineControl.h>
#include <DCS_Utils/include/DCS_ModuleUtils.h>

#include "log_window.h"
#include "hist_window.h"
#include "histsett_window.h"
#include "test_window.h"
#include "connect_window.h"

void MainWindow::AddGenericWindow(const QString& title, QWidget* window, const QIcon& icon, const QString& menu, const ads::DockWidgetArea area)
{
	ads::CDockWidget* dock = new ads::CDockWidget(title);
	dock->setWidget(window);
	dock->setIcon(icon);
	dock->toggleViewAction()->setIcon(icon);

	auto pmenu = menuBar()->findChild<QMenu*>(menu);

	auto it = menus.find(menu);

	if(it == menus.end())
	{
		LOG_ERROR("Assigning window to a unexsting menu. Ignoring...");
	}
	else
	{
		it.value()->addAction(dock->toggleViewAction());
	}

	dock_manager->addDockWidgetTab(area, dock);

	windows.insert(title, window);
}

void MainWindow::AddMenu(const QString& name)
{
	menus.insert(name, menuBar()->addMenu(name));
}

MainWindow::MainWindow(QApplication* app, QWidget *parent) : QMainWindow(parent)
{
	// Set globally usable app
	this->app = app;

	// Setup dock manager
    dock_manager = new ads::CDockManager(this);
    ads::CDockManager::setConfigFlag(ads::CDockManager::FloatingContainerHasWidgetIcon, true);
	ads::CDockManager::setConfigFlag(ads::CDockManager::DockAreaDynamicTabsMenuButtonVisibility, true);

	// Disable style sheet, use own
	dock_manager->setStyleSheet("");

	// Setup a global status bar
	status_bar = new QStatusBar(this);
	status_bar->setSizeGripEnabled(true);
	setStatusBar(status_bar);
	IssueStatusBarText("Loading windows...");

	// Add menus to top bar
	AddMenu("View");
	AddMenu("Options");

	// Construct all windows
	// And add them to the default view
	auto log_window  	  	   = new LogWindow(this);
	auto log_window_icon  	   = QIcon(":/png/log_window.png");
	AddGenericWindow("Log"			 , log_window	   , log_window_icon      , "View", ads::BottomDockWidgetArea);

	auto hist_window 	  	   = new HistWindow(this);
	auto hist_window_icon  	   = QIcon(":/png/spectrum_window.png");
	AddGenericWindow("Spectrum Graph", hist_window     , hist_window_icon     , "View", ads::TopDockWidgetArea);

	auto histSetts_window 	   = new HistSettingsWindow(this);
	auto histSetts_window_icon = QIcon();
	AddGenericWindow("Graph Details", histSetts_window, histSetts_window_icon, "View", ads::RightDockWidgetArea);

	auto test_window           = new TestWindow(this);
	auto test_window_icon      = QIcon(":/png/debug_window.png");
	AddGenericWindow("Debug Window"  , test_window     , test_window_icon     , "View", ads::LeftDockWidgetArea);

	auto remote_window         = new ConnectWindow(this);
	auto remote_window_icon    = QIcon(":/png/remote_cont_window.png");
	AddGenericWindow("Remote Control", remote_window   , remote_window_icon   , "View", ads::LeftDockWidgetArea);

	IssueStatusBarText("Loading perspectives...");

	// Load user perspectives and set the default one active
	QSettings s("ads_perspectives.ini", QSettings::Format::IniFormat);
	dock_manager->loadPerspectives(s);
	dock_manager->openPerspective("Default");

	IssueStatusBarText("Ready.");

	// Maximize window on app start
	showMaximized();
}

MainWindow::~MainWindow()
{
	delete dock_manager;
}

