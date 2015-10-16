#pragma once

#include "CGCom.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "wn\\wn.h"


class Cwordnet
{
public:



public:
	Cwordnet();
	~Cwordnet();

	void wn();
	bool Init_WordNet();

	bool IS_Prototype(const char* chword);
	bool Get_Prototype(const char* chword, char* coutput, size_t buf_size);
	int  do_is_defined(char *searchword);
	void printsearches(char *word, int dbase, unsigned long search);
	int searchwn(int ac, char *av[]);
	int do_search(char *searchword, int pos, int search, int whichsense, char *label);
	int getoptidx(char *searchtype);
	int cmdopt(char *str);
	void search_port(const char* cword, char cpos);
};