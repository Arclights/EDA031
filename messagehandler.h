#include "server.h"
#include "connection.h"
#include "connectionclosedexception.h"
#include "memdatabase.h"

#include <memory>

using namespace std;

void appendString(string& out, const string& s);
void appendNumber(string& out, int number);
void writeMessage(const shared_ptr<Connection>& conn, const string& s);
int readByte(const shared_ptr<Connection>& conn);
void readEndByte(const shared_ptr<Connection>& conn);
string readString(const shared_ptr<Connection>& conn);
int readNumber(const shared_ptr<Connection>& conn);
