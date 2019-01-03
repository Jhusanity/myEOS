#include <pthread.h>
#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include "Toilet.h"
#include <websocketpp/server.hpp>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>
#include <websocketpp/common/thread.hpp>
typedef websocketpp::server<websocketpp::config::asio> server;
typedef websocketpp::client<websocketpp::config::asio_client> client;
client myclient;
std::string clienturi = "ws://localhost:9000";
websocketpp::connection_hdl con_hdl;

#define TOILET0LOCKPIN 0
#define TOILET0OCCUPYPIN 1
#define TOILET1LOCKPIN 2
#define TOILET1OCCUPYPIN 3
#define NUMTHREADS 3
/* 
 * 1. get gpio
 * 2. send to net
 * 3. get camera
 * 4. score
 */

struct Restroom myrestroom={103,0,0};// ID, score, waitingpeople
int scoring = 0;

void SendToNetLoop(){

    //std::stringstream val;
    std::string str;
    char s[100];
    websocketpp::lib::error_code ec;
    delay(5000);        

    while(1) {   
        //val.str("");
        //val << "count is " << count++; 
        sprintf(s,"ID:%3d\tScore:%3d\tWaitingPeople:%3d\n"
                    "Toilet0:\n"
                    "locked:%d\n"
                    "occupied:%d\n"
                    "Toilet1:\n"
                    "locked:%d\n"
                    "occupied:%d\n", myrestroom.ID
                                   , myrestroom.score
                                   , myrestroom.waitingpeople
                                   , myrestroom.toilets[0].locked, myrestroom.toilets[0].occupied
                                   , myrestroom.toilets[1].locked, myrestroom.toilets[1].occupied);
        str.assign(s);
        myclient.get_alog().write(websocketpp::log::alevel::app, str);
        myclient.send(con_hdl,str,websocketpp::frame::opcode::text,ec);
 
        if (ec) {
            myclient.get_alog().write(websocketpp::log::alevel::app,"Send Error: "+ec.message());
            break;
        }
        delay(1000);
    }
}
void toilet0lockChange(void){
    // save the inversed locknum
    printf("Toilet 1:\nlocked: %d\noccupied: %d\n", -myrestroom.toilets[0].locked+1, myrestroom.toilets[0].occupied);
    scoring = 1;
}
void toilet0occupyChange(void){
    // save the inversed occupynum
    printf("Toilet 1:\nlocked: %d\noccupied: %d\n", myrestroom.toilets[0].locked, -myrestroom.toilets[0].occupied+1);
    scoring = 1;
}
void toilet1lockChange(void){
    // save the inversed locknum
    printf("Toilet 2:\nlocked: %d\noccupied: %d\n", -myrestroom.toilets[1].locked+1, myrestroom.toilets[1].occupied);
    scoring = 1;
}
void toilet1occupyChange(void){
    // save the inversed occupynum
    printf("Toilet 2:\nlocked: %d\noccupied: %d\n", myrestroom.toilets[1].locked, -myrestroom.toilets[1].occupied+1);
    scoring = 1;
}

void *GetGPIO(void *threadid)
{
    printf("Hello! I am thread GetGPIO\n");
    while(1){
        myrestroom.toilets[0].locked = digitalRead(TOILET0LOCKPIN);
        myrestroom.toilets[0].occupied = digitalRead(TOILET0OCCUPYPIN);
        myrestroom.toilets[1].locked = digitalRead(TOILET1LOCKPIN);
        myrestroom.toilets[1].occupied = digitalRead(TOILET1OCCUPYPIN);
        delay( 50 );
    }
    pthread_exit(NULL);
}

