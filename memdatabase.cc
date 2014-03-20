#include <algorithm>
#include <iterator>
#include <utility> 
#include <iostream>

#include "memdatabase.h"

MemDatabase::MemDatabase():newsGroupsCounter(1){}

bool MemDatabase::newsGroupTitleExists(const string& title) const{
	return find_if(newsGroups.begin(), newsGroups.end(), [&title](const pair<int, string>& p){return p.second == title;}) != newsGroups.end();
}

bool MemDatabase::newsGroupExists(int ngID) const{
	return newsGroups.find(ngID) != newsGroups.end();
}

bool MemDatabase::articleExists(int artID) const{
	return articles.find(artID) != articles.end();
}

void MemDatabase::addNewsGroup(const string& title){
	newsGroups.insert({newsGroupsCounter++, title});
}

map<int, string> MemDatabase::getNewsGroups(){
	return newsGroups;
}

vector<pair<int, Article>> MemDatabase::getArticles(int ngID){
	vector<pair<int, Article>> foundArticles;
		for(pair<int, Article> art : articles){
			if(art.second.newsGroupID == ngID){
				foundArticles.push_back(art);
		}
	}
	return foundArticles;
}

void MemDatabase::addArticle(int ngID, const string& title, const string& author, const string& text){
	Article article;
	article.newsGroupID = ngID;
	article.title = title;
	article.author = author;
	article.text = text;
	articles.insert({artCounter++, article});
}

Article MemDatabase::getArticle(int artID){
	return articles[artID];
}

void MemDatabase::deleteArticle(int artID){
	articles.erase(artID);
}

void MemDatabase::deleteArticlesInNewsGroup(int ngID){
	for(pair<int, Article> art:articles){
		if(art.second.newsGroupID == ngID){
			deleteArticle(art.first);
		}
	}
}

void MemDatabase::deleteNewsGroup(int ngID){
	newsGroups.erase(ngID);
}
