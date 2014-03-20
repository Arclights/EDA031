#include "messagehandler.h"
#include "misbehavingclientexception.h"

#include <iostream>
#include <algorithm>
#include <sstream>
#include <iterator>
#include <vector>
#include <sstream>

using namespace std;

MessageHandler::MessageHandler():db(MemDatabase()){}

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
	if(paramType != Protocol::PAR_STRING){
		throw MisbehavingClientException();
	}
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
	if(paramType != Protocol::PAR_NUM){
		throw MisbehavingClientException();
	}
	return readInt(conn);
}

void readEndByte(const shared_ptr<Connection>& conn){
	int paramType = readByte(conn);
	if(paramType != Protocol::COM_END){
		throw MisbehavingClientException();
	}
}

vector<char> intToChars(int i){
	vector<char> out;
	out.push_back((i >> 24) & 0x000000FF);
	out.push_back((i >> 16) & 0x000000FF);
	out.push_back((i >> 8) & 0x000000FF);
	out.push_back(i & 0x000000FF);
	return out;
}

void appendIntInBytes(string& out, int i){
	vector<char> cs = intToChars(i);
	for(char c : cs){
		out += c;
	}
}

void MessageHandler::appendString(string& out, const string& s) const{
	out += Protocol::PAR_STRING;
	appendIntInBytes(out, s.size());
	out += s;
}

void MessageHandler::appendNumber(string& out, int number) const{
	out += Protocol::PAR_NUM;
	appendIntInBytes(out, number);
}

void MessageHandler::handleListNG(const shared_ptr<Connection>& conn){
	readEndByte(conn);

	string message = "";
	message += Protocol::ANS_LIST_NG;
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
	readEndByte(conn);
	
	string message = "";
	message += Protocol::ANS_CREATE_NG;
	if(db.newsGroupTitleExists(groupTitle)){
		message += Protocol::ANS_NAK;
		message += Protocol::ERR_NG_ALREADY_EXISTS;
	}else{
		cout << "Creating group " << groupTitle << endl;
		db.addNewsGroup(groupTitle);
		message += Protocol::ANS_ACK;
	}

	writeMessage(conn, message);

}

void MessageHandler::handleDeleteNG(const shared_ptr<Connection>& conn){
	int ngID = readNumber(conn);
	readEndByte(conn);

	string message = "";
	message += Protocol::ANS_DELETE_NG;
	if(!db.newsGroupExists(ngID)){
		message += Protocol::ANS_NAK;
		message += Protocol::ERR_NG_DOES_NOT_EXIST;
	}else{
		db.deleteArticlesInNewsGroup(ngID);
		db.deleteNewsGroup(ngID);
		message += Protocol::ANS_ACK;
	}
	
	writeMessage(conn, message);

}

void MessageHandler::handleListArt(const shared_ptr<Connection>& conn){
	int ngID = readNumber(conn);
	readEndByte(conn);

	string message = "";
	message += Protocol::ANS_LIST_ART;
	if(!db.newsGroupExists(ngID)){
		message += Protocol::ANS_NAK;
		message += Protocol::ERR_NG_DOES_NOT_EXIST;
	}else{
		vector<pair<int, Article>> foundArticles = db.getArticles(ngID);
		message += Protocol::ANS_ACK;
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
	readEndByte(conn);
	
	string message = "";
	message += Protocol::ANS_CREATE_ART;
	if(!db.newsGroupExists(ngID)){
		message += Protocol::ANS_NAK;
		message += Protocol::ERR_NG_DOES_NOT_EXIST;
	}else{
		db.addArticle(ngID, title, author, text);
		message += Protocol::ANS_ACK;
	}

	writeMessage(conn, message);

}

void MessageHandler::handleDeleteArt(const shared_ptr<Connection>& conn){
	int ngID = readNumber(conn);
	int artID = readNumber(conn);
	readEndByte(conn);

	string message = "";
	message += Protocol::ANS_DELETE_ART;
	if(!db.newsGroupExists(ngID)){
		message += Protocol::ANS_NAK;
		message += Protocol::ERR_NG_DOES_NOT_EXIST;
	}else{
		if(!db.articleExists(artID)){
			message += Protocol::ANS_NAK;
			message += Protocol::ERR_ART_DOES_NOT_EXIST;
		}else{
			db.deleteArticle(artID);
			message += Protocol::ANS_ACK;
		}
	}

	writeMessage(conn, message);
}

void MessageHandler::handleGetArt(const shared_ptr<Connection>& conn){
	int ngID = readNumber(conn);
	int artID = readNumber(conn);
	readEndByte(conn);

	string message = "";
	message += Protocol::ANS_GET_ART;
	if(!db.newsGroupExists(ngID)){
		message += Protocol::ANS_NAK;
		message += Protocol::ERR_NG_DOES_NOT_EXIST;
	}else{
		if(!db.articleExists(artID)){
			message += Protocol::ANS_NAK;
			message += Protocol::ERR_ART_DOES_NOT_EXIST;
		}else{
			message += Protocol::ANS_ACK;
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

	try
	{
		switch(command){
		case Protocol::COM_LIST_NG:
			handleListNG(conn);
			break;
		case Protocol::COM_CREATE_NG:
			handleCreateNG(conn);
			break;
		case Protocol::COM_DELETE_NG:
			handleDeleteNG(conn);
			break;
		case Protocol::COM_LIST_ART:
			handleListArt(conn);
			break;
		case Protocol::COM_CREATE_ART:
			handleCreateArt(conn);
			break;
		case Protocol::COM_DELETE_ART:
			handleDeleteArt(conn);
			break;
		case Protocol::COM_GET_ART:
			handleGetArt(conn);
			break;
		default:
			throw MisbehavingClientException();
			break;
		}
	}
	catch(MisbehavingClientException e)
	{
		conn->~Connection();
	}
}

void MessageHandler::writeMessage(const shared_ptr<Connection>& conn, const string& s){
	for (char c : s) {
		conn->write(c);
	}
	conn->write(Protocol::ANS_END);
}
