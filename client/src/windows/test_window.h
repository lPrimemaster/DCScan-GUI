#pragma once

#include "../autogen_macros.h"

#include <QTabWidget>
#include <QWidget>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpinBox>
#include <QComboBox>
#include <QLineEdit>
#include <QLabel>

#include <DCS_Core/include/DCS_ModuleCore.h>
#include <DCS_EngineControl/include/DCS_ModuleEngineControl.h>
#include <DCS_Utils/include/DCS_ModuleUtils.h>

#include "main_window.h"

UI_AUTOGEN(TestWindow)

class TestWindow : public QWidget
{
	Q_OBJECT

public:
	TestWindow(QWidget* parent = nullptr);
	~TestWindow();

	void applyStyle();

	void overrideADSDefaultPerspective();

private:
	Ui::TestWindow* ui;
	QApplication* a;

	ads::CDockManager* manager;
	MainWindow* mainwindow;
};
