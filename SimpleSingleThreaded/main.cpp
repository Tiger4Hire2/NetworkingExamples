#include <WinSock2.h>
#include <asio.hpp>
#include <iostream>

int main()
{
	asio::io_context	context;

	context.post([]() {std::cout << "hello\n";});
	context.run();
}