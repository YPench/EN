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
#include "Psture.h"
#include "ConvertUTF.h"
#include "ECOR.h"
#include "ECCom.h"

//#include "D:\\\WordNet\\\2.1\\\src\\\include\\wn.h"

CPstrue::CPstrue()
{
//	map<string, ParTEXT> pmParedTEXT;
	//string testpath = "F:\\YPench\\ConsensusGraph\\Data\\ECOR\\Parser\\Parsed\\AFP_ENG_20030311.0491.sgm";
	//string file;
	//Load_Stanford_Pased_TEXT_Information(testpath.c_str(), file, pmParedTEXT);
	//wninit();
	//int a=is_defined("FAST",VERB);
	Extrure_Inited_Flag = false;

	p_m_F_PsWordInfo = NULL;
	p_m_S_PsWordInfo = NULL;
	p_m_PsDocInfo = NULL;
	p_m_F_PsSentInfo = NULL;
	p_m_S_PsSentInfo = NULL;
}


CPstrue::~CPstrue()
{

}

void CPstrue::Pased_Feature_Extracting_Port(ACE_entity_mention& pmF_mention, ACE_entity_mention& pmS_mention, vector<string>& pmFeatureVector)
{
	if(strcmp(pmF_mention.DOCID.c_str(), pmS_mention.DOCID.c_str())){
		AppCall::Secretary_Message_Box("Two Mentions should be in a same document...");
		return;
	}
	string DOCID = pmF_mention.DOCID;

	if(PsDocsInfo_map.find(DOCID) == PsDocsInfo_map.end()){
		AppCall::Secretary_Message_Box("Data Error in: CExtrue::Dependency_Feature(1)");
		return;
	}

	if(strcmp(m_Current_DOCID.c_str(), DOCID.c_str())){
		if(m_Current_DOCID.length() != 0){
			PsDocsInfo_map[m_Current_DOCID]->Release();
		}
		m_Current_DOCID = DOCID;
		p_m_PsDocInfo = PsDocsInfo_map[m_Current_DOCID];
		if(!p_m_PsDocInfo->Parsed_Info_Loaded_Flag){
			AppCall::Secretary_Message_Box("Paresed Information is not loaded in: CExtrue::Dependency_Feature(2)");
			return;
		}
		if(!p_m_PsDocInfo->Conversed_Flag){
			p_m_PsDocInfo->Init_PsWordInfo_and_PsSentInfo();
		}
	}
	_PsDocInfo& PsDocInfo = *p_m_PsDocInfo;

	if(ECCOM::Nested_Entity_Mentions(pmF_mention, pmS_mention)){
		//Shoul be controled
		return;
	}

	p_m_F_PsWordInfo = PsDocInfo.Get_Word_PsInfo(pmF_mention);
	p_m_S_PsWordInfo = PsDocInfo.Get_Word_PsInfo(pmS_mention);
	
	if(!p_m_F_PsWordInfo || !p_m_S_PsWordInfo){
		//AppCall::Secretary_Message_Box("Data Error in: CExtrue::Dependency_Feature(2)");
		//Shoul be controled
		return;
	}
	if(p_m_PsDocInfo->Is_the_Same_Sentence(pmF_mention, pmS_mention)){
		Inner_Sentence_Dependency_Feature(pmF_mention, pmS_mention, pmFeatureVector);
	}
	

	p_m_F_PsSentInfo = p_m_PsDocInfo->Get_Sentence_PsInfo(p_m_F_PsWordInfo);
	p_m_S_PsSentInfo = p_m_PsDocInfo->Get_Sentence_PsInfo(p_m_S_PsWordInfo);
	
	if(p_m_F_PsSentInfo && p_m_S_PsSentInfo){
		//AppCall::Secretary_Message_Box("Data Error in: CExtrue::Dependency_Feature(3)");
		Outer_Sentence_Dependency_Feature(pmF_mention, pmS_mention, pmFeatureVector);
	}
	else{
		AppCall::Secretary_Message_Box("Data Error in: CPstrue::Pased_Feature_Extracting_Port()");
	}
	

	p_m_F_PsWordInfo = NULL;
	p_m_S_PsWordInfo = NULL;
	p_m_F_PsSentInfo = NULL;
	p_m_S_PsSentInfo = NULL;
}

