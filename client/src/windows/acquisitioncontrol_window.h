#pragma once

#include "../autogen_macros.h"

#include <QTabWidget>
#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpinBox>
#include <QComboBox>
#include <QLineEdit>
#include <QLabel>
#include <QTimer>
#include <QLineSeries>
#include <QValueAxis>

#include <DCS_Core/include/DCS_ModuleCore.h>
#include <DCS_EngineControl/include/DCS_ModuleEngineControl.h>
#include <DCS_Utils/include/DCS_ModuleUtils.h>
#include <DCS_Network/include/DCS_ModuleNetwork.h>

#include "main_window.h"
#include "connect_window.h"
#include "spectraldisplay_window.h"
// #include "position_window.h"

UI_AUTOGEN(AcquisitionControlWindow)

// TODO : Allow this to know the current acquisition state and disable channel visualization
class AcquisitionControlWindow : public QWidget
{
	Q_OBJECT

public:
	AcquisitionControlWindow(QWidget* parent = nullptr);
	~AcquisitionControlWindow();

private:
	Ui::AcquisitionControlWindow* ui;
	QMap<QString, int> channels;

	bool local_net_status = false;
	bool working = false;
};