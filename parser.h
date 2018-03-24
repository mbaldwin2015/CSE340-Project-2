#ifndef __PARSER__H__
#define __PARSER__H__

#include <string>
#include "inputbuf.h"
#include "lexer.h"

struct sTableItem {
	std::string name;
	std::string scope;
	int permission;
};

struct sTable {
	sTableItem* item;
	sTable* next;
	sTable* prev;
};

struct printOutput {
	std::string line;
	printOutput* next;
};

class Parser {
	public:
		void parse_program();
		void parse_global_vars();
		void parse_scope();
		void parse_public_vars();
		void parse_private_vars();
		void parse_var_list();
		void parse_stmt_list();
		void parse_stmt();
		void addList(sTable * table);
		void addString(std::string s);
		void searchList(std::string Id);
};

#endif