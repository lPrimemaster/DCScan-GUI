#include "connect_window.h"
#include "./ui_connect_window.h"

#include "../splash/loading.h"

#include <qDebug>
#include <QTimer>
#include <QThreadPool>

#include "main_window.h"

ConnectWindow::ConnectWindow(QWidget* parent) : ui(new Ui::ConnectWindow)
{
	DCS::Network::Init();

	ui->setupUi(this);
	ui->status_led->setState(QIndicator::State::RED);

	connection_status = new QIndicator(this);
	connection_status->setMinimumSize(12, 12);

	QWidget* connection_indicator = new QWidget(this);
	connection_indicator->setLayout(new QHBoxLayout());
	//QSpacerItem *item = new QSpacerItem(1, 0, QSizePolicy::Minimum, QSizePolicy::Maximum);
	//dynamic_cast<QHBoxLayout*>(connection_indicator->layout())->addSpacerItem(item);

	QLabel* txt_lbl = new QLabel("Network Status", this);
	connection_indicator->layout()->addWidget(txt_lbl);
	connection_indicator->layout()->addWidget(connection_status);
	connection_indicator->layout()->setContentsMargins(0, 0, 0, 0);
	connection_indicator->setToolTip("Displays server connection status.");
	
	
	connection_status->setState(QIndicator::State::RED);
	dynamic_cast<MainWindow*>(parent)->AddStatusWidget("ConnectionIndicator", connection_indicator);

	cd = new ConnectDialog(this);
	cd->setModal(true);

	(void)connect(ui->remote_connect_button, SIGNAL(clicked()), cd, SLOT(exec()));
	(void)connect(cd, SIGNAL(credentialsSignal(QString, QString)), this, SLOT(connectToServer(QString, QString)));
	(void)connect(this, SIGNAL(connectionChanged(bool)), this, SLOT(changeSettings(bool)));
}

bool ConnectWindow::isNetworkConnected()
{
	return net_connected;
}

void ConnectWindow::enableLayout(bool e)
{
	ui->lineEdit->setEnabled(e);
	ui->lineEdit_2->setEnabled(e);
}

void ConnectWindow::changeSettings(bool v)
{
	if(v)
	{
		ui->remote_connect_button->setText("Disconnect");
		ui->status_led->setState(QIndicator::State::GREEN);
		connection_status->setState(QIndicator::State::GREEN);
		enableLayout(false);
		(void)disconnect(ui->remote_connect_button, SIGNAL(clicked()), cd, SLOT(exec()));
		(void)connect(ui->remote_connect_button, SIGNAL(clicked()), this, SLOT(disconnectFromServer()));
	}
	else
	{
		ui->remote_connect_button->setText("Connect");
		ui->status_led->setState(QIndicator::State::RED);
		connection_status->setState(QIndicator::State::RED);
		enableLayout(true);
		(void)connect(ui->remote_connect_button, SIGNAL(clicked()), cd, SLOT(exec()));
		(void)disconnect(ui->remote_connect_button, SIGNAL(clicked()), this, SLOT(disconnectFromServer()));
	}
}

void ConnectWindow::runNet(QString username, QString password)
{
	QString ip_add = ui->lineEdit->text();
	QString port = ui->lineEdit_2->text();

	LoadingSplash::SetWorkingStatus("Connecting to server...");

	socket = DCS::Network::Client::Connect(ip_add.toLatin1().constData(), port.toInt());

	DCS::Network::Client::Authenticate(socket, username.toLatin1().constData(), password.toLatin1().constData());

	qDebug() << "Connect OK || SOCKET id = " << (uint)socket;

	if (DCS::Network::Client::StartThread(socket))
	{
		net_connected = true;
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
		qDebug() << "Connection established!";
		emit connectionChanged(true);
	}
	else
	{
		net_connected = false;
		qDebug() << "Failed to connect to server at: " << ip_add;
	}

	//LoadingSplash::Finish();
}

void ConnectWindow::connectToServer(QString username, QString password)
{
	// TODO : Fix this!
	//LoadingSplash::Start(this);
	//LoadingSplash::SetWorkingStatus("Initializing...");

	QThreadPool::globalInstance()->start(std::bind(&ConnectWindow::runNet, this, username, password));
}

void ConnectWindow::disconnectFromServer()
{
	if(net_connected)
	{
		net_connected = false;
		emit connectionChanged(false);
		DCS::Network::Client::StopThread(socket);
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
}

void ConnectWindow::stopSplash()
{
	LoadingSplash::Finish();
}

ConnectWindow::~ConnectWindow()
{
	disconnectFromServer();
	DCS::Network::Destroy();
	delete ui;
}
