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
#include "CTest.h"
#include "ConvertUTF.h"
#include "ECOR.h"
#include "ECCom.h"

CTest::CTest()
{
	m_TestFolder = "F:\\YPench\\ConsensusGraph\\Data\\ECOR\\Testing\\";
}
CTest::~CTest()
{
}
void CTest::ECOR_Model_Testing_Parting_Port(ACE_Corpus& pmACE_Corpus, vector<string>& pmTesting_v)
{
	AppCall::Maxen_Responce_Message("Generating Testing Reference Coreference...\n\n");
	//-----------Generating Reference Data
	Generate_Reference_Coreference_Data(pmACE_Corpus, pmTesting_v);
	
	map<string, map<string, set<ACE_entity_mention*>*>> CoferenceSet_mm;
	CoMentionPairVector CoCandit_v;
	CoValueVector CoValue_v;
	FeatureVctor Testing_v;
	CoCandit_v.reserve(300000);
	CoValue_v.reserve(300000);
	Testing_v.reserve(300000);

	AppCall::Maxen_Responce_Message("Generating Testing Candidates...\n\n");
	//------------------------Generate_Coreference_Candidate_Port
	m_pCECOR->m_CCandit.Save_or_Load_Parting_Part(m_TestFolder.c_str(), false, CoValue_v, CoCandit_v);
	
	AppCall::Maxen_Responce_Message("Parting Candidates...\n\n");
	//-------------Partition
	m_pCECOR->m_CPartor.Coreference_Partition_Port(CoCandit_v, CoValue_v, CoferenceSet_mm);
	
	AppCall::Maxen_Responce_Message("Output Candidates...\n\n");
	//-------------Output--------------------------------------------
	string outputlist = m_TestFolder + "Outputlist.txt";
	string outputfolder = m_TestFolder + "OutputList\\";
	Output_Mentions_Documents_List(outputlist.c_str(), outputfolder.c_str(), CoferenceSet_mm);

	NLPOP::Delete_Dir_and_Grate(outputfolder);
	Output_Coference_Mention_Set_with_Erasing(outputfolder.c_str(), pmACE_Corpus.ACE_DocInfo_m, CoferenceSet_mm);
 
	AppCall::Maxen_Responce_Message("Testing is done...\n\n");
	//------------Evaluation-------------------------------------------
}

void CTest::Generate_Reference_Coreference_Data(ACE_Corpus& pmACE_Corpus, vector<string>& pmTesting_v)
{
	map<string, map<string, set<ACE_entity_mention*>*>> CoferenceSet_mm;
	string inputlist;
	string inputfolder;
	
	Generating_Testing_Reference_Documents(pmACE_Corpus, pmTesting_v, CoferenceSet_mm);
	
	inputlist = m_TestFolder + "Inputlist.txt";
	inputfolder = m_TestFolder + "InputList\\";
	
	Output_Mentions_Documents_List(inputlist.c_str(), inputfolder.c_str(), CoferenceSet_mm);

	NLPOP::Delete_Dir_and_Grate(inputfolder);
	Output_Coference_Mention_Set_with_Erasing(inputfolder.c_str(), pmACE_Corpus.ACE_DocInfo_m, CoferenceSet_mm);
}