void CPstrue::Inner_Sentence_Dependency_Feature(ACE_entity_mention& pmF_mention, ACE_entity_mention& pmS_mention, vector<string>& pmFeatureVector)
{
	ostringstream ostrsteam;
	//if(p_m_F_PsWordInfo->Is_Appositional_modifier(p_m_S_PsWordInfo->posit)){
	//	ostrsteam << "DEP_" << "appos";
	//	pmFeatureVector.push_back(ostrsteam.str());
	//}
}

void CPstrue::Outer_Sentence_Dependency_Feature(ACE_entity_mention& pmF_mention, ACE_entity_mention& pmS_mention, vector<string>& pmFeatureVector)
{
	Same_Head_and_Tail_Dependency_Items(pmF_mention, pmS_mention, pmFeatureVector);


}


void CPstrue::Same_Head_and_Tail_Dependency_Items(ACE_entity_mention& pmF_mention, ACE_entity_mention& pmS_mention, vector<string>& pmFeatureVector)
{
	multimap<size_t, DTriple*>& F_HeadedTriple_mm = p_m_F_PsWordInfo->HeadedTriple_mm;
	multimap<size_t, DTriple*>& F_TailedTriple_mm = p_m_F_PsWordInfo->TailedTriple_mm;
	multimap<size_t, DTriple*>& S_HeadedTriple_mm = p_m_S_PsWordInfo->HeadedTriple_mm;
	multimap<size_t, DTriple*>& S_TailedTriple_mm = p_m_S_PsWordInfo->TailedTriple_mm;
	
	set<string> Dependencies_s;
	ostringstream ostrsteam;
	for(multimap<size_t, DTriple*>::iterator FHmite = F_HeadedTriple_mm.begin(); FHmite != F_HeadedTriple_mm.end(); FHmite++){
		for(multimap<size_t, DTriple*>::iterator SHite = S_HeadedTriple_mm.begin(); SHite != S_HeadedTriple_mm.end(); SHite++){
			if(!strcmp(FHmite->second->dependstr.c_str(), SHite->second->dependstr.c_str())){
				/*if(!strcmp(p_m_F_PsSentInfo->Get_Word_By_Posit(FHmite->second->arg2).c_str(), 
					p_m_S_PsSentInfo->Get_Word_By_Posit(SHite->second->arg2).c_str())){
					ostrsteam.str("");
					ostrsteam << "DEP_" << "He_" << FHmite->second->dependstr;
					pmFeatureVector.push_back(ostrsteam.str());
				}*/
				string DependWord_F = p_m_F_PsSentInfo->Get_Word_By_Posit(FHmite->second->arg2);
				string DependWord_S = p_m_S_PsSentInfo->Get_Word_By_Posit(SHite->second->arg2);
				ostrsteam.str("");
				if(strcmp(DependWord_F.c_str(), DependWord_S.c_str())){
					ostrsteam << "DEP_" << "He_" << DependWord_F << "_" << DependWord_S;
				}
				else{
					ostrsteam << "DEP_" << "He_" << DependWord_S << "_" << DependWord_F;
				}
				Dependencies_s.insert(ostrsteam.str());
			}
		}
	}
	for(multimap<size_t, DTriple*>::iterator FTmite = F_TailedTriple_mm.begin(); FTmite != F_TailedTriple_mm.end(); FTmite++){
		for(multimap<size_t, DTriple*>::iterator STmite = S_TailedTriple_mm.begin();  STmite != S_TailedTriple_mm.end(); STmite++){
			if(!strcmp(FTmite->second->dependstr.c_str(), STmite->second->dependstr.c_str())){
				/*if(!strcmp(p_m_F_PsSentInfo->Get_Word_By_Posit(FTmite->second->arg1).c_str(), 
					p_m_S_PsSentInfo->Get_Word_By_Posit(STmite->second->arg1).c_str())){
					ostrsteam.str("");
					ostrsteam << "DEP_" << "Ta_" << FTmite->second->dependstr;
					pmFeatureVector.push_back(ostrsteam.str());
				}*/
				string DependWord_F = p_m_F_PsSentInfo->Get_Word_By_Posit(FTmite->second->arg1);
				string DependWord_S = p_m_S_PsSentInfo->Get_Word_By_Posit(STmite->second->arg1);
				ostrsteam.str("");
				if(strcmp(DependWord_F.c_str(), DependWord_S.c_str())){
					ostrsteam << "DEP_" << "Ta_" << DependWord_F << "_" << DependWord_S;
				}
				else{
					ostrsteam << "DEP_" << "Ta_" << DependWord_S << "_" << DependWord_F;
				}
				Dependencies_s.insert(ostrsteam.str());
				//:YPench
				//if(!strcmp("The world's most powerful fine art auction houses", DependWord_F.c_str())){
				//	int viewpoint = 1;
				//}
			}
		}
	}
	for(set<string>::iterator site = Dependencies_s.begin(); site != Dependencies_s.end(); site++){
		pmFeatureVector.push_back(site->data());
	}
	
}


