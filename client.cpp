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

class Client {
  
    short PRINT_PERIOD = 5;
    private:
    
        int socket_fd;
    
        long count;
        long double sum;
        long double start_time;
        long double last_print_time; 

    public:
        Client() {
            count = 0.0;
            sum = 0.0;
        };

        void init(){
                // Create socket
            socket_fd = socket(AF_INET, SOCK_STREAM, 0);
            if (socket_fd < 0)
            {
                std::cerr << "Error: " << strerror(errno) << std::endl;
                exit(1);
            }        
        }

        void connect_to_host(char* serverHostName, short serverPort){
        
            // Resolve server address (convert from symbolic name to IP number)
            struct hostent *host = gethostbyname(serverHostName);
            if (host == NULL)
            {
                std::cerr << "Error: " << strerror(errno) << std::endl;
                exit(1);
            }

                // Fill in server IP address
            struct sockaddr_in server;
            int serverAddrLen;
            bzero( &server, sizeof( server ) );

            server.sin_family = AF_INET;
            server.sin_port = htons(serverPort);
        
            // Print a resolved address of server (the first IP of the host)
            std::cout << "server address = " << (host->h_addr_list[0][0] & 0xff) << "." <<
                                                (host->h_addr_list[0][1] & 0xff) << "." <<
                                                (host->h_addr_list[0][2] & 0xff) << "." <<
                                                (host->h_addr_list[0][3] & 0xff) << ", port " <<
                                                static_cast<int>(serverPort) << std::endl;
        
            // Write resolved IP address of a server to the address structure
            memmove(&(server.sin_addr.s_addr), host->h_addr_list[0], 4);
        
            // Connect to the remote server
            int res = connect(socket_fd, (struct sockaddr*) &server, sizeof(server));
            if (res < 0)
            {
                std::cerr << "Error: " << strerror(errno) << std::endl;
                exit(1);
            }
        
            std::cout << "Connected" << std::endl;
        }

        // void (*handler)(void*, float)
        void consume(){
            std::cout << "Listening...\n" << std::endl;
            int res;
            double reading;
            while (1)
            {
                char buffer[8];
                res = read(socket_fd, buffer, 8);
                if (res < 0)
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
            // start_time = clock();
            start_time = std::chrono::duration_cast<std::chrono::seconds>(Clock::now().time_since_epoch()).count();
            last_print_time = start_time - PRINT_PERIOD;
        }

        Pair process(float val)
        {
            count ++;
            sum += val;  

            long double current_time = std::chrono::duration_cast<std::chrono::seconds>(Clock::now().time_since_epoch()).count();
            long double duration = current_time - start_time;

            double avg = sum / count;
            double acc = sum / duration;                      
            return std::make_pair(avg, acc);

        }

        void report(double avg, double acc){
            long double current_time = std::chrono::duration_cast<std::chrono::seconds>(Clock::now().time_since_epoch()).count();
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



int main(int argc, char *argv[])
{

    Client c;
    c.init();
    c.connect_to_host("localhost",12345);
    c.post_connect();
    c.consume();
    return 0;
}
