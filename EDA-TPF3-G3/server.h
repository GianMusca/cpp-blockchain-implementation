#pragma once
/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include "jsonHandler.h"
#include <iostream>
#include <cstdio>
#include <cstring>
#include <boost/asio.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>

 /*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
#define MAX_LENGTH 10000

 /*******************************************************************************
  * NAMESPACES
  ******************************************************************************/
using namespace std;

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 *****************************************************************************/
typedef enum { BLOCK, TX, MERKLE, FILTER, HEADER, LAYOUT, PING, ERR } STATE;

class Server
{
public:
	Server(unsigned int port);
	~Server();

	/***********************************************************************************
		 SERVER NETWORKING
	***********************************************************************************/
	void startConnection();
	void listening();
	void receiveMessage();
	void sendMessage(const string& message);

	/***********************************************************************************
		 GETTERS
	***********************************************************************************/
	bool getDoneListening();
	bool getDoneDownloading();
	bool getDoneSending();
	STATE getState();
	string getMessage();
	NodeData getSender(); //Imposible creo

private:

	/***********************************************************************************
		 FLAGS
	***********************************************************************************/
	bool doneListening;
	bool doneDownloading;
	bool doneSending;

	/***********************************************************************************
		MESSAGES
	***********************************************************************************/
	string Msg;
	string myResponse;
	string bodyMsg;
	string flterId;
	char buf[MAX_LENGTH];

	/***********************************************************************************
		 SENDER DATA
	***********************************************************************************/
	void fillSenderData();
	NodeData data;

	/***********************************************************************************
		BOOST SH*T
	***********************************************************************************/
	boost::system::error_code error;
	boost::asio::io_service* IO_handler;
	boost::asio::ip::tcp::socket* socket;
	boost::asio::ip::tcp::acceptor* acceptor;
	boost::asio::io_service::work* active;

	/***********************************************************************************
		THINKING METHODS
	***********************************************************************************/
	STATE parseMessage();
	STATE state;
	jsonHandler JSON;

	/***********************************************************************************
		CALLBACKS/HANDLERS
	***********************************************************************************/
	void receiveHandler(const boost::system::error_code err, std::size_t bytes);
	void connectionHandler(const boost::system::error_code& err);
	void sendHandler(const boost::system::error_code err, std::size_t bytes);
};