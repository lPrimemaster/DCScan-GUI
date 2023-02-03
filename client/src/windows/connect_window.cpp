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
	std::lock_guard<std::mutex> lock(net_mtx);
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

static void ConnectNet(QString username, QString password, QString ip, QString port, DCS::Network::Socket socket, ConnectWindow* window, LoadingSplash* splash)
{
	emit splash->changeDisplayMessage("Connecting to server...");

	socket = DCS::Network::Client::Connect(ip.toLatin1().constData(), port.toInt());

	DCS::Network::Client::Authenticate(socket, username.toLatin1().constData(), password.toLatin1().constData());

	qDebug() << "Connect OK";

	if (DCS::Network::Client::StartThread(socket))
	{
		window->setNetworkConnected(true);
		qDebug() << "Connected at: " << ip;

		std::this_thread::sleep_for(std::chrono::milliseconds(2000));

		unsigned char buffer[4096];
		auto size_written = DCS::Registry::SVParams::GetDataFromParams(buffer,
			SV_CALL_DCS_Threading_GetMaxHardwareConcurrency
		);

		qDebug() << "Waiting...";

		auto max_threads_srv_b = DCS::Network::Message::SendSync(DCS::Network::Message::Operation::REQUEST, buffer, size_written);
		auto max_threads_srv = *(DCS::u16*)max_threads_srv_b.ptr;

		qDebug() << "Got server max thread concurrency: " << max_threads_srv;
		qDebug() << "Connection established!";
		emit window->connectionChanged(true);
	}
	else
	{
		window->setNetworkConnected(false);
		qDebug() << "Failed to connect to server at: " << ip;
	}
}

void ConnectWindow::connectToServer(QString username, QString password)
{
	auto ss = LoadingSplash::Start(this);
	SimpleThread *workerThread = new SimpleThread(std::bind(&ConnectNet, username, password, ui->lineEdit->text(), ui->lineEdit_2->text(), socket, this, ss), this);
    (void)connect(workerThread, &SimpleThread::jobDone, ss, &LoadingSplash::finishExec);
    (void)connect(workerThread, &SimpleThread::finished, workerThread, &QObject::deleteLater);
    workerThread->start();
}

static void DisconnectLocal(DCS::Network::Socket socket)
{
	DCS::Network::Client::StopThread(socket);
	std::this_thread::sleep_for(std::chrono::milliseconds(2000));
}

SimpleThread* ConnectWindow::disconnectFromServerInternal()
{
	// TODO : Cancel any jobs if they are running
	if(net_connected)
	{
		net_connected = false;
		emit connectionChanged(false);
		auto ss = LoadingSplash::Start(this);
		ss->setWorkingStatus("Disconnecting from server...");
		SimpleThread *workerThread = new SimpleThread(std::bind(&DisconnectLocal, socket), this);
		(void)connect(workerThread, &SimpleThread::jobDone, ss, &LoadingSplash::finishExec);
		(void)connect(workerThread, &SimpleThread::finished, workerThread, &QObject::deleteLater);
		workerThread->start();
		return workerThread;
	}
	return nullptr;
}

void ConnectWindow::disconnectFromServer()
{
	(void)disconnectFromServerInternal();
}

void ConnectWindow::disconnectFromServerWait()
{
	disconnectFromServerInternal()->wait();
}

ConnectWindow::~ConnectWindow()
{
	LOG_DEBUG("~ConnectWindow()");
	disconnectFromServerWait();
	DCS::Network::Destroy();
	delete ui;
}
