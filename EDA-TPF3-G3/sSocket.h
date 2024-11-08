#pragma once
#include <iostream>

struct sSocket {
	sSocket() : ip(), port() {}
	sSocket(const std::string& _ip, const std::string& _port) : ip(_ip), port(_port) {}
	sSocket& operator=(const sSocket& s) { ip = s.ip; port = s.port; return *this; }
	//sSocket(size_t s, char c, size_t s2, char c2) : ip(s,c),port(s2,c2){}	//punteros
	std::string ip;
	std::string port;
};