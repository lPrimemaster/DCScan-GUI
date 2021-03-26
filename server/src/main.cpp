#include <iostream>
#include <DCS_Network/include/DCS_ModuleNetwork.h>
#include <DCS_EngineControl/include/DCS_ModuleEngineControl.h>

int main(int argc, char* argv[])
{
	// Initialize Logger
	DCS::Utils::Logger::Init(DCS::Utils::Logger::Verbosity::DEBUG);

	// Initialize network services
	DCS::Network::Init();

	// Initialize control services
	DCS::Control::StartServices();

	std::thread t([] { 
			std::this_thread::sleep_for(std::chrono::milliseconds(5000)); 

			while (true) 
			{ 
				DCS::Network::Message::FibSeqEvt(); 
				std::this_thread::sleep_for(std::chrono::milliseconds(100)); 
			} 
	});

	auto listen = DCS::Network::Server::Create(15777);

	auto connection = DCS::Network::Server::WaitForConnection(listen);

	DCS::Network::Server::StartThread(connection);

	DCS::Network::Server::StopThread(connection, DCS::Network::Server::StopMode::WAIT);

	DCS::Control::StopServices();

	DCS::Network::Destroy();

	DCS::Utils::Logger::Destroy();

	t.join();

	return 0;
}
