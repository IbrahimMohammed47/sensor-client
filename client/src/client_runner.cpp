#include "client.cpp" 

int main(int argc, char *argv[])
{

    Client c;
    c.init();
    c.connect_to_host((char*)"localhost",12345);
    c.post_connect();
    c.consume();
    return 0;
}
