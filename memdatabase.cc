#include <algorithm>
#include <iterator>
#include <utility> 
#include <iostream>

#include "memdatabase.h"

MemDatabase::MemDatabase():newsGroupsCounter(1), protocol(Protocol()){}

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

void MemDatabase::appendString(string& out, string& s){
	out += protocol.PAR_STRING;
	appendIntInBytes(out, s.size());
	out += s;
}

void MemDatabase::appendNumber(string& out, int number){
	out += protocol.PAR_NUM;
	appendIntInBytes(out, number);
}

string MemDatabase::addNewsGroup(const string& title){
	auto exists = find_if(newsGroups.begin(), newsGroups.end(), [&title](const pair<int, string>& p){return p.second == title;});

	string out = "";
	if(exists != newsGroups.end()){
		out += protocol.ANS_NAK;
		out += protocol.ERR_NG_ALREADY_EXISTS;
	}else{
		cout << "Creating group " << title << endl;
		newsGroups.insert({newsGroupsCounter++, title});
		out += protocol.ANS_ACK;
	}
	return out;
}

string MemDatabase::listNewsGroups(){
	cout << "Listing news groups" << endl;
	string out = "";
	appendNumber(out, newsGroups.size());

	for(pair<int, string> ng: newsGroups){
		appendNumber(out, ng.first);
		appendString(out, ng.second);
	}

	return out;
}

string MemDatabase::listArticles(int ngID){
	auto exists = newsGroups.find(ngID);

	string out = "";
	if(exists == newsGroups.end()){
		out += protocol.ANS_NAK;
		out += protocol.ERR_NG_DOES_NOT_EXIST;
	}else{
		vector<pair<int, Article>> foundArticles;
		for(pair<int, Article> art : articles){
			if(art.second.newsGroupID == ngID){
				foundArticles.push_back(art);
			}
		}
		out += protocol.ANS_ACK;
		appendNumber(out, foundArticles.size());
		for(pair<int, Article> art : foundArticles){
			appendNumber(out, art.first);
			appendString(out, art.second.title);
		}
	}

	return out;
}

string MemDatabase::addArticle(int ngID, const string& title, const string& author, const string& text){
	auto exists = newsGroups.find(ngID);

	string out = "";
	if(exists == newsGroups.end()){
		out += protocol.ANS_NAK;
		out += protocol.ERR_NG_DOES_NOT_EXIST;
	}else{
		Article article;
		article.newsGroupID = ngID;
		article.title = title;
		article.author = author;
		article.text = text;
		articles.insert({artCounter++, article});
		out += protocol.ANS_ACK;
	}

	return out;
}

string MemDatabase::getArticle(int ngID, int artID){
	auto ngExists = newsGroups.find(ngID);

	string out = "";
	if(ngExists == newsGroups.end()){
		out += protocol.ANS_NAK;
		out += protocol.ERR_NG_DOES_NOT_EXIST;
	}else{
		auto artExists = articles.find(artID);

		if(artExists == articles.end()){
			out += protocol.ANS_NAK;
			out += protocol.ERR_ART_DOES_NOT_EXIST;
		}else{
			out += protocol.ANS_ACK;
			appendString(out, artExists->second.title);
			appendString(out, artExists->second.author);
			appendString(out, artExists->second.text);
		}
	}

	return out;
}

string MemDatabase::deleteArticle(int ngID, int artID){
	auto ngExists = newsGroups.find(ngID);

	string out = "";
	if(ngExists == newsGroups.end()){
		out += protocol.ANS_NAK;
		out += protocol.ERR_NG_DOES_NOT_EXIST;
	}else{
		auto artExists = articles.find(artID);

		if(artExists == articles.end()){
			out += protocol.ANS_NAK;
			out += protocol.ERR_ART_DOES_NOT_EXIST;
		}else{
			articles.erase(artExists);
			out += protocol.ANS_ACK;
		}
	}

	return out;
}
