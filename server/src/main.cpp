#include <iostream>
#include <DCS_Network/include/DCS_ModuleNetwork.h>
#include <DCS_EngineControl/include/DCS_ModuleEngineControl.h>

#include <atomic>

int main(int argc, char* argv[])
{
	// Initialize Logger
	DCS::Utils::Logger::Init(DCS::Utils::Logger::Verbosity::DEBUG);

	// Initialize network services
	DCS::Network::Init();

	// Initialize control services
	DCS::Control::StartServices();

	std::atomic<bool> stop = false;

	std::thread t([&] { 
			std::this_thread::sleep_for(std::chrono::milliseconds(5000)); 

			while (!stop.load())
			{ 
				DCS::Network::Message::FibSeqEvt(); 
				std::this_thread::sleep_for(std::chrono::milliseconds(100)); 
			}
	});

	auto listen = DCS::Network::Server::Create(15777);

	auto connection = DCS::Network::Server::WaitForConnection(listen);

	DCS::Network::Server::StartThread(connection);

	DCS::CLI::Spin();

	DCS::Network::Server::StopThread(connection, DCS::Network::Server::StopMode::WAIT);

	DCS::Control::StopServices();

	DCS::Network::Destroy();

	DCS::Utils::Logger::Destroy();

	stop.store(true);

	t.join();

	return 0;
}
