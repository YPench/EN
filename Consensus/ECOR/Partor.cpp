/*
 * Copyright (C) 2015 by Yanping Chen <ypench@gmail.com>
 * Begin       : 01-Oct-2015
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser GPL (LGPL) as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program.
 */

#include "stdafx.h"
#include <algorithm>
#include "Partor.h"
#include "ConvertUTF.h"
#include "ECOR.h"

extern bool CECOR_Insert_Error_Check_Flag;
CPartor::CPartor()
{

}
CPartor::~CPartor()
{

}


void CPartor::Init_Doc_CanditPair_Coreference_Value(corvaluepairite& covaluepair, corcanditpairite& cocanditpair)
{
	vector<pair<string, double>>::iterator valueite = covaluepair.first;
	CoMPairItor pairite = cocanditpair.first;
	strcpy_s(cDocName, 64, pairite->first->DOCID.c_str());
	for( ; ((valueite != covaluepair.second) && (pairite != cocanditpair.second)); valueite++, pairite++){
		pair<string, double>& ValuePair = *valueite;
		pair<ACE_entity_mention*, ACE_entity_mention*>& CanditPair = *pairite;
		if(D_CoValueCandit_mmm.find(CanditPair.first->ID) == D_CoValueCandit_mmm.end()){
			D_CoValueCandit_mmm[CanditPair.first->ID];
			D_CoCanditValue_mm[CanditPair.first->ID];
		}
		D_CoValueCandit_mmm[CanditPair.first->ID].insert(make_pair(ValuePair.second, CanditPair.second->ID));
		D_CoCanditValue_mm[CanditPair.first->ID].insert(make_pair(CanditPair.second->ID, ValuePair.second));

		if(D_CoValueCandit_mmm.find(CanditPair.second->ID) == D_CoValueCandit_mmm.end()){
			D_CoValueCandit_mmm[CanditPair.second->ID];
			D_CoCanditValue_mm[CanditPair.second->ID];
		}
		D_CoValueCandit_mmm[CanditPair.second->ID].insert(make_pair(ValuePair.second, CanditPair.first->ID));
		D_CoCanditValue_mm[CanditPair.second->ID].insert(make_pair(CanditPair.first->ID, ValuePair.second));
	}
}

double CPartor::Get_Two_Mention_Coreference_Evaluation(const char* cF_MentionID, const char* cS_MentionID)
{
	//map<string, map<string, double>> D_CoCanditValue_mm;
	if(D_CoCanditValue_mm.find(cF_MentionID) == D_CoCanditValue_mm.end() || D_CoCanditValue_mm.find(cS_MentionID) == D_CoCanditValue_mm.end()){
		AppCall::Secretary_Message_Box("Data Error in: Get_Two_Mention_Coreference_Evaluation()");
		return 0;
	}
	map<string, double>& F_MentionValues_m = D_CoCanditValue_mm[cF_MentionID];
	map<string, double>& S_MentionValues_m = D_CoCanditValue_mm[cS_MentionID];
	double F_S_Value = (F_MentionValues_m.find(cS_MentionID) != F_MentionValues_m.end())?F_MentionValues_m[cS_MentionID]:0;
	double S_F_Value = (S_MentionValues_m.find(cF_MentionID) != S_MentionValues_m.end())?S_MentionValues_m[cF_MentionID]:0;
	if((F_S_Value != S_F_Value) && (F_S_Value != 0) && (S_F_Value  != 0)){
		AppCall::Secretary_Message_Box("Data Error in: Get_Two_Mention_Coreference_Evaluation()");
		return 0;
	}
	if(F_S_Value != 0){
		return F_S_Value;
	}
	else if(S_F_Value != 0){
		return S_F_Value;
	}
	else{
		return 0;
	}
}


double CPartor::Get_Former_Coreference_Set_Evaluation(set<string>& pmCoSet_s, string MentionID)
{
	if(pmCoSet_s.find(MentionID) == pmCoSet_s.end()){
		return 0;
	}
	if(D_CoValueCandit_mmm.find(MentionID) == D_CoValueCandit_mmm.end()){
		return 0;
	}
	multimap<double, string>& locVale_mm = D_CoValueCandit_mmm[MentionID];
	for(multimap<double, string>::reverse_iterator rmmite = locVale_mm.rbegin(); rmmite != locVale_mm.rend(); rmmite++){
		if(pmCoSet_s.find(rmmite->second) != pmCoSet_s.end()){
			return rmmite->first;
		}
	}
	return 0;
}

