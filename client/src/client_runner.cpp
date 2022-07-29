#include "client.cpp" 
#include <string.h>

int main(int argc, char *argv[])
{
    char* host = getenv("SENSOR_HOST");
    if(host == nullptr){
        host = (char*)"localhost";
    }
    char* port_str = getenv("SENSOR_PORT");
    if(port_str == nullptr){
        port_str = (char*)"12345";
    }
    int port = atoi(port_str);

    Client c;
    c.init();
    c.connect_to_host(host,port);
    c.post_connect();
    c.consume();
    return 0;
}

