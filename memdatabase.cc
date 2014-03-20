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

void appendIntInBytes(string& out, int i){
	vector<char> cs = intToChars(i);
	for(char c : cs){
		out += c;
	}
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
	appendIntInBytes(out, newsGroups.size());

	for(pair<int, string> ng: newsGroups){
		out += protocol.PAR_NUM;
		appendIntInBytes(out, ng.first);
		out += protocol.PAR_STRING;
		appendIntInBytes(out, ng.second.size());
		out += ng.second;
	}

	return out;
}

string MemDatabase::listArticles(int ngID){
	auto exists = find_if(newsGroups.begin(), newsGroups.end(), [&ngID](const pair<int, string>& p){return p.first == ngID;});

	string out = "";
	if(exists == newsGroups.end()){
		out += protocol.ANS_NAK;
		out += protocol.ERR_NG_DOES_NOT_EXIST;
		return out;
	}else{
		vector<pair<int, Article>> foundArticles;
		for(pair<int, Article> art : articles){
			if(art.second.newsGroupID == ngID){
				foundArticles.push_back(art);
			}
		}
		out += protocol.ANS_ACK;
		out += protocol.PAR_NUM;
		appendIntInBytes(out, foundArticles.size());
		for(pair<int, Article> art : foundArticles){
			out += protocol.PAR_NUM;
			appendIntInBytes(out, art.first);
			out += protocol.PAR_STRING;
			appendIntInBytes(out, art.second.title.size());
			out += art.second.title;
		}
	}

	return out;
}
