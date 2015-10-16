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
#include "Discrimer.h"
#include "NECom.h"

#include <algorithm>
#include "CEDT.h"

extern int FEEDBACK_CASES_NUM;
extern int GREEDY_MATCH_NUM;

extern double FEEDBACE_PRO_LIMIT;
extern int CAND_FEEDBACK_NUM;
extern int BOARD_n_GRAM_NUM;
extern int INTERNAL_WORDS_NUM;
extern int BOARD_POS_NUM;

extern bool CEDT_Head_Flag;
extern bool CEDT_Detection_Flag;
extern bool CEDT_Extend_Done_Flag;
extern bool CEDT_Head_Done_Flag;
extern bool CEDT_Head_Done_Flag;
extern bool Greedy_Matching_Method_FLag;

CDiscrimer::CDiscrimer()
{
}
CDiscrimer::~CDiscrimer()
{
}

//============================================
void CDiscrimer::Discrimer_Training_and_Testing_Port(vector<NE_Surround*>& training_Surround_v, vector<NE_Surround*>& testing_Surround_v)
{
	//--------for Training_Models_by_Cases
	//CEDT_Detection_Flag = true;
	if(CEDT_Detection_Flag || Greedy_Matching_Method_FLag){
		Generate_Discrimer_for_Extend_or_Head_Only_Recognition(pCCEDT->m_TestingCEDTInfo);
		return;
	}
	//----------------
	MaxentModel loc_maxent;
	
	if(pCCEDT->Apply_for_Model_Training(training_Surround_v.empty(), "Dism")){
		Training_Named_Entity_Discriminater(training_Surround_v, loc_maxent);
		if(!pCCEDT->Forbid_Rewrite_Models_Flag){
			pCCEDT->Save_MaxentModel("Dism", loc_maxent);
		}
	}
	else{
		pCCEDT->Load_MaxentModel("Dism", loc_maxent);
	}
	//----------------------
	if(!testing_Surround_v.empty()){
		Testing_Named_Entity_Discriminater(testing_Surround_v, loc_maxent);
	}
}
void CDiscrimer::Discriminater_Recognization_Port(const char* inputchar, MaxentModel& pm_maxent, CEDTInfo& pmCEDTInfo)
{
	Combined_Matched_Head_and_Extend_for_Discrimer(pmCEDTInfo);

	FeatureVctor Feature_v;
	Generate_Named_Entity_Discrimer_Cases_Feature(pmCEDTInfo.DismCase_v, Feature_v);

	MAXEN::Discrimer_Maxen_Recognization_with_Erasing(pmCEDTInfo.DismCase_v, Feature_v, pm_maxent);
}

//============================================
void CDiscrimer::Training_Named_Entity_Discriminater(vector<NE_Surround*>& training_Surround_v, MaxentModel& pm_maxent)
{
	AppCall::Maxen_Responce_Message("\n\n\n**********************************************************************\n");
	AppCall::Maxen_Responce_Message("**********************Entity_Discriminater\n");

	AppCall::Maxen_Responce_Message("\nGenerating Named Entity Discrimer training features\n");

	Generate_Discrimer_Training_Case(training_Surround_v, pCCEDT->m_TrainingCEDTInfo);

	vector<DismCase*>& locDismCase_v = pCCEDT->m_TrainingCEDTInfo.DismCase_v;
	locDismCase_v.reserve(RESERVED_NUM);
	FeatureVctor Training_v;
	Training_v.reserve(RESERVED_NUM);

	Generate_Named_Entity_Discrimer_Cases_Feature(locDismCase_v, Training_v);

	AppCall::Maxen_Responce_Message("\n\n\n**********************************************************************\n");
	AppCall::Maxen_Responce_Message("**********************Training corpus Named Entity Discrimer classifier\n");

	MAXEN::Maxen_Training_with_Erasing(Training_v, pm_maxent, pCCEDT->m_CanditIte);

}

void CDiscrimer::Testing_Named_Entity_Discriminater(vector<NE_Surround*>& testing_Surround_v, MaxentModel& pm_maxent)
{
	AppCall::Maxen_Responce_Message("\n\n\n**********************************************************************\n");
	AppCall::Maxen_Responce_Message("**********************Testing corpus Named Entity Discriminater\n");
	
	AppCall::Maxen_Responce_Message("Generating Named Entity Testing Cases...\n");
	//Combined_Performance_and_Generate_ACE_Testing_for_Discrimer(pCCEDT->m_TestingCEDTInfo);
	if(pCCEDT->m_TestingCEDTInfo.CanditHead_v.empty() || pCCEDT->m_TestingCEDTInfo.CanditExtend_v.empty()){
		Generate_Discrimer_for_Extend_or_Head_Only_Recognition(pCCEDT->m_TestingCEDTInfo);
	}
	else{
		Combined_Matched_Head_and_Extend_for_Discrimer(pCCEDT->m_TestingCEDTInfo);	
	}
	AppCall::Maxen_Responce_Message("Generating Named Entity Testing Cases Features...\n");
	FeatureVctor Testing_v;
	Testing_v.reserve(RESERVED_NUM);
	vector<DismCase*>& DismCase_v = pCCEDT->m_TestingCEDTInfo.DismCase_v;
	Generate_Named_Entity_Discrimer_Cases_Feature(DismCase_v, Testing_v);
	
	AppCall::Maxen_Responce_Message("Testing Discrimer Cases...\n");
	MAXEN::Discrimer_Maxen_Testing_with_Erasing(DismCase_v, Testing_v, pm_maxent);
}


