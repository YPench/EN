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
#include "ECOR.h"
#include "maxen.h"
#include "ECCom.h"


bool CECOR_Insert_Error_Check_Flag;

CECOR::CECOR()
{
	m_ECOR_space = DATA_FOLDER;
	m_ECOR_space += "ECOR\\";
	if(!NLPOP::FolderExist(NLPOP::string2CString(m_ECOR_space))){
		_mkdir(m_ECOR_space.c_str());
	}
	m_CTrain.m_pCECOR = this;
	m_CTest.m_pCECOR = this;
	m_CExtrue.m_pCECOR = this;
	m_CCandit.m_pCECOR = this;
	//m_CParser.m_pCECOR = this;
	m_Pasture.m_pCECOR = this;
	m_CPartor.m_pCECOR = this;

	CECOR_Insert_Error_Check_Flag = true;
}
CECOR::~CECOR(){
	
}

void CECOR::CECOR_Training_and_Testing_Port(ACE_Corpus& pmACE_Corpus, const char* cCoupusPath)
{
	CTime StartTime;
	StartTime = CTime::GetCurrentTime();

	//EXCLUSIVE_Relation_Statistic_Information(pmACE_Corpus);
	//return;
	
	ECOR_Init_Corpus_TYPE(cCoupusPath);
	
	m_Pasture.Init_Parsed_Info(pmACE_Corpus);

	if(!m_Cwordnet.Init_WordNet()){
		AppCall::Secretary_Message_Box("wn: Fatal error - cannot open WordNet database\n");
	}
	
	ECCOM::Get_Mention_Space(pmACE_Corpus.ACE_Entity_Info_map, MentionSpace_m);

	vector<string> Doc_v;
	for(map<string, ACE_sgm>::iterator mite = pmACE_Corpus.ACE_sgm_mmap.begin(); mite != pmACE_Corpus.ACE_sgm_mmap.end(); mite++){
		Doc_v.push_back(mite->first);
	}
	random_shuffle(Doc_v.begin(), Doc_v.end());
	if(m_nCross > 10){
		m_nCross = 10;
	}
	size_t step = (m_nCross == 0)?0:Doc_v.size()/m_nCross;
	size_t CrossIte = (m_nCross == 0)?1:m_nCross;
	size_t TrainingSentID = 0;
	size_t TestingSentID = 0;
	for(size_t i = 0; i < CrossIte; i++){
		vector<string> training_v;
		vector<string> testing_v;
		size_t Range = (i!=(m_nCross-1))?(i+1)*step:Doc_v.size();
		for(size_t j = 0; j < Doc_v.size(); j++){
			if((j >= i*step) && (j < Range) && (step != 0)){
				testing_v.push_back(Doc_v[j]);
			}
			else{
				training_v.push_back(Doc_v[j]);
			}
		}
		if(testing_v.empty()){
			AppCall::Secretary_Message_Box("No Testing Data is selected!...", MB_OK);
		}
		//:YPench testing and training is changed.
		m_CTrain.ECOR_Model_Training_Port(pmACE_Corpus, training_v, ECOR_Maxent);

		m_CTest.ECOR_Model_Testing_Port(pmACE_Corpus, testing_v, ECOR_Maxent);

		break;
	}
	//WinExec("F:\\YPench\\ConsensusGraph\\EXE\\Perl_Eval\\Eval.bat",SW_SHOW);
	//system("F:\\YPench\\ConsensusGraph\\EXE\\Perl_Eval\\Eval.bat");

	AppCall::Maxen_Responce_Message("Release Parsed Information..\n\n");
	m_Pasture.Parsed_Info_Release();

	ostringstream ostrsteam;
	ostrsteam.str("");
	CTime EndTime = CTime::GetCurrentTime();
	CTimeSpan ts = EndTime - StartTime;
	ostrsteam << "开始时间：" << StartTime.GetHour() << ":" << StartTime.GetMinute() << ":" << StartTime.GetSecond() << endl;
	ostrsteam << "结束时间：" << EndTime.GetHour() << ":" << EndTime.GetMinute() << ":" << EndTime.GetSecond() << endl;
	ostrsteam << "用时：" << ts.GetDays() << "天" << ts.GetHours() << "时" << ts.GetMinutes() <<  "分" << ts.GetSeconds() << "秒..." << endl;
	AppCall::Maxen_Responce_Message(ostrsteam.str().c_str());
}

