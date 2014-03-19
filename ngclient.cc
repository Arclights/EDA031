/* myclient.cc: sample client program */

#include "connection.h"
#include "commands.h"
#include "connectionclosedexception.h"

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
	conn.write((value >> 24) & 0xFF);
	conn.write((value >> 16) & 0xFF);
	conn.write((value >> 8) & 0xFF);
	conn.write(value & 0xFF);
}

/*
 * Read a string from the server.
 */
string readString(const Connection& conn) {
	string s;
	char ch;
	while ((ch = conn.read()) != '$') {
		s += ch;
	}
	return s;
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
	cout << "Type a command: ";
	string command;
	int nbr;
	getline(cin, command);
	while (true) {

		vector<string> line = split(command, ' '); // line is the entire line splitted on whitespace
		cout << line[0] << endl;

		if (line[0] == commands.listNG) {
			cout << "1" << endl;
		} else if (line[0] == commands.createNG) {
			cout << "2" << endl;
		} else if (line[0] == commands.deleteNG) {
			cout << "3" << endl;
		} else if (line[0] == commands.listArt) {
			cout << "4" << endl;
		} else if (line[0] == commands.createArt) {
			cout << "5" << endl;
		} else if (line[0] == commands.deleteArt) {
			cout << "6" << endl;
		} else if (line[0] == commands.readArt) {
			cout << "7" << endl;
		} else if (line[0] == commands.help && line.size() == 1) {
			cout << "Available commands are:" << endl;
			cout << "read" << endl;
		} else {
			cout << "Use help to show available commands." << endl;
		}
		for(int i = 1; i < line.size(); i++){

		}
		try {
			cout << nbr << " is ...";
			writeNumber(conn, nbr);
			string reply = readString(conn);
			cout << " " << reply << endl;
			cout << "Type another command: ";
			getline(cin, command);
		} catch (ConnectionClosedException&) {
			cout << " no reply from server. Exiting." << endl;
			exit(1);
		}
	}
}