void CTest::ECOR_Model_Testing_Port(ACE_Corpus& pmACE_Corpus, vector<string>& pmTesting_v, MaxentModel& ECOR_Maxent)
{
	NLPOP::Checking_Dir_if_none_then_Greate(m_TestFolder);

	AppCall::Maxen_Responce_Message("Generating Testing Reference Coreference...\n\n");
	//-----------Generating Reference Data
	Generate_Reference_Coreference_Data(pmACE_Corpus, pmTesting_v);
	
	map<string, map<string, set<ACE_entity_mention*>*>> CoferenceSet_mm;
	CoMentionPairVector CoCandit_v;
	CoValueVector CoValue_v;
	FeatureVctor Testing_v;
	CoCandit_v.reserve(300000);
	CoValue_v.reserve(300000);
	Testing_v.reserve(300000);

	AppCall::Maxen_Responce_Message("Generating Testing Candidates...\n\n");
	//------------------------Generate_Coreference_Candidate_Port
	m_pCECOR->m_CCandit.Generate_Coreference_Candidate_Port(m_TestFolder.c_str(), pmACE_Corpus, pmTesting_v, CoValue_v, CoCandit_v);
	
	for(size_t i = 0; i < CoCandit_v.size(); i++){
		if(!strcmp(CoValue_v[i].first.c_str(), EXCLUSIVE)){
			//CoValue_v[i].first = NEGETIVE;
			continue;
		}
		//---------Generate Coreference Features;
		vector<string> pair_feature_v;
		m_pCECOR->m_CExtrue.Extracting_Mention_Pair_Feature_Port(CoCandit_v[i], pair_feature_v);
		MAXEN::Push_Back_FeatureCase_v_from_Feature_v_with_Check(Testing_v, pair_feature_v, !strcmp(CoCandit_v[i].first->Entity_ID.c_str(), CoCandit_v[i].second->Entity_ID.c_str())?POSITIVE:NEGETIVE);
		
		CoValue_v[i].first = !strcmp(CoCandit_v[i].first->Entity_ID.c_str(), CoCandit_v[i].second->Entity_ID.c_str())?POSITIVE:NEGETIVE;
		CoValue_v[i].second = 0;
	}
	if(CoValue_v.size() != CoCandit_v.size()){
		AppCall::Secretary_Message_Box("Data Error in: CTrain::ECOR_Model_Training_Port()");
	}

	AppCall::Maxen_Responce_Message("Testing Candidates...\n\n");

	MAXEN::Coreference_Discriminating_with_Erasing(CoValue_v, Testing_v, ECOR_Maxent);
	
	for(size_t i = 0; i < Testing_v.size(); i++){
		delete Testing_v[i];
	}

	if(m_pCECOR->m_SaveTested_Flag){
		m_pCECOR->m_CCandit.Save_or_Load_Parting_Part(m_TestFolder.c_str(), true, CoValue_v, CoCandit_v);
	}

	AppCall::Maxen_Responce_Message("Parting Candidates...\n\n");
	//-------------Partition
	m_pCECOR->m_CPartor.Coreference_Partition_Port(CoCandit_v, CoValue_v, CoferenceSet_mm);
	
	AppCall::Maxen_Responce_Message("Output Candidates...\n\n");
	//-------------Output--------------------------------------------
	string outputlist = m_TestFolder + "Outputlist.txt";
	string outputfolder = m_TestFolder + "OutputList\\";
	Output_Mentions_Documents_List(outputlist.c_str(), outputfolder.c_str(), CoferenceSet_mm);

	NLPOP::Delete_Dir_and_Grate(outputfolder);
	Output_Coference_Mention_Set_with_Erasing(outputfolder.c_str(), pmACE_Corpus.ACE_DocInfo_m, CoferenceSet_mm);
 
	AppCall::Maxen_Responce_Message("Testing is done...\n\n");
	//------------Evaluation-------------------------------------------
}

void CTest::Generating_Testing_Reference_Documents(ACE_Corpus& pmACE_Corpus, vector<string>& RefTest_v, map<string, map<string, set<ACE_entity_mention*>*>>& CoferenceSet_mm)
{
	if(!CoferenceSet_mm.empty()){
		AppCall::Secretary_Message_Box("CoferenceSet_mm is not empty in: CTest::Generating_Testing_Reference_Documents()");
	}
	set<string> TestDoc_s;
	for(size_t i = 0; i < RefTest_v.size(); i++){
		TestDoc_s.insert(RefTest_v[i]);
	}
	for(map<string, ACE_entity>::iterator mite = pmACE_Corpus.ACE_Entity_Info_map.begin();  mite != pmACE_Corpus.ACE_Entity_Info_map.end(); mite++){
		if(TestDoc_s.find(mite->second.DOCID) == TestDoc_s.end()){
			continue;
		}
		map<string, set<ACE_entity_mention*>*>& loc_EntitySet_m = CoferenceSet_mm[mite->second.DOCID];
		set<ACE_entity_mention*>* pMentionSet = new set<ACE_entity_mention*>;
		loc_EntitySet_m.insert(make_pair(mite->first, pMentionSet));
		for(vector<ACE_entity_mention>::iterator vite = mite->second.entity_mention_v.begin(); vite != mite->second.entity_mention_v.end(); vite++){
			pMentionSet->insert(&*vite);
		}
	}

}