void CPartor::Coreference_Partition_Port(CoMentionPairVector& CoCandit_v, 
			vector<pair<string, double>>& Covalue_v, map<string, map<string, set<ACE_entity_mention*>*>>& CoferenceSet_mm)
{
	//---------------Init Data
	if(CoCandit_v.size() != Covalue_v.size()){
		AppCall::Secretary_Message_Box("Data Error in: CPartor::Coreference_Partition_Port()");
		return;
	}
	map<string, ACE_entity_mention*>& RefMentionSpace_m = m_pCECOR->MentionSpace_m;
	for(map<string, ACE_entity_mention*>::iterator mite = RefMentionSpace_m.begin(); mite != RefMentionSpace_m.end(); mite++){
		string DOCID = mite->second->DOCID;
		if(DocMentionsSet_ms.find(DOCID) == DocMentionsSet_ms.end()){
			DocMentionsSet_ms[DOCID];
		}
		DocMentionsSet_ms[DOCID].insert(mite->second->ID);

		if(m_OrderdMention_mm.find(DOCID) == m_OrderdMention_mm.end()){
			m_OrderdMention_mm[DOCID];
		}
		ACE_extent& loc_extent = mite->second->extent;
		map<size_t, map<size_t, ACE_entity_mention*>>& loc_Mention_mm = m_OrderdMention_mm[DOCID];
		if(loc_Mention_mm.find(loc_extent.START) == loc_Mention_mm.end()){
			loc_Mention_mm[loc_extent.START];
		}
		if(loc_Mention_mm[loc_extent.START].find(loc_extent.END) == loc_Mention_mm[loc_extent.START].end()){
			loc_Mention_mm[loc_extent.START].insert(make_pair(loc_extent.END, mite->second));
		}
	}
	p_m_CoferenceSet_mm = &CoferenceSet_mm;

	//--------Processing Doc by Doc
	corvaluepairite covaluepair;
	corcanditpairite cocanditpair;
	char cPreDOCID[256];
	char cCurDocID[256];
	vector<pair<string, double>>::iterator valutite = Covalue_v.begin();
	CoMPairItor canditite = CoCandit_v.begin();

	covaluepair.first = valutite;
	cocanditpair.first = canditite;
	strcpy_s(cPreDOCID, 256, canditite->second->DOCID.c_str());
	for(size_t i = 0; i < CoCandit_v.size(); i++, valutite++, canditite++){
		strcpy_s(cCurDocID, 256, CoCandit_v[i].first->DOCID.c_str());
		if(strcmp(cCurDocID, CoCandit_v[i].second->DOCID.c_str())){
			AppCall::Secretary_Message_Box("Data Error in: CPartor::Coreference_Partition_Port()");
			return;
		}
		
		if(strcmp(cPreDOCID, cCurDocID)){
			covaluepair.second = valutite;
			cocanditpair.second = canditite;
			Init_Doc_CanditPair_Coreference_Value(covaluepair, cocanditpair);
			Document_Coreference_Partition(cPreDOCID, covaluepair, cocanditpair);
			covaluepair.first = valutite;
			cocanditpair.first = canditite;
		}
		strcpy_s(cPreDOCID, 256, cCurDocID);
	}
	covaluepair.second = Covalue_v.end();
	cocanditpair.second = CoCandit_v.end();
	Init_Doc_CanditPair_Coreference_Value(covaluepair, cocanditpair);
	Document_Coreference_Partition(cPreDOCID, covaluepair, cocanditpair);
}

