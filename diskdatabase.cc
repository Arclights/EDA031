#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>

#include <iostream>

#include "diskdatabase.h"

DiskDatabase::DiskDatabase(){
	mkdir("database", 0777);
	//Fixa så att om det är en gammal databas så ska gamla max-nycklar läsas in
}

void DiskDatabase::addNewsGroup(const string& title){// Klar
	mkdir((string("database/") + to_string(newsGroupsCounter++) + title).c_str(), 0777);
}

map<int, string> DiskDatabase::getNewsGroups(){// Klar
	map<int, string> out;
	DIR* dp = opendir("database");
	struct dirent *entry;
	readdir(dp); // För att skippa . och ..
	readdir(dp);
	while((entry = readdir(dp))){
		string name = string(entry->d_name).substr(1, strlen(entry->d_name));
		out.insert({entry->d_name[0] - '0', name});
	}
	return out;
}

vector<pair<int, Article>> DiskDatabase::getArticles(int ngID){
	return vector<pair<int, Article>>();
}

void DiskDatabase::addArticle(int ngID, const string& title, const string& author, const string& text){
	
}

Article DiskDatabase::getArticle(int artID){
	return Article();
}

void DiskDatabase::deleteArticle(int artID){
	
}

void DiskDatabase::deleteArticlesInNewsGroup(int ngID){

}

void DiskDatabase::deleteNewsGroup(int ngID){

}

bool DiskDatabase::newsGroupTitleExists(const string& title) const{// Klar
	DIR* dp = opendir("database");
	struct dirent *entry;
	while((entry = readdir(dp))){
		string name = string(entry->d_name).substr(1, strlen(entry->d_name));
		if(title.compare(name) == 0){
			return 1;
		}
		cout << entry->d_name << endl;
	}
	return 0;
}

bool DiskDatabase::newsGroupExists(int ngID) const{
	return 0;
}

bool DiskDatabase::articleExists(int artID) const{
	return 0;
}