void CTest::Output_Mentions_Documents_List(const char* clistpath, const char* outputfolder, map<string, map<string, set<ACE_entity_mention*>*>>& CoferenceSet_mm)
{
	vector<string> outputlistpath_v;
	map<string, map<string, set<ACE_entity_mention*>*>>::iterator mmmsite = CoferenceSet_mm.begin();
	for( ; mmmsite != CoferenceSet_mm.end(); mmmsite++){
		outputlistpath_v.push_back(outputfolder + mmmsite->first + ".xml");
	}

	ofstream out(clistpath);
	if(out.bad())
		return;
	out.clear();
	out.seekp(0, ios::beg);

	for(vector<string>::iterator vite = outputlistpath_v.begin(); vite != outputlistpath_v.end(); vite++){
		out << vite->data() << endl;
	}

	out.close();
}


void CTest::Output_Coference_Mention_Set_with_Erasing(const char* parthchar, map<string, ACE_DocInfo>& DocInfo_m, map<string, map<string, set<ACE_entity_mention*>*>>& CoferenceSet_mm)
{
	string FilePath;
	map<string, map<string, set<ACE_entity_mention*>*>>::iterator mmmsite = CoferenceSet_mm.begin();
	for( ; mmmsite != CoferenceSet_mm.end(); mmmsite++){
		if(DocInfo_m.find(mmmsite->first) == DocInfo_m.end()){
			AppCall::Secretary_Message_Box("文件夹不存在 in: CTest::Output_Coference_Mention_Set_with_Erasing()");
		}
		ACE_DocInfo& loc_DocInfo = DocInfo_m[mmmsite->first];
		map<string, set<ACE_entity_mention*>*>& loc_EntitySet = mmmsite->second;
		FilePath = parthchar + mmmsite->first + ".xml";
		ofstream out(FilePath.c_str());
		if(out.bad())
			return;
		out.clear();
		out.seekp(0, ios::beg);

		//ostream.str("");
		out << "<?xml version=\"1.0\"?>" << endl;
		out << "<!DOCTYPE source_file SYSTEM \"apf.v5.1.1.dtd\">" << endl;
		out << "<source_file URI=\"" << loc_DocInfo.URI << "\" SOURCE=\"" << loc_DocInfo.SOURCE << "\" TYPE=\"" << loc_DocInfo.TYPE;
		out << "\" AUTHOR=\"" << loc_DocInfo.AUTHOR << "\" ENCODING=\"" << loc_DocInfo.ENCODING << "\">" << endl;
		out << "<document DOCID=\"" << loc_DocInfo.DOCID << "\">" << endl;
		//ASCstr = ostream.str();
		//ASCwstr = SCONVERT::StringToWString(ASCstr);
		//out << SCONVERT::ToUtf8(ASCwstr);
		for(map<string, set<ACE_entity_mention*>*>::iterator msite = loc_EntitySet.begin(); msite != loc_EntitySet.end(); msite++){
			if(msite->second->empty()){
				continue;
			}
			ACE_entity_mention& EntityInfo = **msite->second->begin();
			out << "<entity ID=\"" << msite->first << "\" TYPE=\"" << EntityInfo.Entity_TYPE << "\" SUBTYPE=\"" << EntityInfo.Entity_SUBSTYPE << "\" CLASS=\"" << EntityInfo.Entity_CLASS << "\">" << endl;
			for(set<ACE_entity_mention*>::iterator site = msite->second->begin(); site != msite->second->end(); site++){
				ACE_entity_mention& loc_mention = **site;
				out << "  <entity_mention ID=\"" << loc_mention.ID << "\" TYPE=\"" << loc_mention.TYPE <<  "\" LDCTYPE=\"" << loc_mention.LDCTYPE;
				if(!loc_mention.LDCATR.empty()){
					out << "\" LDCATR=\"" << loc_mention.LDCATR;
				}
				out << "\">" << endl;
				out << "    <extent>" << endl;
				out << "      <charseq START=\"" << loc_mention.extent.START << "\" END=\"" << loc_mention.extent.END << "\">" << loc_mention.extent.charseq << "</charseq>" << endl;
				out << "    </extent>" << endl;
				out << "    <head>" << endl;
				out << "      <charseq START=\"" << loc_mention.head.START << "\" END=\"" << loc_mention.head.END << "\">" << loc_mention.head.charseq << "</charseq>" << endl;
				out << "    </head>" << endl;
				out << "  </entity_mention>" << endl;
			}
			delete msite->second;
			out << "</entity>" << endl;
		}
		out << "</document>" << endl;
		out << "</source_file>" << endl;

		out.close();
	}
	CoferenceSet_mm.clear();


}



