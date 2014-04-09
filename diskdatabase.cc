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
vector<string> getItemNameParts(string name){
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
 *Returns the name of the item based on an id in directory dir
 */
string getItemName(int id, const string& dir){
	string out;
	string id_str(to_string(id));
	function<bool (const string&)> funcToApply = [&out, &id_str](const string& s){return getItemNameParts(s)[0].compare(id_str) == 0 ? &(out = s) : 0;};
	applyOnFiles(funcToApply, dir);
	return out;
}

int getNextUniqueID(const string& dir){
	vector<int> ids;
	function<bool (const string&)> funcToApply = [&ids](const string& s){ids.push_back(stoi(getItemNameParts(s)[0])); return 0;};
	applyOnFiles(funcToApply, dir);

	if(ids.empty()){
		return 1;
	}

	sort(ids.begin(), ids.end());
	return ids[ids.size() - 1] + 1;
}

int getNextUniqueNewsGroupID(){
	return getNextUniqueID("database");
}

int getNextUniqueArticleID(int ngID){
	return getNextUniqueID("database/" + getItemName(ngID, "database"));
}

DiskDatabase::DiskDatabase(){
	mkdir("database", 0777);
}


void DiskDatabase::addNewsGroup(const string& title){
	mkdir((string("database/") + to_string(getNextUniqueNewsGroupID()) + " " + title).c_str(), 0777);
}

/*
 * Returns a map of item names mapped to their key in directory dir
 */
map<int, string> getItemsInDir(const string& dir){
	map<int, string> out;
	function<bool (const string&)> funcToApply = [&out](const string& s){vector<string> parts = getItemNameParts(s); out.insert(make_pair(stoi(parts[0]), parts[1])); return 0;};
	applyOnFiles(funcToApply, dir);
	return out;
}

map<int, string> DiskDatabase::getNewsGroups(){
	return getItemsInDir("database");
}

map<int, string> DiskDatabase::getArticles(int ngID){
	return getItemsInDir("database/" + getItemName(ngID, "database"));
}

void DiskDatabase::addArticle(int ngID, const string& title, const string& author, const string& text){
	FILE *fp;
	fp = fopen(("database/" + getItemName(ngID, "database") + "/" + to_string(getNextUniqueArticleID(ngID)) + " " + title).c_str(), "w");
	fprintf(fp, "%s", author.c_str());
	fprintf(fp, "\n");
	fprintf(fp, "%s", text.c_str());
	fclose(fp);
}

Article DiskDatabase::getArticle(int ngID, int artID){
	string ngName = getItemName(ngID, "database");
	string artName = getItemName(artID, "database/" + ngName);
	ifstream infile(("database/" + ngName + "/" + artName).c_str());
	stringstream buffer;
	buffer << infile.rdbuf();

	string contents(buffer.str());
	int endOfAuthor = contents.find_first_of("\n");
	string title = getItemNameParts(artName)[1];
	string author = contents.substr(0, endOfAuthor);
	string text = contents.substr(endOfAuthor + 1, contents.length() - endOfAuthor - 1);
	return Article{title, author, text};
}

void DiskDatabase::deleteArticle(int ngID, int artID){
	string ngName = getItemName(ngID, "database");
	string artName = getItemName(artID, "database/" + ngName);
	remove(("database/" + ngName + "/" + artName).c_str());
}

void DiskDatabase::deleteNewsGroup(int ngID){
	remove(("database/" + getItemName(ngID, "database")).c_str());
}

bool DiskDatabase::newsGroupTitleExists(const string& title) const{
	function<bool (const string&)> funcToApply = [title](const string& s){return title.compare(getItemNameParts(s)[1]) == 0;};
	return applyOnFiles(funcToApply, "database");
}

bool DiskDatabase::newsGroupExists(int ngID) const{
	function<bool (const string&)> funcToApply = [ngID](const string& s){return to_string(ngID).compare(getItemNameParts(s)[0]) == 0;};
	return applyOnFiles(funcToApply, "database");
}

bool DiskDatabase::articleExists(int ngID, int artID) const{
	function<bool (const string&)> funcToApply = [artID](const string& s){return to_string(artID).compare(getItemNameParts(s)[0]) == 0;};
	return applyOnFiles(funcToApply, "database/" + getItemName(ngID, "database"));
}
