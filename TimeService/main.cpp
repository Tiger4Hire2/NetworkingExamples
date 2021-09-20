#include <asio.hpp>
#include "CommonObj.h"
#include "Common/Json.h"

int main()
{
	asio::io_context context;
	const auto addr = asio::ip::make_address_v4("127.0.0.1");
	asio::ip::tcp::acceptor acceptor{ context, asio::ip::tcp::endpoint{addr,10001} };
	while (true)
	{
		asio::ip::tcp::iostream reply_stream{ acceptor.accept() };
		time_t tm =time(NULL );
		struct tm * curtime = localtime ( &tm );
		CommonObj my_reply{ .text=asctime(curtime), .fixed_len_field=tm };
		reply_stream << AsJson(my_reply);
	}
}