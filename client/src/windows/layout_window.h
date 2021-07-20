#pragma once

#include "../autogen_macros.h"

#include <QWidget>
#include <QString>

#include <DCS_Core/include/DCS_ModuleCore.h>
#include <DCS_EngineControl/include/DCS_ModuleEngineControl.h>
#include <DCS_Utils/include/DCS_ModuleUtils.h>
#include <DCS_Network/include/DCS_ModuleNetwork.h>

#include "main_window.h"

UI_AUTOGEN(LayoutWindow)

// TODO : Implement an overwrite button for non-default layouts
class LayoutWindow : public QWidget
{
	Q_OBJECT

public:
	LayoutWindow(QWidget* parent = nullptr);
	~LayoutWindow();

private:
	Ui::LayoutWindow* ui;
    MainWindow* main_window;
	std::vector<QString> perspectives;
};