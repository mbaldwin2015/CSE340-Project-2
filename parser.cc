/*
Michael Baldwin
1208386656
7/29/2016
*/

#include <iostream>
#include <istream>
#include <fstream>
#include <vector>
#include <string>
#include <cctype>
#include <stdio.h>
#include <cstring>
#include <cstdlib>
#include <cstdio>

#include "lexer.h"
#include "inputbuf.h"
#include "parser.h"

using namespace std;

/*struct sTableItem {
	string name;
	string scope;
	int permission;
};

struct sTable {
	sTableItem* item;
	sTable* next;
	sTable* prev;
};

struct printOutput {
	string line;
	printOutput* next;
};*/

LexicalAnalyzer lexer;
Parser parser;
Token token;
sTable* entry1; // for searching, refer to parse_stmt()
sTable* entry2; // for searching, refer to parse_stmt()
sTable* head;
sTable* temp;
sTable* n;
sTable* ptr;
printOutput* stringHead;
printOutput* stringTemp;
printOutput* stringNew;
string currentScope = "ERROR", line = "ERROR";
int currentPermission = -1;
bool syntaxError = false;

void Parser::parse_program()
{
	bool scopeParsed = false;
	token = lexer.GetToken();
	if (token.token_type == ID) {
		Token token2 = lexer.GetToken();
		if (token2.token_type == COMMA || token2.token_type == SEMICOLON) {
			lexer.UngetToken(token2);
			lexer.UngetToken(token);
			parser.parse_global_vars();
			parser.parse_scope();
			scopeParsed = true;
		}
		else if (token2.token_type == LBRACE) {
			lexer.UngetToken(token2);
			lexer.UngetToken(token);
			parser.parse_scope();
			scopeParsed = true;
		}
		else {
			syntaxError = true;
			return;
		}
	}

	token = lexer.GetToken();
	if (token.token_type == LBRACE || scopeParsed) {
		//cout << "program parsed\n";
	}
	else {
		syntaxError = true;
		return;
	}
}

void Parser::parse_global_vars()
{
	token = lexer.GetToken();
	if (token.token_type == ID) {
		currentScope = "::"; // save scope before creating vars
		currentPermission = 0; // 0 = GLOBAL permission, save before creating vars
		lexer.UngetToken(token);
		parser.parse_var_list();
	}
	else {
		syntaxError = true;
		return;
	}

	token = lexer.GetToken();
	if (token.token_type == SEMICOLON) {
		//cout << "global_vars parsed\n";
	}
	else {
		syntaxError = true;
		return;
	}
}

void Parser::parse_scope()
{
	token = lexer.GetToken();
	if (token.token_type == ID) {
		currentScope = token.lexeme; // save current scope before creating vars
		token = lexer.GetToken();
		if (token.token_type == LBRACE) {
			parse_public_vars();
			parse_private_vars();
			parse_stmt_list();
			token = lexer.GetToken();
			if (token.token_type == RBRACE) {
				//cout << "scope parsed\n";
			} else {
				syntaxError = true;
				return;
			}
		}
		else {
			syntaxError = true;
			return;
		}
	}
	else {
		syntaxError = true;
		return;
	}
}

void Parser::parse_public_vars()
{
	token = lexer.GetToken();
	if (token.token_type == PUBLIC) {
		currentPermission = 1; // 1 = PUBLIC permission, save before creating vars
		token = lexer.GetToken();
		if (token.token_type == COLON) {
			token = lexer.GetToken();
			if (token.token_type == ID) {
				lexer.UngetToken(token);
				parser.parse_var_list();
			} else {
				syntaxError = true;
				return;
			}
		}
		else {
			syntaxError = true;
			return;
		}
	}
	else {
		lexer.UngetToken(token);
		return;
	}

	token = lexer.GetToken();
	if (token.token_type == SEMICOLON) {
		//cout << "public_vars parsed\n";
	}
	else {
		syntaxError = true;
		return;
	}
	return;
}

void Parser::parse_private_vars()
{
	token = lexer.GetToken();
	if (token.token_type == PRIVATE) {
		currentPermission = 2; // 2 = PRIVATE permission, save before creating vars
		token = lexer.GetToken();
		if (token.token_type == COLON) {
			token = lexer.GetToken();
			if (token.token_type == ID) {
				lexer.UngetToken(token);
				parser.parse_var_list();
			}
			else {
				syntaxError = true;
				return;
			}
		}
		else {
			syntaxError = true;
			return;
		}
	}
	else {
		lexer.UngetToken(token);
		return;
	}

	token = lexer.GetToken();
	if (token.token_type == SEMICOLON) {
		//cout << "private_vars parsed\n";
	}
	else {
		syntaxError = true;
		return;
	}
	return;
}

