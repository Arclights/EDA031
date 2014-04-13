#include "messagehandler.h"
#include "misbehavingclientexception.h"
#include "protocol.h"

#include <iostream>
#include <algorithm>
#include <sstream>
#include <iterator>
#include <vector>
#include <sstream>

using namespace std;

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

void readEndByteAns(const shared_ptr<Connection>& conn){
	int paramType = readByte(conn);
	if(paramType != Protocol::ANS_END){
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

void appendString(string& out, const string& s){
	out += Protocol::PAR_STRING;
	appendIntInBytes(out, s.size());
	out += s;
}

void appendNumber(string& out, int number){
	out += Protocol::PAR_NUM;
	appendIntInBytes(out, number);
}


void writeMessage(const shared_ptr<Connection>& conn, const string& s){
	for (char c : s) {
		conn->write(c);
	}
}
