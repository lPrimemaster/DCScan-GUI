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
#include <QTimer>

#include <DCS_Core/include/DCS_ModuleCore.h>
#include <DCS_EngineControl/include/DCS_ModuleEngineControl.h>
#include <DCS_Utils/include/DCS_ModuleUtils.h>
#include <DCS_Network/include/DCS_ModuleNetwork.h>

#include "main_window.h"
#include "position_window.h"
#include "connect_window.h"

UI_AUTOGEN(PositionWindow)

class PositionWindow : public QWidget
{
	Q_OBJECT

public:
	PositionWindow(QWidget* parent = nullptr);
	~PositionWindow();

public slots:
    void update();
    void resetTimer();

private:
	Ui::PositionWindow* ui;
    QTimer* timer;
    ConnectWindow* connect_window;
};
