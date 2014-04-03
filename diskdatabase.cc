#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <limits.h>
#include <functional>

#include <iostream>

#include "diskdatabase.h"

// For testing
void printDir(){
	DIR* dp = opendir("database");
	struct dirent *entry;
	while((entry = readdir(dp))){
		cout << entry->d_name << endl;
	}
}

vector<string> getFolderNameParts(string name){
	vector<string> out;
	auto split = name.find_first_of(" ");
	out.push_back(name.substr(0, split));
	out.push_back(name.substr(++split, name.length()));
	return out;
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

int getNextUniqueNewsGroupID(){
	vector<int> ids;
	DIR* dp = opendir("database");
	struct dirent *entry;
	while((entry = readdir(dp))){
		if(string(entry->d_name).compare(".") != 0 && string(entry->d_name).compare("..") != 0 ){
			vector<string> parts = getFolderNameParts(string(entry->d_name));
			ids.push_back(stoi(parts[0]));
		}
	}
	closedir(dp);

	if(ids.empty()){
		return 1;
	}

	sort(ids.begin(), ids.end());
	return ids[ids.size() - 1] + 1;
}

int getNextUniqueArticleID(int ngID){
	vector<int> ids;
	DIR* dp = opendir(("database/" + getNewGroupDirName(ngID)).c_str());
	struct dirent *entry;
	while((entry = readdir(dp))){
		if(string(entry->d_name).compare(".") != 0 && string(entry->d_name).compare("..") != 0 ){
			ids.push_back(stoi(entry->d_name));
		}
	}
	closedir(dp);

	if(ids.empty()){
		return 0;
	}

	sort(ids.begin(), ids.end());
	return ids[ids.size() - 1] + 1;
}

DiskDatabase::DiskDatabase(){
	mkdir("database", 0777);
}



void DiskDatabase::addNewsGroup(const string& title){
	mkdir((string("database/") + to_string(getNextUniqueNewsGroupID()) + " " + title).c_str(), 0777);
}


map<int, string> DiskDatabase::getNewsGroups(){
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

map<int, string> DiskDatabase::getArticles(int ngID){
	string dir = getNewGroupDirName(ngID);
	map<int, string> out;
	DIR* dp = opendir(("database/" + dir).c_str());
	struct dirent *entry;
	while((entry = readdir(dp))){
		if(string(entry->d_name).compare(".") != 0 && string(entry->d_name).compare("..") != 0 ){
			ifstream infile(("database/" + getNewGroupDirName(ngID) + "/" + entry->d_name).c_str());
			string title = "";
			getline(infile, title);
			out.insert({stoi(entry->d_name), title});
		}
	}
	closedir(dp);
	return out;
}

void DiskDatabase::addArticle(int ngID, const string& title, const string& author, const string& text){
	FILE *fp;
	fp = fopen(("database/" + getNewGroupDirName(ngID) + "/" + to_string(getNextUniqueArticleID(ngID))).c_str(), "w");
	fprintf(fp, "%s", title.c_str());
	fprintf(fp, "\n");
	fprintf(fp, "%s", author.c_str());
	fprintf(fp, "\n");
	fprintf(fp, "%s", text.c_str());
	fclose(fp);
}

Article DiskDatabase::getArticle(int ngID, int artID){
	ifstream infile(("database/" + getNewGroupDirName(ngID) + "/" + to_string(artID)).c_str());
	stringstream buffer;
	buffer << infile.rdbuf();

	string contents(buffer.str());
	int endOfTitle = contents.find_first_of("\n");
	int endOfAuthor = contents.find_first_of("\n", endOfTitle + 1);
	string title = contents.substr(0, endOfTitle);
	string author = contents.substr(endOfTitle + 1, endOfAuthor - endOfTitle - 1);
	string text = contents.substr(endOfAuthor + 1, contents.length() - endOfAuthor - 1);
	return Article{title, author, text};
}

void DiskDatabase::deleteArticle(int ngID, int artID){
	remove(("database/" + getNewGroupDirName(ngID) + "/" + to_string(artID)).c_str());
}

void DiskDatabase::deleteNewsGroup(int ngID){
	DIR* dp = opendir("database");
	struct dirent *entry;
	while((entry = readdir(dp))){
		if(string(entry->d_name).compare(".") != 0 && string(entry->d_name).compare("..") != 0 ){
			vector<string> parts = getFolderNameParts(string(entry->d_name));
			if(parts[0].compare(to_string(ngID)) == 0){
				remove(("database/" + string(entry->d_name)).c_str());
				break;
			}
		}
	}
	closedir(dp);
}

bool applyOnFile(function<bool (const string&)> applyFunc, const string& dir){
	DIR* dp = opendir(dir.c_str());
	struct dirent *entry;
	while((entry = readdir(dp))){
		if(string(entry->d_name).compare(".") != 0 && string(entry->d_name).compare("..") != 0 ){
			if(applyFunc(string(entry->d_name))){
				return 1;
			}
		}
	}
	closedir(dp);
	return 0;	
}

bool DiskDatabase::newsGroupTitleExists(const string& title) const{
	return applyOnFile([title](const string& s){vector<string> parts = getFolderNameParts(s); return title.compare(parts[1]) == 0;}, "database");
}

bool DiskDatabase::newsGroupExists(int ngID) const{
	return applyOnFile([ngID](const string& s){vector<string> parts = getFolderNameParts(s); return to_string(ngID).compare(parts[0]) == 0;}, "database");
}

bool DiskDatabase::articleExists(int ngID, int artID) const{
	return applyOnFile([artID](const string& s){return to_string(artID).compare(s) == 0;}, "database/" + getNewGroupDirName(ngID));
}
