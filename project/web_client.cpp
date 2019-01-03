#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>
#include <websocketpp/common/thread.hpp>
#include <iostream>
#include <pthread.h>
#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>
typedef websocketpp::client<websocketpp::config::asio_client> client;
client c;
websocketpp::connection_hdl con_hdl;

void telemetry_loop(){
    uint64_t count = 0;
    std::stringstream val;
    websocketpp::lib::error_code ec;
    delay(5000);        
    while(1) {   
        val.str("");
        val << "count is " << count++;
    
        c.get_alog().write(websocketpp::log::alevel::app, val.str());
        c.send(con_hdl,val.str(),websocketpp::frame::opcode::text,ec);
        
        // The most likely error that we will get is that the connection is
        // not in the right state. Usually this means we tried to send a 
        // message to a connection that was closed or in the process of 
        // closing. While many errors here can be easily recovered from, 
        // in this simple example, we'll stop the telemetry loop.
        if (ec) {
            c.get_alog().write(websocketpp::log::alevel::app, 
                "Send Error: "+ec.message());
            break;
        }
        delay(1000);
    }
}
int main(int argc, char* argv[]) {

    std::string uri = "ws://localhost:9000";

    try {
        // Set logging to be pretty verbose (everything except message payloads)
        c.set_access_channels(websocketpp::log::alevel::all);
        c.clear_access_channels(websocketpp::log::alevel::frame_payload);
        c.set_error_channels(websocketpp::log::elevel::all);

        // Initialize ASIO
        c.init_asio();

        // Register our message handler
        //c.set_message_handler(&on_message);

        websocketpp::lib::error_code ec;
        client::connection_ptr con = c.get_connection(uri, ec);
        if (ec) {
            std::cout << "could not create connection because: " << ec.message() << std::endl;
            return 0;
        }
    
        con_hdl = con->get_handle();
        // Note that connect here only requests a connection. No network messages are
        // exchanged until the event loop starts running in the next line.
        c.connect(con);

        // Start the ASIO io_service run loop
        // this will cause a single connection to be made to the server. c.run()
        // will exit when this connection is closed.
        //c.run();
        
        websocketpp::lib::thread asio_thread(&client::run, &c);
        websocketpp::lib::thread telemetry_thread(&telemetry_loop);

        asio_thread.join();
        telemetry_thread.join();

    } catch (websocketpp::exception const & e) {
        std::cout << e.what() << std::endl;
    }
}

