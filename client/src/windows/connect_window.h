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

UI_AUTOGEN(ConnectWindow)

class ConnectWindow : public QWidget
{
	Q_OBJECT

public:
	ConnectWindow(QWidget* parent = nullptr);
	~ConnectWindow();

private:
	void runNet(QString username, QString password);

public slots:
	void connectToServer(QString username, QString password);
	void stopSplash();

private:
	Ui::ConnectWindow* ui;
};
