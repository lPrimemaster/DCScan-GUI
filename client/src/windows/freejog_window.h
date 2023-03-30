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
#include <DCS_Network/include/DCS_ModuleNetwork.h>

#include "main_window.h"
#include "connect_window.h"

UI_AUTOGEN(FreejogWindow)

class FreejogWindow : public QWidget
{
	Q_OBJECT

public:
	FreejogWindow(QWidget* parent = nullptr);
	~FreejogWindow();

public slots:
	void enableFreejog(bool e);

	void moveEngine1To();
	void moveEngine2To();

	void moveTiltTo(int stage, int axis, QSpinBox* spin_obj);

	void updatePIDParams();

	void enableMotion(int stage, bool enable);

signals:
	void enableMotionSignal(int stage, bool enable);

private:
	Ui::FreejogWindow* ui;

	bool is_enabled = false;
};
