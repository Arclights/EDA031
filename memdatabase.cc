#include <algorithm>
#include <iterator>
#include <utility> 
#include <iostream>

#include "memdatabase.h"

MemDatabase::MemDatabase():newsGroupsCounter(0), protocol(Protocol()){}

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
	
}
