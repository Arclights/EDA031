#include <map>

#include "database.h"

using namespace std;

class MemDatabase : public Database{
public:
	MemDatabase();
	void addNewsGroup(const string& title) override;
	map<int, string> getNewsGroups() override;
	vector<pair<int, Article>> getArticles(int ngID) override;
	void addArticle(int ngID, const string& title, const string& author, const string& text) override;
	Article getArticle(int artID) override;
	void deleteArticle(int artID) override;
	void deleteArticlesInNewsGroup(int ngID) override;
	void deleteNewsGroup(int ngID) override;
	bool newsGroupTitleExists(const string& title) const override;
	bool newsGroupExists(int ngID) const override;
	bool articleExists(int artID) const override;
private:
	int newsGroupsCounter;
	map<int, string> newsGroups;
	int artCounter;
	map<int, Article> articles;
};
