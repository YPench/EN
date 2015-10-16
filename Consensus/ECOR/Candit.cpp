/*
 * Copyright by Yanping Chen <ypench@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include "stdafx.h"
#include <algorithm>
#include "Candit.h"
#include "ConvertUTF.h"
#include "ECOR.h"


CCandit::CCandit()
{

}
CCandit::~CCandit()
{

}

bool CCandit::Loading_Candidating_Mention_Pair(const char* loadname, CoValueVector& pmCoValue_v, CoMentionPairVector& pmCoCandit_v)
{
	map<string, ACE_entity_mention*>& MentionSpace_m = m_pCECOR->MentionSpace_m;
	char SentenceBuf[MAX_SENTENCE];
	char ClassBuf[256];
	char FirstBuf[256];
	char SecondBuf[256];
	ifstream in(loadname);
	if(in.bad()){
		return false;
	}
	in.clear();
	in.seekg(0, ios::beg);

	while(true){
		if(in.peek() == EOF){
			break;
		}
		in.getline(SentenceBuf, MAX_SENTENCE, '\n');
		if(strlen(SentenceBuf) == 0){
			AppCall::Secretary_Message_Box("Data Error: CExtrue::Load_Stanford_Pased_TEXT_Information(3)", MB_OK);
		}
		istringstream sstream(SentenceBuf);
		double Exclu;

		sstream >> Exclu;
		sstream.getline(SentenceBuf, MAX_SENTENCE, ' ');
		sstream.getline(ClassBuf, 256, ' ');
		sstream.getline(FirstBuf, 256, ' ');
		sstream.getline(SecondBuf, 256, ' ');
		if(MentionSpace_m.find(FirstBuf) == MentionSpace_m.end() || MentionSpace_m.find(SecondBuf) == MentionSpace_m.end()){
			AppCall::Secretary_Message_Box("Data Error: CExtrue::Load_Stanford_Pased_TEXT_Information(4)", MB_OK);
			return false;
		}
		pmCoValue_v.push_back(make_pair(ClassBuf, Exclu));

		pmCoCandit_v.push_back(make_pair(MentionSpace_m[FirstBuf], MentionSpace_m[SecondBuf]));
	}
	in.close();

	return true;
}
void CCandit::Saving_Candidating_Mention_Pair(const char* savename, CoValueVector& pmCoValue_v, CoMentionPairVector& pmCoCandit_v)
{
	ofstream out(savename);
	if(out.bad())
		return;
	out.clear();
	out.seekp(0, ios::beg);

	if(pmCoValue_v.size() != pmCoCandit_v.size()){
		AppCall::Secretary_Message_Box("CCandit::Saving_Candidating_Mention_Pair()");
	}
	for(size_t i = 0; i < pmCoCandit_v.size(); i++){
		out << pmCoValue_v[i].second << " " << pmCoValue_v[i].first << " ";
		out << pmCoCandit_v[i].first->ID << " " << pmCoCandit_v[i].second->ID << " " << endl;
	}
	out.close();
}
void CCandit::Save_or_Load_Parting_Part(const char* cFolder, bool Save_or_Load_Flag, CoValueVector& pmCoValue_v, CoMentionPairVector& pmCoCandit_v)
{
	string CandidatingPairPath = cFolder;
	bool Candidates_Loaded_Flag = false;
	if(!strcmp(m_pCECOR->m_CorpusTYPE.c_str(), "ACE_E")){
		CandidatingPairPath += "ACE_E_Parting.txt";
	}
	else{
		CandidatingPairPath += "ACE_ALL_Parting.txt";
	}
	if(!Save_or_Load_Flag){
		if(NLPOP::Exist_of_This_File(CandidatingPairPath)){
			if(Loading_Candidating_Mention_Pair(CandidatingPairPath.c_str(), pmCoValue_v, pmCoCandit_v)){
				Candidates_Loaded_Flag = true;;
			}
		}
	}

	if(Save_or_Load_Flag){
		Saving_Candidating_Mention_Pair(CandidatingPairPath.c_str(), pmCoValue_v, pmCoCandit_v);
	}
}


void CCandit::Generate_Coreference_Candidate_Port(const char* cFolder, ACE_Corpus& pmACE_Corpus, vector<string>& pmCanditDoc_v, CoValueVector& pmCoValue_v, CoMentionPairVector& pmCoCandit_v)
{
	string CandidatingPairPath = cFolder;
	bool Candidates_Loaded_Flag = false;
	if(!strcmp(m_pCECOR->m_CorpusTYPE.c_str(), "ACE_E")){
		CandidatingPairPath += "ACE_E_Candidating.txt";
	}
	else{
		CandidatingPairPath += "ACE_ALL_Candidating.txt";
	}
	if(m_pCECOR->m_ALoadCandits_Flag){
		if(NLPOP::Exist_of_This_File(CandidatingPairPath)){
			if(Loading_Candidating_Mention_Pair(CandidatingPairPath.c_str(), pmCoValue_v, pmCoCandit_v)){
				Candidates_Loaded_Flag = true;;
			}
		}
	}
	if(!Candidates_Loaded_Flag){
		map<string, vector<ACE_entity_mention*>*> DocsMentons_v;
		ECCOM::Get_Docs_Entity_Mentions(pmACE_Corpus, pmCanditDoc_v, DocsMentons_v);
		for(map<string, vector<ACE_entity_mention*>*>::iterator mite = DocsMentons_v.begin(); mite != DocsMentons_v.end(); mite++){
			ECCOM::Document_Entity_Mention_Ording(*mite->second);
		}
		p_m_ACE_Corpus = &pmACE_Corpus;
		ECCOM::Get_Docs_Related_Mentions(pmACE_Corpus, m_DocsRelatedMentions_mms);

		//Collecting_All_Coreference_Candidates(DocsMentons_v, pmCoValue_v, pmCoCandit_v);
		Collecting_Coreference_Candidates_with_EXCLUSIVE_Detecting(DocsMentons_v, pmCoValue_v, pmCoCandit_v);

		for(map<string, vector<ACE_entity_mention*>*>::iterator mite = DocsMentons_v.begin(); mite != DocsMentons_v.end(); mite++){
			delete mite->second;
		}
	}
	
	if(m_pCECOR->m_UpdateCandits_Flag){
		Saving_Candidating_Mention_Pair(CandidatingPairPath.c_str(), pmCoValue_v, pmCoCandit_v);
	}
}

void CCandit::Collecting_Coreference_Candidates_with_EXCLUSIVE_Detecting(map<string, vector<ACE_entity_mention*>*>& DocsMentons_v, CoValueVector& pmCoValue_v, CoMentionPairVector& pmCoCandit_v)
{
	for(map<string, vector<ACE_entity_mention*>*>::iterator mvite = DocsMentons_v.begin(); mvite != DocsMentons_v.end(); mvite++){
		if(m_DocsRelatedMentions_mms.find(mvite->first) == m_DocsRelatedMentions_mms.end()){
			//ostringstream ostream;
			//ostream << "Missing Relation Documents: " << mvite->first << endl;
			//AppCall::Secretary_Message_Box(ostream.str().c_str());
			//continue;
			m_DocsRelatedMentions_mms[mvite->first];
		}
		size_t mention_size = mvite->second->size();
		p_m_RelatedMention_ms = &m_DocsRelatedMentions_mms[mvite->first];
		vector<ACE_entity_mention*>& loc_Mentions_v = *(mvite->second);
		for(size_t i = 0; i < mention_size; i++){
			ACE_entity_mention& S_Mention = *(loc_Mentions_v[i]);
			//int BacktrackCnt;
			//bool Pronoun_Flag = false;
			//if(!strcmp(S_Mention.TYPE.c_str(), "PRO")){
			//	Pronoun_Flag = true;
			//	BacktrackCnt = Pronoun_Backtracking_Number;
			//}
			for(int j = i-1; j >= 0; j--){
				ACE_entity_mention& F_Mention = *(loc_Mentions_v[j]);
				//if(!strcmp("AFP_ENG_20030311.0491-E11-15", F_Mention.ID.c_str()) || !strcmp("AFP_ENG_20030311.0491-E11-15", S_Mention.ID.c_str())){
				//	int breakpoint = 1;
				//}
				if(strcmp(F_Mention.Entity_SUBSTYPE.c_str(), S_Mention.Entity_SUBSTYPE.c_str())){
					continue;
				}
				//if(Pronoun_Flag){
				//	if(BacktrackCnt-- < 0){
				//		break;
				//	}
				//}
				if(Has_Relation(F_Mention, S_Mention)){
					pmCoCandit_v.push_back(make_pair(loc_Mentions_v[j], loc_Mentions_v[i]));
					pmCoValue_v.push_back(make_pair(EXCLUSIVE, -1));
				}
				else if(Has_Sexual_Distinction(F_Mention, S_Mention)){
					pmCoCandit_v.push_back(make_pair(loc_Mentions_v[j], loc_Mentions_v[i]));
					pmCoValue_v.push_back(make_pair(EXCLUSIVE, -1));
				}
				else if(Has_Numbered_Distinction(F_Mention, S_Mention)){
					pmCoCandit_v.push_back(make_pair(loc_Mentions_v[j], loc_Mentions_v[i]));
					pmCoValue_v.push_back(make_pair(EXCLUSIVE, -1));
				}
				else{
					pmCoCandit_v.push_back(make_pair(loc_Mentions_v[j], loc_Mentions_v[i]));
					pmCoValue_v.push_back(make_pair("", 0));
				}
			}
		}
	}
}

bool CCandit::Has_Relation(ACE_entity_mention& F_Mention, ACE_entity_mention& S_Mention)
{
	//:YPench
	//return false;
	if(p_m_RelatedMention_ms->find(F_Mention.ID) != p_m_RelatedMention_ms->end()){
		if((*p_m_RelatedMention_ms)[F_Mention.ID].find(S_Mention.ID) != (*p_m_RelatedMention_ms)[F_Mention.ID].end()){
			//if(!strcmp(F_Mention.Entity_ID.c_str(), S_Mention.Entity_ID.c_str())){
				//AppCall::Secretary_Message_Box("Has_Relation");
			//}
			return true;
		}
	}
	return false;
}
bool CCandit::Has_Sexual_Distinction(ACE_entity_mention& F_Mention, ACE_entity_mention& S_Mention)
{
	
	return false;
}

bool CCandit::Has_Numbered_Distinction(ACE_entity_mention& F_Mention, ACE_entity_mention& S_Mention)
{
	//:YPench
	return false;

	bool F_Prototype_Flag;
	bool S_Prototype_Flag;

	F_Prototype_Flag = m_pCECOR->m_Cwordnet.IS_Prototype(F_Mention.head.charseq.c_str());
	S_Prototype_Flag = m_pCECOR->m_Cwordnet.IS_Prototype(S_Mention.head.charseq.c_str());
	if(F_Prototype_Flag && S_Prototype_Flag){
		return false;
	}
	if(!F_Prototype_Flag && !S_Prototype_Flag){
		return false;
	}
	return true;
}

void CCandit::Collecting_All_Coreference_Candidates(map<string, vector<ACE_entity_mention*>*>& DocsMentons_v, CoValueVector& pmCoValue_v, CoMentionPairVector& pmCoCandit_v)
{
	for(map<string, vector<ACE_entity_mention*>*>::iterator mvite = DocsMentons_v.begin(); mvite != DocsMentons_v.end(); mvite++){
		size_t mention_size = mvite->second->size();
		for(size_t i = 0; i < mention_size; i++){
			for(int j = i-1; j > 0; j--){
				if((*mvite->second)[j]->Entity_SUBSTYPE == (*mvite->second)[i]->Entity_SUBSTYPE){
					pmCoCandit_v.push_back(make_pair((*mvite->second)[j], (*mvite->second)[i]));
					pmCoValue_v.push_back(make_pair("", 0));
				}
			}
		}
	}
}
