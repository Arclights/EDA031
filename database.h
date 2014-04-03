#ifndef DATABASE_H
#define DATABASE_H

#include <map>
#include <vector>

using namespace std;

struct Article{
	string title;
	string author;
	string text;
};

class Database{
public:
	Database():newsGroupsCounter(1), artCounter(9){}
	virtual void addNewsGroup(const string& title) = 0;
	virtual map<int, string> getNewsGroups() = 0;
	virtual map<int, string> getArticles(int ngID) = 0;
	virtual void addArticle(int ngID, const string& title, const string& author, const string& text) = 0;
	virtual Article getArticle(int ngID, int artID) = 0;
	virtual void deleteArticle(int ngID, int artID) = 0;
	virtual void deleteNewsGroup(int ngID) = 0;
	virtual bool newsGroupTitleExists(const string& title) const = 0;
	virtual bool newsGroupExists(int ngID) const = 0;
	virtual bool articleExists(int ngID, int artID) const = 0;
protected:
	int newsGroupsCounter;
	int artCounter;
};
#endif
