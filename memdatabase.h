#include "protocol.h"

#include <map>

using namespace std;

struct Article{
	int newsGroupID;
	string title;
	string author;
	string text;
};

class MemDatabase{
public:
	MemDatabase();
	void addNewsGroup(const string& title);
	map<int, string> getNewsGroups();
	vector<pair<int, Article>> getArticles(int ngID);
	void addArticle(int ngID, const string& title, const string& author, const string& text);
	Article getArticle(int artID);
	void deleteArticle(int artID);
	void deleteArticlesInNewsGroup(int ngID);
	void deleteNewsGroup(int ngID);
	bool newsGroupTitleExists(const string& title) const;
	bool newsGroupExists(int ngID) const;
	bool articleExists(int artID) const;
private:
	int newsGroupsCounter;
	map<int, string> newsGroups;
	int artCounter;
	map<int, Article> articles;
};