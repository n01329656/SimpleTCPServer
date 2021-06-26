
#include <iostream>
#include<boost/asio.hpp>
#include<signal.h>
#include <windows.h>
#include<thread>
#include<vector>
#include<chrono>
#define define _WIN32_WINNT 


class Server {
	boost::asio::ip::port_type port;
	std::vector<boost::asio::ip::tcp::socket> sockets;
	boost::asio::ip::tcp::acceptor acceptor;
	boost::asio::ip::tcp::endpoint ep;
	std::mutex mutex;
	const std::size_t QUEUE_SIZE = 5;
	const std::size_t numberofsockets;

public:
	~Server() {}
	Server(const Server& rhs) = delete;
	Server(Server&& rhs) = delete;
	Server& operator=(const Server& rhs) = delete;
	Server& operator=(Server&& rhs) = delete;
	Server(unsigned int p,boost::asio::io_service& io,std::size_t numb =  3): port(p),acceptor(io),ep(boost::asio::ip::address_v4::any(),port),mutex(), numberofsockets(numb){
		for (unsigned int i = 0; i != numberofsockets; i++) {
			sockets.emplace_back(boost::asio::ip::tcp::socket(io));
		}
	}

	void StartServer(){
		std::cout << "To terminate the server press: CTRL + BREAK\n";
		try {
			acceptor.open(boost::asio::ip::tcp::v4());
			acceptor.bind(ep);
			std::cout << "listening for a new connection...\n";
			acceptor.listen(QUEUE_SIZE);
			while (true) {
				auto it = IsAvailableSocket();
				if (it != sockets.end()) {
					acceptor.accept(*it);
					mutex.lock();
					std::cout << " A new client connected\n";
					mutex.unlock();
					std::thread t{Server::ListenClient,std::ref(*it),std::ref(mutex)};
					t.detach();
				}
			}
			
		}
		catch(boost::system::error_code& er){
			std::cout << er.value();
			exit(EXIT_FAILURE);
		}
	
	}

	inline std::vector<boost::asio::ip::tcp::socket>::iterator IsAvailableSocket() {	
		for (auto it = sockets.begin(); it != sockets.end(); it++) {
			if (!it->is_open()) return it;
		}
		return sockets.end();
	}

	static void ListenClient(boost::asio::ip::tcp::socket& socket,std::mutex& mutex) {
		boost::asio::streambuf buffer;
		std::istream input(&buffer);
		std::string msg{};
		const char delimeter = '\n';
		while (true) {
			boost::asio::read_until(socket,buffer,delimeter);
			std::getline(input,msg,delimeter);
			if (msg == "End") break;
			mutex.lock();
			std::cout << "thread ID: " << std::this_thread::get_id() << " Client has sent:  " + msg + '\n';
			mutex.unlock();
		}
		mutex.lock();
		std::cout << "thread ID:" << std::this_thread::get_id() << " Client has disconnected\n";
		mutex.unlock();
		socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
		socket.close();
	}

	static BOOL WINAPI signal_handler(DWORD sig) {
		std::cout << "Caught CRTL+BREAK, terminating\n";
		exit(EXIT_SUCCESS);
		}
};



int main()
{

	SetConsoleCtrlHandler(Server::signal_handler,TRUE);
	boost::asio::io_service io;
	Server server{2500,io,6};
	server.StartServer();
	return 0;
}

