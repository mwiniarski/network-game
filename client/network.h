#include <netinet/in.h>
#include <arpa/inet.h>
#include <vector>
#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>

#include "Paddle.h"
using namespace std;

chrono::high_resolution_clock::time_point now;

void clockStart() {
    now = chrono::high_resolution_clock::now();
}

int clockTime() {
    return chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - now).count();
}

void receiver(int socket, sockaddr_in &&addr, int msgCount)
{
    // set timeout 1s
    struct timeval tv = { 1, 0 };
    uint32_t slen, buffer, size, serie = 0, allDropped = 0;
    vector<bool> received(msgCount);
    vector<bool> lastReceived(200);

    if (setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
        cout << "Socket option error" << endl;
        return;
    }
    
    cout << "STARTED RECEIVING" << endl;
    for (int i = 0; i < msgCount; i++)
    {   
        if (recvfrom(socket, &buffer, sizeof(buffer), 0, (sockaddr *)&addr, &slen) <= 0)
            break;

        uint32_t currSerie = buffer / lastReceived.size();
        if (currSerie != serie)
        {
            uint32_t dropped = 0;
            for (auto i : lastReceived)
            {
                if (!i)
                    dropped++;
            }
            allDropped += dropped;
            cout << "Missing: " << (float)dropped/lastReceived.size()*100 << "\%" << endl;
            serie = currSerie;
            std::fill(lastReceived.begin(), lastReceived.end(), false);   
        }
        lastReceived[buffer % lastReceived.size()] = true;
    }

    cout << endl << "Packet drop: " << (float)allDropped/msgCount*100 << "\%" << endl;
}

void connect()
{
    const int PORT = 8888;
    int sock;
    struct sockaddr_in addr;

    //set port && allow any incoming address
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    inet_aton("128.199.57.124", &addr.sin_addr);

    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        cout << "Socket";
        return;
    }

    while (true)
    {
        int LEN = 3000;
        vector<int> messages(LEN);
        for (uint32_t i = 0; i < messages.size(); i++)
            messages[i] = i;

        thread t1(receiver, sock, addr, LEN);
        clockStart();
        for (uint32_t i = 0; i < messages.size(); i++) 
        {
            sendto(sock, &messages[i], sizeof(int), 0, (sockaddr *)&addr, sizeof(sockaddr_in));
            this_thread::sleep_for(10ms);
        }
        cout << clockTime() << endl;
        t1.join();

        break;       
    }
}

class StateSender
{
public:
    struct updt {
        bool updated;
        uint32_t value;
    };

private:
    const int PORT = 8888;
    struct sockaddr_in addr;
    int sock;
    uint32_t slen;
    
    atomic<updt> update;

public:

    StateSender() 
    {
        addr.sin_family = AF_INET;
        addr.sin_port = htons(PORT);
        inet_aton("128.199.57.124", &addr.sin_addr);

        if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        {
            cout << "Socket";
            exit(0);
        }
    }

    void send(int toSend)
    {
        sendto(sock, &toSend, sizeof(int), 0, (sockaddr *)&addr, sizeof(sockaddr_in));
    }

    int receive(Paddle *p)
    {
        uint32_t tmp;
        while(true)
        {
            recvfrom(sock, &tmp, sizeof(tmp), 0, (sockaddr *)&addr, &slen);
            update = {true, tmp};
        }
    }

    updt getUpdate() { return update.exchange({false, 0}); }
};