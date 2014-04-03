#include <algorithm>
#include <iterator>
#include <utility> 
#include <iostream>
#include <string.h>

#include "memdatabase.h"

MemDatabase::MemDatabase(){}

bool MemDatabase::newsGroupTitleExists(const string& title) const{
	return find_if(newsGroups.begin(), newsGroups.end(), [&title](const pair<int, string>& p){return p.second == title;}) != newsGroups.end();
}

bool MemDatabase::newsGroupExists(int ngID) const{
	return newsGroups.find(ngID) != newsGroups.end();
}

bool MemDatabase::articleExists(int ngID, int artID) const{
	return articles.at(ngID).find(artID) != articles.at(ngID).end();
}

void MemDatabase::addNewsGroup(const string& title){
	newsGroups.insert({newsGroupsCounter++, title});
}

map<int, string> MemDatabase::getNewsGroups(){
	return newsGroups;
}

map<int, string> MemDatabase::getArticles(int ngID){
	map<int, string> foundArticles;
		for(pair<int, Article> art : articles[ngID]){
			foundArticles.insert({art.first, art.second.title});
		}
	return foundArticles;
}

void MemDatabase::addArticle(int ngID, const string& title, const string& author, const string& text){
	Article article;
	article.title = title;
	article.author = author;
	article.text = text;
	auto arts = articles.find(ngID);
	if(arts == articles.end()){
		articles.insert({ngID, map<int, Article>()});
	}
	articles[ngID].insert({artCounter++, article});
}

Article MemDatabase::getArticle(int ngID, int artID){
	return articles[ngID][artID];
}

void MemDatabase::deleteArticle(int ngID, int artID){
	articles.at(ngID).erase(artID);
}

void MemDatabase::deleteNewsGroup(int ngID){
	newsGroups.erase(ngID);
}
