#include <thread>
#include "sensor.cpp" 


int main(int argc, char const *argv[])
{
    // cout << "th: "<< std::this_thread::get_id <<endl;
    char* port_str = getenv("SENSOR_PORT");
    if(port_str == nullptr){
        port_str = (char*)"12345";
    }
    int port = atoi(port_str);
    int rate = 1;
    Sensor* s = new Sensor(port, rate);
    // s.listen_clients(); 

    std::thread listener_thread(&Sensor::listen_clients, s);
    std::thread producer_thread(&Sensor::produce_readings, s);

    listener_thread.join();
    producer_thread.join();
    
    delete s;
    // cout << s.id;
    return 0;
}
