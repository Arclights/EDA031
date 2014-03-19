/*
 * commands.h
 */

#ifndef COMMANDS_H_
#define COMMANDS_H_
#include <string>
#include <list>

using namespace std;

struct Commands {
	string listNG = "listNG"; //Listing newsgroups
	string createNG = "createNG"; // Create newsgroup
	string deleteNG = "deleteNG"; // Delete newsgroup
	string listArt = "listArt"; // List articles
	string createArt = "createArt"; // Create an article
	string deleteArt = "deleteArt";// Deletes an article
	string readArt = "read"; // Reads an article

	string help = "help"; // Displays all commands or how to use a certain command

} commands;

std:: list<string> listComm;


#endif /* COMMANDS_H_ */
