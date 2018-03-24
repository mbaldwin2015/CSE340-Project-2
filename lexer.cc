/*
 * Copyright (C) Rida Bazzi, 2016
 *
 * Do not share this file with anyone
 */
#include <iostream>
#include <istream>
#include <vector>
#include <string>
#include <cctype>
#include <stdio.h>
#include <cstring>
#include <algorithm>
#include <functional>
#include <cctype>

#include "lexer.h"
#include "inputbuf.h"

using namespace std;

string reserved[] = { "END_OF_FILE",
    "PUBLIC", "PRIVATE",
	"EQUAL", "COLON", "COMMA", "SEMICOLON",
	"LBRACE", "RBRACE", "ID", "ERROR"
};

#define KEYWORDS_COUNT 2
string keyword[] = { "public", "private" };

void Token::Print()
{
    cout << "{" << this->lexeme << " , "
         << reserved[(int) this->token_type] << " , "
         << this->line_no << "}\n";
}

LexicalAnalyzer::LexicalAnalyzer()
{
    this->line_no = 1;
    tmp.lexeme = "";
    tmp.line_no = 1;
    tmp.token_type = ERROR;
}

bool LexicalAnalyzer::SkipSpace()
{
    char c;
    bool space_encountered = false;

    input.GetChar(c);
    line_no += (c == '\n');

    while (!input.EndOfInput() && isspace(c)) {
        space_encountered = true;
        input.GetChar(c);
        line_no += (c == '\n');
    }

    if (!input.EndOfInput()) {
        input.UngetChar(c);
    }
    return space_encountered;
}

bool LexicalAnalyzer::IsKeyword(string s)
{
    for (int i = 0; i < KEYWORDS_COUNT; i++) {
        if (s == keyword[i]) {
            return true;
        }
    }
    return false;
}

TokenType LexicalAnalyzer::FindKeywordIndex(string s)
{
    for (int i = 0; i < KEYWORDS_COUNT; i++) {
        if (s == keyword[i]) {
            return (TokenType) (i + 1);
        }
    }
    return ERROR;
}

/*
Token LexicalAnalyzer::ScanNumber()
{
    char c;

    input.GetChar(c);
    if (isdigit(c)) {
        if (c == '0') {
            tmp.lexeme = "0";
        } else {
            tmp.lexeme = "";
            while (!input.EndOfInput() && isdigit(c)) {
                tmp.lexeme += c;
                input.GetChar(c);
            }
            if (!input.EndOfInput()) {
                input.UngetChar(c);
            }
        }

		input.GetChar(c);
		if (c == '.') {
			char tmpChar = c;
			input.GetChar(c);
			if (isdigit(c)) {
				tmp.lexeme += tmpChar;
				tmp.lexeme += c;
				input.GetChar(c);
				while (!input.EndOfInput() && isdigit(c)) {
					tmp.lexeme += c;
					input.GetChar(c);
				}
				if (!input.EndOfInput()) {
					input.UngetChar(c);
				}
				tmp.token_type = REALNUM;
				tmp.line_no = line_no;
				return tmp;
			}
			input.UngetChar(c);
			input.UngetChar(tmpChar);
		} else if (c == 'x') {
			char tmpChar = c;
			input.GetChar(c);
			if (c == '0') {
				char tmpChar2 = c;
				input.GetChar(c);
				if (c == '8') {
					bool isDigits = true;
					char lexString[1024];
					strcpy(lexString, tmp.lexeme.c_str());
					int i = 0;

					if ((int)(lexString[i] - '0') > 7 && (int)(lexString[i] - '0') < 1)
						isDigits = false;

					i++;

					while (isDigits && i < tmp.lexeme.size() - 1) {
						if ((int)(lexString[i] - '0') > 7 && (int)(lexString[i] - '0') < 0)
							isDigits = false;
						i++;
					}

					if (tmp.lexeme.size() == 1 && tmp.lexeme == "0") isDigits = true;

					if (isDigits) {
						tmp.lexeme += tmpChar;
						tmp.lexeme += tmpChar2;
						tmp.lexeme += c;
						tmp.token_type = BASE08NUM;
						tmp.line_no = line_no;
						return tmp;
					}
				}
				input.UngetChar(c);
				input.UngetChar(tmpChar2);
			}
			else if (c == '1') {
				char tmpChar2 = c;
				input.GetChar(c);
				if (c == '6') {
					bool isDigits = true;
					char lexString[1024];
					strcpy(lexString, tmp.lexeme.c_str());
					int i = 0;

					if ((int)(lexString[i] - '0') > 9 && (int)(lexString[i] - '0') < 1)
						isDigits = false;

					if ((lexString[i] == 'A' || lexString[i] == 'B' || lexString[i] == 'C' ||
						lexString[i] == 'D' || lexString[i] == 'E' || lexString[i] == 'F'))
						isDigits = true;

					i++;

					while (isDigits && i < tmp.lexeme.size() - 1) {
						if ((int)(lexString[i] - '0') > 9 && (int)(lexString[i] - '0') < 1)
							isDigits = false;
						if ((lexString[i] == 'A' || lexString[i] == 'B' || lexString[i] == 'C' ||
							lexString[i] == 'D' || lexString[i] == 'E' || lexString[i] == 'F'))
							isDigits = true;
						i++;
					}

					if (tmp.lexeme.size() == 1 && tmp.lexeme == "0") isDigits = true;

					if (isDigits) {
						tmp.lexeme += tmpChar;
						tmp.lexeme += tmpChar2;
						tmp.lexeme += c;
						tmp.token_type = BASE16NUM;
						tmp.line_no = line_no;
						return tmp;
					}
				}
				input.UngetChar(c);
				input.UngetChar(tmpChar2);
			}
			input.UngetChar(tmpChar);
		}
		else {
			if (c != ' ') input.UngetChar(c);
		}
		
        tmp.token_type = NUM;
        tmp.line_no = line_no;
        return tmp;
    } else {
        if (!input.EndOfInput()) {
            input.UngetChar(c);
        }
        tmp.lexeme = "";
        tmp.token_type = ERROR;
        tmp.line_no = line_no;
        return tmp;
    }
}*/

