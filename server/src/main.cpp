#include <iostream>
#include <DCS_Network/include/DCS_ModuleNetwork.h>
#include <DCS_EngineControl/include/DCS_ModuleEngineControl.h>
#include <DCS_Core/include/DCS_ModuleCore.h>

#include <atomic>

int main(int argc, char* argv[])
{
	printf("0\n");

	// Initialize Logger
	DCS::Utils::Logger::Init(DCS::Utils::Logger::Verbosity::DEBUG);

	printf("1\n");

	// Initialize network services
	DCS::Network::Init();

	printf("1\n");

	// Initialize control services
	DCS::Control::StartServices();

	// Initialize acquisition services
	DCS::DAQ::Init();

	auto listen = DCS::Network::Server::Create(15777);

	DCS::Network::Server::WaitForConnections(listen);

	DCS::CLI::Spin();

	DCS::Network::Server::StopListening(listen);

	DCS::DAQ::Terminate();

	DCS::Control::StopServices();

	DCS::Network::Destroy();

	DCS::Utils::Logger::Destroy();

	return 0;
}
