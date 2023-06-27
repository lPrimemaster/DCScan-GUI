#include <iostream>
#include <DCS_Network/include/DCS_ModuleNetwork.h>
#include <DCS_EngineControl/include/DCS_ModuleEngineControl.h>
#include <DCS_Core/include/DCS_ModuleCore.h>
#include <DCS_Acquisition/include/DCS_ModuleAcquisition.h>

int main(int argc, char* argv[])
{
	// Initialize Logger
	DCS::Utils::Logger::Init(DCS::Utils::Logger::Verbosity::DEBUG);

	// Initialize database
	DCS::Database::Open();

	// Initialize network services
	DCS::Network::Init();

	// Initialize control services
	DCS::Control::StartServices();

	// Initialize acquisition services
	DCS::DAQ::Init();

#ifdef DCS_MODULE_ENCODER
	// Initializes encoder services
	DCS::f64 per_rev[] = {36000.0, 36000.0, 36000.0, 36000.0};
	DCS::ENC::Init("10.80.0.99", 0b1010, per_rev);
#endif

	auto listen = DCS::Network::Server::Create(15777);

	DCS::Network::Server::WaitForConnections(listen);

	DCS::CLI::Spin();

	DCS::Network::Server::StopListening(listen);

#ifdef DCS_MODULE_ENCODER
	DCS::ENC::Terminate();
#endif

	DCS::DAQ::Terminate();

	DCS::Control::StopServices();

	DCS::Network::Destroy();

	DCS::Database::Close();

	DCS::Utils::Logger::Destroy();

	return 0;
}
