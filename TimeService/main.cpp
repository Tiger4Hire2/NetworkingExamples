#include <WinSock2.h>
#undef min
#undef max
#include <asio.hpp>
#include "CommonObj.h"
#include "Common/JSON.h"

int main()
{
	asio::io_context context;
	const auto addr = asio::ip::make_address_v4("127.0.0.1");
	asio::ip::tcp::acceptor acceptor{ context, asio::ip::tcp::endpoint{addr,10001} };
	while (true)
	{
		asio::ip::tcp::iostream reply_stream{ acceptor.accept() };
		CommonObj my_reply{ .text="Test message", .fixed_len_field="123456789", .number=1 };
		my_reply.fixed_len_field[9] = '0';
		reply_stream << AsJson(my_reply);
	}
}