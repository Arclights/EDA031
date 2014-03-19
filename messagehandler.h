#include "server.h"
#include "connection.h"
#include "connectionclosedexception.h"
#include "protocol.h"
#include "memdatabase.h"

#include <memory>

using namespace std;

class MessageHandler{
public:
	MessageHandler();
	int handleMessage(const shared_ptr<Connection>& conn);
	void writeMessage(const shared_ptr<Connection>& conn, const string& s);
private:
	void handleCreateNG(const shared_ptr<Connection>& conn);
	Protocol protocol;
	MemDatabase db;
};
