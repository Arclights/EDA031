#include "protocol.h"

#include <map>

using namespace std;

class MemDatabase{
public:
	MemDatabase();
	string addNewsGroup(const string& title);
	string listNewsGroups();

private:
	int newsGroupsCounter;
	map<int, string> newsGroups;
	Protocol protocol;
};