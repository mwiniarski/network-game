#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <thread>
#include <vector>

using namespace std;

int main()
{
    //create a UDP socket
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
	//vector<int> missing;
        int num;
        socklen_t slen = sizeof(sockaddr_in);
        ssize_t msglen = recvfrom(sock, &num, sizeof(int), 0, (sockaddr *)&new_addr, &slen);
        if (num < current)
            current = num;
	for (int i = current + 1; i < num; i++)
        {
            cout << "Missing: " << i << endl;
        }
        current = num;
	//cout << num << endl;
        //cout << inet_ntoa(new_addr.sin_addr) << " port ";
       	//cout << ntohs(new_addr.sin_port) << endl;
        //cout << "Message: " << buffer << endl;

        sendto(sock, &num, msglen, 0, (sockaddr *)&new_addr, slen);
        continue;
    }

    return 1;
}