void CPstrue::Init_Parsed_Info(ACE_Corpus& pmACE_Corpus)
{
	string PasingFolder;
	if(!strcmp(m_pCECOR->m_CorpusTYPE.c_str(), "ACE_E")){
		PasingFolder = ".\\..\\..\\Data\\ECOR\\Parser_E\\";
	}
	else{
		PasingFolder = ".\\..\\..\\Data\\ECOR\\Parser\\";
	}

	m_pCECOR->m_CParser.Generating_Parsed_DOC_Info_Port(pmACE_Corpus, PasingFolder.c_str(), PsDocsInfo_map);


#ifdef ECOR_DATA_CHECKING
	//----Error Checking---------------------
	AppCall::Maxen_Responce_Message("*: Begining PARS::PsDocs_Information_Checking()\n\n");
	PARS::PsDocs_Information_Checking(pmACE_Corpus.ACE_sgm_mmap, PsDocsInfo_map);
#endif
	/*//----Words---------------------
	AppCall::Maxen_Responce_Message("4: Begining PARS::PsDocsInfo_map_to_PsSentInfo_Mmap()\n\n");
	PARS::PsDocsInfo_map_to_PsWordInfo_Mmap(PsDocsInfo_map);
	//----Sentences---------------------
	AppCall::Maxen_Responce_Message("5: Begining PARS::PsDocsInfo_map_to_PsSentInfo_Mmap()\n\n");
	PARS::PsDocsInfo_map_to_PsSentInfo_Mmap(PsDocsInfo_map);
	//----Slimming---------------------
	AppCall::Maxen_Responce_Message("6: Begining PARS::Slimming()\n\n");
	for(map<string, _PsDocInfo*>::iterator mite = PsDocsInfo_map.begin(); mite != PsDocsInfo_map.end(); mite++){
		mite->second->Slimming();
	}*/
	//-----done-----------------
	AppCall::Maxen_Responce_Message("Init_Parsed_Info() is done...\n\n");
	Extrure_Inited_Flag = true;
}
void CPstrue::Parsed_Info_Release()
{
	for(map<string, _PsDocInfo*>::iterator mcite = PsDocsInfo_map.begin(); mcite != PsDocsInfo_map.end(); mcite++){
		 (*mcite->second).Release();
		delete mcite->second;
	}
}
