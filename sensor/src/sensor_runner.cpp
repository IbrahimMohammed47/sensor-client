#include <thread>
#include "sensor.cpp" 


int main(int argc, char const *argv[])
{
    // cout << "th: "<< std::this_thread::get_id <<endl;
    Sensor* s = new Sensor();
    // s.listen_clients(); 

    thread listener_thread(&Sensor::listen_clients, s);
    thread producer_thread(&Sensor::produce_readings, s);

    listener_thread.join();
    producer_thread.join();
    
    delete s;
    // cout << s.id;
    return 0;
}
