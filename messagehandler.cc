#include "messagehandler.h"

#include <iostream>
#include <algorithm>
#include <sstream>
#include <iterator>
#include <vector>
#include <sstream>

using namespace std;

MessageHandler::MessageHandler():protocol(Protocol()), db(MemDatabase()){}

int readByte(const shared_ptr<Connection>& conn){
	unsigned char byte1 = conn->read();
	return byte1;
}

int readInt(const shared_ptr<Connection>& conn) {
	unsigned char byte1 = conn->read();
	unsigned char byte2 = conn->read();
	unsigned char byte3 = conn->read();
	unsigned char byte4 = conn->read();
	return (byte1 << 24) | (byte2 << 16) | (byte3 << 8) | byte4;
}

vector<string> splitString(string& str){
	istringstream buf(str);
    istream_iterator<string> beg(buf), end;
	vector<string> arguments(beg, end);
	return arguments;
}

string readString(const shared_ptr<Connection>& conn) {
	string s;
	char ch;
	int charsRead = 0;
	int paramType = readByte(conn);
	int charsToRead = readInt(conn);
	while (charsRead < charsToRead) {
		ch = conn->read();
		s += ch;
		charsRead++;
	}
	return s;
}

int readNumber(const shared_ptr<Connection>& conn){
	int paramType = readByte(conn);
	return readInt(conn);
}

void MessageHandler::handleListNG(const shared_ptr<Connection>& conn){
	readByte(conn); // End byte

	string dbReply = db.listNewsGroups();

	string message = "";
	message += protocol.ANS_LIST_NG;
	message += dbReply;
	
	writeMessage(conn, message);
}

void MessageHandler::handleCreateNG(const shared_ptr<Connection>& conn){
	string groupTitle = readString(conn);
	readByte(conn); // End byte
	
	string dbReply = db.addNewsGroup(groupTitle);

	string message = "";
	message += protocol.ANS_CREATE_NG;
	message += dbReply;
	
	writeMessage(conn, message);

}

void handleDeleteNG(const shared_ptr<Connection>& conn){
	int paramType;
	int n;
	string groupTitle;
	paramType = readByte(conn);
	cout << paramType << endl;
	readByte(conn);
	readByte(conn);
	readByte(conn); // Av någon anledning skickas det en massa nollor innan N (?)
	n = readByte(conn);
	readByte(conn); // End byte
	// Contact database
	// Reply

}

void MessageHandler::handleListArt(const shared_ptr<Connection>& conn){
	int ngID = readNumber(conn);
	readByte(conn); // End byte

	string dbReply = db.listArticles(ngID);

	string message = "";
	message += protocol.ANS_LIST_ART;
	message += dbReply;
	
	writeMessage(conn, message);

}

void MessageHandler::handleCreateArt(const shared_ptr<Connection>& conn){
	int ngID = readNumber(conn);
	string title = readString(conn);
	string author = readString(conn);
	string text = readString(conn);
	readByte(conn); // End byte
	
	string dbReply = db.addArticle(ngID, title, author, text);

	string message = "";
	message += protocol.ANS_CREATE_ART;
	message += dbReply;
	
	writeMessage(conn, message);
	// writeMessage(conn, "");

}

void handleDeleteArt(const shared_ptr<Connection>& conn){ // Ej testad
	int paramType;
	int n;
	string groupTitle;
	paramType = readByte(conn);
	cout << paramType << endl;
	readByte(conn);
	readByte(conn);
	readByte(conn); // Av någon anledning skickas det en massa nollor innan N (?)
	n = readByte(conn);
	readByte(conn); // End byte
	// Contact database
	// Reply
}

void handleGetArt(const shared_ptr<Connection>& conn){ // Ej testad
	int paramType;
	int n;
	string groupTitle;
	paramType = readByte(conn);
	cout << paramType << endl;
	readByte(conn);
	readByte(conn);
	readByte(conn); // Av någon anledning skickas det en massa nollor innan N (?)
	n = readByte(conn);
	readByte(conn); // End byte
	// Contact database
	// Reply
}

int MessageHandler::handleMessage(const shared_ptr<Connection>& conn){
	int command = readByte(conn);
	cout << command << endl;

	switch(command){
	case protocol.COM_LIST_NG:
		handleListNG(conn);
		break;
	case protocol.COM_CREATE_NG:
		handleCreateNG(conn);
		break;
	case protocol.COM_DELETE_NG:
		handleDeleteNG(conn);
		break;
	case protocol.COM_LIST_ART:
		handleListArt(conn);
		break;
	case protocol.COM_CREATE_ART:
		handleCreateArt(conn);
		break;
	case protocol.COM_DELETE_ART:
		handleDeleteArt(conn);
		break;
	case protocol.COM_GET_ART:
		handleGetArt(conn);
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
	conn->write(protocol.ANS_END);
}
