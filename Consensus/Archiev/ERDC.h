#pragma once
#include "..\\Include\\CGCom.h"
#include "..\\Include\\ace.h"
#include "maxen.h"
#include "CSegmter.h"
class CERDC
{
public:

	CSegmter m_CSegmter;

	//multimap<string, vector<pair<string, unsigned long>>> Training_map;
	size_t SENID;
	BOOL TYPE_Flag;
	BOOL SUBTYPE_Flag;
	BOOL POS_Flag;
	BOOL CaseCheck_Flag;
	BOOL Enitity_Subtype_Flag;


public:
	CERDC();
	~CERDC();

	void For_English_Segmentation();

	void Delete_Sentence_Contianing_Positive_Relation_Case(list<Relation_Case>& Relation_Case_l, deque<ACE_relation>& ACE_Relation_Info_d);
	void Extract_Sentence_with_Two_Named_Entities(ACE_Corpus& ACE_Corpus, list<Relation_Case>& Relation_Case_l);
	void Generate_English_Training_Cases(const char* infopath, vector<Relation_Case>& Relation_Case_v, vector<pair<me_context_type, me_outcome_type>>& Maxentraining_v);
	void Generate_Positive_And_Negetive_ACE_Relation_Cases(ACE_Corpus& m_ACE_Corpus, vector<Relation_Case>& Relation_Case_v);
	void Generate_Negative_Training_Case_Using_Antity(ACE_Corpus& m_ACE_Corpus, list<Relation_Case>& Relation_Case_l);
	void Adding_Positive_Training_Case_in_Relation_Mention(ACE_Corpus& m_ACE_Corpus, list<Relation_Case>& Relation_Case_l);

	void Delet_0AH_in_string(RelationContext& pm_context);
	void Delet_0AH_in_string(string &pmstr);
	void Read_English_Relation_Case_POS(const char* cinputpath, map<string, vector<string>>& RelationPOS_m);
};
