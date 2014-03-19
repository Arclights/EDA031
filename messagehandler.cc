#include "messagehandler.h"

#include <iostream>
#include <algorithm>
#include <sstream>
#include <iterator>

using namespace std;

MessageHandler::MessageHandler():protocol(Protocol()){}

string MessageHandler::readArguments(const shared_ptr<Connection>& conn){
	cout << "before string" << endl;
	string arguments = "";	
	while(true){
		unsigned char byte = conn->read();
		if(byte == protocol.COM_END){
			break;
		}
		arguments += byte;
	}
	
	//istringstream buf(arguments);
    //istream_iterator<string> beg(buf), end;
	//vector<string> arguments(beg, end);

	return "";
}

int MessageHandler::readCommand(const shared_ptr<Connection>& conn){
	unsigned char byte1 = conn->read();
	return byte1;
}

string MessageHandler::readString(const shared_ptr<Connection>& conn) {
	string s;
	char ch;
	while ((ch = conn->read()) != protocol.COM_END) {
		s += ch;
	}
	return s;
}

int MessageHandler::readMessage(const shared_ptr<Connection>& conn){
	string line = readString(conn);
	cout << line << endl;
	int command = readCommand(conn);
	cout << command << endl;

	switch(command){
	case protocol.COM_LIST_NG:
		break;
	case protocol.COM_CREATE_NG:
		cout << "create new group" << endl;
		//cout << readArgument(conn) << endl;
		break;
	case protocol.COM_DELETE_NG:
		cout << readArguments(conn) << endl;
		break;
	case protocol.COM_LIST_ART:
		cout << readArguments(conn) << endl;
		break;
	case protocol.COM_CREATE_ART:
		cout << readArguments(conn) << endl;
		break;
	case protocol.COM_DELETE_ART:
		cout << readArguments(conn) << endl;
		break;
	case protocol.COM_GET_ART:
		cout << readArguments(conn) << endl;
		break;
	default:
		cout << "default" << endl;
		break;
	}
	return command;
}

void MessageHandler::writeMessage(const shared_ptr<Connection>& conn, const string& s){
	for (char c : s) {
		conn->write(c);
	}
	conn->write('$');
}