//============================================
void CDiscrimer::Generate_Discrimer_Training_Case(vector<NE_Surround*>& Surround_v, CEDTInfo& pmCEDTInfo)
{
	if(CEDT_Detection_Flag){
		AppCall::Secretary_Message_Box("Detection of NE not need this function! For Recognition?", MB_OK);
	}
	vector<CanditCase*>& CanditHead_v = pmCEDTInfo.CanditHead_v;
	vector<CanditCase*>& CanditExtend_v = pmCEDTInfo.CanditExtend_v;
	vector<DismCase*>& DismCase_v = pmCEDTInfo.DismCase_v;

	if(!CEDT_Extend_Done_Flag && !CEDT_Head_Done_Flag){
		AppCall::Secretary_Message_Box("Data Error in CDiscrimer::Generate_Discrimer_Training_Case(): \nExtend or Head should be done!", MB_OK);
		return;
	}

	size_t CASID = 0;
	for(vector<NE_Surround*>::iterator ne_vite = Surround_v.begin(); ne_vite != Surround_v.end(); ne_vite++){
		const char* insurchar = (*ne_vite)->insur.c_str();
		size_t SENID = (*ne_vite)->SENID;
		set<size_t> headSTART_s;
		set<size_t> headEND_s;
		map<size_t, map<size_t, ACE_entity_mention*>> EntityMention_mm;
		ace_op::Get_Entity_Mention_head_Map((*ne_vite)->entity_mention_v, EntityMention_mm, headSTART_s, headEND_s);

		for(map<size_t, map<size_t, ACE_entity_mention*>>::iterator mmite = EntityMention_mm.begin(); mmite != EntityMention_mm.end(); mmite++){
			for(map<size_t, ACE_entity_mention*>::iterator mite = mmite->second.begin(); mite != mmite->second.end(); mite++){
				ACE_extent& locExtend = mite->second->extent;
				ACE_entity_head& locHead = mite->second->head;
				pDismCase ploc_DisM = new DismCase;
				ploc_DisM->SENID = SENID;
				ploc_DisM->CASID = CASID++;
				ploc_DisM->Dism_Flag = true;
				ploc_DisM->org_TYPE = mite->second->Entity_TYPE;
				ploc_DisM->TYPE_value = 0.0;
				
				if(CEDT_Extend_Done_Flag){
					pCanditCase plocExtend = new CanditCase;
					plocExtend->SENID = SENID;
					plocExtend->CASID = CASID;
					plocExtend->Cand_Flag = true;
					plocExtend->CEDT_Head_Flag = false;
					plocExtend->value = 0.0;
					plocExtend->org_TYPE = mite->second->Entity_TYPE;
					plocExtend->prix = Sentop::Get_Substr_by_Chinese_Character_Cnt(insurchar, 0, locExtend.START);
					plocExtend->prox = Sentop::Get_Substr_by_Chinese_Character_Cnt(insurchar, locExtend.END+1, -1);
					plocExtend->mention = locExtend.charseq;
					plocExtend->START = locExtend.START;
					plocExtend->END =  locExtend.END;
					ploc_DisM->pExtend = plocExtend;
					CanditExtend_v.push_back(plocExtend); //add to CanditExtend_v for delete, will not be used;
					ploc_DisM->Extend_Held_Flag = true;
				}
				else{
					ploc_DisM->Extend_Held_Flag = false;
					ploc_DisM->pExtend = NULL;
				}

				if(CEDT_Head_Done_Flag){
					pCanditCase plocHead = new CanditCase;
					plocHead->SENID = SENID;
					plocHead->CASID = CASID;
					plocHead->Cand_Flag = true;
					plocHead->CEDT_Head_Flag = true;
					plocHead->value = 0.0;
					plocHead->org_TYPE = mite->second->Entity_TYPE;
					plocHead->prix = Sentop::Get_Substr_by_Chinese_Character_Cnt(insurchar, 0, locHead.START);
					plocHead->prox = Sentop::Get_Substr_by_Chinese_Character_Cnt(insurchar, locHead.END+1, -1);
					plocHead->mention = locHead.charseq;
					plocHead->START = locHead.START;
					plocHead->END = locHead.END;
					ploc_DisM->pHead =  plocHead;
					CanditHead_v.push_back(plocHead); //add to CanditExtend_v for delete, will not be used;
					ploc_DisM->Head_Held_Flag = true;
				}
				else{
					ploc_DisM->Head_Held_Flag = false;
					ploc_DisM->pHead = NULL;
				}
				ploc_DisM->pNE_mention = mite->second;
				DismCase_v.push_back(ploc_DisM);
			}
		}
		//delete *ne_vite;
	}
	//Surround_v.clear();
}
void CDiscrimer::Generate_Discrimer_for_Extend_or_Head_Only_Recognition(CEDTInfo& pmCEDTInfo)
{
	vector<CanditCase*>& CanditHead_v = pmCEDTInfo.CanditHead_v;
	vector<CanditCase*>& CanditExtend_v = pmCEDTInfo.CanditExtend_v;
	vector<DismCase*>& DismCase_v = pmCEDTInfo.DismCase_v;

	if(CanditHead_v.empty() && CanditExtend_v.empty()){
		AppCall::Secretary_Message_Box("Emtpy data in: CDiscrimer::Generate_Discrimer_for_Extend_or_Head_Only_Recognition()", MB_OK);
		return;
	}
	if(!CanditHead_v.empty() && !CanditExtend_v.empty()){
		AppCall::Secretary_Message_Box("Using CDiscrimer::Combined_Matched_Head_and_Extend_for_Discrimer() instead...", MB_OK);
		return;
	}
	vector<CanditCase*>& LocCanditcase_v = !CanditHead_v.empty()?CanditHead_v:CanditExtend_v;
	for(size_t i = 0; i < LocCanditcase_v.size(); i++){
		CanditCase& locCandit = *LocCanditcase_v[i];
		if(!strcmp(locCandit.predit_TYPE.c_str(), NEGETIVE)){
			delete LocCanditcase_v[i];
			LocCanditcase_v[i] = NULL;
			continue;
		}
		DismCase* ploc_DisM = new DismCase;
		ploc_DisM->SENID = locCandit.SENID;
		ploc_DisM->CASID = locCandit.CASID;
		ploc_DisM->Dism_Flag = false;
		ploc_DisM->Matched_Extend_Flag = false;
		ploc_DisM->predit_TYPE = locCandit.predit_TYPE;
		ploc_DisM->TYPE_value = locCandit.value;
		if(locCandit.CEDT_Head_Flag){
			ploc_DisM->pHead = LocCanditcase_v[i];
			ploc_DisM->Head_Held_Flag = true;
			ploc_DisM->pExtend = NULL;
			ploc_DisM->Extend_Held_Flag = false;
			if(ploc_DisM->pHead->Cand_Flag){
				ploc_DisM->org_TYPE = CEDT_Detection_Flag?POSITIVE:ploc_DisM->pHead->pNE_mention->Entity_TYPE;
			}
			else{
				ploc_DisM->org_TYPE = NEGETIVE;
			}
		}
		else{
			ploc_DisM->pHead = NULL;
			ploc_DisM->Head_Held_Flag = false;
			ploc_DisM->pExtend = LocCanditcase_v[i];
			ploc_DisM->Extend_Held_Flag = true;
			if(ploc_DisM->pExtend->Cand_Flag){
				ploc_DisM->org_TYPE = CEDT_Detection_Flag?POSITIVE:ploc_DisM->pExtend->pNE_mention->Entity_TYPE;
			}
			else{
				ploc_DisM->org_TYPE = NEGETIVE;
			}
		}
		DismCase_v.push_back(ploc_DisM);
	}
}

