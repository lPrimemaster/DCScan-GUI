#include <iostream>
#include <DCS_Network/include/DCS_ModuleNetwork.h>

int main(int argc, char* argv[])
{
	DCS::Network::Init();

	std::thread t([] { while (true) { DCS::Network::Message::callRandom(); std::this_thread::sleep_for(std::chrono::milliseconds(100)); } });

	auto listen = DCS::Network::Server::Create(15777);

	auto connection = DCS::Network::Server::WaitForConnection(listen);

	DCS::Network::Server::StartThread(connection);

	DCS::Network::Server::StopThread(connection, DCS::Network::Server::StopMode::WAIT);

	DCS::Network::Destroy();

	t.join();

	return 0;
}
