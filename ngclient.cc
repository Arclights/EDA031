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

		vector<string> line = split(command, ' '); // line is the entire line splitted on whitespace
		cout << line[0] << endl;

		string message = "";
		if (line[0] == commands.listNG) {
			writeNumber(conn, Protocol::COM_LIST_NG);
			writeNumber(conn, Protocol::COM_END);
		} else if (line[0] == commands.createNG) {
			cout << "2" << endl;
			if (line.size() == 2) {
				message += Protocol::COM_CREATE_NG;
				appendString(message, line[1]);
				message += Protocol::COM_END;
				writeMessage(cpconn, message);
			} else {
				cout
						<< "Incorrect number of parameters. Should be \"create NAME\""
						<< endl;
			}
		} else if (line[0] == commands.deleteNG) {
			cout << "3" << endl;
			int deletes = -1;
			if (line.size() == 2) {
				deletes = stoi(line[1]);
				writeNumber(conn, Protocol::COM_DELETE_NG);
				writeNumber(conn, Protocol::PAR_NUM);
				writeNumber(conn, deletes);
				writeNumber(conn, Protocol::COM_END);
			} else {

			}
		} else if (line[0] == commands.listArt) {
			cout << "4" << endl;
			int listng = -1;
			if (line.size() == 2) {
				listng = stoi(line[1]);
				writeNumber(conn, Protocol::COM_LIST_ART);
				writeNumber(conn, Protocol::PAR_NUM);
				writeNumber(conn, listng);
				writeNumber(conn, Protocol::COM_END);
			} else {

			}
		} else if (line[0] == commands.createArt) {
			cout << "5" << endl;
			int grpnbr = -1;
			if (line.size() == 5) {
				writeNumber(conn, Protocol::COM_DELETE_ART);
				writeNumber(conn, Protocol::PAR_NUM);
				grpnbr = stoi(line[1]);
				writeNumber(conn, grpnbr);
				writeNumber(conn, Protocol::PAR_STRING);
				writeNumber(conn, line[2].size());
				writeMessage(cpconn, line[2]);
				writeNumber(conn, Protocol::PAR_STRING);
				writeNumber(conn, line[3].size());
				writeMessage(cpconn, line[3]);
				writeNumber(conn, Protocol::PAR_STRING);
				writeNumber(conn, line[4].size());
				writeMessage(cpconn, line[4]);
				writeNumber(conn, Protocol::COM_END);
			}
		} else if (line[0] == commands.deleteArt) {
			cout << "6" << endl;
			int deleteng = -1;
			int deleteart = -1;
			if (line.size() == 3) {
				deleteng = stoi(line[1]);
				deleteart = stoi(line[2]);
				writeNumber(conn, Protocol::COM_DELETE_ART);
				writeNumber(conn, Protocol::PAR_NUM);
				writeNumber(conn, deleteng);
				writeNumber(conn, Protocol::PAR_NUM);
				writeNumber(conn, deleteart);
				writeNumber(conn, Protocol::COM_END);
			}
		} else if (line[0] == commands.readArt) {
			cout << "7" << endl;
			int newsg = -1;
			int artnr = -1;
			if (line.size() == 3) {
				newsg = stoi(line[1]);
				artnr = stoi(line[2]);
				writeNumber(conn, Protocol::COM_GET_ART);
				writeNumber(conn, Protocol::PAR_NUM);
				writeNumber(conn, newsg);
				writeNumber(conn, Protocol::PAR_NUM);
				writeNumber(conn, artnr);
				writeNumber(conn, Protocol::COM_END);
			}
		} else if (line[0] == commands.help && line.size() == 1) {
			cout << "Available commands are:" << endl;
			cout << "read" << endl;
		} else {
			cout << "Use help to show available commands." << endl;
		}
		for (int i = 1; i < line.size(); i++) {

		}
		try {
			switch (readByte(cpconn)) {
			case Protocol::ANS_LIST_NG: {
				int nbrOfNG = 0;
				nbrOfNG = readNumber(cpconn);
				cout << nbrOfNG << endl;
				for (int i = 0; i < nbrOfNG; i++) {
					int temp = readNumber(cpconn);
					cout << temp << endl;
				}
			}
				readEndByteAns(cpconn);
				break;
			case Protocol::ANS_CREATE_NG: {
				if (readByte(cpconn) == Protocol::ANS_ACK) {
					cout << "Newsgroup was successfully created." << endl;
				} else {
					cout << "Newsgroup already exist." << endl;
				}
			}
				readEndByteAns(cpconn);
				break;
			default:
				cout << "BLARGH" << endl;
				break;
			}

			//string reply = readString(make_shared<Connection>(conn));
			//cout << reply << endl;
			cout << "Type another command: ";
			getline(cin, command);
		} catch (ConnectionClosedException&) {
			cout << " no reply from server. Exiting." << endl;
			exit(1);
		}
	}
}

