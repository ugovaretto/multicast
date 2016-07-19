//simple multicast receiver: specify port and multicast address
//on command line
//compile with -std=c++11
//author: Ugo Varetto

//multicast: from 224.0.0.0 to 239.255.255.255
//see: https://en.wikipedia.org/wiki/Multicast_address
//use 225.x.x.x
//reserved multicast addresses
//http://www.iana.org/assignments/multicast-addresses/multicast-addresses.xhtml

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <string>
#include <iostream>
#include <cstdlib>
#include <vector>

using namespace std;

int main(int argc, char** argv) {
    if(argc < 3) {
        cerr << "usage: " << argv[0] << " <port> <multicast ip group>"
             << endl;
        return EXIT_FAILURE;
    }

    const int port = stoi(argv[1]);
    const string mcastIP = argv[2];
    const size_t MSGSIZE = 0x100;
    const int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(fd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
     }
    //allow multiple sockets to reuse the same address/port
    const int YES = 1;
    if(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &YES, sizeof(YES)) < 0) {
        perror("SO_REUSEADDR");
        exit(EXIT_FAILURE);
    }
    
    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);
    
    if(::bind(fd, reinterpret_cast< sockaddr* >(&addr), sizeof(addr)) < 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }
    
    ip_mreq mreq;
    memset(&mreq, 0, sizeof(mreq));
    /* use setsockopt() to request that the kernel join a multicast group */
    mreq.imr_multiaddr.s_addr = inet_addr(mcastIP.c_str());
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    if (setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) {
        perror("setsockopt IP_ADD_MEMBERSHIP");
        exit(EXIT_FAILURE);
    }
    int nbytes = -1;
    vector< char > msgbuf(MSGSIZE);
    socklen_t addrsize = socklen_t();
    while(true) {
      nbytes = recvfrom(fd, msgbuf.data(), MSGSIZE, 0,
                        (struct sockaddr *) &addr, &addrsize);;
      if(nbytes < 0) {
          perror("recvfrom");
          exit(EXIT_FAILURE);
      }
       msgbuf[nbytes] = '\0';  
       cout << msgbuf.data() << endl;
    }
    return EXIT_SUCCESS;
}
