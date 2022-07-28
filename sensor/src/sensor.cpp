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

 

using namespace std; 


class Sensor {
private:
  int port = 12345;
  int rate = 1;
  int listener_fd;
  // int comm_fd;
  // vector<int> subscribers;
  ThreadSafeDeque<int> subscribers;

public:
  Sensor() {
    listener_fd = socket(AF_INET, SOCK_STREAM, 0);
    // comm_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (listener_fd < 0 )
    {
        std::cerr << "Error: " << strerror(errno) << std::endl;
        exit(1);
    }
    // Fill in the address structure containing self address
    struct sockaddr_in myaddr;
    memset(&myaddr, 0, sizeof(struct sockaddr_in));
    myaddr.sin_family = AF_INET;
    myaddr.sin_port = htons(port);        // Port to listen
    myaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    int res1 = bind(listener_fd, (struct sockaddr*) &myaddr, sizeof(myaddr));
    // int res2 = bind(comm_fd, (struct sockaddr*) &myaddr, sizeof(myaddr));
    
    if (res1 < 0)
    {
        std::cerr << "Error: " << strerror(errno) << std::endl;
        exit(1);
    }
    // Set the "LINGER" timeout to zero, to close the listen socket
    // immediately at program termination.
    // struct linger linger_opt = { 1, 0 }; // Linger active, timeout 0
    // setsockopt(listener_fd, SOL_SOCKET, SO_LINGER, &linger_opt, sizeof(linger_opt)); 
    int opt = 1;  
    setsockopt(listener_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt));
    // Set the "LINGER" timeout to zero, to close the listen socket
    // immediately at program termination.
    // struct linger linger_opt2 = { 1, 0 }; // Linger active, timeout 0
    // setsockopt(comm_fd, SOL_SOCKET, SO_LINGER, &linger_opt2, sizeof(linger_opt2));

  };

  ~Sensor() {
    subscribers.close();
    close(listener_fd);    // Close the listen socket
  }

  void listen_clients(){
    int res = listen(listener_fd, 1);    // "1" is the maximal length of the queue
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
            // exit(1);
            continue;
        }
        // A connection is accepted. The new socket "s1" is created
        // for data input/output. The peeraddr structure is filled in with
        // the address of connected entity, print it.
        std::cout << "Connection from IP "
                << ( ( ntohl(peeraddr.sin_addr.s_addr) >> 24) & 0xff ) << "."  // High byte of address
                << ( ( ntohl(peeraddr.sin_addr.s_addr) >> 16) & 0xff ) << "."
                << ( ( ntohl(peeraddr.sin_addr.s_addr) >> 8) & 0xff )  << "."
                <<   ( ntohl(peeraddr.sin_addr.s_addr) & 0xff ) << ", port "   // Low byte of addr
                << ntohs(peeraddr.sin_port) << endl;        
        subscribers.push(client_socket);
    }
  }
  
  void produce_readings()
  {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dist(35, 50);
    int i;
    for(;;)
    {
        float reading = dist(gen);
        string reading_str= to_string(reading);
        char const *reading_bytes = reading_str.c_str();
        // delay for {rate} seconds
        for(i = 0 ; i < rate ; i++) { usleep(1000 * 1000); }
        
        for (int i = 0; i < subscribers.size(); i++)
        {      
          int subscriber = subscribers.pop();
          int optval;
          socklen_t optlen = sizeof(optval);
          
          getsockopt(subscriber,SOL_SOCKET,SO_ERROR,&optval, &optlen);
          if(optval!=0){ //|| res !=0
            close(subscriber);
            // printf("%d\n", optval);            
          }
          else{
            subscribers.push(subscriber);
            // write(subscriber, count  + " Hello!\r\n", 8);
            
            write(subscriber, reading_bytes, 8);
          }
        }
    }
  }
};