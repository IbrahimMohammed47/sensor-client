#include <iostream>
#include <sstream>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <chrono>
#include <utility>
typedef std::chrono::high_resolution_clock Clock;
typedef std::pair<double,double> Pair;  

double get_time_now_in_seconds(){
    return std::chrono::duration_cast<std::chrono::seconds>(Clock::now().time_since_epoch()).count();
}

class Client {
  
    short PRINT_PERIOD = 5;
    private:
    
        int socket_fd;
        long double start_time;
        long double last_print_time; 

    public:
        long count;
        long double sum;
        
        Client() {
            count = 0.0;
            sum = 0.0;
        };

        void init(){
            socket_fd = socket(AF_INET, SOCK_STREAM, 0);
            if (socket_fd < 0)
            {
                std::cerr << "Error: " << strerror(errno) << std::endl;
                exit(1);
            }        
        }

        void connect_to_host(char* serverHostName, short serverPort){
        
            struct hostent *host = gethostbyname(serverHostName);
            if (host == NULL)
            {
                std::cerr << "Error: " << strerror(errno) << std::endl;
                exit(1);
            }

            struct sockaddr_in server;
            bzero( &server, sizeof( server ) );

            server.sin_family = AF_INET;
            server.sin_port = htons(serverPort);
        
            std::cout << "server address = " << (host->h_addr_list[0][0] & 0xff) << "." <<
                                                (host->h_addr_list[0][1] & 0xff) << "." <<
                                                (host->h_addr_list[0][2] & 0xff) << "." <<
                                                (host->h_addr_list[0][3] & 0xff) << ", port " <<
                                                static_cast<int>(serverPort) << std::endl;
        
            memmove(&(server.sin_addr.s_addr), host->h_addr_list[0], 4);
        
            int res = connect(socket_fd, (struct sockaddr*) &server, sizeof(server));
            if (res < 0)
            {
                std::cerr << "Error: " << strerror(errno) << std::endl;
                exit(1);
            }
        
            std::cout << "Connected" << std::endl;
        }

        void consume(){
            std::cout << "Listening...\n" << std::endl;
            int res;
            double reading;
            while (1)
            {
                char buffer[8];
                res = read(socket_fd, buffer, 8);
                if (res==0){
                    std::cerr << "Sensor is down\n" << std::endl;
                    exit(1);
                }
                else if (res < 0)
                {
                    std::cerr << "Error: " << strerror(errno) << std::endl;
                    exit(1);
                }
                reading = (float)atof(buffer);
                Pair calculations = process(reading) ;
                report(calculations.first, calculations.second);
            
            } 
        }
        void post_connect(){
            start_time = get_time_now_in_seconds();
            last_print_time = start_time - PRINT_PERIOD;
        }

        Pair process(float val)
        {
            count ++;
            sum += val;  
            long double current_time = get_time_now_in_seconds();
            long double duration = current_time - start_time;
            if(duration == 0){
               duration = 1; 
            }
            double avg = sum / count;
            double acc = sum / duration;                      
            return std::make_pair(avg, acc);

        }

        void report(double avg, double acc){
            long double current_time = get_time_now_in_seconds();
            
            if (current_time - last_print_time >= PRINT_PERIOD){
                std::cout << "Avg: " << avg << " °C" << "\n";
                std::cout << "Acc: " << acc  << " °C/s" << "\n";
                std::cout << "------\n";
                last_print_time = current_time;
            }
        }
        

        ~Client() {
            close(socket_fd);
        }
};  
