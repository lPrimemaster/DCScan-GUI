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
#include <QLineSeries>
#include <QValueAxis>

#include <DCS_Core/include/DCS_ModuleCore.h>
#include <DCS_EngineControl/include/DCS_ModuleEngineControl.h>
#include <DCS_Utils/include/DCS_ModuleUtils.h>
#include <DCS_Network/include/DCS_ModuleNetwork.h>

#include "main_window.h"
#include "spectraldisplay_window.h"

UI_AUTOGEN(SpectralDisplayWindow)

// TODO : Allow this to know the current acquisition state and disable channel visualization
class SpectralDisplayWindow : public QWidget
{
	Q_OBJECT

public:
	SpectralDisplayWindow(QWidget* parent = nullptr);
	~SpectralDisplayWindow();

private:
	Ui::SpectralDisplayWindow* ui;
};