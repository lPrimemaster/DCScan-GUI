#include <iostream>
#include <DCS_Network/include/DCS_ModuleNetwork.h>
#include <DCS_EngineControl/include/DCS_ModuleEngineControl.h>
#include <DCS_Core/include/DCS_ModuleCore.h>
#include <DCS_Acquisition/include/DCS_ModuleAcquisition.h>

#include <atomic>

int main(int argc, char* argv[])
{
	// Initialize Logger
	DCS::Utils::Logger::Init(DCS::Utils::Logger::Verbosity::DEBUG);

	// Initialize network services
	DCS::Network::Init();

	// Initialize control services
	DCS::Control::StartServices();

	// Initialize acquisition services
	DCS::DAQ::Init();

	// Initializes encoder services
	DCS::f64 per_rev[] = {36000.0, 0.0, 0.0, 36000.0};
	DCS::ENC::Init("10.80.0.99", 0b1001, per_rev);

	auto listen = DCS::Network::Server::Create(15777);

	DCS::Network::Server::WaitForConnections(listen);

	DCS::CLI::Spin();

	DCS::Network::Server::StopListening(listen);

	DCS::ENC::Terminate();

	DCS::DAQ::Terminate();

	DCS::Control::StopServices();

	DCS::Network::Destroy();

	DCS::Utils::Logger::Destroy();

	return 0;
}
