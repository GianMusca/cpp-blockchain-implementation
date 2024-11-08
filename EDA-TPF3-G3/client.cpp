/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include "Client.h"

/*******************************************************************************
 * LOCAL FUNCTIONS DECLARATIONS
 ******************************************************************************/
static size_t myCallback(void* contents, size_t size, size_t nmemb, void* userp);


/*******************************************************************************
	CONSTRUTOR
 ******************************************************************************/
Client::Client(NodeData data)
	:receiver(data)
{
	curlm = curl_multi_init();
	curl = curl_easy_init();
	receiver = data;
	running = 0;
}

/*******************************************************************************
	DESTRUCTOR
 ******************************************************************************/
Client::~Client()
{
	cout << "Destroyed Client" << endl;
	curl_multi_remove_handle(curlm, curl);
	curl_easy_cleanup(curl);
	curl_multi_cleanup(curlm);
}


/*******************************************************************************
	NETWORK CONFIGURATION
 ******************************************************************************/
void Client::POST(string path, string json)
{
	cType = POSTClient;
	string url = "http://" + receiver.getSocket().getIPString() + ":" + receiver.getSocket().getPortString() + path;

	if (curl && curlm)
	{
		//Le decimos a cURL que imprima todos los mensajes que se envían entre cliente y servidor.
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

		//Seteamos primero la pagina donde nos vamos a conectar.
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

		// Le decimos a cURL que trabaje con HTTP.
		curl_easy_setopt(curl, CURLOPT_PROTOCOLS, CURLPROTO_HTTP);

		//Le decimos a cURL que cuando haya que escribir llame a myCallback
		//y que use al string readString como user data.
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, myCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

		//Seteo para seguir el redireccionamiento
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)(json.size()) + 1);
		curl_easy_setopt(curl, CURLOPT_COPYPOSTFIELDS, json.c_str());

		curl_multi_add_handle(curlm, curl);
	}
}


void Client::GET(string path, string json)
{
	cType = GETClient;
	string url = "http://" + receiver.getSocket().getIPString() + ":" + receiver.getSocket().getPortString() + path;

	if (curl && curlm)
	{
		//Le decimos a cURL que imprima todos los mensajes que se envían entre cliente y servidor.
		//curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

		//Seteamos primero la pagina donde nos vamos a conectar.
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

		// Le decimos a cURL que trabaje con HTTP.
		curl_easy_setopt(curl, CURLOPT_PROTOCOLS, CURLPROTO_HTTP);

		//Le decimos a cURL que cuando haya que escribir llame a myCallback
		//y que use al string readString como user data.
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, myCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

		//Seteo para seguir el redireccionamiento
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

		struct curl_slist* header = NULL;
		header = curl_slist_append(header, json.c_str());
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header);

		curl_multi_add_handle(curlm, curl);

		//curl_slist_free_all(header);
	}

}


/*******************************************************************************
	COMMUNICATION
 ******************************************************************************/
errorType Client::sendRequest(void)
{
	errorType err = { false, "OK Client" };
	CURLMcode res;
	res = curl_multi_perform(curlm, &running);
	res = curl_multi_perform(curlm, &running);
	if (res != CURLM_OK)
	{
		err = { true, "Connection error" };
	}

	return err;
}


/*******************************************************************************
	GETTERS
 ******************************************************************************/
string Client::getResponse(void) { return response; }
int Client::getRunning() { return running; }
ClientType Client::getClientType() { return cType; }
NodeData Client::getReceiverData() { return receiver; }
string Client::getTranslatedResponse()
{
	if (response.find("\"status\":false}") != string::npos)
	{
		return MSG_NETWORK_NOT_READY;
	}

	else if (response.find("\"status\":true}") != string::npos)
	{
		return MSG_NETWORK_READY;
	}

	else if (response.find("true") != string::npos)
	{
		return HTTP_OK;
	}
	else
		return "";
}



/*******************************************************************************
 * LOCAL FUNCTIONS DEFINITIONS
 ******************************************************************************/
 //Concatena lo recibido en content a s

static size_t myCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
	size_t realsize = size * nmemb;
	char* response = (char*)contents;
	string* data = (string*)userp;
	data->append(response, size * nmemb);

	return realsize;						//Recordar siempre devolver realsize
}

