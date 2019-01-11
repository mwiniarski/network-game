#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <thread>
#include <vector>

using namespace std;
using namespace chrono;

struct Player {
    struct sockaddr_in addr;
    high_resolution_clock::time_point timestamp;
};

int main()
{
    //create a UDP socket
    vector<Player> players;

    const int PORT = 8888;
    int sock;
    struct sockaddr_in addr;

    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        cout << "Socket";
        return 0;
    }

    //set port && allow any incoming address
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    //bind socket
    if (::bind(sock, (sockaddr *)&addr, sizeof(addr)) < 0)
    {
        cout << "Bind";
        return 0;
    }

    // accept connections
    struct sockaddr_in new_addr;
    int current = -1;
    while (true)
    {
        int num;
        socklen_t slen = sizeof(sockaddr_in);
        ssize_t msglen = recvfrom(sock, &num, sizeof(int), 0, (sockaddr *)&new_addr, &slen);
        // find if user exists
        bool exists = false;
        for (auto it = players.begin(); it != players.end(); ++it)
        {
            if (new_addr.sin_addr.s_addr == it->addr.sin_addr.s_addr && 
                new_addr.sin_port == it->addr.sin_port)
            {
                exists = true;
                it->timestamp = high_resolution_clock::now();
            } 
            else
            {
                if (duration_cast<milliseconds>(high_resolution_clock::now() - it->timestamp).count() > 1000)
                {
                    it-- = players.erase(it);
                    cout << "Dropped player: " << inet_ntoa(new_addr.sin_addr) << ":" << ntohs(new_addr.sin_port) << endl;
                }
                else 
                {
                    sendto(sock, &num, msglen, 0, (sockaddr *)&(it->addr), slen);
                }
            }
        }

        if (!exists)
        { 
            players.push_back({new_addr, chrono::high_resolution_clock::now()});
            cout << "New player: " << inet_ntoa(new_addr.sin_addr) << ":" << ntohs(new_addr.sin_port) << endl;
        }
    }

    return 1;
}