void *SendToNet(void *threadid)
{
    printf("Hello! I am thread SendToNet\n");
    

    try {
        // Set logging to be pretty verbose (everything except message payloads)
        myclient.set_access_channels(websocketpp::log::alevel::all);
        myclient.clear_access_channels(websocketpp::log::alevel::frame_payload);
        myclient.set_error_channels(websocketpp::log::elevel::all);
        // Initialize ASIO
        myclient.init_asio();

        websocketpp::lib::error_code ec;
        client::connection_ptr con = myclient.get_connection(clienturi, ec);
        if (ec) {
            std::cout << "could not create connection because: " << ec.message() << std::endl;
            return 0;
        }

        con_hdl = con->get_handle();
        // Note that connect here only requests a connection. No network messages are
        // exchanged until the event loop starts running in the next line.
        myclient.connect(con);
        
        websocketpp::lib::thread asio_thread(&client::run, &myclient);
        websocketpp::lib::thread telemetry_thread(&SendToNetLoop);

        asio_thread.join();
        telemetry_thread.join();

    } catch (websocketpp::exception const & e) {
        std::cout << e.what() << std::endl;
    }
    pthread_exit(NULL);
}

void receiving(websocketpp::connection_hdl hdl, server::message_ptr msg)
{
    printf("Get #people: %d\n", std::stoi(msg->get_payload()));
    scoring = 1;
    myrestroom.waitingpeople = std::stoi(msg->get_payload());
}   

void *GetCam(void *threadid)
{
    printf("Hello! I am thread GetCam\n");
    server print_server;

    print_server.set_message_handler(&receiving);

    print_server.init_asio();
    print_server.listen(9000);
    print_server.start_accept();

    print_server.run();
 
    pthread_exit(NULL);
}

void *Score(void *threadid)
{
    printf("Hello! I am thread Score\n");
    double x[2];
    double k;
    while(1){
        if(scoring == 1){
            for(int i=0;i<2;i++){
                x[i] = (-myrestroom.toilets[i].locked+1)*(1-myrestroom.toilets[i].occupied*0.5);
            }  
            k = 2 + myrestroom.waitingpeople - x[0] - x[1];
            myrestroom.score = round(100 * pow(0.9,k));
            printf("New cosre: %d\n",myrestroom.score);
            scoring = 0;
        }
        delay( 50 );
    }
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    if (wiringPiSetup () < 0) {
        fprintf (stderr, "Unable to setup wiringPi: %s\n", strerror (errno));
        return 1;
    }

    if ( wiringPiISR (TOILET0LOCKPIN, INT_EDGE_BOTH, &toilet0lockChange) < 0 ) {
        fprintf (stderr, "Unable to setup ISR: %s\n", strerror (errno));
        return 1;
    }
    if ( wiringPiISR (TOILET0OCCUPYPIN, INT_EDGE_BOTH, &toilet0occupyChange) < 0 ) {
        fprintf (stderr, "Unable to setup ISR: %s\n", strerror (errno));
        return 1;
    }  
    if ( wiringPiISR (TOILET1LOCKPIN, INT_EDGE_BOTH, &toilet1lockChange) < 0 ) {
        fprintf (stderr, "Unable to setup ISR: %s\n", strerror (errno));
        return 1;
    }
    if ( wiringPiISR (TOILET1OCCUPYPIN, INT_EDGE_BOTH, &toilet1occupyChange) < 0 ) {
        fprintf (stderr, "Unable to setup ISR: %s\n", strerror (errno));
        return 1;
    }         

    pthread_t threads[NUMTHREADS];
    int rc;

    for(long t=0;t<NUMTHREADS;t++){
        printf("Main: creating thread %ld\n", t);

        if (t == 0)
            rc = pthread_create(&threads[t], NULL, GetGPIO, (void *)t);
        else if (t == 1)
            rc = pthread_create(&threads[t], NULL, SendToNet, (void *)t);
        else if (t == 2)
            rc = pthread_create(&threads[t], NULL, GetCam, (void *)t);
        else if (t == 3)
            rc = pthread_create(&threads[t], NULL, Score, (void *)t);

        if(rc){
            printf("ERROR: return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }   

    for(int t=0;t<NUMTHREADS;t++){
        rc = pthread_join(threads[t], NULL);
        if(rc){
            printf("ERROR: return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
        printf("Main: join with thread %d\n", t);
    }   
    printf("Main: program completed Exiting. \n");

    return 0;
}