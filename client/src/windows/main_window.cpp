#include "main_window.h"

#include <QLabel>
#include <QToolButton>
#include <QProgressBar>
#include <QMenuBar>
#include <QSettings>
#include <QBitmap>
#include <QApplication>


#include <DCS_Core/include/DCS_ModuleCore.h>
#include <DCS_EngineControl/include/DCS_ModuleEngineControl.h>
#include <DCS_Utils/include/DCS_ModuleUtils.h>

#include "log_window.h"
#include "hist_window.h"
#include "test_window.h"
#include "connect_window.h"
#include "freejog_window.h"
#include "position_window.h"
#include "channelviz_window.h"
#include "layout_window.h"
#include "spectraldisplay_window.h"
#include "mcaspectrum_window.h"
#include "acquisitioncontrol_window.h"
#include "clinometer_window.h"

void MainWindow::AddSeparator(const QString& menu)
{
	auto pmenu = menuBar()->findChild<QMenu*>(menu);

	auto it = menus.find(menu);

	if(it == menus.end())
	{
		LOG_ERROR("Assigning separator to an unexsting menu. Ignoring...");
	}
	else
	{
		it.value()->addSeparator();
	}
}

void MainWindow::AddGenericAction(QAction* action, const QIcon& icon, const QString& menu)
{
	auto pmenu = menuBar()->findChild<QMenu*>(menu);

	auto it = menus.find(menu);

	if(it == menus.end())
	{
		LOG_ERROR("Assigning action to an unexsting menu. Ignoring...");
	}
	else
	{
		it.value()->addAction(action);
	}
}

void MainWindow::InsertLayout(QAction* action, const QIcon& icon, const QString& menu)
{
	auto pmenu = menuBar()->findChild<QMenu*>(menu);

	auto it = menus.find(menu);

	if(it == menus.end())
	{
		LOG_ERROR("Assigning action to an unexsting menu. Ignoring...");
	}
	else
	{
		auto as = it.value()->actions();
		auto ait = std::find_if(as.begin(), as.end(), [&](const QAction* val) -> bool {
			return val->text() == lastLayout;
		});

		if(ait != as.end())
		{
			it.value()->insertAction(*ait, action);
		}
	}
}

void MainWindow::RemoveLayout(const QString& action, const QString& menu)
{
	auto pmenu = menuBar()->findChild<QMenu*>(menu);

	auto it = menus.find(menu);

	if(it == menus.end())
	{
		LOG_ERROR("Removing action from an unexsting menu. Ignoring...");
	}
	else
	{
		auto as = it.value()->actions();
		auto ait = std::find_if(as.begin(), as.end(), [&](const QAction* val) -> bool {
			return val->text() == action;
		});

		if(ait != as.end())
		{
			it.value()->removeAction(*ait);
		}
	}
}

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
		LOG_ERROR("Assigning window to an unexsting menu. Ignoring...");
	}
	else
	{
		it.value()->addAction(dock->toggleViewAction());
	}

	dock_manager->addDockWidgetTab(area, dock);

	windows.insert(title, window);
	docks.insert(title, dock);
}

void MainWindow::AddMenu(const QString& name)
{
	menus.insert(name, menuBar()->addMenu(name));
}

void MainWindow::SetSubWIcon(const QString& title, QIcon icon)
{
	docks.value(title)->setIcon(icon);
}