void CDiscrimer::Combined_Matched_Head_and_Extend_for_Discrimer(CEDTInfo& pmCEDTInfo)
{
	map<size_t, map<size_t, double>*>& rtnExtend_START_Test_mm = pmCEDTInfo.Extend_START_mm;
	map<size_t, map<size_t, double>*>& rtnExtend_END_Test_mm = pmCEDTInfo.Extend_END_mm;
	map<size_t, map<size_t, double>*>& rtnHead_START_Test_mm = pmCEDTInfo.Head_START_mm;
	map<size_t, map<size_t, double>*>& rtnHead_END_Test_mm = pmCEDTInfo.Head_END_mm;
	vector<CanditCase*>& CanditHead_v = pmCEDTInfo.CanditHead_v;
	vector<CanditCase*>& CanditExtend_v = pmCEDTInfo.CanditExtend_v;
	vector<DismCase*>& DismCase_v = pmCEDTInfo.DismCase_v;
	
	if(CanditHead_v.empty() && CanditExtend_v.empty()){
		//AppCall::Secretary_Message_Box("Emtpy data in: CDiscrimer::Combined_Matched_Head_and_Extend_for_Discrimer()", MB_OK);
		return;
	}
	/*if(CanditHead_v.empty() || CanditExtend_v.empty()){
		AppCall::Secretary_Message_Box("Using Generate_Discrimer_for_Extend_or_Head_Only_Recognition() instead....", MB_OK);
		return;
	}*/
	
	map<size_t, map<size_t, map<size_t, CanditCase*>>*> ENExtend_mm;
	map<size_t, map<size_t, map<size_t, CanditCase*>>*> ReverseExtend_mm;
	map<size_t, map<size_t, map<size_t, CanditCase*>>*> ENHead_mm;
	//Init ENExtend_mm, ReverseExtend_mm;
	for(size_t i = 0; i < CanditExtend_v.size(); i++){
		CanditCase& locCandit = *CanditExtend_v[i];
		VERIFY(!locCandit.CEDT_Head_Flag);
		VERIFY(locCandit.Detection_Flag);
		if(!strcmp(locCandit.predit_TYPE.c_str(), NEGETIVE)){
			delete CanditExtend_v[i];
			CanditExtend_v[i] = NULL;
			continue;
		}
		if(ENExtend_mm.find(locCandit.SENID) == ENExtend_mm.end()){
			ENExtend_mm[locCandit.SENID];
			ENExtend_mm[locCandit.SENID] = new map<size_t, map<size_t, CanditCase*>>;
			ReverseExtend_mm[locCandit.SENID];
			ReverseExtend_mm[locCandit.SENID] = new map<size_t, map<size_t, CanditCase*>>;
		}
		map<size_t, map<size_t, CanditCase*>>& loc_mm = *ENExtend_mm[locCandit.SENID];
		if(loc_mm.find(locCandit.START) == loc_mm.end()){
			loc_mm[locCandit.START];
		}
		if(loc_mm[locCandit.START].find(locCandit.END) == loc_mm[locCandit.START].end()){
			loc_mm[locCandit.START].insert(make_pair(locCandit.START, CanditExtend_v[i]));
		}
		else{
			AppCall::Secretary_Message_Box("Reduplicated extend mention...", MB_OK);
		}

		map<size_t, map<size_t, CanditCase*>>& Revloc_mm = *ReverseExtend_mm[locCandit.SENID];
		if(Revloc_mm.find(locCandit.END) == Revloc_mm.end()){
			Revloc_mm[locCandit.END];
		}
		if(Revloc_mm[locCandit.END].find(locCandit.START) == Revloc_mm[locCandit.END].end()){
			Revloc_mm[locCandit.END].insert(make_pair(locCandit.START, CanditExtend_v[i]));
		}
		else{
			AppCall::Secretary_Message_Box("Reduplicated extend mention...", MB_OK);
		}
	}
	//Init ENHead_mm;
	for(size_t i = 0; i < CanditHead_v.size(); i++){
		CanditCase& locCandit = *CanditHead_v[i];
		VERIFY(locCandit.CEDT_Head_Flag);
		if(!strcmp(locCandit.predit_TYPE.c_str(), NEGETIVE)){
			delete CanditHead_v[i];
			CanditHead_v[i] = NULL;
			continue;
		}
		if(ENHead_mm.find(locCandit.SENID) == ENHead_mm.end()){
			ENHead_mm[locCandit.SENID];
			ENHead_mm[locCandit.SENID] = new map<size_t, map<size_t, CanditCase*>>;
		}
		map<size_t, map<size_t, CanditCase*>>& loc_mm = *ENHead_mm[locCandit.SENID];
		if(loc_mm.find(locCandit.START) == loc_mm.end()){
			loc_mm[locCandit.START];
		}
		if(loc_mm[locCandit.START].find(locCandit.END) == loc_mm[locCandit.START].end()){
			loc_mm[locCandit.START].insert(make_pair(locCandit.END, CanditHead_v[i]));
		}
		else{
			AppCall::Secretary_Message_Box("Reduplicated extend mention...", MB_OK);
		}
	}
	//Matching Head and Extend;
	for(size_t i = 0; i < CanditHead_v.size(); i++){
		if(CanditHead_v[i] == NULL){
			continue;
		}
		CanditCase& locHeadCandit = *CanditHead_v[i];
		size_t SENID = locHeadCandit.SENID;
		bool Exist_Related_Extend_Flag = false;
		CanditCase* pMatchedExtend = NULL;

		//Matching Extend
		if(!((ENExtend_mm.find(SENID) == ENExtend_mm.end()) || (rtnExtend_END_Test_mm.find(SENID) == rtnExtend_END_Test_mm.end()) || (rtnExtend_START_Test_mm.find(SENID) == rtnExtend_START_Test_mm.end()))){
			map<size_t, map<size_t, CanditCase*>>& locExtend_mm = *ENExtend_mm[SENID];
			map<size_t, map<size_t, CanditCase*>>& locHead_mm = *ENHead_mm[SENID];
			map<size_t, map<size_t, CanditCase*>>& locRevExtend_mm = *ReverseExtend_mm[SENID];
			map<size_t, double>& locExtendSTART_m = *rtnExtend_START_Test_mm[SENID];
			map<size_t, double>& locExtendEND_m = *rtnExtend_END_Test_mm[SENID];

			if(locRevExtend_mm.find(locHeadCandit.END) != locRevExtend_mm.end()){
				if(locRevExtend_mm[locHeadCandit.END].find(locHeadCandit.START) != locRevExtend_mm[locHeadCandit.END].end()){
					Exist_Related_Extend_Flag = true;
					pMatchedExtend = locRevExtend_mm[locHeadCandit.END][locHeadCandit.START];
				}
				else{
					size_t STARTlimit;
					double maxValue = 0;
					map<size_t, map<size_t, CanditCase*>>::iterator STARTite= locRevExtend_mm.find(locHeadCandit.END);
					if(STARTite == locRevExtend_mm.begin()){
						STARTlimit = 0;
					}
					else{
						STARTlimit = (--STARTite)->first;
					}
					for(map<size_t, CanditCase*>::reverse_iterator rSTARTmite = locRevExtend_mm[locHeadCandit.END].rbegin(); rSTARTmite != locRevExtend_mm[locHeadCandit.END].rend(); rSTARTmite++){
						if(rSTARTmite->first < STARTlimit){
							break;
						}
						if((int)rSTARTmite->first <= locHeadCandit.START){//if(maxValue < rSTARTmite->second->value){//???
							if(maxValue < rSTARTmite->second->value){
								Exist_Related_Extend_Flag = true;
								pMatchedExtend = rSTARTmite->second;
								maxValue = rSTARTmite->second->value;
								//break;
							}
						}
					}
					if(!Exist_Related_Extend_Flag){
						double maxSTARTvalue = 0;
						for(map<size_t, double>::reverse_iterator  rmite = locExtendSTART_m.rbegin(); rmite != locExtendSTART_m.rend(); rmite++){
							if(rmite->first < STARTlimit){
								break;
							}
							if((int)rmite->first <= locHeadCandit.START){
								if(rmite->second > maxSTARTvalue){
									maxSTARTvalue = rmite->second;
								}
							} 
						}
					}
				}
			}//if matched END;
			else if(locExtend_mm.find(locHeadCandit.START) != locExtend_mm.end()){
				if(locExtend_mm[locHeadCandit.START].find(locHeadCandit.END) != locExtend_mm[locHeadCandit.START].end()){
					Exist_Related_Extend_Flag = true;
					pMatchedExtend = locExtend_mm[locHeadCandit.START][locHeadCandit.END];
				}
				else{
					size_t ENDlimit;
					double maxValue = 0;
					map<size_t, map<size_t, CanditCase*>>::iterator ENDite = locExtend_mm.find(locHeadCandit.START);
					ENDlimit = (++ENDite == locExtend_mm.end())?(--ENDite)->first:ENDite->first;
					for(map<size_t, CanditCase*>::iterator ENDmite = locExtend_mm[locHeadCandit.START].begin(); ENDmite != locExtend_mm[locHeadCandit.START].end(); ENDmite++){
						if(ENDmite->second->END > (int)ENDlimit){
							break;
						}
						if(ENDmite->second->END >= locHeadCandit.END){//if(maxValue < rSTARTmite->second->value){//???
							if(maxValue < ENDmite->second->value){
								Exist_Related_Extend_Flag = true;
								pMatchedExtend = ENDmite->second;
								maxValue = ENDmite->second->value;
								//break;
							}
						}
					}
					if(!Exist_Related_Extend_Flag){
						double maxENDvalue = 0;
						for(map<size_t, double>::iterator mite = locExtendEND_m.begin(); mite != locExtendEND_m.end(); mite++){
							if(mite->first > ENDlimit){
								break;
							}
							if((int)mite->first >= locHeadCandit.END){
								if(mite->second > maxENDvalue){
									maxENDvalue = mite->second;
								}
							}
						}
					}
				}
			}//if matched START
			else{
				int maxSTART = -1;
				int maxEND = -1;
				double maxValue = 0;
				for(map<size_t, map<size_t, CanditCase*>>::iterator mmite = locExtend_mm.begin(); mmite != locExtend_mm.end(); mmite++){
					for(map<size_t, CanditCase*>::iterator mite = mmite->second.begin(); mite != mmite->second.end(); mite++){
						if(((int)mmite->first <= locHeadCandit.START) && ((int)mite->first >= locHeadCandit.END)){
							if(mite->second->value > maxValue){
								maxSTART = mmite->first;
								maxEND = mite->first;
								maxValue = mite->second->value;
								Exist_Related_Extend_Flag = true;
								pMatchedExtend = mite->second;
								maxValue = mite->second->value;
							}
							//if((mmite->first > maxSTART) || (mite->first < maxEND)){
							//}
						}
					}
				}
				if(!Exist_Related_Extend_Flag){
					//------------START
					//------------END
				}
			}//else no STAR and END*/
		}//Matching Extend

		DismCase* ploc_DisM = new DismCase;
		ploc_DisM->SENID = locHeadCandit.SENID;
		ploc_DisM->CASID = locHeadCandit.CASID;
		ploc_DisM->Dism_Flag = false;
		ploc_DisM->Matched_Extend_Flag = false;
		ploc_DisM->TYPE_value = 0;

		ploc_DisM->pHead = CanditHead_v[i];
		ploc_DisM->Head_Held_Flag = true;
		if(ploc_DisM->pHead->Cand_Flag){
			ploc_DisM->org_TYPE = CEDT_Detection_Flag?POSITIVE:ploc_DisM->pHead->pNE_mention->Entity_TYPE;
		}
		else{
			ploc_DisM->org_TYPE = NEGETIVE;
		}
		ploc_DisM->predit_TYPE = ploc_DisM->pHead->predit_TYPE;

		if(Exist_Related_Extend_Flag){
			ploc_DisM->pExtend = pMatchedExtend;
		}
		else{
			ploc_DisM->pExtend = new CanditCase;
			*ploc_DisM->pExtend = *CanditHead_v[i];
			ploc_DisM->pExtend->CEDT_Head_Flag = false;
			ploc_DisM->pExtend->Cand_Flag = false;
			ploc_DisM->pExtend->pNE_mention = NULL;
			if(ploc_DisM->pHead->Cand_Flag){
				ploc_DisM->pExtend->org_TYPE = CEDT_Detection_Flag?POSITIVE:ploc_DisM->pHead->pNE_mention->Entity_TYPE;
			}
			else{
				ploc_DisM->pExtend->org_TYPE = NEGETIVE;
			}
			ploc_DisM->pExtend->predit_TYPE = ploc_DisM->pHead->predit_TYPE;
			ploc_DisM->pExtend->value = 0;

			CanditExtend_v.push_back(ploc_DisM->pExtend);
		}
		ploc_DisM->Extend_Held_Flag = true;
		DismCase_v.push_back(ploc_DisM);
	}

	for(map<size_t, map<size_t, map<size_t, CanditCase*>>*>::iterator mmite = ENExtend_mm.begin(); mmite != ENExtend_mm.end(); mmite++){
		delete mmite->second;
	}
	ENExtend_mm.clear();
	for(map<size_t, map<size_t, map<size_t, CanditCase*>>*>::iterator mmite = ENHead_mm.begin(); mmite != ENHead_mm.end(); mmite++){
		delete mmite->second;
	}
	ENHead_mm.clear();
	for(map<size_t, map<size_t, map<size_t, CanditCase*>>*>::iterator mmite = ReverseExtend_mm.begin(); mmite != ReverseExtend_mm.end(); mmite++){
		delete mmite->second;
	}
	ReverseExtend_mm.clear();
}



