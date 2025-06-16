#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mutex>
#include <vector>
#include <stdexcept>

#include <winsock2.h>
#include <pthread.h>

// -lws2_32 -lpthread

class Broadcast {
private:
	int port;
	SOCKET udp_socket;
	sockaddr_in send_addr;
	sockaddr_in recv_addr;
public:
	Broadcast(int port) {
		this->port = port;
		udp_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (udp_socket == INVALID_SOCKET)
			throw std::runtime_error("Socket creation failed - " + WSAGetLastError());

		send_addr.sin_family = AF_INET;
		send_addr.sin_port = htons(port);
		send_addr.sin_addr.s_addr = htonl(INADDR_BROADCAST);

		BOOL broadcast = TRUE;
		if (setsockopt(udp_socket, SOL_SOCKET, SO_BROADCAST, (char*)&broadcast, sizeof(broadcast)) == SOCKET_ERROR)
			throw std::runtime_error("Socket option failed - " + WSAGetLastError());

		recv_addr.sin_family = AF_INET;
		recv_addr.sin_port = htons(port);
		recv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

		if (bind(udp_socket, (SOCKADDR*)&recv_addr, sizeof(recv_addr)) == SOCKET_ERROR)
			throw std::runtime_error("Binding failed - " + WSAGetLastError());
	}

	void Send(const char* message) {
		if (sendto(udp_socket, message, strlen(message), 0, (SOCKADDR*)&send_addr, sizeof(send_addr)) < 0)
			throw std::runtime_error("Error sending broadcast message - " + WSAGetLastError());
	}

	void Receive(char* buffer, int buffer_size) {
		int recv_addr_size = sizeof(recv_addr);
		int bytes_received = recvfrom(udp_socket, buffer, buffer_size, 0, (SOCKADDR*)&recv_addr, &recv_addr_size);
		if (bytes_received == SOCKET_ERROR)
			throw std::runtime_error("Error receiving broadcast message - " + WSAGetLastError());
		buffer[bytes_received] = '\0';
	}
};

std::mutex msg_mutex;
std::vector<std::string> messages;
void Callback(const char* message) {
	msg_mutex.lock();
	messages.push_back(message);
	msg_mutex.unlock();
}

void* ReceiveThread(void* arg) {
	static char buffer[2048];
	Broadcast* broadcast = (Broadcast*)arg;
	while (true) {
		memset(buffer, 0, sizeof(buffer));
		broadcast->Receive(buffer, sizeof(buffer));
		Callback(buffer);
	}
}

int main() {
	WSADATA wsa_data;
	WSAStartup(MAKEWORD(2, 2), &wsa_data);

	try {
		Broadcast broadcast(8038);
	
		pthread_t thread;
		pthread_create(&thread, NULL, ReceiveThread, &broadcast);

		Sleep(1000);
		broadcast.Send("Hello World!");

		Sleep(2000);
		broadcast.Send("Broadcast message!");

		msg_mutex.lock();
		for (std::vector<std::string>::iterator it = messages.begin(); it != messages.end(); ++it)
			printf("Received message: %s\n", it->c_str());
		messages.clear();
		msg_mutex.unlock();

		pthread_join(thread, NULL);
	} catch (const std::exception& e) {
		printf("%s\n", e.what());
	}
	WSACleanup();
	return 0;
}