void CPartor::Document_Coreference_Partition(const char* DocChar, corvaluepairite& covaluepair, corcanditpairite& cocanditpair)
{
	if(p_m_CoferenceSet_mm->find(DocChar) != p_m_CoferenceSet_mm->end()){
		AppCall::Secretary_Message_Box("Data Error in: CPartor::Document_Coreference_Partition(1)");
	}
	if(DocMentionsSet_ms.find(DocChar)->second.empty()){
		return;
	}
	//-----------------------Init Data
	map<string, ACE_entity_mention*>& RefMentionSpace_m = m_pCECOR->MentionSpace_m;
	p_m_DocCoferenceSet_ms = &(*p_m_CoferenceSet_mm)[DocChar];
	p_m_OrdDocMention_mm = &m_OrderdMention_mm[DocChar];
	p_m_DocCanditSpace_s = &DocMentionsSet_ms[DocChar];
	//if(p_m_DocCanditSpace_s->size() != D_CoCanditValue_mm.size()){
	//	AppCall::Secretary_Message_Box("Data Error in: CPartor::Init_Doc_CanditPair_Coreference_Value()");
	//}
	//-----------------------Do Partition
	m_EntityID = 0;
	CoPairValueItor valueite = covaluepair.first;
	CoMPairItor canditite = cocanditpair.first;

	while((valueite != covaluepair.second) && (canditite != cocanditpair.second)){
		//if(!strcmp("AFP_ENG_20030304.0250-E21-13", canditite->first->ID.c_str()) || !strcmp("AFP_ENG_20030304.0250-E21-13", canditite->second->ID.c_str())){
		//	int breakpoint = 1;
		//}
		if(D_One2PreMany_mm.find(canditite->first->ID) == D_One2PreMany_mm.end()){
			D_One2PreMany_mm[canditite->first->ID];
		}
		if(D_One2PreMany_mm.find(canditite->second->ID) == D_One2PreMany_mm.end()){
			D_One2PreMany_mm[canditite->second->ID];
		}
		
		if(D_One2ProMany_mm.find(canditite->first->ID) == D_One2ProMany_mm.end()){
			D_One2ProMany_mm[canditite->first->ID];
		}
		if(D_One2ProMany_mm.find(canditite->second->ID) == D_One2ProMany_mm.end()){
			D_One2ProMany_mm[canditite->second->ID];
		}

		map<string, string>& locOne2PreMany_m = D_One2PreMany_mm[canditite->second->ID];
		map<string, string>& locOne2ProMany_m = D_One2ProMany_mm[canditite->first->ID];
		if(locOne2PreMany_m.find(canditite->first->ID) == locOne2PreMany_m.end()){
			locOne2PreMany_m.insert(make_pair(canditite->first->ID, valueite->first));
		}
		else{
			AppCall::Secretary_Message_Box("Data Error in: CPartor::Document_Coreference_Partition(1-2)");
		}
		if(locOne2ProMany_m.find(canditite->second->ID) == locOne2ProMany_m.end()){
			locOne2ProMany_m.insert(make_pair(canditite->second->ID, valueite->first));
		}
		else{
			AppCall::Secretary_Message_Box("Data Error in: CPartor::Document_Coreference_Partition(1-2)");
		}
		canditite++;
		valueite++;
	}
	valueite = covaluepair.first;
	canditite = cocanditpair.first;
	while((valueite != covaluepair.second) && (canditite != cocanditpair.second)){
		ACE_entity_mention& F_Mention = *canditite->first;
		ACE_entity_mention& S_Mention = *canditite->second;
		const char* cF_MentionID = F_Mention.ID.c_str();
		const char* cS_MentionID = S_Mention.ID.c_str();

		if(!strcmp("AFP_ENG_20030319.0879-E46-88", cF_MentionID) || !strcmp("AFP_ENG_20030319.0879-E46-88", cS_MentionID)){
			int breakpoint = 1;
		}
		if(m_ResolvedMention_m.find(cF_MentionID) == m_ResolvedMention_m.end()){
			Adding_Mention_as_non_Former_Coreference(F_Mention);
		}
		pair<string, double>& CoValue = *valueite;
		if(!strcmp(CoValue.first.c_str(), POSITIVE)){
			//Insert positive
			Inserting_Posivie_Coreference(valueite, canditite);
		}
		if(!strcmp(CoValue.first.c_str(), NEGETIVE)){
			//Insert negetive
			Inserting_Negetive_Coreference(valueite, canditite);
		}	
		if(!strcmp(CoValue.first.c_str(), EXCLUSIVE)){
			//Insert Exclusive
			Inserting_Exclusive_Coreference(valueite, canditite);
		}
		canditite++;
		valueite++;
	}
	if(!p_m_DocCanditSpace_s->empty()){
		set<string>::iterator site = p_m_DocCanditSpace_s->begin();
		while(site != p_m_DocCanditSpace_s->end()){
			if(RefMentionSpace_m.find(site->data()) == RefMentionSpace_m.end()){
				AppCall::Secretary_Message_Box("Data Error in: CPartor::Document_Coreference_Partition(2)");
			}
			ACE_entity_mention& loc_Mention = *(RefMentionSpace_m[site->data()]);
			Adding_Mention_as_non_Former_Coreference(loc_Mention);
			site = p_m_DocCanditSpace_s->begin();
		}
	}
//-----------Collecting----------------------
	map<string, ACE_entity_mention*>& MentionSpace_m = m_pCECOR->MentionSpace_m;
	for(map<string, set<string>>::iterator msite = m_ClusteredMention_ms.begin(); msite != m_ClusteredMention_ms.end(); msite++){
		if(p_m_DocCoferenceSet_ms->find(msite->first) == p_m_DocCoferenceSet_ms->end()){
			p_m_DocCoferenceSet_ms->insert(make_pair(msite->first, new set<ACE_entity_mention*>));
		}
		else{
			AppCall::Secretary_Message_Box("Data Error In: CPartor::Document_Coreference_Partition(3)");
		}
		set<ACE_entity_mention*>& locMentions_s = *((*p_m_DocCoferenceSet_ms)[msite->first]);
		for(set<string>::iterator site = msite->second.begin(); site != msite->second.end(); site++){
			if(MentionSpace_m.find(site->data()) == MentionSpace_m.end()){
				AppCall::Secretary_Message_Box("Data Error In: CPartor::Document_Coreference_Partition(4)");
			}
			locMentions_s.insert(MentionSpace_m[site->data()]);
		}
	}
//-----------------------Reset Data
	p_m_DocCanditSpace_s->clear();
	m_ClusteredMention_ms.clear();
	m_ResolvedMention_m.clear();
	D_CoValueCandit_mmm.clear();
	D_CoCanditValue_mm.clear();
	D_One2PreMany_mm.clear();
	D_One2ProMany_mm.clear();
}