void Parser::parse_var_list()
{
	token = lexer.GetToken();
	if (token.token_type == ID) {
		// add a new var to the table
		if (head->item->permission == -2) {
			n->item->name = token.lexeme;
			n->item->scope = currentScope;
			n->item->permission = currentPermission;
		}
		else {
			n = new sTable;
			n->item = new sTableItem;
			n->item->name = token.lexeme;
			n->item->scope = currentScope;
			n->item->permission = currentPermission;
			addList(n);
		}

		token = lexer.GetToken();
		if (token.token_type == COMMA) {
			parser.parse_var_list();
		} else if (token.token_type == SEMICOLON) {
			lexer.UngetToken(token);
		}
	}
	else {
		syntaxError = true;
		return;
	}
}

void Parser::parse_stmt_list()
{
	token = lexer.GetToken();
	if (token.token_type == ID) {
		Token token2 = lexer.GetToken();
		if (token2.token_type == EQUAL || token2.token_type == LBRACE) {
			lexer.UngetToken(token2);
			lexer.UngetToken(token);
			parser.parse_stmt();
		}
		else {
			lexer.UngetToken(token2);
			lexer.UngetToken(token);
			return;
		}

		token = lexer.GetToken();
		if (token.token_type == ID) {
			lexer.UngetToken(token);
			parser.parse_stmt_list();
		}
		else {
			lexer.UngetToken(token);
			//cout << "stmt_list parsed\n";
		}

		return;
	}
}

void Parser::parse_stmt()
{
	token = lexer.GetToken();
	if (token.token_type == ID) {
		Token token2 = lexer.GetToken();
		if (token2.token_type == EQUAL) {
			Token token3 = lexer.GetToken();
			Token token4 = lexer.GetToken();
			if (token3.token_type != ID && token4.token_type != SEMICOLON) {
				syntaxError = true;
				return;
			}
			else {
				// Resolve the names
				line = "";
				searchList(token.lexeme);
				line += " = ";
				searchList(token3.lexeme);
				line += "\n";
				addString(line);
			}
		}
		else if (token2.token_type == LBRACE) {
			lexer.UngetToken(token2);
			lexer.UngetToken(token);
			parser.parse_scope();
		}
		else {
			syntaxError = true;
			return;
		}
	}
	else {
		lexer.UngetToken(token);
	}
	//cout << "stmt parsed\n";
	return;
}

void Parser::addList(sTable* table) {
	temp->next = table;
	table->prev = temp;
	table->next = NULL;
	temp = temp->next;
}

void Parser::addString(string s) {
	if (stringHead->line == "BRAND NEW LIST") {
		stringNew->line = s;
	} else {
		stringNew = new printOutput;
		stringNew->line = s;
		stringTemp->next = stringNew;
		stringNew->next = NULL;
		stringTemp = stringTemp->next;
	}
}

void Parser::searchList(string Id) {
	bool found = false;
	ptr = temp;
	if (ptr->prev == NULL) {
		if (ptr->item->name == Id) {
			if (ptr->item->scope != currentScope && ptr->item->scope != "::") line += "?.";
			else {
				line += ptr->item->scope;
				if (ptr->item->scope != "::") line += ".";
			}
			line += Id;
		}
		else {
			line += "?.";
			line += Id;
		}
	}	else {
		while (ptr != NULL && !found) {
			if (ptr->item->name == Id) {
				if ((ptr->item->scope != currentScope && (ptr->item->permission == 0 || ptr->item->permission == 1)) 
					|| ptr->item->scope == currentScope || ptr->item->scope == "::") {
					line += ptr->item->scope;
					if (ptr->item->scope != "::") line += ".";
					found = true;
				}
			}
			ptr = ptr->prev;
		}
		if (!found) line += "?.";
		line += Id;
	}
}

int main()
{
	n = new sTable;
	n->next = NULL;
	n->prev = NULL;
	n->item = new sTableItem;
	n->item->permission = -2;
	head = n;
	temp = n;
	stringNew = new printOutput;
	stringNew->next = NULL;
	stringNew->line = "BRAND NEW LIST";
	stringHead = stringNew;
	stringTemp = stringNew;

	parser.parse_program();

	// Print final name resolution result
	if (syntaxError == true) {
		cout << "Syntax Error\n";
	}
	else {
		stringTemp = stringHead;
		while (stringTemp != NULL) {
			cout << stringTemp->line;
			stringTemp = stringTemp->next;
		}
	}
}