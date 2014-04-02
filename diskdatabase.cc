#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <stdio.h>

#include <iostream>

#include "diskdatabase.h"

DiskDatabase::DiskDatabase(){
	mkdir("database", 0777);
	//Fixa så att om det är en gammal databas så ska gamla max-nycklar läsas in
}

void DiskDatabase::addNewsGroup(const string& title){// Klar
	mkdir((string("database/") + to_string(newsGroupsCounter++) + " " + title).c_str(), 0777);
}

vector<string> getFolderNameParts(string name){
	vector<string> out;
	auto split = name.find_first_of(" ");
	out.push_back(name.substr(0, split));
	out.push_back(name.substr(++split, name.length()));
	return out;
}

// For testing
void printDir(){
	DIR* dp = opendir("database");
	struct dirent *entry;
	while((entry = readdir(dp))){
		cout << entry->d_name << endl;
	}
}

map<int, string> DiskDatabase::getNewsGroups(){// Klar
	map<int, string> out;
	DIR* dp = opendir("database");
	struct dirent *entry;
	while((entry = readdir(dp))){
		if(string(entry->d_name).compare(".") != 0 && string(entry->d_name).compare("..") != 0 ){
			vector<string> parts = getFolderNameParts(string(entry->d_name));
			out.insert({stoi(parts[0]), parts[1]});
		}
	}
	closedir(dp);
	return out;
}

vector<pair<int, string>> DiskDatabase::getArticles(int ngID){
	return vector<pair<int, string>>();
}

string getNewGroupDirName(int ngID){
	DIR* dp = opendir("database");
	struct dirent *entry;
	while((entry = readdir(dp))){
		if(string(entry->d_name).compare(".") != 0 && string(entry->d_name).compare("..") != 0 ){
			vector<string> parts = getFolderNameParts(string(entry->d_name));
			if(parts[0].compare(to_string(ngID)) == 0){
				closedir(dp);
				return string(entry->d_name);
			}
		}
	}
	closedir(dp);
	return NULL;
}

void DiskDatabase::addArticle(int ngID, const string& title, const string& author, const string& text){// Klar
	FILE *fp;

	fp = fopen(("database/" + getNewGroupDirName(ngID) + "/" + to_string(artCounter++)).c_str(), "w");
	fprintf(fp, title.c_str());
	fprintf(fp, "\n");
	fprintf(fp, author.c_str());
	fprintf(fp, "\n");
	fprintf(fp, text.c_str());
	fclose(fp);
}

Article DiskDatabase::getArticle(int artID){
	return Article();
}

void DiskDatabase::deleteArticle(int artID){
	
}

void DiskDatabase::deleteArticlesInNewsGroup(int ngID){

}

void DiskDatabase::deleteNewsGroup(int ngID){// Klar
	DIR* dp = opendir("database");
	struct dirent *entry;
	cout << "Want to delete " << ngID << endl;
	while((entry = readdir(dp))){
		if(string(entry->d_name).compare(".") != 0 && string(entry->d_name).compare("..") != 0 ){
			vector<string> parts = getFolderNameParts(string(entry->d_name));
			if(parts[0].compare(to_string(ngID)) == 0){
				cout << "removing " << ("database/" + string(entry->d_name)) << endl;
				remove(("database/" + string(entry->d_name)).c_str());
				break;
			}
		}
	}
	closedir(dp);
}

bool DiskDatabase::newsGroupTitleExists(const string& title) const{// Klar
	DIR* dp = opendir("database");
	struct dirent *entry;
	while((entry = readdir(dp))){
		if(string(entry->d_name).compare(".") != 0 && string(entry->d_name).compare("..") != 0 ){
			vector<string> parts = getFolderNameParts(string(entry->d_name));
			if(title.compare(parts[1]) == 0){
				return 1;
			}
		}
	}
	closedir(dp);
	return 0;
}

bool DiskDatabase::newsGroupExists(int ngID) const{// Klar
	DIR* dp = opendir("database");
	struct dirent *entry;
	while((entry = readdir(dp))){
		if(string(entry->d_name).compare(".") != 0 && string(entry->d_name).compare("..") != 0 ){
			vector<string> parts = getFolderNameParts(string(entry->d_name));
			if(ngID == stoi(parts[0])){
				return 1;
			}
		}
	}
	closedir(dp);
	return 0;
}

bool DiskDatabase::articleExists(int artID) const{
	return 0;
}