void CPartor::Delete_Mention(const char* pmcMentionID)
{
	if(!strcmp("AFP_ENG_20030319.0879-E46-88", pmcMentionID)){
		int viewpoint = 0;
	}
	//m_ClusteredMention_ms
	for(map<string, set<string>>::iterator msite = m_ClusteredMention_ms.begin(); msite != m_ClusteredMention_ms.end(); msite++){
		if(msite->second.find(pmcMentionID) != msite->second.end()){
			msite->second.erase(pmcMentionID);
			if(msite->second.empty()){
				msite = m_ClusteredMention_ms.erase(msite);
			}
		}
	}
	if(m_ResolvedMention_m.find(pmcMentionID) != m_ResolvedMention_m.end()){
		m_ResolvedMention_m.erase(pmcMentionID);
	}
	p_m_DocCanditSpace_s->insert(pmcMentionID);
}
void CPartor::Insert_Mention(const char* Pri_MentionID, const char* CanditMentionID)
{
	if(m_ResolvedMention_m.find(Pri_MentionID) == m_ResolvedMention_m.end()){
		AppCall::Secretary_Message_Box("Data Error in: Insert_Mention()");
		return;
	}
	Insert_Mention(m_ResolvedMention_m[Pri_MentionID].c_str(), CanditMentionID, false);

}
void CPartor::Insert_Mention(const char* pmEntityName, const char* cMentionID, bool A_New_Nntity_Flag)
{
	if(A_New_Nntity_Flag){
		char intchar[64];
		char cEntityName[64];
		_itoa_s(m_EntityID++,intchar,64,10);
		strcpy_s(cEntityName, 64, cDocName);
		strcat_s(cEntityName, 64, "-E");
		strcat_s(cEntityName, 64, intchar);
		Insert_Mention(cEntityName, cMentionID, false);
		return;
	}
	if(!strcmp("AFP_ENG_20030319.0879-E46-88", cMentionID)){
		int viewpoint = 0;
	}
	if(CECOR_Insert_Error_Check_Flag){
		if(m_ResolvedMention_m.find(cMentionID) !=  m_ResolvedMention_m.end()){
			//Delete_Mention(cMentionID);
			string sCoEntity = m_ResolvedMention_m[cMentionID];
			if(strcmp(pmEntityName, sCoEntity.c_str())){
				AppCall::Secretary_Message_Box("Already In: m_ResolvedMention_m 1");
			}
			else{
				AppCall::Secretary_Message_Box("Already In: m_ClusteredMention_ms 2");
			}
		}
		m_ResolvedMention_m.insert(make_pair(cMentionID, pmEntityName));
		for(map<string, set<string>>::iterator msite = m_ClusteredMention_ms.begin(); msite != m_ClusteredMention_ms.end(); msite++){
			if(msite->second.find(cMentionID) != msite->second.end()){
				AppCall::Secretary_Message_Box("Already In: m_ClusteredMention_ms 3");
			}
		}
		if(Coreference_Partition_Error_Check(pmEntityName, cMentionID)){
			//AppCall::Secretary_Message_Box("CPartor::Coreference_Partition_Error_Check()");
		}
	}
	if(m_ClusteredMention_ms.find(pmEntityName) == m_ClusteredMention_ms.end()){
		m_ClusteredMention_ms[pmEntityName];
	}
	m_ClusteredMention_ms[pmEntityName].insert(cMentionID);
	if(p_m_DocCanditSpace_s->find(cMentionID) != p_m_DocCanditSpace_s->end()){
		p_m_DocCanditSpace_s->erase(cMentionID);
	}
}
bool CPartor::Coreference_Partition_Error_Check(const char* cEntityName, const char* cMentionID)
{
	string mentionstr = cMentionID;
	if(m_ClusteredMention_ms.find(cEntityName) == m_ClusteredMention_ms.end()){
		for(map<string, set<string>>::iterator msite = m_ClusteredMention_ms.begin(); msite != m_ClusteredMention_ms.end(); msite++){
			for(set<string>::iterator site = msite->second.begin(); site != msite->second.end(); site++){
				if(!strcmp((*site).substr(0, (*site).rfind('-')).c_str(), mentionstr.substr(0, mentionstr.rfind('-')).c_str())){
					//AppCall::Secretary_Message_Box("CPartor::Coreference_Partition_Error_Check()");
					return true;
				}
			}
		}
	}
	else{
		m_ClusteredMention_ms[cEntityName];
		for(set<string>::iterator site = m_ClusteredMention_ms[cEntityName].begin(); site != m_ClusteredMention_ms[cEntityName].end(); site++){
			if(strcmp((*site).substr(0, (*site).rfind('-')).c_str(), mentionstr.substr(0, mentionstr.rfind('-')).c_str())){
				//AppCall::Secretary_Message_Box("CPartor::Coreference_Partition_Error_Check()");
				return true;
			}
		}
	}

	return false;
}

