#include <websocketpp/server.hpp>
#include <websocketpp/config/asio_no_tls.hpp>
typedef websocketpp::server<websocketpp::config::asio> server;

using namespace std;
void on_message(websocketpp::connection_hdl hdl, server::message_ptr msg)
{
	std::cout << msg->get_payload() << std::endl;
}
  
int main()
{
	server print_server;

	print_server.set_message_handler(&on_message);

	print_server.init_asio();
	print_server.listen(9010);
	print_server.start_accept();

	print_server.run();
}
