#include <algorithm>
#include <iterator>
#include <utility> 
#include <iostream>

#include "memdatabase.h"

MemDatabase::MemDatabase():newsGroupsCounter(0), protocol(Protocol()){}

vector<char> intToChars(int i){
	vector<char> out;
	out.push_back((i >> 24) & 0x000F);
	out.push_back((i >> 16) & 0x000F);
	out.push_back((i >> 8) & 0x000F);
	out.push_back((i) & 0x000F);
	return out;
}

string MemDatabase::addNewsGroup(const string& title){
	auto exists = find_if(newsGroups.begin(), newsGroups.end(), [&title](const pair<int, string>& p){return p.second == title;});

	string out = "";
	if(exists != newsGroups.end()){
		out += protocol.ANS_NAK;
		out += protocol.ERR_NG_ALREADY_EXISTS;
		return out;
	}else{
		cout << "Creating group " << title << endl;
		newsGroups.insert({newsGroupsCounter++, title});
		out += protocol.ANS_ACK;
		return out;
	}
}

string MemDatabase::listNewsGroups(){
	cout << "Listing news groups" << endl;
	string out = "";
	out += protocol.PAR_NUM;
	vector<char> cs = intToChars(newsGroups.size());
	for(char c : cs){
		out += c;
	}

	for(pair<int, string> ng: newsGroups){
		out += protocol.PAR_NUM;
		vector<char> cs = intToChars(ng.first);
		for(char c : cs){
			out += c;
		}
		out += protocol.PAR_STRING;
		cs = intToChars(ng.second.size());
		for(char c : cs){
			out += c;
		}
		out += ng.second;
	}

	return out;
}
