#include <WinSock2.h>
#include <asio.hpp>
#include <iostream>
#include <thread>




int main()
{
	asio::io_context	context;

	context.post([]() {std::cout << "hello\n"; });
	context.post([]() {std::cout << "world\n"; });
	std::thread one{ [&]() {context.run(); } };
	std::thread two{ [&]() {context.run(); } };
	one.join();
	two.join();
}