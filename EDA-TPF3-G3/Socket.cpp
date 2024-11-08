#include "Socket.h"


Socket::Socket(uint port, ip_t ip) :
	port(port), ip(ip)
{
}

Socket::Socket(uint port, uint ip1, uint ip2, uint ip3, uint ip4) :
	port(port), ip({ ip1,ip2,ip3,ip4 })
{
}

Socket::Socket(const Socket& c) :
	port(c.port), ip(c.ip)
{
}

string Socket::getPortString()
{
	return to_string(port);
}

string Socket::getIPString()
{
	return (to_string(ip.b1) + '.' + to_string(ip.b2) + '.' + to_string(ip.b3) + '.' + to_string(ip.b4));
}

uint Socket::getPort()
{
	return port;
}

ip_t Socket::getIP()
{
	return ip;
}

bool Socket::operator==(const Socket& s)
{
	if ((this->ip == s.ip) && (this->port == s.port))
		return true;
	else
		return false;
}

Socket& Socket::operator=(const Socket& s)
{
	port = s.port;
	ip = s.ip;

	return *this;
}