void CDiscrimer::Combined_Performance_and_Generate_ACE_Testing_for_Discrimer(CEDTInfo& pmCEDTInfo)
{
	AppCall::Secretary_Message_Box("Function unsueable...", MB_OK);
	//map<size_t, map<size_t, map<size_t, ACE_extent*>>*>& HeadExtend_mm = pCCEDT->HeadExtend_mm;
	map<size_t, map<size_t, map<size_t, ACE_extent*>>*> HeadExtend_mm;

	map<size_t, map<size_t, double>*>& rtnExtend_START_Test_mm = pmCEDTInfo.Extend_START_mm;
	map<size_t, map<size_t, double>*>& rtnExtend_END_Test_mm = pmCEDTInfo.Extend_END_mm;
	map<size_t, map<size_t, double>*>& rtnHead_START_Test_mm = pmCEDTInfo.Head_START_mm;
	map<size_t, map<size_t, double>*>& rtnHead_END_Test_mm = pmCEDTInfo.Head_END_mm;
	vector<CanditCase*>& CanditHead_v = pmCEDTInfo.CanditHead_v;
	vector<CanditCase*>& CanditExtend_v = pmCEDTInfo.CanditExtend_v;
	vector<DismCase*>& DismCase_v = pmCEDTInfo.DismCase_v;

	if(CanditHead_v.empty() || CanditExtend_v.empty()){
		return;
	}
	map<size_t, map<size_t, map<size_t, CanditCase*>>*> ENExtend_mm;
	map<size_t, map<size_t, map<size_t, CanditCase*>>*> ReverseExtend_mm;
	map<size_t, map<size_t, map<size_t, CanditCase*>>*> ENHead_mm;

	for(size_t i = 0; i < CanditExtend_v.size(); i++){
		CanditCase& locCandit = *CanditExtend_v[i];
		if(!strcmp(locCandit.predit_TYPE.c_str(), NEGETIVE)){
			delete CanditExtend_v[i];
			CanditExtend_v[i] = NULL;
			continue;
		}
		if(ENExtend_mm.find(locCandit.SENID) == ENExtend_mm.end()){
			ENExtend_mm[locCandit.SENID];
			ENExtend_mm[locCandit.SENID] = new map<size_t, map<size_t, CanditCase*>>;
		}
		if(ReverseExtend_mm.find(locCandit.SENID) == ReverseExtend_mm.end()){
			ReverseExtend_mm[locCandit.SENID];
			ReverseExtend_mm[locCandit.SENID] = new map<size_t, map<size_t, CanditCase*>>;
		}
		map<size_t, map<size_t, CanditCase*>>& loc_mm = *ENExtend_mm[locCandit.SENID];
		if(loc_mm.find(locCandit.START) == loc_mm.end()){
			loc_mm[locCandit.START];
		}
		if(loc_mm[locCandit.START].find(locCandit.END) == loc_mm[locCandit.START].end()){
			loc_mm[locCandit.START].insert(make_pair(locCandit.END, CanditExtend_v[i]));
		}
		else{
			AppCall::Secretary_Message_Box("Reduplicated extend mention...", MB_OK);
		}

		map<size_t, map<size_t, CanditCase*>>& Revloc_mm = *ReverseExtend_mm[locCandit.SENID];
		if(Revloc_mm.find(locCandit.END) == Revloc_mm.end()){
			Revloc_mm[locCandit.END];
		}
		if(Revloc_mm[locCandit.END].find(locCandit.START) == Revloc_mm[locCandit.END].end()){
			Revloc_mm[locCandit.END].insert(make_pair(locCandit.START, CanditExtend_v[i]));
		}
		else{
			AppCall::Secretary_Message_Box("Reduplicated extend mention...", MB_OK);
		}
	}

	for(size_t i = 0; i < CanditHead_v.size(); i++){
		CanditCase& locCandit = *CanditHead_v[i];
		if(ENHead_mm.find(locCandit.SENID) == ENHead_mm.end()){
			ENHead_mm[locCandit.SENID];
			ENHead_mm[locCandit.SENID] = new map<size_t, map<size_t, CanditCase*>>;
		}
		map<size_t, map<size_t, CanditCase*>>& loc_mm = *ENHead_mm[locCandit.SENID];
		if(loc_mm.find(locCandit.START) == loc_mm.end()){
			loc_mm[locCandit.START];
		}
		if(loc_mm[locCandit.START].find(locCandit.END) == loc_mm[locCandit.START].end()){
			loc_mm[locCandit.START].insert(make_pair(locCandit.END, CanditHead_v[i]));
		}
		else{
			AppCall::Secretary_Message_Box("Reduplicated extend mention...", MB_OK);
		}
	}

	map<string, pair<size_t, size_t>> Pricision_map;
	map<string, pair<size_t, size_t>> Recall_map;
	
	for(size_t i = 0; i < CanditHead_v.size(); i++){
		CanditCase& locHeadCandit = *CanditHead_v[i];
		VERIFY(locHeadCandit.CEDT_Head_Flag);
		if(Pricision_map.find(locHeadCandit.org_TYPE) == Pricision_map.end()){
			Pricision_map.insert(make_pair(locHeadCandit.org_TYPE, make_pair(0,0)));
			Recall_map.insert(make_pair(locHeadCandit.org_TYPE, make_pair(0,0)));
		}
		if(Pricision_map.find(locHeadCandit.predit_TYPE) == Pricision_map.end()){
			Pricision_map.insert(make_pair(locHeadCandit.predit_TYPE, make_pair(0,0)));
			Recall_map.insert(make_pair(locHeadCandit.predit_TYPE, make_pair(0,0)));
		}
		Pricision_map[locHeadCandit.predit_TYPE].second++;
		Recall_map[locHeadCandit.org_TYPE].second++;

		bool Exist_Matched_Extend_Flag = false;
		pCanditCase pMatchedExtend = NULL;
		int preditMentSTART = -1;
		int PreditMentEND = -1;
		if(!((ENHead_mm.find(locHeadCandit.SENID) == ENHead_mm.end()) || (ENExtend_mm.find(locHeadCandit.SENID) == ENExtend_mm.end()) || (HeadExtend_mm.find(locHeadCandit.SENID) == HeadExtend_mm.end()) ||
			(rtnExtend_START_Test_mm.find(locHeadCandit.SENID) == rtnExtend_START_Test_mm.end()) || (rtnExtend_END_Test_mm.find(locHeadCandit.SENID) == rtnExtend_END_Test_mm.end()))){
			map<size_t, map<size_t, CanditCase*>>& locExtend_mm = *ENExtend_mm[locHeadCandit.SENID];
			map<size_t, map<size_t, CanditCase*>>& locHead_mm = *ENHead_mm[locHeadCandit.SENID];
			map<size_t, map<size_t, CanditCase*>>& locRevExtend_mm = *ReverseExtend_mm[locHeadCandit.SENID];
			map<size_t, map<size_t, ACE_extent*>>& locHeadExtend_mm = *HeadExtend_mm[locHeadCandit.SENID];
			map<size_t, double>& locExtendSTART_m = *rtnExtend_START_Test_mm[locHeadCandit.SENID];
			map<size_t, double>& locExtendEND_m = *rtnExtend_END_Test_mm[locHeadCandit.SENID];
			// Positive Cases
			if(locHeadCandit.Cand_Flag){
				//Corrected Positive Cases
				if(!strcmp(locHeadCandit.org_TYPE.c_str(), locHeadCandit.predit_TYPE.c_str())){
					size_t MentionSTART = locHeadExtend_mm[locHeadCandit.START][locHeadCandit.END]->START;
					size_t MentionEND = locHeadExtend_mm[locHeadCandit.START][locHeadCandit.END]->END;
					if(locRevExtend_mm.find(MentionEND) != locRevExtend_mm.end()){
						if(locRevExtend_mm[MentionEND].find(MentionSTART) != locRevExtend_mm[MentionEND].end()){
							Pricision_map[locHeadCandit.predit_TYPE].first++;
							Recall_map[locHeadCandit.org_TYPE].first++;
							Exist_Matched_Extend_Flag = true;
							preditMentSTART = MentionSTART;
							PreditMentEND = MentionEND;
						}
						else{
							size_t STARTlimit;
							int maxSTART = -1;
							double maxSTARTvalue = 0;
							map<size_t, map<size_t, CanditCase*>>::iterator STARTite= locRevExtend_mm.find(MentionEND);
							if(STARTite == locRevExtend_mm.begin()){
								STARTlimit = 0;
							}
							else{
								STARTlimit = (--STARTite)->first;
							}
							for(map<size_t, double>::reverse_iterator  rmite = locExtendSTART_m.rbegin(); rmite != locExtendSTART_m.rend(); rmite++){
								if((int)rmite->first <= locHeadCandit.START){
									if(rmite->second > maxSTARTvalue){
										maxSTART = rmite->first;
										maxSTARTvalue = rmite->second;
									}
								}
								else if(rmite->first < STARTlimit){
									break;
								}
							}
							if(maxSTART == MentionSTART){
								Pricision_map[locHeadCandit.predit_TYPE].first++;
								Recall_map[locHeadCandit.org_TYPE].first++;
								Exist_Matched_Extend_Flag = true;
							}
							preditMentSTART = maxSTART;
							PreditMentEND = MentionEND;
						}
					}
					else if(locExtend_mm.find(MentionSTART) != locExtend_mm.end()){
						if(locExtend_mm[MentionSTART].find(MentionEND) != locExtend_mm[MentionSTART].end()){
							Pricision_map[locHeadCandit.predit_TYPE].first++;
							Recall_map[locHeadCandit.org_TYPE].first++;
							Exist_Matched_Extend_Flag = true;
							preditMentSTART = MentionSTART;
							PreditMentEND = MentionEND;
						}
						else{
							size_t ENDlimit;
							int maxEND = -1;
							double maxENDvalue = 0;
							map<size_t, map<size_t, CanditCase*>>::iterator ENDite = locExtend_mm.find(MentionSTART);
							ENDlimit = (++ENDite == locExtend_mm.end())?(--ENDite)->first:ENDite->first;
							for(map<size_t, double>::iterator mite = locExtendEND_m.begin(); mite != locExtendEND_m.end(); mite++){
								if((int)mite->first >= locHeadCandit.END){
									if(mite->second > maxENDvalue){
										maxEND = mite->first;
										maxENDvalue = mite->second;
									}
								}
								else if(mite->first > ENDlimit){
									break;
								}
							}
							if(maxEND == MentionEND){
								Pricision_map[locHeadCandit.predit_TYPE].first++;
								Recall_map[locHeadCandit.org_TYPE].first++;
								Exist_Matched_Extend_Flag = true;
							}
							preditMentSTART = MentionSTART;
							PreditMentEND = maxEND;
						}
					}
					else{
						int maxSTART = -1;
						int maxEND = -1;
						double maxValue = 0;
						for(map<size_t, map<size_t, CanditCase*>>::iterator mmite = locExtend_mm.begin(); mmite != locExtend_mm.end(); mmite++){
							for(map<size_t, CanditCase*>::iterator mite = mmite->second.begin(); mite != mmite->second.end(); mite++){
								if(((int)mmite->first <= locHeadCandit.START) && ((int)mite->first >= locHeadCandit.END)){
									if(mite->second->value > maxValue){
										maxSTART = mmite->first;
										maxEND = mite->first;
										maxValue = mite->second->value;
									}
									//if((mmite->first > maxSTART) || (mite->first < maxEND)){
									//}
								}
							}
						}
						if((maxSTART == MentionSTART) && (maxEND == MentionEND)){
							Pricision_map[locHeadCandit.predit_TYPE].first++;
							Recall_map[locHeadCandit.org_TYPE].first++;
							Exist_Matched_Extend_Flag = true;
						}
						preditMentSTART = maxSTART;
						PreditMentEND = maxEND;
					}
				}//Corrected Positive Cases
			}//Positive Cases
			if(Exist_Matched_Extend_Flag){
				if(locExtend_mm.find(preditMentSTART) != locExtend_mm.end()){
					if(locExtend_mm[preditMentSTART].find(PreditMentEND) != locExtend_mm[preditMentSTART].end()){
						pMatchedExtend = locExtend_mm[preditMentSTART][PreditMentEND];
						if(!locExtend_mm[preditMentSTART][PreditMentEND]->Cand_Flag){
							Exist_Matched_Extend_Flag = false;
						}
					}
					else{
						Exist_Matched_Extend_Flag = false;
					} 
				}
				else{
					Exist_Matched_Extend_Flag = false;
				}
			}
		}//if(!((ENHead_mm.find(locHeadCandit.SENID) == ENHead_mm.end()) ||......
		if(!strcmp(locHeadCandit.predit_TYPE.c_str(), POSITIVE)){
			if(Exist_Matched_Extend_Flag){
				DismCase* ploc_DisM = new DismCase;
				ploc_DisM->SENID = locHeadCandit.SENID;
				ploc_DisM->CASID = locHeadCandit.CASID;
				ploc_DisM->Matched_Extend_Flag = true;
				ploc_DisM->Dism_Flag = true;
				ploc_DisM->org_TYPE = CanditHead_v[i]->pNE_mention->Entity_TYPE;
				ploc_DisM->TYPE_value = 0;
				
				ploc_DisM->pExtend = pMatchedExtend;
				ploc_DisM->pHead = CanditHead_v[i];
				ploc_DisM->Head_Held_Flag = true;
				ploc_DisM->Extend_Held_Flag = true;

				DismCase_v.push_back(ploc_DisM);
			}
			else{
				DismCase* ploc_DisM = new DismCase;
				ploc_DisM->SENID = locHeadCandit.SENID;
				ploc_DisM->CASID = locHeadCandit.CASID;
				ploc_DisM->Matched_Extend_Flag = false;
				ploc_DisM->Dism_Flag = false;
				ploc_DisM->org_TYPE = NEGETIVE;
				ploc_DisM->TYPE_value = 0;
				
				ploc_DisM->pExtend = new CanditCase;
				*ploc_DisM->pExtend = *CanditHead_v[i];
				ploc_DisM->pExtend->CEDT_Head_Flag = false;
				CanditExtend_v.push_back(ploc_DisM->pExtend);
				
				ploc_DisM->pHead = CanditHead_v[i];
				ploc_DisM->Head_Held_Flag = true;
				ploc_DisM->Extend_Held_Flag = true;

				DismCase_v.push_back(ploc_DisM);
			}
		}
	}

	/*VERIFY(!Positive_Mentions.empty());
	for(map<string, size_t>::iterator mite = Positive_Mentions.begin(); mite != Positive_Mentions.end(); mite++){
		if(Recall_map.find(mite->first) != Recall_map.end()){
			Recall_map[mite->first].second = mite->second;
		}
	}
	MAXEN::Display_Performance_for_MAXEN(true, true, Pricision_map, Recall_map, "Correct Head and Extend match Performance...");*/

	for(map<size_t, map<size_t, map<size_t, CanditCase*>>*>::iterator mmite = ENExtend_mm.begin(); mmite != ENExtend_mm.end(); mmite++){
		delete mmite->second;
	}
	ENExtend_mm.clear();
	for(map<size_t, map<size_t, map<size_t, CanditCase*>>*>::iterator mmite = ENHead_mm.begin(); mmite != ENHead_mm.end(); mmite++){
		delete mmite->second;
	}
	ENHead_mm.clear();
	for(map<size_t, map<size_t, map<size_t, CanditCase*>>*>::iterator mmite = ReverseExtend_mm.begin(); mmite != ReverseExtend_mm.end(); mmite++){
		delete mmite->second;
	}
	ReverseExtend_mm.clear();
}



//============================================
void CDiscrimer::Generate_Named_Entity_Discrimer_Cases_Feature(vector<DismCase*>& DismCase_v, FeatureVctor& Training_v)
{
	for(size_t i = 0; i < DismCase_v.size(); i++){
		vector<string> adjacent_feature_v;
		
		DismCase& loc_DismCase = *DismCase_v[i];

		pCCEDT->m_CNEFeator.Extract_Feature_Vector_of_Deterimer(loc_DismCase, adjacent_feature_v);

		MAXEN::Push_Back_FeatureCase_v_from_Feature_v_with_Check(Training_v, adjacent_feature_v, loc_DismCase.Dism_Flag?loc_DismCase.org_TYPE:NEGETIVE);
	}
}
