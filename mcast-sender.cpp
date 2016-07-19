
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <string>
#include <iostream>
#include <cstdlib>
#include <thread>
#include <chrono>

using namespace std;
using namespace chrono;
int main(int argc, char** argv) {
    if(argc < 3) {
        cerr << "usage: " << argv[0] << " <port> <multicast group ip address> [message]"
             << endl;
        return EXIT_FAILURE;
    }

    const int port = stoi(argv[1]);
    const string mcastIP = argv[2];
    const int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd< 0) {
         perror("socket");
         exit(EXIT_FAILURE);
     }
    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(mcastIP.c_str());
    addr.sin_port = htons(port);
    
    const string message = argc > 3 ? argv[3] : "Hello Multicast!";
    while(true) {
        if(sendto(fd, message.c_str(), message.size(), 0, 
                  reinterpret_cast< sockaddr* >(&addr),
                  sizeof(addr)) < 0) {
            perror("sendto");
            exit(EXIT_FAILURE);
        }
        this_thread::sleep_for(seconds(1));
     }
    return EXIT_SUCCESS;
}
