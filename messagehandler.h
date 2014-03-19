#include "server.h"
#include "connection.h"
#include "connectionclosedexception.h"
#include "protocol.h"

#include <memory>

using namespace std;

class MessageHandler{
public:
	MessageHandler();
	int readMessage(const shared_ptr<Connection>& conn);
	void writeMessage(const shared_ptr<Connection>& conn, const string& s);
private:
	string readArguments(const shared_ptr<Connection>& conn);
	int readCommand(const shared_ptr<Connection>& conn);
	string readString(const shared_ptr<Connection>& conn);
	Protocol protocol;
};
