#include "database.h"

using namespace std;

class MemDatabase : public Database{
public:
	MemDatabase();
	void addNewsGroup(const string& title) override;
	map<int, string> getNewsGroups() override;
	map<int, string> getArticles(int ngID) override;
	void addArticle(int ngID, const string& title, const string& author, const string& text) override;
	Article getArticle(int ngID, int artID) override;
	void deleteArticle(int ngID, int artID) override;
	void deleteNewsGroup(int ngID) override;
	bool newsGroupTitleExists(const string& title) const override;
	bool newsGroupExists(int ngID) const override;
	bool articleExists(int ngID, int artID) const override;
private:
	map<int, string> newsGroups;
	map<int, map<int, Article>> articles;
};
