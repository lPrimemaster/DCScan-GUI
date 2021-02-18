#pragma once

#include <QMainWindow>
#include <QTextBrowser>

#include <qtadvanceddocking/DockManager.h>

#include <DCS_Core/include/DCS_ModuleCore.h>
#include <DCS_EngineControl/include/DCS_ModuleEngineControl.h>
#include <DCS_Utils/include/DCS_ModuleUtils.h>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    ads::CDockManager* dock_manager;
	QStatusBar* status_bar;
};
