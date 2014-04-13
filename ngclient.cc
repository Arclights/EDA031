/* myclient.cc: sample client program */

#include "connection.h"
#include "commands.h"
#include "connectionclosedexception.h"
#include "messagehandler.h"
#include "protocol.h"

#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>
#include <cstdlib>
#include <vector>

using namespace std;

vector<string> &split(const string &s, char delim, vector<string> &elems) {
	stringstream ss(s);
	string item;
	while (getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}

vector<string> split(const string &s, char delim) {
	vector<string> elems;
	split(s, delim, elems);
	return elems;
}

/*
 * Send an integer to the server as four bytes.
 */
void writeNumber(const Connection& conn, int value) {
	//conn.write((value >> 24) & 0xFF);
	//conn.write((value >> 16) & 0xFF);
//	conn.write((value >> 8) & 0xFF);
	conn.write(value & 0xFF);
}

int main(int argc, char* argv[]) {
	if (argc != 3) {
		cerr << "Usage: myclient host-name port-number" << endl;
		exit(1);
	}

	int port = -1;
	try {
		port = stoi(argv[2]);
	} catch (exception& e) {
		cerr << "Wrong port number. " << e.what() << endl;
		exit(1);
	}

	Connection conn(argv[1], port);
	if (!conn.isConnected()) {
		cerr << "Connection attempt failed" << endl;
		exit(1);
	}
	auto cpconn = make_shared < Connection > (conn);
	cout << "Type a command: ";
	string command;
	getline(cin, command);
	while (true) {
		bool goingGood = true;
		vector<string> line = split(command, ' '); // line is the entire line splitted on whitespace

		string message = "";
		if (line[0] == commands.listNG) {
			message += Protocol::COM_LIST_NG;
			message += Protocol::COM_END;
			writeMessage(cpconn, message);
		} else if (line[0] == commands.createNG) {
			string stuff;
			cout << "Creating newsgroup..." << endl;
			message += Protocol::COM_CREATE_NG;
			cout << "Name of the newsgroup: ";
			getline(cin, stuff);
			appendString(message, stuff);
			message += Protocol::COM_END;
			writeMessage(cpconn, message);
		} else if (line[0] == commands.deleteNG) {
			int deletes = -1;
			string stuff;
			cout << "Deleting newsgroup..." << endl;
			message += Protocol::COM_DELETE_NG;
			cout << "ID of the newsgroup you want to delete: ";
			getline(cin, stuff);
			deletes = stoi(stuff);
			appendNumber(message, deletes);
			message += Protocol::COM_END;
			writeMessage(cpconn, message);
		} else if (line[0] == commands.listArt) {
			int listng = -1;
			string stuff;
			cout << "Listing articles..." << endl;
			cout << "Which newsgroup? ";
			getline(cin, stuff);
			listng = stoi(stuff);
			message += Protocol::COM_LIST_ART;
			appendNumber(message, listng);
			message += Protocol::COM_END;
			writeMessage(cpconn, message);
		} else if (line[0] == commands.createArt) {
			int grpnbr = -1;
			string stuff;
			message += Protocol::COM_CREATE_ART;
			cout << "Creating article..." << endl;
			cout << "In which newsgroup? ";
			getline(cin, stuff);
			grpnbr = stoi(stuff);
			cout << endl;
			appendNumber(message, grpnbr);

			cout << "Title:";
			getline(cin, stuff);
			appendString(message, stuff);
			cout << endl;
			cout << "Author:";
			getline(cin, stuff);
			appendString(message, stuff);
			cout << endl;
			cout << "Text:";
			getline(cin, stuff);
			appendString(message, stuff);
			cout << endl;
			message += Protocol::COM_END;
			writeMessage(cpconn, message);
		} else if (line[0] == commands.deleteArt) {
			string stuff;
			int deleteng = -1;
			int deleteart = -1;
			cout << "Deleting article..." << endl;
			cout << "From which newsgroup (ID): ";
			getline(cin, stuff);
			deleteng = stoi(stuff);
			cout << endl;
			cout << "Which article (ID): ";
			getline(cin, stuff);
			deleteart = stoi(stuff);
			message += Protocol::COM_DELETE_ART;
			appendNumber(message, deleteng);
			appendNumber(message, deleteart);
			message += Protocol::COM_END;
			writeMessage(cpconn, message);
		} else if (line[0] == commands.readArt) {
			string stuff;
			int newsg = -1;
			int artnr = -1;
			cout << "Fetching article...";
			cout << "From which newsgroup (ID): ";
			getline(cin, stuff);
			newsg = stoi(stuff);
			cout << endl;
			cout << "Which article (ID): ";
			getline(cin, stuff);
			artnr = stoi(stuff);
			message += Protocol::COM_GET_ART;
			appendNumber(message, newsg);
			appendNumber(message, artnr);
			message += Protocol::COM_END;
			writeMessage(cpconn, message);
		} else if (line[0] == commands.help) {
			cout << "Available commands are:" << endl;
			cout << "listNG" << endl;
			cout << "createNG" << endl;
			cout << "deleteNG" << endl;
			cout << "listArt" << endl;
			cout << "createArt" << endl;
			cout << "deleteArt" << endl;
			cout << "read" << endl;
			cout << "exit" << endl;
			goingGood = false;
		} else if (line[0] == commands.exit){
			cout << "Terminating, thank you for using me!" << endl;
			exit(0);
		} else {
			cout << "Use help to show available commands." << endl;
			goingGood = false;
		}
		if (goingGood) {
			try {
				int nbrOfNG = 0;
				switch (readByte(cpconn)) {
				case Protocol::ANS_LIST_NG:

					nbrOfNG = readNumber(cpconn);
					cout << "Number of newsgroups: " << nbrOfNG << endl;
					for (int i = 0; i < nbrOfNG; i++) {
						int tempInt = readNumber(cpconn);
						cout << tempInt << " ";
						string temp = readString(cpconn);
						cout << temp << endl;
					}

					readEndByteAns(cpconn);

					break;
				case Protocol::ANS_CREATE_NG:
					if (readByte(cpconn) == Protocol::ANS_ACK) {
						cout << "Newsgroup was successfully created." << endl;
					} else {
						readByte(cpconn);
						cout << "Newsgroup already exist." << endl;
					}

					readEndByteAns(cpconn);

					break;
				case Protocol::ANS_DELETE_NG:
					if (readByte(cpconn) == Protocol::ANS_ACK) {
						cout << "Newsgroup was successfully deleted." << endl;
					} else {
						readByte(cpconn);
						cout << "Newsgroup doesn't exist." << endl;
					}
					readEndByteAns(cpconn);
					break;
				case Protocol::ANS_LIST_ART:
					if (readByte(cpconn) == Protocol::ANS_ACK) {
						nbrOfNG = readNumber(cpconn);
						cout << "There are " << nbrOfNG
								<< " articles in this newsgroup." << endl;
						for (int i = 0; i < nbrOfNG; i++) {
							int tempInt = readNumber(cpconn);
							cout << tempInt << " ";
							string temp = readString(cpconn);
							cout << temp << endl;
						}
					} else {
						readByte(cpconn);
						cout << "Newsgroup doesn't exist." << endl;
					}

					readEndByteAns(cpconn);

					break;
				case Protocol::ANS_CREATE_ART:
					if (readByte(cpconn) == Protocol::ANS_ACK) {
						cout << "Article was successfully created." << endl;
					} else {
						readByte(cpconn);
						cout << "Newsgroup doesn't exist." << endl;
					}

					readEndByteAns(cpconn);

					break;
				case Protocol::ANS_DELETE_ART:
					if (readByte(cpconn) == Protocol::ANS_ACK) {
						cout << "Article was successfully deleted." << endl;
					} else {
						if (readByte(cpconn)
								== Protocol::ERR_NG_DOES_NOT_EXIST) {
							cout << "Newsgroup doesn't exist." << endl;
						} else {
							cout << "Article doesn't exist." << endl;
						}
					}

					readEndByteAns(cpconn);

					break;
				case Protocol::ANS_GET_ART:
					if (readByte(cpconn) == Protocol::ANS_ACK) {
						cout << readString(cpconn) << endl;
						cout << readString(cpconn) << endl;
						cout << readString(cpconn) << endl;
					} else {
						if (readByte(cpconn)
								== Protocol::ERR_NG_DOES_NOT_EXIST) {
							cout << "Newsgroup doesn't exist." << endl;
						} else {
							cout << "Article doesn't exist." << endl;
						}
					}

					readEndByteAns(cpconn);
					break;
				default:
					cout << "BLARGH" << endl;
					break;
				}
				cout << "Type another command: ";
				getline(cin, command);
			} catch (ConnectionClosedException&) {
				cout << " no reply from server. Exiting." << endl;
				exit(1);
			}
		} else {
			cout << "Type another command: ";
			getline(cin, command);
		}
	}
}