void CPartor::Inserting_Exclusive_Coreference(const CoPairValueItor& valueite, const CoMPairItor& canditite)
{
	ACE_entity_mention& F_Mention = *canditite->first;
	ACE_entity_mention& S_Mention = *canditite->second;
	const char* cF_MentionID = F_Mention.ID.c_str();
	const char* cS_MentionID = S_Mention.ID.c_str();
	ACE_entity_mention* pAdding_Mention = canditite->second;

	if(m_ResolvedMention_m.find(cS_MentionID) != m_ResolvedMention_m.end()){
		if(m_ResolvedMention_m.find(cF_MentionID) == m_ResolvedMention_m.end()){
			Greedy_Matched_to_the_Former_without_Conflict(F_Mention);
		}
		string sF_CoEntity = m_ResolvedMention_m[cF_MentionID];
		string sS_CoEntity = m_ResolvedMention_m[cS_MentionID];
		if(!strcmp(sF_CoEntity.c_str(), sS_CoEntity.c_str())){//
			//tow exclusive coreference loacted in a single set
			set<string> comCanditSet_s;
			for(set<string>::iterator site = m_ClusteredMention_ms[sF_CoEntity].begin(); site != m_ClusteredMention_ms[sF_CoEntity].end(); site++){
				comCanditSet_s.insert(site->data());
			}
			for(set<string>::iterator site = comCanditSet_s.begin(); site != comCanditSet_s.end(); site++){
				Delete_Mention(site->data());
			}
			comCanditSet_s.erase(cF_MentionID);
			comCanditSet_s.erase(cS_MentionID);
			Insert_Mention(NULL, cF_MentionID, true);
			Insert_Mention(NULL, cS_MentionID, true);
			for(set<string>::iterator site = comCanditSet_s.begin(); site != comCanditSet_s.end(); site++){
				if(Get_Two_Mention_Coreference_Evaluation(cF_MentionID, site->data()) > Get_Two_Mention_Coreference_Evaluation(cS_MentionID, site->data())){
					Insert_Mention(cF_MentionID, site->data());
				}
				else{
					Insert_Mention(cS_MentionID, site->data());
				}
			}
			return;
		}
		if(m_ClusteredMention_ms.find(sF_CoEntity) == m_ClusteredMention_ms.end()){
			AppCall::Secretary_Message_Box("Data Error in: CPartor::Inserting_Posivie_Coreference()");
			return;
		}
		set<string>& F_CoCanditSet_s = m_ClusteredMention_ms[sF_CoEntity];
		double F_MaxCoValue = Get_Former_Coreference_Set_Evaluation(F_CoCanditSet_s, cF_MentionID);
		double S_MaxCoValue = Get_Former_Coreference_Set_Evaluation(F_CoCanditSet_s, cS_MentionID);
	    
		if(F_MaxCoValue > S_MaxCoValue){
			Delete_Mention(cS_MentionID);
			pAdding_Mention = canditite->second;
		}
		else{
			Delete_Mention(cS_MentionID);
			pAdding_Mention = canditite->first;
		}
	}

	Greedy_Matched_to_the_Former_without_Conflict(*pAdding_Mention);

}


