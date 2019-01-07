#include <netinet/in.h>
#include <arpa/inet.h>
#include <vector>
#include <iostream>
#include <thread>
#include <chrono>
using namespace std;

chrono::high_resolution_clock::time_point now;

void clockStart() {
    now = chrono::high_resolution_clock::now();
}

int clockTime() {
    return chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - now).count();
}

void receiver(vector<int> &&msgs, int socket, sockaddr_in &&addr)
{
    int bNumber;
    socklen_t slen;
    cout << "STARTED RECEIVING" << endl;
    for (int i = 0; i < msgs.size(); i++)
    {   
        recvfrom(socket, &bNumber, sizeof(bNumber), 0, (sockaddr *)&addr, &slen);
        cout << i << " recv: " << bNumber << endl;
    }
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
        int LEN = 1000;
        vector<int> messages(LEN);
        vector<int> responses(LEN);
        for (uint32_t i = 0; i < messages.size(); i++)
            messages[i] = i;

        cout << messages.size() << endl;
        thread t1(receiver, messages, sock, addr);
        t1.detach();
        this_thread::sleep_for(100ms);
        clockStart();
        for (uint32_t i = 0; i < messages.size(); i++) 
        {
            sendto(sock, &messages[i], sizeof(int), 0, (sockaddr *)&addr, sizeof(sockaddr_in));
            //cout << messages[i] << " send" << endl;
            this_thread::sleep_for(1ms);
        }
        cout << clockTime() << endl;


        //cout << "Message: " << bNumber << endl;
        this_thread::sleep_for(100s);        
        continue;
    }
}