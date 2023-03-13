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

#include <mutex>

#include <DCS_Core/include/DCS_ModuleCore.h>
#include <DCS_EngineControl/include/DCS_ModuleEngineControl.h>
#include <DCS_Utils/include/DCS_ModuleUtils.h>
#include <DCS_Network/include/DCS_ModuleNetwork.h>

#include "../dialogs/connect_dialog.h"
#include "../extras/qindicator.h"
#include "../utils/worker.h"

UI_AUTOGEN(ConnectWindow)

class ConnectWindow : public QWidget
{
	Q_OBJECT

public:
	ConnectWindow(QWidget* parent = nullptr);
	~ConnectWindow();

	bool isNetworkConnected();

	void setNetworkConnected(bool value)
	{
		std::lock_guard<std::mutex> lock(net_mtx);
		net_connected = value;
	}

private:
	void enableLayout(bool e);

public slots:
	void connectToServer(QString username, QString password);
	void disconnectFromServer();
	void disconnectFromServerWait();
	void changeSettings(bool v);

signals:
	void connectionChanged(bool connected);

private:
	SimpleThread* disconnectFromServerInternal();

private:
	Ui::ConnectWindow* ui;
	ConnectDialog* cd;
	QIndicator* connection_status;

	bool net_connected = false;
	std::mutex net_mtx;

	DCS::Network::Socket socket;
};
