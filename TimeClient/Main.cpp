#include <WinSock2.h>
#undef min
#undef max
#include <asio.hpp>
#include "TimeService/CommonObj.h"
#include "Common/JSON.h"

int main()
{
	using namespace asio::ip;
	CommonObj reply;
	asio::io_context context;
	asio::ip::tcp::iostream send_stream{ tcp::socket{context} };
	const auto addr = asio::ip::make_address_v4("127.0.0.1");
	send_stream.connect(tcp::endpoint{ addr, 10001 });
	send_stream >> AsJson(reply);
	std::cout << AsJson(reply);
}