Token LexicalAnalyzer::ScanIdOrKeyword()
{
    char c;
    input.GetChar(c);

    if (isalpha(c)) {
        tmp.lexeme = "";
        while (!input.EndOfInput() && isalnum(c)) {
            tmp.lexeme += c;
            input.GetChar(c);
        }
        if (!input.EndOfInput()) {
            input.UngetChar(c);
        }
        tmp.line_no = line_no;
		if (IsKeyword(tmp.lexeme))
			tmp.token_type = FindKeywordIndex(tmp.lexeme);
		else
			tmp.token_type = ID;
    } else {
        if (!input.EndOfInput()) {
            input.UngetChar(c);
        }
        tmp.lexeme = "";
        tmp.token_type = ERROR;
    }
    return tmp;
}


// you should unget tokens in the reverse order in which they
// are obtained. If you execute
//
//    t1 = lexer.GetToken();
//    t2 = lexer.GetToken();
//    t3 = lexer.GetToken();
//
// in this order, you should execute
//
//    lexer.UngetToken(t3);
//    lexer.UngetToken(t2);
//    lexer.UngetToken(t1);
//
// if you want to unget all three tokens. Note that it does not
// make sense to unget t1 without first ungetting t2 and t3
//
TokenType LexicalAnalyzer::UngetToken(Token tok)
{
    tokens.push_back(tok);;
    return tok.token_type;
}

Token LexicalAnalyzer::GetToken()
{
    char c, c2;

    // if there are tokens that were previously
    // stored due to UngetToken(), pop a token and
    // return it without reading from input
    if (!tokens.empty()) {
        tmp = tokens.back();
        tokens.pop_back();
        return tmp;
    }

    SkipSpace();
    tmp.lexeme = "";
    tmp.line_no = line_no;
    input.GetChar(c);
	while (c == '/') { // to ignore comments
		input.GetChar(c2);
		if (c2 != '/') {
			input.UngetChar(c2);
			tmp.token_type = ERROR;
			return tmp;
		}
		else {
			while (c != '\n' && !input.EndOfInput()) input.GetChar(c);
			SkipSpace();
			input.GetChar(c);
		}
	}
    switch (c) {
        case '=':
            tmp.token_type = EQUAL;
            return tmp;
		case ':':
            tmp.token_type = COLON;
            return tmp;
        case ',':
            tmp.token_type = COMMA;
            return tmp;
		case ';':
            tmp.token_type = SEMICOLON;
            return tmp;
        case '{':
            tmp.token_type = LBRACE;
            return tmp;
        case '}':
            tmp.token_type = RBRACE;
            return tmp;
        default:
            if (isdigit(c)) {
                input.UngetChar(c);
				tmp.token_type = ERROR;
                return tmp;
            } else if (isalpha(c)) {
                input.UngetChar(c);
                return ScanIdOrKeyword();
            } else if (input.EndOfInput())
                tmp.token_type = END_OF_FILE;
            else
                tmp.token_type = ERROR;

            return tmp;
    }
}