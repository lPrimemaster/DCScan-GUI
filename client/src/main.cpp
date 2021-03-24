#include "windows/main_window.h"
#include "windows/histsett_window.h"

#include <fstream>
#include <iterator>

#include <QApplication>
#include <QDebug>
#include <QThread>
#include <QStyleFactory>
#include <QFile>

using namespace DCS::Utils;
using namespace DCS::Network;

static std::vector<float> RunToWaitPoll(int rep, float acc_min = 1.0f, float acc_max = 200.0f)
{
	LOG_DEBUG("Starting RotDev test battery...");

	unsigned char buffer[1024];
	auto size_written = DCS::Registry::SVParams::GetDataFromParams(buffer,
		SV_CALL_DCS_Control_IssueGenericCommand,
		DCS::Control::UnitTarget::ESP301,
		DCS::Utils::BasicString{ "2MO;2OR1;2WS;2AU200;" }
		//DCS::Utils::BasicString{ "2MO;2PA0.0;" }
	);
	Message::SendSync(Message::Operation::REQUEST, buffer, size_written);

	size_written = DCS::Registry::SVParams::GetDataFromParams(buffer,
		SV_CALL_DCS_Control_IssueGenericCommandResponse,
		DCS::Control::UnitTarget::ESP301,
		DCS::Utils::BasicString{ "2ZH?;" }
	);
	auto ret = Message::SendSync(Message::Operation::REQUEST, buffer, size_written);
	LOG_DEBUG("ZH: %s", (*(DCS::Utils::BasicString*)ret.ptr));

	size_written = DCS::Registry::SVParams::GetDataFromParams(buffer,
		SV_CALL_DCS_Control_IssueGenericCommandResponse,
		DCS::Control::UnitTarget::ESP301,
		DCS::Utils::BasicString{ "2ZS?;" }
	);
	ret = Message::SendSync(Message::Operation::REQUEST, buffer, size_written);
	LOG_DEBUG("ZS: %s", (*(DCS::Utils::BasicString*)ret.ptr));

	std::vector<float> data;
	float acc_step = (acc_max - acc_min) / rep;

	for (int i = 0; i < rep; i++)
	{
		float acc = acc_min + acc_step * i;
		std::string acc_str = "2OR0;2WS;2AC" + std::to_string(acc) + ";2PA45;2WS;2PA0;2WS;2TP?;";

		DCS::Utils::BasicString reqstr;

		memcpy(reqstr.buffer, acc_str.c_str(), acc_str.size());

		size_written = DCS::Registry::SVParams::GetDataFromParams(buffer,
			SV_CALL_DCS_Control_IssueGenericCommandResponse,
			DCS::Control::UnitTarget::ESP301,
			reqstr
		);
		auto ret = Message::SendSync(Message::Operation::REQUEST, buffer, size_written);
		float rpos = atof((*(DCS::Utils::BasicString*)ret.ptr).buffer);
		LOG_DEBUG("Acc: %f || Dev: %f", acc, rpos);
		data.push_back(rpos);
	}
	return data;
}

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

	auto data = RunToWaitPoll(100);

	std::ofstream output_file("1_200ACC_S0_M45_S0_ABS.txt");
	std::ostream_iterator<float> output_iterator(output_file, "\n");
	std::copy(data.begin(), data.end(), output_iterator);

	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	Client::StopThread(c);

	Destroy();

 //   QApplication a(argc, argv);

	//// Register custom signal data
	//qRegisterMetaType<GraphSettings>("GraphSettings");

 //   MainWindow w;

	//QFile f("../../../client/styles/default.css"); // TODO : In a proper env this needs to be replaced for a app recognized path
	//if (!f.open(QFile::ReadOnly | QFile::Text))
	//{
	//	LOG_ERROR("Failed to load stylesheet. Maybe location is wrong?");
	//}
	//else
	//{
	//	QTextStream in(&f);

	//	if (f.size() > 0)
	//	{
	//		LOG_MESSAGE("Loaded custom stylesheet.");
	//	}

	//	QString s = in.readAll();
	//	a.setStyleSheet(s);
	//}

 //   w.show();
	//const int retval = a.exec();

	Logger::Destroy();

	//return retval;
	return 0;
}
