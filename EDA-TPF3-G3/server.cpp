/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include "server.h"

/*******************************************************************************
* CONSTANT AND MACRO DEFINITIONS USING #DEFINE
******************************************************************************/
#define CRLF "\x0D\x0A"

/*******************************************************************************
	CONSTRUCTOR
 ******************************************************************************/
Server::Server(unsigned int port)
	:data("",0,0,0,0,0)
{
	doneDownloading = false;
	doneListening = false;
	doneSending = false;
	myResponse.clear();
	bodyMsg.clear();
	Msg.clear();
	state = ERR;
	*buf = {};


	IO_handler = new boost::asio::io_service();
	boost::asio::ip::tcp::endpoint ep(boost::asio::ip::tcp::v4(), port);
	socket = new boost::asio::ip::tcp::socket(*IO_handler);
	acceptor = new boost::asio::ip::tcp::acceptor(*IO_handler, ep);
	active = new boost::asio::io_service::work(*IO_handler);
}


/*******************************************************************************
	DESTRUCTOR
 ******************************************************************************/
Server::~Server()
{
	acceptor->close();
	socket->close();
	delete acceptor;
	delete socket;
	delete active;
	delete IO_handler;
}


/***********************************************************************************
	 SERVER NETWORKING
***********************************************************************************/
void Server::startConnection()
{
	doneListening = false;
	doneSending = false;
	acceptor->non_blocking(true);
	IO_handler->poll();
}

void Server::listening()
{
	IO_handler->poll();
	acceptor->async_accept(*socket, boost::bind(&Server::connectionHandler, this, boost::asio::placeholders::error));
}

void Server::receiveMessage()
{
	IO_handler->poll();
	socket->async_read_some(boost::asio::buffer(buf), boost::bind(&Server::receiveHandler, this,
		boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

void Server::sendMessage(const string& message) //ES BLOQUEANTE POR AHORA
{
	//size_t len;
	//len = socket->write_some(boost::asio::buffer(message, strlen(message.c_str())), error);

	//if (error.value() != WSAEWOULDBLOCK)
	//{
	//	doneSending = true;
	//	cout << "Done sending" << endl;
	//	cout << message << endl;
	//}


	myResponse = message;
	IO_handler->poll();
	socket->async_write_some(boost::asio::buffer(myResponse, myResponse.size()), boost::bind(&Server::sendHandler, this,
		boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));



}

/***********************************************************************************
	CALLBACKS/HANDLERS
***********************************************************************************/
void Server::connectionHandler(const boost::system::error_code& err)
{
	if (!err)
	{
		doneListening = true;
		fillSenderData();
		cout << "Connected" << endl;
	}

	else if(err)
	{
		cout << err.message() << endl;
		cout << "Error while listening" << endl;
	}


	else
	{
		cout << "Listening" << endl;
	}
}

void Server::receiveHandler(const boost::system::error_code err, std::size_t bytes)
{
	string aux = buf;
	Msg += buf;

	if (!err && (aux.find("Expect") != string::npos))
	{
		doneDownloading = false;
	}

	else if (!err)
	{
		doneDownloading = true;
		cout << "Done receviving" << endl;
		state = parseMessage();
	}

	else
	{
		cout << "Error while receiving" << endl;
	}

}


void Server::sendHandler(const boost::system::error_code err, std::size_t bytes)
{
	if (!err)
	{
		doneSending = true;
		cout << myResponse << endl;
	}
}
/***********************************************************************************
	 GETTERS
***********************************************************************************/
STATE Server::getState() { return state; }
string Server::getMessage() { return bodyMsg; }
NodeData Server::getSender() { return data; }
bool Server::getDoneListening() { return doneListening; }
bool Server::getDoneSending() { return doneSending; }
bool Server::getDoneDownloading() { return doneDownloading; }



/***********************************************************************************
	 SENDER DATA
***********************************************************************************/
void Server::fillSenderData()
{
	NodeData("Dummy", socket->remote_endpoint().port(),JSON.crackIp(socket->remote_endpoint().address().to_string()));
}



/***********************************************************************************
	THINKING METHODS
***********************************************************************************/
STATE Server::parseMessage()
{
	STATE rta = ERR;

	if (Msg.find("GET") != string::npos)
	{
		if (Msg.find("/eda_coin/get_block_header/") && Msg.find("Header:'block_id':") != string::npos)
		{
			size_t pos = Msg.find("'block_id':");
			size_t pos1 = Msg.find(CRLF, pos);
			bodyMsg = Msg.substr(pos + strlen("'block_id':"),pos1 ); 
			rta = HEADER;
		}
	}

	else if (Msg.find("POST") != string::npos)
	{
		size_t pos = Msg.find_last_of(CRLF, Msg.length() - strlen(CRLF));
		bodyMsg = Msg.substr(pos + 1);

		if (Msg.find("/eda_coin/send_block") != string::npos)
		{
			if (!JSON.validateBlock(bodyMsg).error)
			{
				rta = BLOCK;
			}
		}

		else if (Msg.find("/eda_coin/send_tx") != string::npos)
		{
			if (!JSON.validateTx(bodyMsg).error)
			{
				rta = TX;
			}
		}

		else if (Msg.find("/eda_coin/send_merkle_block") != string::npos)
		{
			if (!JSON.validateMerkle(bodyMsg).error)
			{
				rta = MERKLE;
			}
		}

		else if (Msg.find("/eda_coin/send_filter") != string::npos)
		{
			if (!JSON.validateFilter(bodyMsg).error)
			{
				rta = FILTER;
			}
		}


		else if (Msg.find("/eda_coin/NETWORK_LAYOUT") != string::npos)
		{
			if (!JSON.validateLayout(bodyMsg).error)
			{
				rta = LAYOUT;
			}
		}

		else if (Msg.find("/eda_coin/PING") != string::npos)
		{
			rta = PING;
		}
	}

	else
	{
		rta = ERR;
	}

	return rta;
}