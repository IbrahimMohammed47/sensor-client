#include <iostream>
#include <errno.h>
#include <vector>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> 
#include <algorithm> 
#include <random>
#include "concurrent_queue.cpp" 

 

class Sensor {
private:
  int port = 12345;
  int rate = 1;
  int listener_fd;
  ThreadSafeDeque<int> subscribers;

public:
  // Sensor() {
  Sensor(int port, int rate): port(port), rate(rate) {

    listener_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (listener_fd < 0 )
    {
        std::cerr << "Error: " << strerror(errno) << std::endl;
        exit(1);
    }
    struct sockaddr_in myaddr;
    memset(&myaddr, 0, sizeof(struct sockaddr_in));
    myaddr.sin_family = AF_INET;
    myaddr.sin_port = htons(port);        
    myaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    int opt = 1;  
    setsockopt(listener_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt));

    int res1 = bind(listener_fd, (struct sockaddr*) &myaddr, sizeof(myaddr));    
    if (res1 < 0)
    {
        std::cerr << "Error: " << strerror(errno) << std::endl;
        exit(1);
    }

  };

  ~Sensor() {
    subscribers.close();
    close(listener_fd); 
  }

  void listen_clients(){
    int res = listen(listener_fd, 1); 
    if (res < 0)
    {
        std::cerr << "Error: " << strerror(errno) << std::endl;
        exit(1);
    }
    struct sockaddr_in peeraddr;
    socklen_t peeraddr_len;
    while (true)
    {
        int client_socket = accept(listener_fd, (struct sockaddr*) &peeraddr, &peeraddr_len);
        if (client_socket < 0)
        {
            std::cerr << "Error: " << strerror(errno) << std::endl;
            continue;
        }        
        add_subscriber(client_socket, peeraddr);
    }
  }

  void add_subscriber(int client_socket, struct sockaddr_in peeraddr){
    std::cout << "Connection from IP "
              << ( ( ntohl(peeraddr.sin_addr.s_addr) >> 24) & 0xff ) << "."  // High byte of address
              << ( ( ntohl(peeraddr.sin_addr.s_addr) >> 16) & 0xff ) << "."
              << ( ( ntohl(peeraddr.sin_addr.s_addr) >> 8) & 0xff )  << "."
              <<   ( ntohl(peeraddr.sin_addr.s_addr) & 0xff ) << ", port "   // Low byte of addr
              << ntohs(peeraddr.sin_port) << std::endl;
    subscribers.push(client_socket);
  }
  
  void produce_readings()
  {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dist(35, 50);
    int i;
    int j;
    int optval;
    socklen_t optlen = sizeof(optval);
          
    for(;;)
    {
        float reading = dist(gen);
        std::string reading_str= std::to_string(reading);
        char const *reading_bytes = reading_str.c_str();
        // delay for {rate} seconds
        for(i = 0 ; i < rate ; i++) { usleep(1000 * 1000); }
        
        for (j = 0; j < subscribers.size(); j++)
        {      
          int subscriber = subscribers.pop();
          
          getsockopt(subscriber,SOL_SOCKET,SO_ERROR,&optval, &optlen);
          if(optval!=0){ //|| res !=0
            close(subscriber);
          }
          else{
            subscribers.push(subscriber);
            write(subscriber, reading_bytes, 8);
          }
        }
    }
  }
};