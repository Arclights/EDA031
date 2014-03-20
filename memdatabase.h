#include "protocol.h"

#include <map>

using namespace std;

struct Article{
	int newsGroupID;
	string title;
	string text;
};

class MemDatabase{
public:
	MemDatabase();
	string addNewsGroup(const string& title);
	string listNewsGroups();
	string listArticles(int ngID);

private:
	int newsGroupsCounter;
	map<int, string> newsGroups;
	int artCounter;
	map<int, Article> articles;
	Protocol protocol;
};