void CECOR::ECOR_Coreference_Parting_Port(ACE_Corpus& pmACE_Corpus, const char* cPath)
{
	CTime StartTime;
	StartTime = CTime::GetCurrentTime();

	
	ECOR_Init_Corpus_TYPE(cPath);
	
	ECCOM::Get_Mention_Space(pmACE_Corpus.ACE_Entity_Info_map, MentionSpace_m);

	vector<string> Doc_v;
	for(map<string, ACE_sgm>::iterator mite = pmACE_Corpus.ACE_sgm_mmap.begin(); mite != pmACE_Corpus.ACE_sgm_mmap.end(); mite++){
		Doc_v.push_back(mite->first);
	}
	random_shuffle(Doc_v.begin(), Doc_v.end());
	if(m_nCross > 10){
		m_nCross = 10;
	}
	size_t step = (m_nCross == 0)?0:Doc_v.size()/m_nCross;
	size_t CrossIte = (m_nCross == 0)?1:m_nCross;
	size_t TrainingSentID = 0;
	size_t TestingSentID = 0;
	for(size_t i = 0; i < CrossIte; i++){
		vector<string> training_v;
		vector<string> testing_v;
		size_t Range = (i!=(m_nCross-1))?(i+1)*step:Doc_v.size();
		for(size_t j = 0; j < Doc_v.size(); j++){
			if((j >= i*step) && (j < Range) && (step != 0)){
				testing_v.push_back(Doc_v[j]);
			}
			else{
				training_v.push_back(Doc_v[j]);
			}
		}
		if(testing_v.empty()){
			AppCall::Secretary_Message_Box("No Testing Data is selected!...", MB_OK);
		}

		m_CTest.ECOR_Model_Testing_Parting_Port(pmACE_Corpus, testing_v);

		break;
	}


	ostringstream ostrsteam;
	ostrsteam.str("");
	CTime EndTime = CTime::GetCurrentTime();
	CTimeSpan ts = EndTime - StartTime;
	ostrsteam << "开始时间：" << StartTime.GetHour() << ":" << StartTime.GetMinute() << ":" << StartTime.GetSecond() << endl;
	ostrsteam << "结束时间：" << EndTime.GetHour() << ":" << EndTime.GetMinute() << ":" << EndTime.GetSecond() << endl;
	ostrsteam << "用时：" << ts.GetDays() << "天" << ts.GetHours() << "时" << ts.GetMinutes() <<  "分" << ts.GetSeconds() << "秒..." << endl;
	AppCall::Maxen_Responce_Message(ostrsteam.str().c_str());
}


void CECOR::ECOR_Stanford_Parsing_Port(ACE_Corpus& pmACE_Corpus, const char* cCoupusPath)
{
	string PasingFolder;
	ECOR_Init_Corpus_TYPE(cCoupusPath);

	if(m_CorpusTYPE == "ACE_E"){
		PasingFolder = ".\\..\\..\\Data\\ECOR\\Parser_E\\";
	}
	else{
		PasingFolder = ".\\..\\..\\Data\\ECOR\\Parser\\";
	}
	m_CParser.Stanford_Parsing_Port(pmACE_Corpus, PasingFolder.c_str());

}

void CECOR::ECOR_Init_Corpus_TYPE(const char* cCoupusPath)
{
	m_CorpusTYPE = cCoupusPath;
	m_CorpusTYPE = m_CorpusTYPE.substr(0, m_CorpusTYPE.length()-1);
	m_CorpusTYPE = m_CorpusTYPE.substr(m_CorpusTYPE.rfind("\\")+1, m_CorpusTYPE.length()-m_CorpusTYPE.rfind("\\"));
}

