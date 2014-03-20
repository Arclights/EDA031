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
	void handleMessage(const shared_ptr<Connection>& conn);
private:
	void handleListNG(const shared_ptr<Connection>& conn);
	void handleCreateNG(const shared_ptr<Connection>& conn);
	void handleListArt(const shared_ptr<Connection>& conn);
	void handleCreateArt(const shared_ptr<Connection>& conn);
	void handleGetArt(const shared_ptr<Connection>& conn);
	void handleDeleteArt(const shared_ptr<Connection>& conn);
	void handleDeleteNG(const shared_ptr<Connection>& conn);
	void appendString(string& out, const string& s) const;
	void appendNumber(string& out, int number) const;
	void writeMessage(const shared_ptr<Connection>& conn, const string& s);
	MemDatabase db;
};
