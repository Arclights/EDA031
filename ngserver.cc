/* myserver.cc: sample server program */
#include "messagehandler.h"
#include "protocol.h"
#include "misbehavingclientexception.h"
#include "memdatabase.h"
#include "diskdatabase.h"

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
	message += Protocol::ANS_END;

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
	message += Protocol::ANS_END;

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
		db.deleteNewsGroup(ngID);
		message += Protocol::ANS_ACK;
	}
	message += Protocol::ANS_END;
	
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
		map<int, string> foundArticles = db.getArticles(ngID);
		message += Protocol::ANS_ACK;
		appendNumber(message, foundArticles.size());
		for(pair<int, string> art : foundArticles){
			appendNumber(message, art.first);
			appendString(message, art.second);
		}
	}
	message += Protocol::ANS_END;

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
	message += Protocol::ANS_END;

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
		if(!db.articleExists(ngID, artID)){
			message += Protocol::ANS_NAK;
			message += Protocol::ERR_ART_DOES_NOT_EXIST;
		}else{
			db.deleteArticle(ngID, artID);
			message += Protocol::ANS_ACK;
		}
	}
	message += Protocol::ANS_END;

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
		if(!db.articleExists(ngID, artID)){
			message += Protocol::ANS_NAK;
			message += Protocol::ERR_ART_DOES_NOT_EXIST;
		}else{
			message += Protocol::ANS_ACK;
			Article art = db.getArticle(ngID, artID);
			appendString(message, art.title);
			appendString(message, art.author);
			appendString(message, art.text);
		}
	}
	message += Protocol::ANS_END;

	writeMessage(conn, message);
}

int main(int argc, char* argv[]){
	if (argc != 3) {
		cerr << "Usage: myserver port-number mem|disk" << endl;
		cerr << endl;
		cerr << "port-number - Defines which port the server should run on" << endl;
		cerr << "mem|disk - Defines whether the database shoud be stored in memory or on disk repectively" << endl;
		cerr << endl;
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
		db = new DiskDatabase;
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
		cout << "Waiting for client..." << endl;
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
				cout << "The client is misbehaving. The connection will now close" << endl;
				conn->~Connection();
			}
		} else {
			conn = make_shared<Connection>();
			server.registerConnection(conn);
			cout << "New client connects" << endl;
		}
	}
}