void CPartor::Inserting_Negetive_Coreference(const CoPairValueItor& valueite, const CoMPairItor& canditite)
{
	ACE_entity_mention& F_Mention = *canditite->first;
	ACE_entity_mention& S_Mention = *canditite->second;
	const char* cF_MentionID = F_Mention.ID.c_str();
	const char* cS_MentionID = S_Mention.ID.c_str();

	if(m_ResolvedMention_m.find(cS_MentionID) != m_ResolvedMention_m.end()){
		return;
	}
	if(D_One2PreMany_mm.find(cS_MentionID) == D_One2PreMany_mm.end()){
		AppCall::Secretary_Message_Box("Data Error in: CPartor::Inserting_Negetive_Coreference()");
	}
	for(map<string, string>::iterator mite = D_One2PreMany_mm[cS_MentionID].begin(); mite != D_One2PreMany_mm[cS_MentionID].end(); mite++){
		if(!strcmp(mite->second.c_str(), POSITIVE)){
			return;
		}
	}

	Greedy_Matched_to_the_Former_without_Conflict(S_Mention);

}


void CPartor::Inserting_Posivie_Coreference(const CoPairValueItor& valueite, const CoMPairItor& canditite)
{
	ACE_entity_mention& F_Mention = *canditite->first;
	ACE_entity_mention& S_Mention = *canditite->second;
	const char* cF_MentionID = F_Mention.ID.c_str();
	const char* cS_MentionID = S_Mention.ID.c_str();
	if(m_ResolvedMention_m.find(cF_MentionID) == m_ResolvedMention_m.end()){
		AppCall::Secretary_Message_Box("Data Error in: CPartor::Inserting_Posivie_Coreference()");
		return;
	}
	string sF_CoEntity = m_ResolvedMention_m[cF_MentionID];
	if(m_ClusteredMention_ms.find(sF_CoEntity) == m_ClusteredMention_ms.end()){
		AppCall::Secretary_Message_Box("Data Error in: CPartor::Inserting_Posivie_Coreference()");
		return;
	}
	set<string>& F_CoCanditSet_s = m_ClusteredMention_ms[sF_CoEntity];

	//----------------------------------
	if(m_ResolvedMention_m.find(cS_MentionID) != m_ResolvedMention_m.end()){//Second Mention is existed
		set<string> Confict_s;
		if(IS_Conflicting(F_CoCanditSet_s, cS_MentionID, Confict_s)){
			return;
		}
		string sS_CoEntity = m_ResolvedMention_m[cS_MentionID];
		if(m_ClusteredMention_ms.find(sS_CoEntity) == m_ClusteredMention_ms.end()){
			AppCall::Secretary_Message_Box("Data Error in: CPartor::Inserting_Posivie_Coreference()");
			return;
		}
		set<string>& S_CoCanditSet_s = m_ClusteredMention_ms[sS_CoEntity];
		
		double S_MaxCoValue = Get_Former_Coreference_Set_Evaluation(S_CoCanditSet_s, cS_MentionID);
		if(S_MaxCoValue >= valueite->second){
			return;
		}
		Delete_Mention(cS_MentionID);
		Insert_Mention(sS_CoEntity.c_str(), cS_MentionID, true);
	}
	else{
		set<string> Confict_s;
		if(IS_Conflicting(F_CoCanditSet_s, cS_MentionID, Confict_s)){
			//AppCall::Secretary_Message_Box("Need to be modified in: CPartor::Inserting_Posivie_Coreference()");
			double F_S_Value = Get_Two_Mention_Coreference_Evaluation(cF_MentionID, cS_MentionID);
			for(set<string>::iterator site = Confict_s.begin(); site != Confict_s.end(); site++){
				if(Get_Two_Mention_Coreference_Evaluation(site->data(), cF_MentionID) < F_S_Value){
					Delete_Mention(site->data());
				}
			}
		}
		//Adding_Mention_as_non_Former_Coreference(S_Mention);
		Insert_Mention(sF_CoEntity.c_str(), cS_MentionID, false);	

	}
}
void CPartor::Adding_Mention_as_non_Former_Coreference(ACE_entity_mention& pmMention)
{
	const char* MentionID = pmMention.ID.c_str();
	set<string> Positive_s;
	if(D_One2PreMany_mm.find(MentionID) == D_One2PreMany_mm.end() || D_One2ProMany_mm.find(MentionID) == D_One2ProMany_mm.end()){
		AppCall::Secretary_Message_Box("Data Error in: CPartor::Adding_Mention_as_non_Former_Coreference()");
	}
	for(map<string, string>::iterator mite = D_One2PreMany_mm[MentionID].begin(); mite != D_One2PreMany_mm[MentionID].end(); mite++){
		if(!strcmp(mite->second.c_str(), POSITIVE)){
			Positive_s.insert(mite->first);
		}
	}
	for(map<string, string>::iterator mite = D_One2ProMany_mm[MentionID].begin(); mite != D_One2ProMany_mm[MentionID].end(); mite++){
		if(!strcmp(mite->second.c_str(), POSITIVE)){
			Positive_s.insert(mite->first);
		}
	}
	for(set<string>::iterator site = Positive_s.begin(); site != Positive_s.end(); ){
		if(m_ResolvedMention_m.find(site->data()) == m_ResolvedMention_m.end()){
			continue;
		}
		if(IS_Conflicting(site->data(), MentionID)){
			site = Positive_s.erase(site);
		}
		else{
			site++;
		}
	}

	if(Positive_s.empty()){
		Greedy_Matched_to_the_Former_without_Conflict(pmMention);
		return;
	}
	double MaxValue = 0;
	string MaxMatchingStr;
	for(set<string>::iterator site = Positive_s.begin(); site != Positive_s.end(); site++){
		double value = Get_Two_Mention_Coreference_Evaluation(site->data(), MentionID);
		if(value > MaxValue){
			MaxValue = value;
			MaxMatchingStr = site->data();
		}
	}

	if(MaxValue == 0){
		Greedy_Matched_to_the_Former_without_Conflict(pmMention);
		return;
	}

	Insert_Mention(MaxMatchingStr.c_str(), MentionID);
}

