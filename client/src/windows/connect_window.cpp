#include "connect_window.h"
#include "./ui_connect_window.h"

#include "../dialogs/connect_dialog.h"
#include "../splash/loading.h"

#include <qDebug>
#include <QTimer>
#include <QThreadPool>

#include <DCS_Network/include/DCS_ModuleNetwork.h>

ConnectWindow::ConnectWindow(QWidget* parent) : ui(new Ui::ConnectWindow)
{
	DCS::Network::Init();

	ui->setupUi(this);

	// Auto delete via parenting
	//ConnectDialog* cd = new ConnectDialog(this);
	
	//cd->setModal(true);
	(void)connect(ui->remote_connect_button, SIGNAL(clicked()), this, SLOT(connectToServer()));
}

void ConnectWindow::runNet()
{
	QString ip_add = ui->lineEdit->text();

	LoadingSplash::SetWorkingStatus("Connecting to server...");
	
	auto connection = DCS::Network::Client::Connect(ip_add.toLatin1().constData(), 15777);

	qDebug() << "Connect OK || SOCKET id = " << (uint)connection;

	if (DCS::Network::Client::StartThread(connection))
	{
		qDebug() << "Connected at: " << ip_add;

		std::this_thread::sleep_for(std::chrono::milliseconds(2000));

		unsigned char buffer[1024];
		auto size_written = DCS::Registry::SVParams::GetDataFromParams(buffer,
			SV_CALL_DCS_Threading_GetMaxHardwareConcurrency
		);

		qDebug() << "Waiting...";

		auto max_threads_srv_b = DCS::Network::Message::SendSync(DCS::Network::Message::Operation::REQUEST, buffer, size_written);
		auto max_threads_srv = *(DCS::u16*)max_threads_srv_b.ptr;

		qDebug() << "Got server max thread concurrency: " << max_threads_srv;

		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		DCS::Network::Client::StopThread(connection);
	}
	else
	{
		qDebug() << "Failed to connect to server at: " << ip_add;
	}

	LoadingSplash::Finish();
}

// NOTE : This is just a test function
void ConnectWindow::connectToServer()
{
	LoadingSplash::Start(this);
	LoadingSplash::SetWorkingStatus("Initializing...");

	QThreadPool::globalInstance()->start(std::bind(&ConnectWindow::runNet, this));
}

void ConnectWindow::stopSplash()
{
	LoadingSplash::Finish();
}

ConnectWindow::~ConnectWindow()
{
	DCS::Network::Destroy();
	delete ui;
}
