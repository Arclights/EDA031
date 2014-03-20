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
	string addNewsGroup(const string& title);
	string listNewsGroups();
	string listArticles(int ngID);
	string addArticle(int ngID, const string& title, const string& author, const string& text);
	string getArticle(int ngID, int artID);
	string deleteArticle(int ngID, int artID);
	string deleteNewsGroup(int ngID);
private:
	void appendString(string& out, string& s);
	void appendNumber(string& out, int number);
	int newsGroupsCounter;
	map<int, string> newsGroups;
	int artCounter;
	map<int, Article> articles;
	Protocol protocol;
};