void CECOR::EXCLUSIVE_Relation_Statistic_Information(ACE_Corpus& pmACE_Corpus)
{
	vector<ACE_relation>& ACE_Relation_Info_v = pmACE_Corpus.ACE_Relation_Info_v;
	map<string, ACE_entity>& ACE_Entity_Info_map = pmACE_Corpus.ACE_Entity_Info_map;

	map<string, string> EMention2E_m;
	for(map<string, ACE_entity>::iterator mite = ACE_Entity_Info_map.begin(); mite != ACE_Entity_Info_map.end(); mite++){
		vector<ACE_entity_mention>& entity_mention_v = mite->second.entity_mention_v;
		for(size_t i = 0; i < entity_mention_v.size(); i++){
			if(EMention2E_m.find(entity_mention_v[i].ID) == EMention2E_m.end()){
				EMention2E_m.insert(make_pair(entity_mention_v[i].ID, entity_mention_v[i].Entity_ID));
			}
		}
	}

	map<string, pair<size_t, size_t>> TYPE_info;
	map<string, pair<size_t, size_t>> SUBTYPE_info;
	map<string, bool> RelMentionCoref_m;

	for(size_t i = 0; i < ACE_Relation_Info_v.size(); i++){
		vector<ACE_relation_mention>& relation_mention_v = ACE_Relation_Info_v[i].relation_mention_v;
		string Rel_TYPE = ACE_Relation_Info_v[i].TYPE;
		string Rel_SUBTYPE = ACE_Relation_Info_v[i].SUBTYPE;
		/*if(Rel_TYPE.length() == 0 || Rel_SUBTYPE.length() == 0){
			ostringstream ostrsteam;
			ostrsteam << "Relation TYPE: " << Rel_TYPE << endl;
			ostrsteam << "Relation SUBTYPE: " << Rel_SUBTYPE << endl;
			ostrsteam << "Document: " << ACE_Relation_Info_v[i].DOCID << endl;
			ostrsteam << "Relation: " << ACE_Relation_Info_v[i].ID << endl;
			AppCall::Secretary_Message_Box(ostrsteam.str().c_str());
		}*/
		if(TYPE_info.find(Rel_TYPE) == TYPE_info.end()){
			TYPE_info.insert(make_pair(Rel_TYPE, make_pair(0, 0)));
		}
		if(SUBTYPE_info.find(Rel_SUBTYPE) == SUBTYPE_info.end()){
			SUBTYPE_info.insert(make_pair(Rel_SUBTYPE, make_pair(0, 0)));
		}
		for(size_t j = 0; j < relation_mention_v.size(); j++){
			multimap<string, ACE_relation_mention_arg>& arg_mention_mmap = relation_mention_v[j].arg_mention_mmap;
			string REFID_1;
			string REFID_2;
			for(multimap<string, ACE_relation_mention_arg>::iterator emvite = arg_mention_mmap.begin();  emvite != arg_mention_mmap.end(); emvite++){
				if(!strcmp("Arg-1", emvite->first.c_str())){
					REFID_1 = emvite->second.REFID;
				}
				if(!strcmp("Arg-2", emvite->first.c_str())){
					REFID_2 = emvite->second.REFID;
				}
			}
			if(EMention2E_m.find(REFID_1) == EMention2E_m.end() || EMention2E_m.find(REFID_2) == EMention2E_m.end()){
				AppCall::Secretary_Message_Box("Data Error in: CECOR::EXCLUSIVE_Relation_Statistic_Information()");
			}
			string rMentionID = relation_mention_v[j].ID;
			if(!strcmp(EMention2E_m[REFID_1].c_str(), EMention2E_m[REFID_2].c_str())){
				TYPE_info[Rel_TYPE].first++;
				SUBTYPE_info[Rel_SUBTYPE].first++;
			}
			else{
				TYPE_info[Rel_TYPE].second++;
				SUBTYPE_info[Rel_SUBTYPE].second++;
			}
		}
	}
	
	ostringstream ostrsteam;
	ostrsteam << "-----------------TYPE Information-----------------" << endl;
	for(map<string, pair<size_t,size_t>>::iterator mite = TYPE_info.begin(); mite != TYPE_info.end(); mite++){
		ostrsteam << mite->first << ": " << mite->second.first << "/" << mite->second.second << ";\t";
	}
	ostrsteam << endl;
	ostrsteam << "-----------------SUBTYPE Information-----------------" << endl;
	for(map<string, pair<size_t,size_t>>::iterator mite = SUBTYPE_info.begin(); mite != SUBTYPE_info.end(); mite++){
		ostrsteam << mite->first << ": " << mite->second.first << "/" << mite->second.second << ";\t";
	}
	ostrsteam << endl;

	AppCall::Secretary_Message_Box(ostrsteam.str().c_str());

}