void CPartor::Greedy_Matched_to_the_Former_without_Conflict(ACE_entity_mention& pmMention)
{
	ACE_extent& loc_extent = pmMention.extent;
	const char* cMentionID = pmMention.ID.c_str();
	map<size_t, map<size_t, ACE_entity_mention*>>& OrdDocMention = *p_m_OrdDocMention_mm;

	//Existed positive coreference in pre pair;
	map<string, string>& locone2PreMany_m = D_One2PreMany_mm[cMentionID];
	for(map<string, string>::iterator mite = locone2PreMany_m.begin(); mite != locone2PreMany_m.end(); mite++){
		if(!strcmp(mite->second.c_str(), POSITIVE)){
			string sCoEntity;
			if(m_ResolvedMention_m.find(mite->first) != m_ResolvedMention_m.end()){
				sCoEntity = m_ResolvedMention_m[mite->first];
			}
			else{
				AppCall::Secretary_Message_Box("Data Error in: CPartor::Greedy_Matched_to_the_Former_without...()");
			}
			Insert_Mention(sCoEntity.c_str(), cMentionID, false);
			return;
		}
	}
	//Existed positive coreference in pro pair;
	map<string, string>& locone2ProMany_m = D_One2ProMany_mm[cMentionID];
	for(map<string, string>::iterator mite = locone2ProMany_m.begin(); mite != locone2ProMany_m.end(); mite++){
		if(!strcmp(mite->second.c_str(), POSITIVE)){
			Insert_Mention(NULL, pmMention.ID.c_str(), true);
			return;
		}
	}
	//:YPench, Need to be modified...

	Insert_Mention(NULL, pmMention.ID.c_str(), true);
	return;

	if(strcmp(pmMention.TYPE.c_str(), "PRO")){
		Insert_Mention(NULL, pmMention.ID.c_str(), true);
		return;
	}

	map<size_t, map<size_t, ACE_entity_mention*>>::reverse_iterator rmmite = OrdDocMention.rbegin();
	for( ; rmmite != OrdDocMention.rend(); rmmite++){
		if(rmmite->first == loc_extent.START){
			break;
		}
	}
	if(rmmite == OrdDocMention.rend()){
		AppCall::Secretary_Message_Box("Data Error in: CPartor::Greedy_Matched_to_the_Former_without(1)");
		return;
	}
	map<size_t, ACE_entity_mention*>::reverse_iterator rmite = rmmite->second.rbegin();
	for( ; rmite != rmmite->second.rend(); rmite++){
		if(rmite->first == loc_extent.END){
			break;
		}
	}
	
	rmite++; //------point to pre-mention

	bool Detected_Coreference_Flag = false;
	string sCoEntity;
	while(true){
		while(rmite != rmmite->second.rend()){
			ACE_entity_mention& coMention = *rmite->second;
			const char* coMentionID = coMention.ID.c_str();
			if(!strcmp(coMention.Entity_SUBSTYPE.c_str(), pmMention.Entity_SUBSTYPE.c_str())){
				if(m_ResolvedMention_m.find(coMentionID) == m_ResolvedMention_m.end()){
					Greedy_Matched_to_the_Former_without_Conflict(coMention);
				}
				if(m_ResolvedMention_m.find(coMentionID) == m_ResolvedMention_m.end()){
					AppCall::Secretary_Message_Box("Data Error in: CPartor::Greedy_Matched_to_the_Former_without(2");
				}
				sCoEntity = m_ResolvedMention_m[coMentionID];
				if(m_ClusteredMention_ms.find(sCoEntity) == m_ClusteredMention_ms.end()){
					AppCall::Secretary_Message_Box("Data Error in: CPartor::Greedy_Matched_to_the_Former_without(3)");
					rmite++;
					continue;
				}
				set<string> Confict_s;
				if(!IS_Conflicting(m_ClusteredMention_ms[sCoEntity], cMentionID, Confict_s)){
					Detected_Coreference_Flag = true;
					break;
				}
			}
			rmite++;
		}
		if(Detected_Coreference_Flag){
			break;
		}
		rmmite++;
		if(rmmite == OrdDocMention.rend()){
			break;
		}
		rmite = rmmite->second.rbegin();
	}
	if(Detected_Coreference_Flag){
		Insert_Mention(sCoEntity.c_str(), cMentionID, false);
	}
	else{
		Insert_Mention(NULL, cMentionID, true);
	}
}


