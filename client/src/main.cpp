#include "windows/main_window.h"
#include "windows/histsett_window.h"

#include <QApplication>
#include <QDebug>
#include <QThread>
#include <QStyleFactory>
#include <QFile>

using namespace DCS::Utils;
using namespace DCS::Network;

int main(int argc, char *argv[])
{
	Logger::Init(Logger::Verbosity::DEBUG);

	Init();

	Socket c = Client::Connect("127.0.0.1", 15777);

	Client::StartThread(c);

	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	unsigned char buffer[1024];
	auto size_written = DCS::Registry::SVParams::GetDataFromParams(buffer,
		SV_CALL_DCS_Threading_GetMaxHardwareConcurrency
	);

	auto max_threads_srv_b = Message::SendSync(Message::Operation::REQUEST, buffer, size_written);
	auto max_threads_srv = *(DCS::u16*)max_threads_srv_b.ptr;

	LOG_DEBUG("Got server max thread concurrency: %d", max_threads_srv);

	size_written = DCS::Registry::SetupEvent(buffer, SV_EVT_OnTestFibSeq, [] (DCS::u8* data) {
		LOG_DEBUG("FibEvent returned: %llu", *(DCS::u64*)data);
	});

	Message::SendAsync(Message::Operation::EVT_SUB, buffer, size_written);

	std::this_thread::sleep_for(std::chrono::milliseconds(30000));

	Client::StopThread(c);

	Destroy();

    QApplication a(argc, argv);

	// Register custom signal data
	qRegisterMetaType<GraphSettings>("GraphSettings");

    MainWindow w;

	QFile f("../../../client/styles/default.css"); // TODO : In a proper env this needs to be replaced for a app recognized path
	if (!f.open(QFile::ReadOnly | QFile::Text))
	{
		LOG_ERROR("Failed to load stylesheet. Maybe location is wrong?");
	}
	else
	{
		QTextStream in(&f);

		if (f.size() > 0)
		{
			LOG_MESSAGE("Loaded custom stylesheet.");
		}

		QString s = in.readAll();
		a.setStyleSheet(s);
	}

    w.show();
	const int retval = a.exec();

	Logger::Destroy();

	return retval;
}
