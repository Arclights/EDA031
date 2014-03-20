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

vector<char> intToChars(int i){
	vector<char> out;
	out.push_back((i >> 24) & 0x000000FF);
	out.push_back((i >> 16) & 0x000000FF);
	out.push_back((i >> 8) & 0x000000FF);
	out.push_back((i) & 0x000000FF);
	return out;
}

void appendIntInBytes2(string& out, int i){
	vector<char> cs = intToChars(i);
	for(char c : cs){
		out += c;
	}
}

void MessageHandler::appendString(string& out, const string& s) const{
	out += protocol.PAR_STRING;
	appendIntInBytes2(out, s.size());
	out += s;
}

void MessageHandler::appendNumber(string& out, int number) const{
	out += protocol.PAR_NUM;
	appendIntInBytes2(out, number);
}

void MessageHandler::handleListNG(const shared_ptr<Connection>& conn){
	readByte(conn); // End byte

	string message = "";
	message += protocol.ANS_LIST_NG;
	cout << "Listing news groups" << endl;
	map<int, string> newsGroups = db.getNewsGroups();
	appendNumber(message, newsGroups.size());

	for(pair<int, string> ng: newsGroups){
		appendNumber(message, ng.first);
		appendString(message, ng.second);
	}

	writeMessage(conn, message);
}

void MessageHandler::handleCreateNG(const shared_ptr<Connection>& conn){
	string groupTitle = readString(conn);
	readByte(conn); // End byte
	
	string message = "";
	message += protocol.ANS_CREATE_NG;
	if(db.newsGroupTitleExists(groupTitle)){
		message += protocol.ANS_NAK;
		message += protocol.ERR_NG_ALREADY_EXISTS;
	}else{
		cout << "Creating group " << groupTitle << endl;
		db.addNewsGroup(groupTitle);
		message += protocol.ANS_ACK;
	}

	writeMessage(conn, message);

}

void MessageHandler::handleDeleteNG(const shared_ptr<Connection>& conn){
	int ngID = readNumber(conn);
	readByte(conn); // End byte

	string message = "";
	message += protocol.ANS_DELETE_NG;
	if(!db.newsGroupExists(ngID)){
		message += protocol.ANS_NAK;
		message += protocol.ERR_NG_DOES_NOT_EXIST;
	}else{
		db.deleteArticlesInNewsGroup(ngID);
		db.deleteNewsGroup(ngID);
		message += protocol.ANS_ACK;
	}
	
	writeMessage(conn, message);

}

void MessageHandler::handleListArt(const shared_ptr<Connection>& conn){
	int ngID = readNumber(conn);
	readByte(conn); // End byte

	string message = "";
	message += protocol.ANS_LIST_ART;
	if(!db.newsGroupExists(ngID)){
		message += protocol.ANS_NAK;
		message += protocol.ERR_NG_DOES_NOT_EXIST;
	}else{
		vector<pair<int, Article>> foundArticles = db.getArticles(ngID);
		message += protocol.ANS_ACK;
		appendNumber(message, foundArticles.size());
		for(pair<int, Article> art : foundArticles){
			appendNumber(message, art.first);
			appendString(message, art.second.title);
		}
	}

	writeMessage(conn, message);

}

void MessageHandler::handleCreateArt(const shared_ptr<Connection>& conn){
	int ngID = readNumber(conn);
	string title = readString(conn);
	string author = readString(conn);
	string text = readString(conn);
	readByte(conn); // End byte
	
	string message = "";
	message += protocol.ANS_CREATE_ART;
	if(!db.newsGroupExists(ngID)){
		message += protocol.ANS_NAK;
		message += protocol.ERR_NG_DOES_NOT_EXIST;
	}else{
		db.addArticle(ngID, title, author, text);
		message += protocol.ANS_ACK;
	}

	writeMessage(conn, message);

}

void MessageHandler::handleDeleteArt(const shared_ptr<Connection>& conn){
	int ngID = readNumber(conn);
	int artID = readNumber(conn);
	readByte(conn); // End byte

	string message = "";
	message += protocol.ANS_DELETE_ART;
	if(!db.newsGroupExists(ngID)){
		message += protocol.ANS_NAK;
		message += protocol.ERR_NG_DOES_NOT_EXIST;
	}else{
		if(!db.articleExists(artID)){
			message += protocol.ANS_NAK;
			message += protocol.ERR_ART_DOES_NOT_EXIST;
		}else{
			db.deleteArticle(artID);
			message += protocol.ANS_ACK;
		}
	}

	writeMessage(conn, message);
}

void MessageHandler::handleGetArt(const shared_ptr<Connection>& conn){
	int ngID = readNumber(conn);
	int artID = readNumber(conn);
	readByte(conn); // End byte

	string message = "";
	message += protocol.ANS_GET_ART;
	if(!db.newsGroupExists(ngID)){
		message += protocol.ANS_NAK;
		message += protocol.ERR_NG_DOES_NOT_EXIST;
	}else{
		if(!db.articleExists(artID)){
			message += protocol.ANS_NAK;
			message += protocol.ERR_ART_DOES_NOT_EXIST;
		}else{
			message += protocol.ANS_ACK;
			Article art = db.getArticle(artID);
			appendString(message, art.title);
			appendString(message, art.author);
			appendString(message, art.text);
		}
	}

	writeMessage(conn, message);
}

void MessageHandler::handleMessage(const shared_ptr<Connection>& conn){
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
}

void MessageHandler::writeMessage(const shared_ptr<Connection>& conn, const string& s){
	for (char c : s) {
		conn->write(c);
	}
	conn->write(protocol.ANS_END);
}