bool CPartor::IS_Conflicting(set<string>& pmCoSet_s, string MentionID, set<string>& pmConfict_s)
{
	if(D_CoCanditValue_mm.find(MentionID) == D_CoCanditValue_mm.end()){
		AppCall::Secretary_Message_Box("Data Error in: CPartor::IS_Conflicting()");
	}
	if(D_CoCanditValue_mm[MentionID].empty()){
		return false;
	}
	map<string, double>& CoMention_m = D_CoCanditValue_mm[MentionID];
	for(set<string>::iterator site = pmCoSet_s.begin(); site != pmCoSet_s.end(); site++){
		if(CoMention_m.find(site->data()) != CoMention_m.end()){
			if(CoMention_m[site->data()] == -1){
				//return true;
				pmConfict_s.insert(site->data());
			}
		}
	}
	return pmConfict_s.empty()?false:true;
}
bool CPartor::IS_Conflicting(const char* Pri_MentionID, const char* CanditMentionID)
{
	if(m_ResolvedMention_m.find(Pri_MentionID) == m_ResolvedMention_m.end()){
		AppCall::Secretary_Message_Box("Data Error in: CPartor::IS_Conflicting()");
	}
	string Pri_CoEntity = m_ResolvedMention_m[Pri_MentionID];
	if(m_ClusteredMention_ms.find(Pri_CoEntity) == m_ClusteredMention_ms.end()){
		AppCall::Secretary_Message_Box("Data Error in: CPartor::IS_Conflicting()");
	}
	set<string>& PriCo_s = m_ClusteredMention_ms[Pri_CoEntity];

	for(set<string>::iterator site = PriCo_s.begin(); site != PriCo_s.end(); site++){
		if(D_CoCanditValue_mm.find(Pri_MentionID) == D_CoCanditValue_mm.end()){
			continue;
		}
		map<string, double>& CoMention_m = D_CoCanditValue_mm[site->data()];
		if(CoMention_m.find(CanditMentionID) != CoMention_m.end()){
			if(CoMention_m[CanditMentionID] == -1){
				return true;
			}
		}
	}
	return false;
}

