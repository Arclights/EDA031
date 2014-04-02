/* myserver.cc: sample server program */
#include "messagehandler.h"
#include "protocol.h"
#include "misbehavingclientexception.h"
#include "memdatabase.h"

#include <memory>
#include <iostream>
#include <string>
#include <stdexcept>
#include <cstdlib>
#include <string.h>

using namespace std;

void handleListNG(const shared_ptr<Connection>& conn, Database& db){
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

void handleCreateNG(const shared_ptr<Connection>& conn, Database& db){
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

void handleDeleteNG(const shared_ptr<Connection>& conn, Database& db){
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

void handleListArt(const shared_ptr<Connection>& conn, Database& db){
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

void handleCreateArt(const shared_ptr<Connection>& conn, Database& db){
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

void handleDeleteArt(const shared_ptr<Connection>& conn, Database& db){
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

void handleGetArt(const shared_ptr<Connection>& conn, Database& db){
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

int main(int argc, char* argv[]){
	if (argc != 3) {
		cerr << "Usage: myserver port-number [mem|disk]" << endl;
		exit(1);
	}

	int port = -1;
	try {
		port = stoi(argv[1]);
	} catch (exception& e) {
		cerr << "Wrong port number. " << e.what() << endl;
		exit(1);
	}

	Database* db;
	if(strcmp(argv[2],"mem") == 0){
		db = new MemDatabase;
	} else if(strcmp(argv[2], "disk") == 0){
		cerr << "Disk database is not yet implemented." << endl;
		exit(1);
	} else{
		cerr << argv[2] << " is an unknown database type." << endl;
		exit(1);
	}
	
	Server server(port);
	if (!server.isReady()) {
		cerr << "Server initialization error." << endl;
		exit(1);
	}
	

	while (true) {
		auto conn = server.waitForActivity();
		if (conn != nullptr) {
			cout << "The client wants something" << endl;
			try {
				int command = readByte(conn);
				cout << "Command: " << command << endl;
				switch(command){
				case Protocol::COM_LIST_NG:
					handleListNG(conn, *db);
					break;
				case Protocol::COM_CREATE_NG:
					handleCreateNG(conn, *db);
					break;
				case Protocol::COM_DELETE_NG:
					handleDeleteNG(conn, *db);
					break;
				case Protocol::COM_LIST_ART:
					handleListArt(conn, *db);
					break;
				case Protocol::COM_CREATE_ART:
					handleCreateArt(conn, *db);
					break;
				case Protocol::COM_DELETE_ART:
					handleDeleteArt(conn, *db);
					break;
				case Protocol::COM_GET_ART:
					handleGetArt(conn, *db);
					break;
				default:
					throw MisbehavingClientException();
					break;
				}
			} catch (ConnectionClosedException&) {
				server.deregisterConnection(conn);
				cout << "Client closed connection" << endl;
			}catch(MisbehavingClientException e){
				conn->~Connection();
			}
		} else {
			conn = make_shared<Connection>();
			server.registerConnection(conn);
			cout << "New client connects" << endl;
		}
	}
}
