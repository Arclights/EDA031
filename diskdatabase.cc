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

/*
 * Splits the name of folders that represent news groups.
 * The returned values are the id ocf the news group and the name
 */
vector<string> getFolderNameParts(string name){
	vector<string> out;
	auto split = name.find_first_of(" ");
	out.push_back(name.substr(0, split));
	out.push_back(name.substr(++split, name.length()));
	return out;
}

/*
 * Applies a function to one or more files in directory dir.
 * The function passed needs to return a boolean to signal if this function should stop or continue
 */
bool applyOnFiles(function<bool (const string&)> funcToApply, const string& dir){
	DIR* dp = opendir(dir.c_str());
	struct dirent *entry;
	while((entry = readdir(dp))){
		if(string(entry->d_name).compare(".") != 0 && string(entry->d_name).compare("..") != 0 ){
			if(funcToApply(string(entry->d_name))){
				return 1;
			}
		}
	}
	closedir(dp);
	return 0;	
}

/*
 *Returns the news group name based on a news group id
 */
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
	function<bool (const string&)> funcToApply = [&ids](const string& s){vector<string> parts = getFolderNameParts(s); ids.push_back(stoi(parts[0])); return 0;};
	applyOnFiles(funcToApply, "database");

	if(ids.empty()){
		return 1;
	}

	sort(ids.begin(), ids.end());
	return ids[ids.size() - 1] + 1;
}

int getNextUniqueArticleID(int ngID){
	vector<int> ids;
	function<bool (const string&)> funcToApply = [&ids](const string& s){ids.push_back(stoi(s)); return 0;};
	applyOnFiles(funcToApply, "database/" + getNewGroupDirName(ngID));

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
	function<bool (const string&)> funcToApply = [&out](const string& s){vector<string> parts = getFolderNameParts(s); out.insert(make_pair(stoi(parts[0]), parts[1])); return 0;};
	applyOnFiles(funcToApply, "database");
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
	function<bool (const string&)> funcToApply = [ngID](const string& s){vector<string> parts = getFolderNameParts(string(s)); return parts[0].compare(to_string(ngID)) == 0 ? remove(("database/" + string(s)).c_str()) : 0;};
	applyOnFiles(funcToApply, "database");
}

bool DiskDatabase::newsGroupTitleExists(const string& title) const{
	function<bool (const string&)> funcToApply = [title](const string& s){vector<string> parts = getFolderNameParts(s); return title.compare(parts[1]) == 0;};
	return applyOnFiles(funcToApply, "database");
}

bool DiskDatabase::newsGroupExists(int ngID) const{
	function<bool (const string&)> funcToApply = [ngID](const string& s){vector<string> parts = getFolderNameParts(s); return to_string(ngID).compare(parts[0]) == 0;};
	return applyOnFiles(funcToApply, "database");
}

bool DiskDatabase::articleExists(int ngID, int artID) const{
	function<bool (const string&)> funcToApply = [artID](const string& s){return to_string(artID).compare(s) == 0;};
	return applyOnFiles(funcToApply, "database/" + getNewGroupDirName(ngID));
}
