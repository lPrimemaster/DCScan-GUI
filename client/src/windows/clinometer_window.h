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
#include "connect_window.h"

UI_AUTOGEN(ClinometerWindow)

class ClinometerWindow : public QWidget
{
	Q_OBJECT

public:
	ClinometerWindow(QWidget* parent = nullptr);
	~ClinometerWindow();

public slots:
    void requestAIStart();
    void requestAIStop();

private:
	Ui::ClinometerWindow* ui;
};