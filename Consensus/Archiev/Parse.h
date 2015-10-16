#pragma once
#include "CGCom.h"
#include "ace.h"
#include "CSegmter.h"
#include "Charniak\\InputTree.h"
#include "Charniak\\Feature.h"



class CParse
{
public:
	CParse(CSegmter& ref_Segmter);
	~CParse();

	void Generate_Parsing_Sentence(const char* parspath, vector<Relation_Case>& Relation_Case_v);
	void Generate_Segmented_Relation_Mention_Case(const char* c_relaitoncasepath);
	void Output_For_Charniak_PARSE(const char* csavepath, map<size_t,vector<string>>& SegRelationstr_map);
	
	void Read_Parsed_Tree(const char* c_treepath);

public:
	CSegmter& m_CSegmter;

	int numTerm[MAXNUMNTS];
	vector<InputTree*> pParse_v; 

};