MainWindow::MainWindow(QApplication* app, QWidget *parent) : QMainWindow(parent)
{
	// Set globally usable app
	this->app = app;

	// Setup dock manager
    dock_manager = new ads::CDockManager(this);
    ads::CDockManager::setConfigFlag(ads::CDockManager::FloatingContainerHasWidgetIcon, true);
    ads::CDockManager::setConfigFlag(ads::CDockManager::AlwaysShowTabs, false);
	ads::CDockManager::setConfigFlag(ads::CDockManager::DockAreaDynamicTabsMenuButtonVisibility, true);
	ads::CDockManager::setConfigFlag(ads::CDockManager::FloatingContainerHasWidgetTitle, true);

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
	AddMenu("Layout");

	// Construct all windows
	// And add them to the default view
	auto log_window  	  	   = new LogWindow(this);
	auto log_window_icon  	   = QIcon(":/png/log_window.png");
	AddGenericWindow("Log"			 , log_window	   , log_window_icon      , "View", ads::BottomDockWidgetArea);

	auto hist_window 	  	   = new HistWindow(this);
	auto hist_window_icon  	   = QIcon(":/png/spectrum_window.png");
	AddGenericWindow("Spectrum Graph", hist_window     , hist_window_icon     , "View", ads::TopDockWidgetArea);

	auto test_window           = new TestWindow(this);
	auto test_window_icon      = QIcon(":/png/debug_window.png");
	AddGenericWindow("Debug Window"  , test_window     , test_window_icon     , "View", ads::LeftDockWidgetArea);

	auto remote_window         = new ConnectWindow(this);
	auto remote_window_icon    = QIcon(":/png/remote_cont_window.png");
	AddGenericWindow("Remote Control", remote_window   , remote_window_icon   , "View", ads::LeftDockWidgetArea);

	auto freejog_window         = new FreejogWindow(this);
	auto freejog_window_icon    = QIcon(":/png/freejog_window.png");
	AddGenericWindow("Free Jog Control", freejog_window , freejog_window_icon , "View", ads::LeftDockWidgetArea);

	auto position_window         = new PositionWindow(this);
	auto position_window_icon    = QIcon(":/png/position_window.png");
	AddGenericWindow("Position Monitoring", position_window , position_window_icon , "View", ads::LeftDockWidgetArea);

	auto channelviz_window         = new ChannelvizWindow(this);
	auto channelviz_window_icon    = QIcon(":/png/channelviz_window.png");
	AddGenericWindow("Channel Visualizer", channelviz_window , channelviz_window_icon , "View", ads::NoDockWidgetArea);

	auto spectraldisplay_window         = new SpectralDisplayWindow(this);
	auto spectraldisplay_window_icon    = QIcon(":/png/spectraldisplay_window.png");
	AddGenericWindow("Acquisition Details", spectraldisplay_window , spectraldisplay_window_icon , "View", ads::NoDockWidgetArea);

	auto acquisitioncontrol_window         = new AcquisitionControlWindow(this);
	auto acquisitioncontrol_window_icon    = QIcon(":/png/acquisitioncontrol_window.png");
	AddGenericWindow("Acquisition Control", acquisitioncontrol_window , acquisitioncontrol_window_icon , "View", ads::NoDockWidgetArea);

	auto mcaspectrum_window         = new MCASpectrumWindow(this);
	auto mcaspectrum_window_icon    = QIcon(":/png/mcaspectrum_window.png");
	AddGenericWindow("MCA Spectrum", mcaspectrum_window , mcaspectrum_window_icon , "View", ads::NoDockWidgetArea);

	auto clinometer_window         = new ClinometerWindow(this);
	auto clinometer_window_icon    = QIcon(":/png/clinometer_window.png"); // TODO: arranjar um ícone para a janela
	AddGenericWindow("Clinometer Monitoring", clinometer_window , clinometer_window_icon , "View", ads::NoDockWidgetArea);

	IssueStatusBarText("Loading perspectives...");

	// Load user perspectives and set the default one active
	QSettings s("ads_perspectives.ini", QSettings::Format::IniFormat);
	dock_manager->loadPerspectives(s);

	UpdatePerspectives();

	AddSeparator("Layout");

	auto layout_window         = new LayoutWindow(this);
	auto layout_window_icon    = QIcon(":/png/layout_window.png");
	AddGenericWindow("Layout Manager", layout_window , layout_window_icon , "Layout", ads::NoDockWidgetArea);

	dock_manager->openPerspective("Default");

	IssueStatusBarText("Ready.");

	// Maximize window on app start
	showMaximized();
}

MainWindow::~MainWindow()
{
	delete dock_manager;
}

