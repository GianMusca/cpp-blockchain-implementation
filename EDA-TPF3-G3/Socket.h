#pragma once

#include <string>

using namespace std;
typedef unsigned int uint;

struct ip_t {
	uint b1;
	uint b2;
	uint b3;
	uint b4;

	bool operator==(ip_t ip) {
		if ((this->b1 == ip.b1) &&
			(this->b2 == ip.b2) &&
			(this->b3 == ip.b3) &&
			(this->b4 == ip.b4))
			return true;
		else
			return false;
	}
};


class Socket
{
public:
	Socket(uint port, ip_t ip);
	Socket(uint port, uint ip1, uint ip2, uint ip3, uint ip4);
	Socket(const Socket& c);

	string getPortString();
	string getIPString();
	uint getPort();
	ip_t getIP();

	bool operator==(const Socket& s);
	Socket& operator=(const Socket& s);

private:
	uint port;
	ip_t ip;
};



