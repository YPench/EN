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
#include "CCRF.h"
#include "NECom.h"
#include <algorithm>
#include "CEDT.h"
#include "ConvertUTF.h"
#include "CSegmter.h"
#include "CMaxmatch.h"
#include "NEFeator.h"

#include "..\\Consensus.h"

#define CRF_OUTPUT_FOLDER ".\\..\\..\\EXE\\CRF\\Output\\"
#define CRF_ACE_FOLDER ".\\..\\..\\EXE\\CRF\\ACE\\"
#define CRF_PFR_FOLDER ".\\..\\..\\EXE\\CRF\\PFR\\"
#define CRF_FOLDER ".\\..\\..\\EXE\\CRF\\"

extern bool CEDT_Head_Flag;
extern bool CEDT_Detection_Flag;
extern bool CEDT_Extend_Done_Flag;
extern bool CEDT_Head_Done_Flag;

extern string Detect_Single_NE_TYPE;
extern bool CRF_Max_Flag;
extern bool CRF_POS_Flag;
extern bool CRF_Merging_Max_Min_Flag;
extern int CRF_N_BEST_NUM;
extern bool CRF_START_or_END_DetBound_Flag;
extern bool CRF_Multi_TYPE_Flag;

extern bool CEDT_Word_Boundary_Flag;
//extern bool CRF_DetBound_Flag;
#define CRF_Get_Line_Cloumn 9

CCRF::CCRF()//(CSegmter& rCSegmter, CMaxmatch& rCMaxmatch, set<string>& rlexicon_set): m_CSegmter(rCSegmter)
//, m_CMaxmatch(rCMaxmatch)
//, lexicon_set(rlexicon_set)
{
	//vector<pair<string, string>> CRFResult_v;
	//Read_Result_of_Consensus_CRF_Port(".\\..\\..\\EXE\\CRF\\Output\\output.txt", CRFResult_v);
	//Performance_of_Consensus_CRF_Port(CRFResult_v);

	//PFR_Port();

	//Max_Min_Performance_Merging();

	string LexiconPath = DATA_FOLDER;
	LexiconPath += "LCWCC.dat";
	if(!NLPOP::FileExist(NLPOP::string2CString(LexiconPath))){
		AppCall::Secretary_Message_Box(LexiconPath.c_str(), MB_OK);
	}
	NLPOP::LoadWordsStringSet(LexiconPath, m_Lexicon_set);

	nCrossID = 0;
	NLPOP::Delete_Files_In_Dir(_T(CRF_ACE_FOLDER));
	//NLPOP::Delete_Files_In_Dir(_T(CRF_OUTPUT_FOLDER));
}
CCRF::~CCRF(){

	for(size_t i = 0; i < mergedPFRcorpus_v.size();i++){
		delete mergedPFRcorpus_v[i];
	}
	for(size_t i = 0; i < G_CRFResult_v.size(); i++){
		delete G_CRFResult_v[i];
	}
}
void CCRF::Changing_CRFResult_v_To_Pair_TYPE_with_Erasing(vector<pair<string, string>*>& CRFResult_v, map<size_t, map<size_t, string>>& OriginPridic_mm, bool Origin_Flag)
{
	bool Pre_BEGIN = false;
	size_t START, END;
	char getlineBuf[58];
	string BTags;
	string sResult;
	for(size_t i = 0; i < CRFResult_v.size(); i++){
		if(Origin_Flag){
			sResult = CRFResult_v[i]->first;
		}
		else{
			sResult = CRFResult_v[i]->second;
		}
		if(sResult.c_str()[0] == 'B'){
			if(Pre_BEGIN){
				OriginPridic_mm[START];
				OriginPridic_mm[START].insert(make_pair(END, BTags));
			}
			istringstream instream(sResult.c_str());
			instream.getline(getlineBuf, 58, '-');
			instream >> BTags;
			Pre_BEGIN = true;
			START = i;
		}
		else if(sResult.c_str()[0] == 'O'){
			if(Pre_BEGIN){
				OriginPridic_mm[START];
				OriginPridic_mm[START].insert(make_pair(END, BTags));
			}
			Pre_BEGIN = false;
		}
		END = i;
	}
}
void CCRF::Collecting_CRF_n_Cross_Result_for_Inside_out_with_Erasing()
{
	vector<pair<string, string>*> Out_CRFResult_v;
	vector<pair<string, string>*> In_CRFResult_v;

	string locstr = "F:\\YPench\\ConsensusGraph\\EXE\\CRF\\Inside\\";
	ostringstream parthstream;
	parthstream << locstr << "Inside_out.txt";
	Read_CRF_n_Cross_Result_for_Inside_out_in_with_Erasing(parthstream.str().c_str(), Out_CRFResult_v);

	Performance_of_Consensus_CRF_Port_with_Multi_TYPE_Tags(Out_CRFResult_v);

	parthstream.str("");
	parthstream << locstr << "Inside_in.txt";
	Read_CRF_n_Cross_Result_for_Inside_out_in_with_Erasing(parthstream.str().c_str(), In_CRFResult_v);

	map<size_t, map<size_t, string>> out_Origin_mm;
	map<size_t, map<size_t, string>> out_Pridic_mm;

	map<size_t, map<size_t, string>> in_Origin_mm;
	map<size_t, map<size_t, string>> in_Pridic_mm;

	Changing_CRFResult_v_To_Pair_TYPE_with_Erasing(Out_CRFResult_v, out_Origin_mm, true);
	Changing_CRFResult_v_To_Pair_TYPE_with_Erasing(Out_CRFResult_v, out_Pridic_mm, false);

	Changing_CRFResult_v_To_Pair_TYPE_with_Erasing(In_CRFResult_v, in_Origin_mm, true);
	Changing_CRFResult_v_To_Pair_TYPE_with_Erasing(In_CRFResult_v, in_Pridic_mm, false);

	map<string, pair<size_t, size_t>> Recall_map;
	map<string, pair<size_t, size_t>> Pricision_map;

	string PredTag, OriginTag; 

	for(map<size_t, map<size_t, string>>::iterator in_mmite = in_Origin_mm.begin(); in_mmite != in_Origin_mm.end(); ){
		for(map<size_t, string>::iterator in_mite = in_mmite->second.begin(); in_mite != in_mmite->second.end(); ){
			if(out_Origin_mm.find(in_mmite->first) == out_Origin_mm.end()){
				out_Origin_mm[in_mmite->first];
			}
			if(out_Origin_mm[in_mmite->first].find(in_mite->first) == out_Origin_mm[in_mmite->first].end()){
				out_Origin_mm[in_mmite->first].insert(make_pair(in_mite->first, in_mite->second));
			}
			in_mite = in_mmite->second.erase(in_mite);
		}
		in_mmite = in_Origin_mm.erase(in_mmite);
	}

	for(map<size_t, map<size_t, string>>::iterator in_mmite = in_Pridic_mm.begin(); in_mmite != in_Pridic_mm.end(); ){
		for(map<size_t, string>::iterator in_mite = in_mmite->second.begin(); in_mite != in_mmite->second.end(); ){

			bool Detected_Nested_Flag = false;
			for(map<size_t, map<size_t, string>>::iterator out_mmite = out_Pridic_mm.begin(); out_mmite != out_Pridic_mm.end(); out_mmite++){
				if(in_mmite->first < out_mmite->first){
					continue;
				}
				for(map<size_t, string>::iterator out_mite = out_mmite->second.begin(); out_mite != out_mmite->second.end(); out_mite++){
					if(in_mite->first > out_mite->first){
						break;
					}
					if(out_Pridic_mm.find(in_mmite->first) == out_Pridic_mm.end()){
						out_Pridic_mm[in_mmite->first];
					}
					if(out_Pridic_mm[in_mmite->first].find(in_mite->first) == out_Pridic_mm[in_mmite->first].end()){
						out_Pridic_mm[in_mmite->first].insert(make_pair(in_mite->first, in_mite->second));
					}
					Detected_Nested_Flag = true;
					break;
				}
				if(Detected_Nested_Flag){
					break;
				}
			}
			in_mite = in_mmite->second.erase(in_mite);
		}
		in_mmite = in_Pridic_mm.erase(in_mmite);
	}

	for(map<size_t, map<size_t, string>>::iterator mmite = out_Pridic_mm.begin(); mmite != out_Pridic_mm.end(); mmite++){
		for(map<size_t, string>::iterator mite = mmite->second.begin(); mite != mmite->second.end(); mite++){ 
			
			if(out_Origin_mm.find(mmite->first) != out_Origin_mm.end()){
				if(out_Origin_mm[mmite->first].find(mite->first) != out_Origin_mm[mmite->first].end()){
					
					PredTag = out_Pridic_mm[mmite->first][mite->first];
					OriginTag = out_Origin_mm[mmite->first][mite->first];
					
					if(!strcmp(PredTag.c_str(), OriginTag.c_str())){
						if(Recall_map.find(OriginTag) == Recall_map.end()){
							Recall_map.insert(make_pair(OriginTag, make_pair(0, 0)));
							Pricision_map.insert(make_pair(OriginTag, make_pair(0, 0)));
						}
						Recall_map[OriginTag].first++;
						Pricision_map[OriginTag].first++;
					}
				}
				else{
					bool error_Flag = true;
				}
			}
			else{
				bool error_Flag = true;
			}
		}
	}

	for(map<size_t, map<size_t, string>>::iterator mmite = out_Pridic_mm.begin(); mmite != out_Pridic_mm.end(); mmite++){
		for(map<size_t, string>::iterator mite = mmite->second.begin(); mite != mmite->second.end(); mite++){
			PredTag = mite->second;
			if(Pricision_map.find(PredTag) == Pricision_map.end()){
				Pricision_map.insert(make_pair(PredTag, make_pair(0, 0)));
			}
			Pricision_map[PredTag].second++;
		}
	}

	for(map<size_t, map<size_t, string>>::iterator mmite = out_Origin_mm.begin(); mmite != out_Origin_mm.end(); mmite++){
		for(map<size_t, string>::iterator mite = mmite->second.begin(); mite != mmite->second.end(); mite++){
			OriginTag = mite->second;
			if(Recall_map.find(OriginTag) == Recall_map.end()){
				Recall_map.insert(make_pair(OriginTag, make_pair(0, 0)));
			}
			Recall_map[OriginTag].second++;
		}
	}

	MAXEN::Display_Performance_for_MAXEN(true, true, Pricision_map, Recall_map, "\n****************************************\n\nThe n Cross Performance...\n");
	
}
void CCRF::Collecting_CRF_n_Cross_Result_for_Inside_in_with_Erasing()
{
	vector<pair<string, string>*> Out_CRFResult_v;
	vector<pair<string, string>*> In_CRFResult_v;

	string locstr = "F:\\YPench\\ConsensusGraph\\EXE\\CRF\\Inside\\";
	ostringstream parthstream;
	parthstream << locstr << "Inside_out.txt";
	Read_CRF_n_Cross_Result_for_Inside_out_in_with_Erasing(parthstream.str().c_str(), Out_CRFResult_v);

	parthstream.str("");
	parthstream << locstr << "Inside_in.txt";
	Read_CRF_n_Cross_Result_for_Inside_out_in_with_Erasing(parthstream.str().c_str(), In_CRFResult_v);

	
	Performance_of_Consensus_CRF_Port_with_Multi_TYPE_Tags(In_CRFResult_v);

	map<size_t, map<size_t, string>> out_Origin_mm;
	map<size_t, map<size_t, string>> out_Pridic_mm;

	map<size_t, map<size_t, string>> in_Origin_mm;
	map<size_t, map<size_t, string>> in_Pridic_mm;

	Changing_CRFResult_v_To_Pair_TYPE_with_Erasing(Out_CRFResult_v, out_Origin_mm, true);
	Changing_CRFResult_v_To_Pair_TYPE_with_Erasing(Out_CRFResult_v, out_Pridic_mm, false);

	Changing_CRFResult_v_To_Pair_TYPE_with_Erasing(In_CRFResult_v, in_Origin_mm, true);
	Changing_CRFResult_v_To_Pair_TYPE_with_Erasing(In_CRFResult_v, in_Pridic_mm, false);

	map<string, pair<size_t, size_t>> Recall_map;
	map<string, pair<size_t, size_t>> Pricision_map;

	string PredTag, OriginTag; 

	for(map<size_t, map<size_t, string>>::iterator out_mmite = out_Origin_mm.begin(); out_mmite != out_Origin_mm.end(); ){
		for(map<size_t, string>::iterator out_mite = out_mmite->second.begin(); out_mite != out_mmite->second.end(); ){
			if(in_Origin_mm.find(out_mmite->first) == in_Origin_mm.end()){
				in_Origin_mm[out_mmite->first];
			}
			if(in_Origin_mm[out_mmite->first].find(out_mite->first) == in_Origin_mm[out_mmite->first].end()){
				in_Origin_mm[out_mmite->first].insert(make_pair(out_mite->first, out_mite->second));
			}
			out_mite = out_mmite->second.erase(out_mite);
		}
		out_mmite = out_Origin_mm.erase(out_mmite);
	}

	for(map<size_t, map<size_t, string>>::iterator out_mmite = out_Pridic_mm.begin(); out_mmite != out_Pridic_mm.end(); ){
		for(map<size_t, string>::iterator out_mite = out_mmite->second.begin(); out_mite != out_mmite->second.end(); ){

			bool Detected_Nested_Flag = false;
			for(map<size_t, map<size_t, string>>::iterator in_mmite = in_Pridic_mm.begin(); in_mmite != in_Pridic_mm.end(); in_mmite++){

				for(map<size_t, string>::iterator in_mite = in_mmite->second.begin(); in_mite != in_mmite->second.end(); in_mite++){
					if(out_mmite->first <= in_mmite->first && out_mite->first >= in_mite->first){
						if(in_Pridic_mm.find(out_mmite->first) == in_Pridic_mm.end()){
							in_Pridic_mm[out_mmite->first];
						}
						if(in_Pridic_mm[out_mmite->first].find(out_mite->first) == in_Pridic_mm[out_mmite->first].end()){
							in_Pridic_mm[out_mmite->first].insert(make_pair(out_mite->first, out_mite->second));
						}
						Detected_Nested_Flag = true;
						break;
					}
				}
				if(Detected_Nested_Flag){
					break;
				}
			}
			out_mite = out_mmite->second.erase(out_mite);
		}
		out_mmite = out_Pridic_mm.erase(out_mmite);
	}

	for(map<size_t, map<size_t, string>>::iterator mmite = in_Pridic_mm.begin(); mmite != in_Pridic_mm.end(); mmite++){
		for(map<size_t, string>::iterator mite = mmite->second.begin(); mite != mmite->second.end(); mite++){ 
			if(in_Origin_mm.find(mmite->first) != in_Origin_mm.end()){
				if(in_Origin_mm[mmite->first].find(mite->first) != in_Origin_mm[mmite->first].end()){
					PredTag = in_Pridic_mm[mmite->first][mite->first];
					OriginTag = in_Origin_mm[mmite->first][mite->first];
					if(!strcmp(PredTag.c_str(), OriginTag.c_str())){
						if(Recall_map.find(OriginTag) == Recall_map.end()){
							Recall_map.insert(make_pair(OriginTag, make_pair(0, 0)));
							Pricision_map.insert(make_pair(OriginTag, make_pair(0, 0)));
						}
						Recall_map[OriginTag].first++;
						Pricision_map[OriginTag].first++;
					}
				}
			}
		}
	}

	for(map<size_t, map<size_t, string>>::iterator mmite = in_Pridic_mm.begin(); mmite != in_Pridic_mm.end(); mmite++){
		for(map<size_t, string>::iterator mite = mmite->second.begin(); mite != mmite->second.end(); mite++){
			PredTag = mite->second;
			if(Pricision_map.find(PredTag) == Pricision_map.end()){
				Pricision_map.insert(make_pair(PredTag, make_pair(0, 0)));
			}
			Pricision_map[PredTag].second++;
		}
	}

	for(map<size_t, map<size_t, string>>::iterator mmite = in_Origin_mm.begin(); mmite != in_Origin_mm.end(); mmite++){
		for(map<size_t, string>::iterator mite = mmite->second.begin(); mite != mmite->second.end(); mite++){
			OriginTag = mite->second;
			if(Recall_map.find(OriginTag) == Recall_map.end()){
				Recall_map.insert(make_pair(OriginTag, make_pair(0, 0)));
			}
			Recall_map[OriginTag].second++;
		}
	}

	MAXEN::Display_Performance_for_MAXEN(true, true, Pricision_map, Recall_map, "\n****************************************\n\nThe n Cross Performance...\n");
	
}

void CCRF::Read_CRF_n_Cross_Result_for_Inside_out_in_with_Erasing(const char* path, vector<pair<string, string>*>& CRFResult_v)
{
	char getlineBuf[MAX_CLAUSE_LENGTH];
	ifstream in(path);
	if(in.bad()){
		in.close();
		return ;
	}
	in.clear();
	in.seekg(0, ios::beg);

	while(in.peek() != EOF){
		pair<string, string>* pLocPair = new pair<string, string>;
		in.getline(getlineBuf, MAX_CLAUSE_LENGTH,'\n');
		istringstream instream(getlineBuf);
		instream.getline(getlineBuf, MAX_CLAUSE_LENGTH, '\t');
		pLocPair->first = getlineBuf;
		instream.getline(getlineBuf, MAX_CLAUSE_LENGTH, '\n');
		pLocPair->second = getlineBuf;
		CRFResult_v.push_back(pLocPair);
	} 

	in.close();
}


void CCRF::Consensus_CRF_Port(vector<NE_Surround*>& training_Surround_v, vector<NE_Surround*>& testing_Surround_v)
{
	ostringstream outputFilestream;
	ostringstream inputFilestream;
	ostringstream parameterstream;
	//-------Training
	inputFilestream << CRF_ACE_FOLDER << nCrossID << "_" << "Training.txt";
	//Output_NE_Surround_for_CRF_Routine_Sequence(inputFilestream.str().c_str(), training_Surround_v, false);
	Output_NE_Surround_for_CRF_with_Local_Feature(inputFilestream.str().c_str(), training_Surround_v);

	string Templatestr = (CRF_POS_Flag || CEDT_Word_Boundary_Flag)?"templatePOS":"template";
	parameterstream << Templatestr.c_str() << " " << inputFilestream.str() << " " << "ACE\\ACE_crf_Model" << "_" << nCrossID;
	CRF_Training_Port(parameterstream.str().c_str(), inputFilestream.str().c_str());

	//-------Testing
	outputFilestream.str("");
	inputFilestream.str("");
	parameterstream.str("");
	inputFilestream << CRF_ACE_FOLDER << nCrossID << "_" << "Testing.txt";
	//Output_NE_Surround_for_CRF_Routine_Sequence(inputFilestream.str().c_str(), testing_Surround_v, false);
	Output_NE_Surround_for_CRF_with_Local_Feature(inputFilestream.str().c_str(), testing_Surround_v);

	parameterstream << CRF_FOLDER << "crf_test.exe" << " " << "-m ACE\\ACE_crf_Model_" << nCrossID << " " << inputFilestream.str().c_str();
	outputFilestream << CRF_OUTPUT_FOLDER << "\\output.txt";
	AppCall::Console_Running_with_Output_Redirection(CRF_FOLDER, parameterstream.str().c_str(), outputFilestream.str().c_str());

	vector<pair<string, string>*> CRFResult_v;
	Read_Result_of_Consensus_CRF_Port(outputFilestream.str().c_str(), CRFResult_v);
	if(CRF_Multi_TYPE_Flag){
		Performance_of_Consensus_CRF_Port_with_Multi_TYPE_Tags(CRFResult_v);
	}
	else{
		Performance_of_Consensus_CRF_Port(CRFResult_v);
	}
	nCrossID++;

	if(CRF_Merging_Max_Min_Flag || CRF_Multi_TYPE_Flag){
		for(size_t i = 0; i < CRFResult_v.size(); i++){
			G_CRFResult_v.push_back(CRFResult_v[i]);
		}
		CRFResult_v.clear();
	}
}

void CCRF::Output_CRF_n_Cross_Result_for_Inside_out_in_with_Erasing(){

	ostringstream parthstream;
	parthstream << CRF_OUTPUT_FOLDER;
	if(CRF_Max_Flag){
		parthstream << "\\Inside_out.txt";
	}
	else{
		parthstream << "\\Inside_in.txt";
	}
	ofstream out(parthstream.str().c_str());
	if(out.bad()){
		return;
	}
	out.clear();
	out.seekp(0, ios::beg);
	for(size_t i = 0; i < G_CRFResult_v.size(); i++){
		pair<string, string>& refpair = *G_CRFResult_v[i];
		out << refpair.first << '\t' << refpair.second << endl;
		delete G_CRFResult_v[i];
	}
	out.close();
	G_CRFResult_v.clear();
}



void CCRF::CRF_Generate_Routine_and_Reverse_Sequence_Candit_Port(vector<NE_Surround*>& training_Surround_v, vector<NE_Surround*>& testing_Surround_v, 
										CEDTInfo& m_TrainingCEDTInfo, CEDTInfo& m_TestingCEDTInfo)
{
	ostringstream outputFilestream;
	ostringstream inputFilestream;
	ostringstream parameterstream;
	map<size_t, map<size_t, map<size_t, string>>*> Candits_mm;
	string Templatestr;

	//----------------------------------Routine Detection
	inputFilestream << CRF_ACE_FOLDER << "Routine_Training_" << nCrossID;
	Templatestr = (CRF_POS_Flag || CEDT_Word_Boundary_Flag)?"templatePOS":"template";
	parameterstream << Templatestr.c_str() << " " << inputFilestream.str() << " " << "ACE\\Routine_Model_" << nCrossID;

	//Output_NE_Surround_for_CRF_Routine_Sequence(inputFilestream.str().c_str(), training_Surround_v, false);
	CRF_Training_Port(parameterstream.str().c_str(), inputFilestream.str().c_str());
	
	//------------------------------------------
	inputFilestream.str("");
	outputFilestream.str("");
	parameterstream.str("");
	inputFilestream << CRF_ACE_FOLDER << "Routine_Testing_" << nCrossID;
	outputFilestream << CRF_OUTPUT_FOLDER << "Routine_TestingEval_" << nCrossID;
	parameterstream << CRF_FOLDER << "crf_test.exe" << " " << "-m ACE\\Routine_Model_" << nCrossID << " " << inputFilestream.str().c_str();
	
	//Output_NE_Surround_for_CRF_Routine_Sequence(inputFilestream.str().c_str(), testing_Surround_v, false);
	AppCall::Console_Running_with_Output_Redirection(CRF_FOLDER, parameterstream.str().c_str(), outputFilestream.str().c_str());
	
	//---Read Result
	Read_CRF_Routine_Resusts_as_SequenceMap(outputFilestream.str().c_str(), Candits_mm);

	//----------------------------------Reverse Detection
	inputFilestream.str("");
	outputFilestream.str("");
	parameterstream.str("");

	inputFilestream << CRF_ACE_FOLDER  << "Reverse_Training_" << nCrossID;
	outputFilestream << CRF_OUTPUT_FOLDER << "Reverse_TraingEval_" << nCrossID;
	Templatestr = (CRF_POS_Flag || CEDT_Word_Boundary_Flag)?"templatePOS":"template";
	parameterstream << Templatestr.c_str() << " " << inputFilestream.str() << " " << "ACE\\Reverse_Model_" << nCrossID;
	
	//---Training
	//Output_NE_Surround_for_CRF_Reverse_Sequence(inputFilestream.str().c_str(), training_Surround_v, false);
	Output_NE_Surround_for_CRF_Reverse_with_Local_Feature(inputFilestream.str().c_str(), training_Surround_v);
	CRF_Training_Port(parameterstream.str().c_str(), inputFilestream.str().c_str());

	//----------------------------------Reverse Test Data Detection
	inputFilestream.str("");
	outputFilestream.str("");
	parameterstream.str("");

	inputFilestream << CRF_ACE_FOLDER << "Reverse_Testing_" << nCrossID;
	outputFilestream << CRF_OUTPUT_FOLDER << "Reverse_TestingEval_" << nCrossID;
	parameterstream << CRF_FOLDER << "crf_test.exe" << " " << "-m ACE\\Reverse_Model_" << nCrossID << " " << inputFilestream.str().c_str();
	
	//Output_NE_Surround_for_CRF_Reverse_Sequence(inputFilestream.str().c_str(), testing_Surround_v, false);
	Output_NE_Surround_for_CRF_Reverse_with_Local_Feature(inputFilestream.str().c_str(), testing_Surround_v);
	AppCall::Console_Running_with_Output_Redirection(CRF_FOLDER, parameterstream.str().c_str(), outputFilestream.str().c_str());
	
	//---Read Result
	Read_CRF_Reverse_Candits_CEDTInfo(outputFilestream.str().c_str(), Candits_mm);

	Generate_CRF_Candits_From_SequenceMap_with_Erasing(Candits_mm, testing_Surround_v, m_TestingCEDTInfo);

	AppCall::Maxen_Responce_Message("CRF Candidate Testing Cases Information...\n");
	map<string, size_t> Positive_Mentions;
	NECOM::Positvie_Mention_Collection(testing_Surround_v, Positive_Mentions, CEDT_Detection_Flag?true:false);
	NECOM::Generated_Candidate_Testing_Cases_Information(Positive_Mentions, CEDT_Head_Flag?m_TestingCEDTInfo.CanditHead_v:m_TestingCEDTInfo.CanditExtend_v, ProcesedCasesInfor_m);

	nCrossID++;
}
void CCRF::Read_CRF_Reverse_Candits_CEDTInfo(const char* outputpath, map<size_t, map<size_t, map<size_t, string>>*>& Candits_mm)
{
	map<size_t, map<size_t, map<size_t, string>>*> ReverseCandits_mm;
	map<size_t, size_t> SentenceLength_m;
	size_t SentenceID = 0;
	size_t charID = 0;
	string character;
	string mentionstr;
	size_t START;
	size_t END;
	bool BEGIN_Detected_Flag;
	bool Pri_BEGIN_Detected_Flag;
	char getlineBuf[MAX_SENTENCE];
	map<size_t, map<size_t, string>>*  pCurLocCandit_m = NULL;

	ifstream in(outputpath);
	if(in.bad())
		return;
	in.clear();
	in.seekg(0, ios::beg);
	if(in.peek() == EOF){
		in.close();
		return;
	}
	while(true){
		if(in.peek() == EOF){	
			break;
		}
		if(in.peek() == '\n'){
			in.getline(getlineBuf, MAX_SENTENCE, '\n');
			continue;
		}

		BEGIN_Detected_Flag = false;
		Pri_BEGIN_Detected_Flag = false;
		START = 0;
		END = 0;
		charID = 0;

		pCurLocCandit_m = new map<size_t, map<size_t, string>>;
		ReverseCandits_mm.insert(make_pair(SentenceID, pCurLocCandit_m));
		BEGIN_Detected_Flag = false;
		Pri_BEGIN_Detected_Flag = false;
		mentionstr = "";

		map<size_t, map<size_t, string>>& locCandit_m = *pCurLocCandit_m;
		while(in.peek() != EOF){
			in.getline(getlineBuf, MAX_SENTENCE, '\n');
			if(strlen(getlineBuf) == 0){
				if(BEGIN_Detected_Flag){
					END = charID-1;
					if(locCandit_m.find(START) == locCandit_m.end()){
						locCandit_m[START];
					}
					if(locCandit_m[START].find(END) == locCandit_m[START].end()){
						locCandit_m[START].insert(make_pair(END, mentionstr));
					}
					else if(strcmp(locCandit_m[START][END].c_str(), mentionstr.c_str())){
						AppCall::Secretary_Message_Box("CCRF::Read_CRF_nBest_Candit_Info() 2", MB_OK);
					}
					mentionstr = "";
				}
				SentenceLength_m.insert(make_pair(SentenceID, charID));
				break;
			}
			istringstream instream(getlineBuf);
			instream.getline(getlineBuf, MAX_SENTENCE,'\t');
			character = getlineBuf;
			if(CRF_POS_Flag || CEDT_Word_Boundary_Flag){
				instream.getline(getlineBuf, MAX_SENTENCE,'\t');
			}
			instream.getline(getlineBuf, MAX_SENTENCE,'\t');
			instream.getline(getlineBuf, MAX_SENTENCE,'\t');
			if(!strcmp(getlineBuf, "B")){
				if(Pri_BEGIN_Detected_Flag){
					END = charID-1;
					if(locCandit_m.find(START) == locCandit_m.end()){
						locCandit_m[START];
					}
					if(locCandit_m[START].find(END) == locCandit_m[START].end()){
						locCandit_m[START].insert(make_pair(END, mentionstr));
					}
					else if(strcmp(locCandit_m[START][END].c_str(), mentionstr.c_str())){
						AppCall::Secretary_Message_Box("CCRF::Read_CRF_nBest_Candit_Info() 3", MB_OK);
					}
					mentionstr = "";
				}
				START = charID;
				BEGIN_Detected_Flag = true;
			}
			else if(!strcmp(getlineBuf, "O")){
				if(BEGIN_Detected_Flag){
					END = charID-1;
					if(locCandit_m.find(START) == locCandit_m.end()){
						locCandit_m[START];
					}
					if(locCandit_m[START].find(END) == locCandit_m[START].end()){
						locCandit_m[START].insert(make_pair(END, mentionstr));
					}
					else if(strcmp(locCandit_m[START][END].c_str(), mentionstr.c_str())){
						AppCall::Secretary_Message_Box("CCRF::Read_CRF_nBest_Candit_Info() 4", MB_OK);
					}
					mentionstr = "";
				}
				BEGIN_Detected_Flag = false;
			}
			if(BEGIN_Detected_Flag){
				mentionstr += character;
			}
			Pri_BEGIN_Detected_Flag = BEGIN_Detected_Flag;
			charID++;
		}
		SentenceID++;
	}
	in.close();

	//map<size_t, map<size_t, map<size_t, string>>*> ReverseCandits_mm;
	//map<size_t, size_t> SentenceLength_m;
	//Candits_mm
	for(map<size_t, map<size_t, map<size_t, string>>*>::iterator Sentmmite = ReverseCandits_mm.begin(); Sentmmite != ReverseCandits_mm.end(); Sentmmite++){
		if(Candits_mm.find(Sentmmite->first) == Candits_mm.end()){
			//AppCall::Secretary_Message_Box("Data Error in: CCRF::Read_CRF_Reverse_Candits_CEDTInfo()", MB_OK);
			Candits_mm.insert(make_pair(Sentmmite->first, new map<size_t, map<size_t, string>>));
		}
		map<size_t, map<size_t, string>>& ReverseSentence_m = *(Sentmmite->second);
		map<size_t, map<size_t, string>>& RoutineSentence_m = *Candits_mm[Sentmmite->first];
		for(map<size_t, map<size_t, string>>::iterator mmite = ReverseSentence_m.begin(); mmite != ReverseSentence_m.end(); mmite++){
			for(map<size_t, string>::iterator mite = mmite->second.begin(); mite != mmite->second.end(); mite++){
				int START = SentenceLength_m[Sentmmite->first]-mite->first-1;
				int END = SentenceLength_m[Sentmmite->first]-mmite->first-1;
				if(START < 0 || END < 0){
					AppCall::Secretary_Message_Box("Data Error in: CCRF::Read_CRF_Reverse_Candits_CEDTInfo()", MB_OK);
				}
				vector<string> mention_v;
				NLPOP::charseq_to_vector(mite->second.c_str(), mention_v);
				string mentionstr;
				for(vector<string>::reverse_iterator rvite = mention_v.rbegin(); rvite != mention_v.rend(); rvite++){
					mentionstr += rvite->data();
				}

				if(RoutineSentence_m.find(START) == RoutineSentence_m.end()){
					RoutineSentence_m[START];
				}
				if(RoutineSentence_m[START].find(END) == RoutineSentence_m[START].end()){
					RoutineSentence_m[START].insert(make_pair(END, mentionstr));
				}
				else if(strcmp(RoutineSentence_m[START][END].c_str(), mentionstr.c_str())){
					AppCall::Secretary_Message_Box("Data Error in: CCRF::Read_CRF_Reverse_Candits_CEDTInfo()", MB_OK);
				}
			}
		}
		delete Sentmmite->second;
	}
}

void CCRF::Read_CRF_Routine_Resusts_as_SequenceMap(const char* outputpath, map<size_t, map<size_t, map<size_t, string>>*>& Candits_mm)
{
	size_t SentenceID = 0;
	size_t charID = 0;
	string character;
	string mentionstr;
	size_t START;
	size_t END;
	bool BEGIN_Detected_Flag;
	bool Pri_BEGIN_Detected_Flag;
	char getlineBuf[MAX_SENTENCE];
	map<size_t, map<size_t, string>>*  pCurLocCandit_m = NULL;

	ifstream in(outputpath);
	if(in.bad())
		return;
	in.clear();
	in.seekg(0, ios::beg);
	if(in.peek() == EOF){
		in.close();
		return;
	}
	while(true){
		if(in.peek() == EOF){	
			break;
		}
		if(in.peek() == '\n'){
			in.getline(getlineBuf, MAX_SENTENCE, '\n');
			continue;
		}

		BEGIN_Detected_Flag = false;
		Pri_BEGIN_Detected_Flag = false;
		START = 0;
		END = 0;
		charID = 0;

		pCurLocCandit_m = new map<size_t, map<size_t, string>>;
		Candits_mm.insert(make_pair(SentenceID++, pCurLocCandit_m));
		BEGIN_Detected_Flag = false;
		Pri_BEGIN_Detected_Flag = false;
		mentionstr = "";

		map<size_t, map<size_t, string>>& locCandit_m = *pCurLocCandit_m;
		while(in.peek() != EOF){
			in.getline(getlineBuf, MAX_SENTENCE, '\n');
			if(strlen(getlineBuf) == 0){
				if(BEGIN_Detected_Flag){
					END = charID-1;
					if(locCandit_m.find(START) == locCandit_m.end()){
						locCandit_m[START];
					}
					if(locCandit_m[START].find(END) == locCandit_m[START].end()){
						locCandit_m[START].insert(make_pair(END, mentionstr));
					}
					else if(strcmp(locCandit_m[START][END].c_str(), mentionstr.c_str())){
						AppCall::Secretary_Message_Box("CCRF::Read_CRF_nBest_Candit_Info() 2", MB_OK);
					}
					mentionstr = "";
				}
				break;
			}
			istringstream instream(getlineBuf);
			instream.getline(getlineBuf, MAX_SENTENCE,'\t');
			character = getlineBuf;
			if(CRF_POS_Flag || CEDT_Word_Boundary_Flag){
				instream.getline(getlineBuf, MAX_SENTENCE,'\t');
			}
			instream.getline(getlineBuf, MAX_SENTENCE,'\t');
			instream.getline(getlineBuf, MAX_SENTENCE,'\t');
			if(!strcmp(getlineBuf, "B")){
				if(Pri_BEGIN_Detected_Flag){
					END = charID-1;
					if(locCandit_m.find(START) == locCandit_m.end()){
						locCandit_m[START];
					}
					if(locCandit_m[START].find(END) == locCandit_m[START].end()){
						locCandit_m[START].insert(make_pair(END, mentionstr));
					}
					else if(strcmp(locCandit_m[START][END].c_str(), mentionstr.c_str())){
						AppCall::Secretary_Message_Box("CCRF::Read_CRF_nBest_Candit_Info() 3", MB_OK);
					}
					mentionstr = "";
				}
				START = charID;
				BEGIN_Detected_Flag = true;
			}
			else if(!strcmp(getlineBuf, "O")){
				if(BEGIN_Detected_Flag){
					END = charID-1;
					if(locCandit_m.find(START) == locCandit_m.end()){
						locCandit_m[START];
					}
					if(locCandit_m[START].find(END) == locCandit_m[START].end()){
						locCandit_m[START].insert(make_pair(END, mentionstr));
					}
					else if(strcmp(locCandit_m[START][END].c_str(), mentionstr.c_str())){
						AppCall::Secretary_Message_Box("CCRF::Read_CRF_nBest_Candit_Info() 4", MB_OK);
					}
					mentionstr = "";
				}
				BEGIN_Detected_Flag = false;
			}
			if(BEGIN_Detected_Flag){
				mentionstr += character;
			}
			Pri_BEGIN_Detected_Flag = BEGIN_Detected_Flag;
			charID++;
		}
	}
	in.close();
}
void CCRF::Generate_CRF_Candits_From_SequenceMap_with_Erasing(map<size_t, map<size_t, map<size_t, string>>*>& Candits_mm, vector<NE_Surround*>& Surround_v, CEDTInfo& pmCEDTInfo)
{

	vector<CanditCase*>& Candit_v = CEDT_Head_Flag?pmCEDTInfo.CanditHead_v:pmCEDTInfo.CanditExtend_v;

	map<size_t, map<size_t, map<size_t, string>>*>::iterator canditmite = Candits_mm.begin();
	size_t CASID = 0;
	for(vector<NE_Surround*>::iterator ne_vite = Surround_v.begin(); ne_vite != Surround_v.end(); ne_vite++){
		size_t SENID = (*ne_vite)->SENID;
		vector<string> character_v;
		string& insurRef = (*ne_vite)->insur;
		const char* inputchar = insurRef.c_str();
		size_t length = NLPOP::Get_Chinese_Sentence_Length_Counter(inputchar);

		string inputstr = ace_op::Delet_0AH_and_20H_in_string(insurRef.c_str());
		if(inputstr.length() == 0){
			continue;
		}

		NLPOP::charseq_to_vector(insurRef.c_str(), character_v);

		if(canditmite == Candits_mm.end()){
			AppCall::Secretary_Message_Box("Data Error in: CCRF::Read_CRF_nBest_Candit_Info()\ncanditmite == Candits_mm.end()", MB_OK);
		}

		map<size_t, map<size_t, string>>& locMentions_m = *canditmite->second;
		map<size_t, size_t> RevizeMap;
		size_t charID = 0;
		for(size_t i = 0; i < character_v.size(); i++){
			if(!strcmp(character_v[i].c_str(), "\n") || !strcmp(character_v[i].c_str(), " ")){
				continue;
			}
			RevizeMap.insert(make_pair(charID++, i));
		}
		set<size_t> START_s;
		set<size_t> END_s;
		map<size_t, map<size_t, ACE_entity_mention*>> EntityMention_mm;
		if(CEDT_Head_Flag){
			ace_op::Get_Entity_Mention_head_Map((*ne_vite)->entity_mention_v, EntityMention_mm, START_s, END_s);
		}
		else{
			ace_op::Get_Entity_Mention_extent_Map((*ne_vite)->entity_mention_v, EntityMention_mm, START_s, END_s);
		}
		START_s.clear();
		END_s.clear();

		for(map<size_t, map<size_t, string>>::iterator mmite = locMentions_m.begin(); mmite != locMentions_m.end(); mmite++){
			for(map<size_t, string>::iterator mite = mmite->second.begin(); mite != mmite->second.end(); mite++){
				START_s.insert(RevizeMap[mmite->first]);
				END_s.insert(RevizeMap[mite->first]);
			}
		}
		for(map<size_t, map<size_t, string>>::iterator mmite = locMentions_m.begin(); mmite != locMentions_m.end(); mmite++){
			for(map<size_t, string>::iterator mite = mmite->second.begin(); mite != mmite->second.end(); mite++){

				pCanditCase ploc_Candit = new CanditCase;
				ploc_Candit->SENID = SENID;
				ploc_Candit->CASID = CASID++;
				ploc_Candit->CEDT_Head_Flag = CEDT_Head_Flag;
				ploc_Candit->START = RevizeMap[mmite->first];
				ploc_Candit->END = RevizeMap[mite->first];

				int Left_Out_Range = NECOM::Get_Left_Outer_Feature_Range_For_START(START_s, END_s, ploc_Candit->START, -1);
				int Right_Out_Range = NECOM::Get_Right_Outer_Feature_Range_For_END(START_s, END_s, ploc_Candit->END, length-1);
				ploc_Candit->l_outmstr = Sentop::Get_Substr_by_Chinese_Character_Cnt(inputchar, Left_Out_Range+1, ploc_Candit->START-Left_Out_Range-1);
				ploc_Candit->r_outmstr = Sentop::Get_Substr_by_Chinese_Character_Cnt(inputchar, ploc_Candit->END+1, Right_Out_Range-ploc_Candit->END-1);
				
				ploc_Candit->prix = Sentop::Get_Substr_by_Chinese_Character_Cnt(inputchar, 0, ploc_Candit->START);
				ploc_Candit->prox = Sentop::Get_Substr_by_Chinese_Character_Cnt(inputchar, ploc_Candit->END+1, -1);
				ploc_Candit->mention = Sentop::Get_Substr_by_Chinese_Character_Cnt(inputchar, ploc_Candit->START, ploc_Candit->END-ploc_Candit->START+1);
				
				string CheckMention = ace_op::Delet_0AH_and_20H_in_string(ploc_Candit->mention.c_str());
				if(strcmp(CheckMention.c_str(), mite->second.c_str())){
					AppCall::Secretary_Message_Box("Data Error in: CCRF::Read_CRF_nBest_Candit_Info() 6", MB_OK);
				}

				ploc_Candit->Cand_Flag = false;
				ploc_Candit->pNE_mention = NULL;
				ploc_Candit->org_TYPE = NEGETIVE;
				if(EntityMention_mm.find(ploc_Candit->START) != EntityMention_mm.end()){
					if(EntityMention_mm[ploc_Candit->START].find(ploc_Candit->END) != EntityMention_mm[ploc_Candit->START].end()){
						ploc_Candit->Cand_Flag = true;
						ploc_Candit->pNE_mention = EntityMention_mm[ploc_Candit->START][ploc_Candit->END];
						ploc_Candit->org_TYPE = CEDT_Detection_Flag?POSITIVE:EntityMention_mm[ploc_Candit->START][ploc_Candit->END]->Entity_TYPE;
					}
				}
				ploc_Candit->predit_TYPE = POSITIVE;
				Candit_v.push_back(ploc_Candit);
			}
		}
		canditmite++;
	}
	for(map<size_t, map<size_t, map<size_t, string>>*>::iterator mite = Candits_mm.begin(); mite != Candits_mm.end(); mite++){
		delete mite->second;
	}
}






void CCRF::CRF_Generate_nBest_Candit_Port(vector<NE_Surround*>& training_Surround_v, vector<NE_Surround*>& testing_Surround_v, 
										CEDTInfo& m_TrainingCEDTInfo, CEDTInfo& m_TestingCEDTInfo)
{
	ostringstream outputFilestream;
	ostringstream inputFilestream;
	ostringstream parameterstream;

	//---Training
	inputFilestream << CRF_ACE_FOLDER << "nBest_Training_" << nCrossID;
	string Templatestr = (CRF_POS_Flag || CEDT_Word_Boundary_Flag)?"templatePOS":"template";
	parameterstream << Templatestr.c_str() << " " << inputFilestream.str() << " " << "ACE\\nBest_Model_" << nCrossID;

	//Output_NE_Surround_for_CRF_Routine_Sequence(inputFilestream.str().c_str(), training_Surround_v, false);
	Output_NE_Surround_for_CRF_with_Local_Feature(inputFilestream.str().c_str(), training_Surround_v);
	CRF_Training_Port(parameterstream.str().c_str(), inputFilestream.str().c_str());

	//---Testing
	outputFilestream.str("");
	inputFilestream.str("");
	parameterstream.str("");
	inputFilestream << CRF_ACE_FOLDER << "nBest_Testing_" << nCrossID;
	outputFilestream << CRF_OUTPUT_FOLDER << "nBest_TestingEval_" << nCrossID;
	parameterstream << CRF_FOLDER << "crf_test.exe" << " " << "-n " << CRF_N_BEST_NUM << " -m ACE\\nBest_Model_" << nCrossID << " " << inputFilestream.str().c_str();
	
	Output_NE_Surround_for_CRF_with_Local_Feature(inputFilestream.str().c_str(), testing_Surround_v);
	AppCall::Console_Running_with_Output_Redirection(CRF_FOLDER, parameterstream.str().c_str(), outputFilestream.str().c_str());
	
	//---Read Results
	Read_CRF_nBest_Candit_Info(outputFilestream.str().c_str(), testing_Surround_v, m_TestingCEDTInfo);

	nCrossID++;
}

void CCRF::CRF_Boundary_Training_and_Testing_Port(vector<NE_Surround*>& training_Surround_v, vector<NE_Surround*>& testing_Surround_v, 
										CEDTInfo& m_TrainingCEDTInfo, CEDTInfo& m_TestingCEDTInfo)
{
	ostringstream inputFilestream;
	ostringstream outputFilestream;
	ostringstream parameterstream;

	//----------------------------------START Detection
	inputFilestream << CRF_ACE_FOLDER << "START_Training_" << nCrossID;
	outputFilestream << CRF_OUTPUT_FOLDER << "START_TraingEval_" << nCrossID;
	string Templatestr = (CRF_POS_Flag || CEDT_Word_Boundary_Flag)?"templatePOS":"template";
	parameterstream << Templatestr.c_str() << " " << inputFilestream.str() << " " << "ACE\\START_Model_" << nCrossID;

	CRF_START_or_END_DetBound_Flag = true;
	Output_NE_Surround_for_START_or_END_Boundary_with_Local_Feature(inputFilestream.str().c_str(), training_Surround_v);
	CRF_Training_Port(parameterstream.str().c_str(), inputFilestream.str().c_str());
	
	//---Testing Training Data
	parameterstream.str("");
	parameterstream << CRF_FOLDER << "crf_test.exe" << " " << "-v2 -m ACE\\START_Model_" << nCrossID << " " << inputFilestream.str().c_str();
	AppCall::Console_Running_with_Output_Redirection(CRF_FOLDER, parameterstream.str().c_str(), outputFilestream.str().c_str());
	
	//---Read Result
	Read_CRF_STRAT_or_END_Boundary_CEDTInfo(outputFilestream.str().c_str(), training_Surround_v, m_TrainingCEDTInfo);

	//------------------------------------------
	inputFilestream.str("");
	outputFilestream.str("");
	parameterstream.str("");
	inputFilestream << CRF_ACE_FOLDER << "START_Testing_" << nCrossID;
	outputFilestream << CRF_OUTPUT_FOLDER << "START_TestingEval_" << nCrossID;
	parameterstream << CRF_FOLDER << "crf_test.exe" << " " << "-v2 -m ACE\\START_Model_" << nCrossID << " " << inputFilestream.str().c_str();
	
	Output_NE_Surround_for_START_or_END_Boundary_with_Local_Feature(inputFilestream.str().c_str(), testing_Surround_v);
	AppCall::Console_Running_with_Output_Redirection(CRF_FOLDER, parameterstream.str().c_str(), outputFilestream.str().c_str());
	
	//---Read Result
	Read_CRF_STRAT_or_END_Boundary_CEDTInfo(outputFilestream.str().c_str(), testing_Surround_v, m_TestingCEDTInfo);
	
	START_or_END_Boundary_Detection_Perforamnce(testing_Surround_v, m_TestingCEDTInfo);
	//----------------------------------END Detection
	inputFilestream.str("");
	outputFilestream.str("");
	parameterstream.str("");

	inputFilestream << CRF_ACE_FOLDER  << "END_Training_" << nCrossID;
	outputFilestream << CRF_OUTPUT_FOLDER << "END_TraingEval_" << nCrossID;
	Templatestr = (CRF_POS_Flag || CEDT_Word_Boundary_Flag)?"templatePOS":"template";
	parameterstream << Templatestr.c_str() << " " << inputFilestream.str() << " " << "ACE\\END_Model_" << nCrossID;
	
	CRF_START_or_END_DetBound_Flag = false;
	Output_NE_Surround_for_START_or_END_Boundary_with_Local_Feature(inputFilestream.str().c_str(), training_Surround_v);
	CRF_Training_Port(parameterstream.str().c_str(), inputFilestream.str().c_str());

	//---Testing Training Data
	parameterstream.str("");
	parameterstream << CRF_FOLDER << "crf_test.exe" << " " << "-v2 -m ACE\\END_Model_" << nCrossID << " " << inputFilestream.str().c_str();
	AppCall::Console_Running_with_Output_Redirection(CRF_FOLDER, parameterstream.str().c_str(), outputFilestream.str().c_str());
	
	//---Read Result
	Read_CRF_STRAT_or_END_Boundary_CEDTInfo(outputFilestream.str().c_str(), training_Surround_v, m_TrainingCEDTInfo);

	//----------------------------------END Test Data Detection
	inputFilestream.str("");
	outputFilestream.str("");
	parameterstream.str("");

	inputFilestream << CRF_ACE_FOLDER << "END_Testing_" << nCrossID;
	outputFilestream << CRF_OUTPUT_FOLDER << "END_TestingEval_" << nCrossID;
	parameterstream << CRF_FOLDER << "crf_test.exe" << " " << "-v2 -m ACE\\END_Model_" << nCrossID << " " << inputFilestream.str().c_str();
	
	Output_NE_Surround_for_START_or_END_Boundary_with_Local_Feature(inputFilestream.str().c_str(), testing_Surround_v);
	AppCall::Console_Running_with_Output_Redirection(CRF_FOLDER, parameterstream.str().c_str(), outputFilestream.str().c_str());
	
	//---Read Result
	Read_CRF_STRAT_or_END_Boundary_CEDTInfo(outputFilestream.str().c_str(), testing_Surround_v, m_TestingCEDTInfo);
	
	START_or_END_Boundary_Detection_Perforamnce(testing_Surround_v, m_TestingCEDTInfo);
	nCrossID++;
}
/*void CCRF::CRF_Boundary_Training_and_Testing_Port(vector<NE_Surround*>& training_Surround_v, vector<NE_Surround*>& testing_Surround_v, 
										CEDTInfo& m_TrainingCEDTInfo, CEDTInfo& m_TestingCEDTInfo)
{
	ostringstream inputFilestream;
	ostringstream outputFilestream;
	ostringstream parameterstream;

	//----------------------------------START Detection
	inputFilestream << CRF_ACE_FOLDER << "START_Training_" << nCrossID;
	outputFilestream << CRF_OUTPUT_FOLDER << "START_TraingEval_" << nCrossID;
	string Templatestr = (CRF_POS_Flag || CEDT_Word_Boundary_Flag)?"templatePOS":"template";
	parameterstream << Templatestr.c_str() << " " << inputFilestream.str() << " " << "ACE\\START_Model_" << nCrossID;

	//Output_NE_Surround_for_CRF_Routine_Sequence(inputFilestream.str().c_str(), training_Surround_v, false);
	Output_NE_Surround_for_CRF_with_Local_Feature(inputFilestream.str().c_str(), training_Surround_v);
	CRF_Training_Port(parameterstream.str().c_str(), inputFilestream.str().c_str());
	
	//---Testing Training Data
	parameterstream.str("");
	parameterstream << CRF_FOLDER << "crf_test.exe" << " " << "-v2 -m ACE\\START_Model_" << nCrossID << " " << inputFilestream.str().c_str();
	AppCall::Console_Running_with_Output_Redirection(CRF_FOLDER, parameterstream.str().c_str(), outputFilestream.str().c_str());
	
	//---Read Result
	Read_CRF_STRAT_or_END_Boundary_CEDTInfo(outputFilestream.str().c_str(), training_Surround_v, m_TrainingCEDTInfo);

	//------------------------------------------
	inputFilestream.str("");
	outputFilestream.str("");
	parameterstream.str("");
	inputFilestream << CRF_ACE_FOLDER << "START_Testing_" << nCrossID;
	outputFilestream << CRF_OUTPUT_FOLDER << "START_TestingEval_" << nCrossID;
	parameterstream << CRF_FOLDER << "crf_test.exe" << " " << "-v2 -m ACE\\START_Model_" << nCrossID << " " << inputFilestream.str().c_str();
	
	//Output_NE_Surround_for_CRF_Routine_Sequence(inputFilestream.str().c_str(), testing_Surround_v, false);
	Output_NE_Surround_for_CRF_with_Local_Feature(inputFilestream.str().c_str(), testing_Surround_v);
	AppCall::Console_Running_with_Output_Redirection(CRF_FOLDER, parameterstream.str().c_str(), outputFilestream.str().c_str());
	
	//---Read Result
	Read_CRF_STRAT_or_END_Boundary_CEDTInfo(outputFilestream.str().c_str(), testing_Surround_v, m_TestingCEDTInfo);

	//----------------------------------END Detection
	inputFilestream.str("");
	outputFilestream.str("");
	parameterstream.str("");

	inputFilestream << CRF_ACE_FOLDER  << "END_Training_" << nCrossID;
	outputFilestream << CRF_OUTPUT_FOLDER << "END_TraingEval_" << nCrossID;
	Templatestr = (CRF_POS_Flag || CEDT_Word_Boundary_Flag)?"templatePOS":"template";
	parameterstream << Templatestr.c_str() << " " << inputFilestream.str() << " " << "ACE\\END_Model_" << nCrossID;
	
	//---Testing Training Data
	//Output_NE_Surround_for_CRF_Reverse_Sequence(inputFilestream.str().c_str(), training_Surround_v, false);
	Output_NE_Surround_for_CRF_Reverse_with_Local_Feature(inputFilestream.str().c_str(), training_Surround_v);
	CRF_Training_Port(parameterstream.str().c_str(), inputFilestream.str().c_str());

	//---Testing Training Data
	parameterstream.str("");
	parameterstream << CRF_FOLDER << "crf_test.exe" << " " << "-v2 -m ACE\\END_Model_" << nCrossID << " " << inputFilestream.str().c_str();
	AppCall::Console_Running_with_Output_Redirection(CRF_FOLDER, parameterstream.str().c_str(), outputFilestream.str().c_str());
	
	//---Read Result
	Read_CRF_Reversed_END_Boundary_CEDTInfo(outputFilestream.str().c_str(), training_Surround_v, m_TrainingCEDTInfo);

	//----------------------------------END Test Data Detection
	inputFilestream.str("");
	outputFilestream.str("");
	parameterstream.str("");

	inputFilestream << CRF_ACE_FOLDER << "END_Testing_" << nCrossID;
	outputFilestream << CRF_OUTPUT_FOLDER << "END_TestingEval_" << nCrossID;
	parameterstream << CRF_FOLDER << "crf_test.exe" << " " << "-v2 -m ACE\\END_Model_" << nCrossID << " " << inputFilestream.str().c_str();
	
	//Output_NE_Surround_for_CRF_Reverse_Sequence(inputFilestream.str().c_str(), testing_Surround_v, false);
	Output_NE_Surround_for_CRF_Reverse_with_Local_Feature(inputFilestream.str().c_str(), testing_Surround_v);
	AppCall::Console_Running_with_Output_Redirection(CRF_FOLDER, parameterstream.str().c_str(), outputFilestream.str().c_str());
	
	//---Read Result
	Read_CRF_Reversed_END_Boundary_CEDTInfo(outputFilestream.str().c_str(), testing_Surround_v, m_TestingCEDTInfo);

	nCrossID++;
}*/
void CCRF::Read_CRF_nBest_Candit_Info(const char* outputpath, vector<NE_Surround*>& Surround_v, CEDTInfo& pmCEDTInfo)
{
	map<size_t, map<size_t, map<size_t, string>>*> Candits_mm;
	size_t SentenceID = 0;
	size_t charID = 0;
	size_t nBestID;
	string character;
	string mentionstr;
	size_t START;
	size_t END;
	bool BEGIN_Detected_Flag;
	char getlineBuf[MAX_SENTENCE];
	map<size_t, map<size_t, string>>*  pCurLocCandit_m = NULL;

	ifstream in(outputpath);
	if(in.bad())
		return;
	in.clear();
	in.seekg(0, ios::beg);
	if(in.peek() == EOF){
		in.close();
		return;
	}
	while(true){
		if(in.peek() == EOF){	
			break;
		}
		if(in.peek() == '\n'){
			in.getline(getlineBuf, MAX_SENTENCE, '\n');
			continue;
		}
		if(in.peek() != '#'){
			AppCall::Secretary_Message_Box("Data error in CCRF::Read_CRF_nBest_Candit_Info(1)", MB_OK);
		}
		BEGIN_Detected_Flag = false;
		START = 0;
		END = 0;
		charID = 0;
		in.getline(getlineBuf, MAX_SENTENCE, ' ');
		in >> nBestID;
		if(0 == nBestID){
			pCurLocCandit_m = new map<size_t, map<size_t, string>>;
			Candits_mm.insert(make_pair(SentenceID++, pCurLocCandit_m));
			mentionstr = "";
		}
		map<size_t, map<size_t, string>>& locCandit_m = *pCurLocCandit_m;
		in.getline(getlineBuf, MAX_SENTENCE, '\n');
		while(in.peek() != EOF){
			in.getline(getlineBuf, MAX_SENTENCE, '\n');
			if(strlen(getlineBuf) == 0){
				if(BEGIN_Detected_Flag){
					END = charID-1;
					if(locCandit_m.find(START) == locCandit_m.end()){
						locCandit_m[START];
					}
					if(locCandit_m[START].find(END) == locCandit_m[START].end()){
						locCandit_m[START].insert(make_pair(END, mentionstr));
					}
					else if(strcmp(locCandit_m[START][END].c_str(), mentionstr.c_str())){
						AppCall::Secretary_Message_Box("CCRF::Read_CRF_nBest_Candit_Info() 2", MB_OK);
					}
					mentionstr = "";
				}
				break;
			}
			istringstream instream(getlineBuf);
			instream.getline(getlineBuf, MAX_SENTENCE,'\t');
			character = getlineBuf;

			for(size_t i = 0; i < 9; i++){
				instream.getline(getlineBuf, MAX_SENTENCE,'\t');
			}
			instream.getline(getlineBuf, MAX_SENTENCE,'\t');

			if(!strcmp(getlineBuf, "B")){
				if(BEGIN_Detected_Flag){
					if(locCandit_m.find(START) == locCandit_m.end()){
						locCandit_m[START];
					}
					if(locCandit_m[START].find(END) == locCandit_m[START].end()){
						locCandit_m[START].insert(make_pair(END, mentionstr));
					}
					else if(strcmp(locCandit_m[START][END].c_str(), mentionstr.c_str())){
						AppCall::Secretary_Message_Box("CCRF::Read_CRF_nBest_Candit_Info() 3", MB_OK);
					}
					mentionstr = "";
				}
				START = charID;
				BEGIN_Detected_Flag = true;
			}
			else if(!strcmp(getlineBuf, "O")){
				if(BEGIN_Detected_Flag){
					if(locCandit_m.find(START) == locCandit_m.end()){
						locCandit_m[START];
					}
					if(locCandit_m[START].find(END) == locCandit_m[START].end()){
						locCandit_m[START].insert(make_pair(END, mentionstr));
					}
					else if(strcmp(locCandit_m[START][END].c_str(), mentionstr.c_str())){
						AppCall::Secretary_Message_Box("CCRF::Read_CRF_nBest_Candit_Info() 4", MB_OK);
					}
					mentionstr = "";
				}
				BEGIN_Detected_Flag = false;
			}
			if(BEGIN_Detected_Flag){
				mentionstr += character;
			}
			END = charID;
			charID++;
		}
	}
	in.close();
//------------------------------------------------------------------------------------------
	//:YPench Test
	//size_t TotalMention = 0;
	//size_t PositiveMention = 0;
	vector<CanditCase*>& Candit_v = CEDT_Head_Flag?pmCEDTInfo.CanditHead_v:pmCEDTInfo.CanditExtend_v;
	map<size_t, map<size_t, map<size_t, string>>*>::iterator canditmite = Candits_mm.begin();
	size_t CASID = 0;
	bool Regested_Flag = false;
	map<size_t, map<size_t, set<size_t>>> Regested_mms;
	if(!Candit_v.empty()){
		Regested_Flag = true;
		for(size_t i = 0; i < Candit_v.size(); i++){
			if(Regested_mms.find(Candit_v[i]->SENID) == Regested_mms.end()){
				Regested_mms[Candit_v[i]->SENID];
			}
			map<size_t, set<size_t>>& loc_reg_ms = Regested_mms[Candit_v[i]->SENID];
			if(loc_reg_ms.find(Candit_v[i]->START) == loc_reg_ms.end()){
				loc_reg_ms[Candit_v[i]->START];
			}
			loc_reg_ms[Candit_v[i]->START].insert(Candit_v[i]->END);
		}
	}
	for(vector<NE_Surround*>::iterator ne_vite = Surround_v.begin(); ne_vite != Surround_v.end(); ne_vite++){
		size_t SENID = (*ne_vite)->SENID;
		vector<string> character_v;
		string& insurRef = (*ne_vite)->insur;
		const char* inputchar = insurRef.c_str();
		size_t length = NLPOP::Get_Chinese_Sentence_Length_Counter(inputchar);

		string inputstr = ace_op::Delet_0AH_and_20H_in_string(insurRef.c_str());
		if(inputstr.length() == 0){
			continue;
		}

		NLPOP::charseq_to_vector(insurRef.c_str(), character_v);

		if(canditmite == Candits_mm.end()){
			AppCall::Secretary_Message_Box("Data Error in: CCRF::Read_CRF_nBest_Candit_Info()\ncanditmite == Candits_mm.end()", MB_OK);
		}

		map<size_t, map<size_t, string>>& locMentions_m = *canditmite->second;
		map<size_t, size_t> RevizeMap;
		size_t charID = 0;
		for(size_t i = 0; i < character_v.size(); i++){
			if(!strcmp(character_v[i].c_str(), "\n") || !strcmp(character_v[i].c_str(), " ")){
				continue;
			}
			RevizeMap.insert(make_pair(charID++, i));
		}
		set<size_t> START_s;
		set<size_t> END_s;
		map<size_t, map<size_t, ACE_entity_mention*>> EntityMention_mm;
		if(CEDT_Head_Flag){
			ace_op::Get_Entity_Mention_head_Map((*ne_vite)->entity_mention_v, EntityMention_mm, START_s, END_s);
		}
		else{
			ace_op::Get_Entity_Mention_extent_Map((*ne_vite)->entity_mention_v, EntityMention_mm, START_s, END_s);
		}
		START_s.clear();
		END_s.clear();
		map<size_t, size_t> Labled_m;
		Init_Labled_Sequence(EntityMention_mm, Labled_m);

		for(map<size_t, map<size_t, string>>::iterator mmite = locMentions_m.begin(); mmite != locMentions_m.end(); mmite++){
			for(map<size_t, string>::iterator mite = mmite->second.begin(); mite != mmite->second.end(); mite++){
				START_s.insert(RevizeMap[mmite->first]);
				END_s.insert(RevizeMap[mite->first]);
			}
		}
		map<size_t, set<size_t>>* ploc_reg_ms = NULL;
		if(Regested_Flag){
			if(Regested_mms.find(SENID) != Regested_mms.end()){
				//AppCall::Secretary_Message_Box("Data Error in: CCRF::Read_CRF_nBest_Candit_Info()");
				ploc_reg_ms = &Regested_mms[SENID];
			}
		}
		for(map<size_t, map<size_t, string>>::iterator mmite = locMentions_m.begin(); mmite != locMentions_m.end(); mmite++){
			for(map<size_t, string>::iterator mite = mmite->second.begin(); mite != mmite->second.end(); mite++){
				size_t START =  RevizeMap[mmite->first];
				size_t END =	RevizeMap[mite->first];
				if(Regested_Flag && ploc_reg_ms){
					if(ploc_reg_ms->find(START) != ploc_reg_ms->end()){
						if((*ploc_reg_ms)[START].find(END) != (*ploc_reg_ms)[START].end()){
							continue;
						}
					}
				}
				pCanditCase ploc_Candit = new CanditCase;
				ploc_Candit->SENID = SENID;
				ploc_Candit->CASID = CASID++;
				ploc_Candit->CEDT_Head_Flag = CEDT_Head_Flag;
				ploc_Candit->START = START;
				ploc_Candit->END = END;

				int Left_Out_Range = NECOM::Get_Left_Outer_Feature_Range_For_START(START_s, END_s, ploc_Candit->START, -1);
				int Right_Out_Range = NECOM::Get_Right_Outer_Feature_Range_For_END(START_s, END_s, ploc_Candit->END, length-1);
				ploc_Candit->l_outmstr = Sentop::Get_Substr_by_Chinese_Character_Cnt(inputchar, Left_Out_Range+1, ploc_Candit->START-Left_Out_Range-1);
				ploc_Candit->r_outmstr = Sentop::Get_Substr_by_Chinese_Character_Cnt(inputchar, ploc_Candit->END+1, Right_Out_Range-ploc_Candit->END-1);
				
				ploc_Candit->prix = Sentop::Get_Substr_by_Chinese_Character_Cnt(inputchar, 0, ploc_Candit->START);
				ploc_Candit->prox = Sentop::Get_Substr_by_Chinese_Character_Cnt(inputchar, ploc_Candit->END+1, -1);
				ploc_Candit->mention = Sentop::Get_Substr_by_Chinese_Character_Cnt(inputchar, ploc_Candit->START, ploc_Candit->END-ploc_Candit->START+1);
				
				string CheckMention = ace_op::Delet_0AH_and_20H_in_string(ploc_Candit->mention.c_str());
				if(strcmp(CheckMention.c_str(), mite->second.c_str())){
					AppCall::Secretary_Message_Box("Data Error in: CCRF::Read_CRF_nBest_Candit_Info() 6", MB_OK);
				}

				ploc_Candit->Cand_Flag = false;
				ploc_Candit->pNE_mention = NULL;
				ploc_Candit->org_TYPE = NEGETIVE;
				if(Labled_m.find(ploc_Candit->START) != Labled_m.end()){
					if(Labled_m[ploc_Candit->START] == ploc_Candit->END){
						ploc_Candit->Cand_Flag = true;
						ploc_Candit->pNE_mention = EntityMention_mm[ploc_Candit->START][ploc_Candit->END];
						ploc_Candit->org_TYPE = CEDT_Detection_Flag?POSITIVE:EntityMention_mm[ploc_Candit->START][ploc_Candit->END]->Entity_TYPE;
						//--------------------
						//PositiveMention++;
					}
				}
				ploc_Candit->predit_TYPE = POSITIVE;
				Candit_v.push_back(ploc_Candit);
				//--------
				//TotalMention++;
			}
		}
		canditmite++;
	}
	//YPench
	/*ostringstream outstream;
	outstream << "Total:\t " << TotalMention <<endl;
	outstream << "Positive:\t " << PositiveMention << endl;
	AppCall::Secretary_Message_Box(outstream.str().c_str());*/

	for(map<size_t, map<size_t, map<size_t, string>>*>::iterator mite = Candits_mm.begin(); mite != Candits_mm.end(); mite++){
		delete mite->second;
	}
}

void CCRF::Init_Labled_Sequence(map<size_t, map<size_t, ACE_entity_mention*>>& EntityMention_mm, map<size_t, size_t>& pmLabled_m)
{
	map<size_t, map<size_t, ACE_entity_mention*>>::iterator mmite = EntityMention_mm.begin();
	int END = -1;
	for( ; mmite != EntityMention_mm.end(); mmite++){
		if(END < (int)mmite->first){
			if(pmLabled_m.find(mmite->first) == pmLabled_m.end()){
				pmLabled_m[mmite->first];
			}
			if(CRF_Max_Flag){
				pmLabled_m[mmite->first] = mmite->second.rbegin()->first;
			}
			else{
				pmLabled_m[mmite->first] = mmite->second.begin()->first;
			}
			END = pmLabled_m[mmite->first];
		}
	}

}

void CCRF::Read_CRF_Reversed_END_Boundary_CEDTInfo(const char* outputpath, vector<NE_Surround*>& Surround_v, CEDTInfo& pmCEDTInfo)
{
	//map<size_t, map<size_t, double>*>& START_mm = CEDT_Head_Flag?pmCEDTInfo.Head_START_mm:pmCEDTInfo.Extend_START_mm;
	map<size_t, map<size_t, double>*>& END_mm = CEDT_Head_Flag?pmCEDTInfo.Head_END_mm:pmCEDTInfo.Extend_END_mm;
	map<size_t, vector<pair<string, double>>*> InputRtn_mv;
	size_t SentenceID = 0;
	string character;
	double value;
	char getlineBuf[MAX_SENTENCE];

	ifstream in(outputpath);
	if(in.bad())
		return;
	in.clear();
	in.seekg(0, ios::beg);
	if(in.peek() == EOF){
		in.close();
		return;
	}
	while(true){
		if(in.peek() == EOF){	
			break;
		}
		if(in.peek() == '\n'){
			in.getline(getlineBuf, MAX_SENTENCE, '\n');
			continue;
		}
		if(in.peek() != '#'){
			AppCall::Secretary_Message_Box("Data error in CCRF::Read_CRF_STRAT_or_END_Boundary_CEDTInfo()", MB_OK);
		}
		in.getline(getlineBuf, MAX_SENTENCE, '\n');
		vector<pair<string, double>>* pLocSent_v = new vector<pair<string, double>>;
		InputRtn_mv.insert(make_pair(SentenceID++, pLocSent_v));
		while(in.peek() != EOF){
			in.getline(getlineBuf, MAX_SENTENCE, '\n');
			if(strlen(getlineBuf) == 0){
				break;
			}
			istringstream instream(getlineBuf);
			instream.getline(getlineBuf, MAX_SENTENCE,'\t');
			character = getlineBuf;
			for(size_t i = 0; i < 9; i++){
				instream.getline(getlineBuf, MAX_SENTENCE,'\t');
			}
			instream.getline(getlineBuf, MAX_SENTENCE,'\t');
			instream.getline(getlineBuf, MAX_SENTENCE,'/');
			instream >> value;
			pLocSent_v->push_back(make_pair(character, value));
		}
	}
	in.close();

	map<size_t, vector<pair<string, double>>*>::iterator mvite = InputRtn_mv.begin();
	size_t CASID = 0;
	for(vector<NE_Surround*>::iterator ne_vite = Surround_v.begin(); ne_vite != Surround_v.end(); ne_vite++){
		size_t SENID = (*ne_vite)->SENID;
		vector<string> character_v;
		string& insurRef = (*ne_vite)->insur;

		string inputstr = ace_op::Delet_0AH_and_20H_in_string(insurRef.c_str());
		if(inputstr.length() == 0){
			continue;
		}

		NLPOP::charseq_to_vector(insurRef.c_str(), character_v);
		if(END_mm.find(SENID) == END_mm.end()){
			END_mm[SENID];
			END_mm[SENID] = new map<size_t, double>;
		}
		map<size_t, double>& locValue_m = *END_mm[SENID];

		vector<pair<string, double>>::reverse_iterator vite = mvite->second->rbegin();
		for(size_t i = 0; i < character_v.size(); i++){
			if(!strcmp(character_v[i].c_str(), "\n") || !strcmp(character_v[i].c_str(), " ")){
				continue;
			}
			if(strcmp(vite->first.c_str(), character_v[i].c_str())){
				AppCall::Secretary_Message_Box("Data Error in: CCRF::Read_CRF_Reversed_END_Boundary_CEDTInfo() 1", MB_OK);
			}
			locValue_m.insert(make_pair(i, vite->second));
			vite++;
		}
		if(vite == mvite->second->rend()){
			mvite++;
		}
		else{
			AppCall::Secretary_Message_Box("Data Error in: CCRF::Read_CRF_Reversed_END_Boundary_CEDTInfo() 2", MB_OK);
		}
	}
	for(map<size_t, vector<pair<string, double>>*>::iterator mvite = InputRtn_mv.begin(); mvite != InputRtn_mv.end(); mvite++){
		delete mvite->second;
	}
	InputRtn_mv.clear();
}



void CCRF::Read_CRF_STRAT_or_END_Boundary_CEDTInfo(const char* outputpath, vector<NE_Surround*>& Surround_v, CEDTInfo& pmCEDTInfo)
{
	map<size_t, map<size_t, double>*>& START_mm = CEDT_Head_Flag?pmCEDTInfo.Head_START_mm:pmCEDTInfo.Extend_START_mm;
	map<size_t, map<size_t, double>*>& END_mm = CEDT_Head_Flag?pmCEDTInfo.Head_END_mm:pmCEDTInfo.Extend_END_mm;
	map<size_t, map<size_t, double>*>& Bound_mm =  CRF_START_or_END_DetBound_Flag?START_mm:END_mm;
	
	map<size_t, vector<pair<string, double>>*> InputRtn_mv;
	size_t SentenceID = 0;
	string character;
	double value;
	char getlineBuf[MAX_SENTENCE];

	ifstream in(outputpath);
	if(in.bad())
		return;
	in.clear();
	in.seekg(0, ios::beg);
	if(in.peek() == EOF){
		in.close();
		return;
	}
	while(true){
		if(in.peek() == EOF){	
			break;
		}
		if(in.peek() == '\n'){
			in.getline(getlineBuf, MAX_SENTENCE, '\n');
			continue;
		}
		if(in.peek() != '#'){
			AppCall::Secretary_Message_Box("Data error in CCRF::Read_CRF_STRAT_or_END_Boundary_CEDTInfo()", MB_OK);
		}
		in.getline(getlineBuf, MAX_SENTENCE, '\n');
		vector<pair<string, double>>* pLocSent_v = new vector<pair<string, double>>;
		InputRtn_mv.insert(make_pair(SentenceID++, pLocSent_v));
		while(in.peek() != EOF){
			in.getline(getlineBuf, MAX_SENTENCE, '\n');
			if(strlen(getlineBuf) == 0){
				break;
			}
			istringstream instream(getlineBuf);
			instream.getline(getlineBuf, MAX_SENTENCE,'\t');
			character = getlineBuf;
			for(size_t i = 0; i < 9; i++){
				instream.getline(getlineBuf, MAX_SENTENCE,'\t');
			}
			instream.getline(getlineBuf, MAX_SENTENCE,'\t');
			instream.getline(getlineBuf, MAX_SENTENCE,'/');
			instream >> value;
			pLocSent_v->push_back(make_pair(character, value));
		}
	}
	in.close();

	map<size_t, vector<pair<string, double>>*>::iterator mvite = InputRtn_mv.begin();
	size_t CASID = 0;
	for(vector<NE_Surround*>::iterator ne_vite = Surround_v.begin(); ne_vite != Surround_v.end(); ne_vite++){
		size_t SENID = (*ne_vite)->SENID;
		vector<string> character_v;
		string& insurRef = (*ne_vite)->insur;

		string inputstr = ace_op::Delet_0AH_and_20H_in_string(insurRef.c_str());
		if(inputstr.length() == 0){
			continue;
		}

		NLPOP::charseq_to_vector(insurRef.c_str(), character_v);
		if(Bound_mm.find(SENID) == Bound_mm.end()){
			Bound_mm[SENID];
			Bound_mm[SENID] = new map<size_t, double>;
		}
		map<size_t, double>& locValue_m = *Bound_mm[SENID];

		vector<pair<string, double>>::iterator vite = mvite->second->begin();
		for(size_t i = 0; i < character_v.size(); i++){
			if(!strcmp(character_v[i].c_str(), "\n") || !strcmp(character_v[i].c_str(), " ")){
				continue;
			}
			if(strcmp(vite->first.c_str(), character_v[i].c_str())){
				AppCall::Secretary_Message_Box("Data Error in: CCRF::Read_CRF_STRAT_or_END_Boundary_CEDTInfo() 1", MB_OK);
			}
			locValue_m.insert(make_pair(i, vite->second));
			vite++;
		}
		if(vite == mvite->second->end()){
			mvite++;
		}
		else{
			AppCall::Secretary_Message_Box("Data Error in: CCRF::Read_CRF_STRAT_or_END_Boundary_CEDTInfo() 2", MB_OK);
		}
	}
	for(map<size_t, vector<pair<string, double>>*>::iterator mvite = InputRtn_mv.begin(); mvite != InputRtn_mv.end(); mvite++){
		delete mvite->second;
	}
	InputRtn_mv.clear();
}



void CCRF::CRF_Training_Port(const char* parameters, const char* cFilePath)
{
	wchar_t awTrainingFileName[256];
	wchar_t awParameters[256];

	if(!NLPOP::Exist_of_This_File(cFilePath) || (strlen(parameters) == 0)){
		AppCall::Secretary_Message_Box("File or Parameters not exist in: CCRF::CRF_Training_Port()", MB_OK);
		return;
	}
	SCONVERT::AnsiToUnicode(parameters, awParameters);
	SCONVERT::AnsiToUnicode(cFilePath, awTrainingFileName);

	SHELLEXECUTEINFO ShExecInfo = {0};
	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	ShExecInfo.hwnd = NULL;
	ShExecInfo.lpVerb = _T("open");
	ShExecInfo.lpDirectory = _T(CRF_FOLDER);
	ShExecInfo.nShow = SW_HIDE;
	ShExecInfo.hInstApp = NULL;
	ShExecInfo.lpFile = _T("crf_learn.exe");

	ShExecInfo.lpParameters = awParameters;
	ShellExecuteEx(&ShExecInfo);
	WaitForSingleObject(ShExecInfo.hProcess,INFINITE);
}






void CCRF::Max_Min_Performance_Merging()
{
	vector<pair<string, string>*> Max_CRFResult_v;
	vector<pair<string, string>*> Min_CRFResult_v;
	string outfile = CRF_FOLDER;

	if(!NLPOP::Exist_of_This_File(outfile+"G_CRF_Results_0.txt") || !NLPOP::Exist_of_This_File(outfile+"G_CRF_Results_1.txt")){
		AppCall::Secretary_Message_Box("File not exist in  CCRF::Max_Min_Performance_Merging()", MB_OK);
		return;
	}
	outfile += "G_CRF_Results_0.txt";
	Input_Global_CRF_Results(outfile.c_str(), Max_CRFResult_v);

	outfile = CRF_FOLDER;
	outfile += "G_CRF_Results_1.txt";
	Input_Global_CRF_Results(outfile.c_str(), Min_CRFResult_v);

	if(Max_CRFResult_v.size() != Min_CRFResult_v.size()){
		AppCall::Secretary_Message_Box("Data Error in  CCRF::Max_Min_Performance_Merging()", MB_OK);
		return;
	}

	map<string, pair<size_t, size_t>> Recall_map;
	map<string, pair<size_t, size_t>> Pricision_map;

	Recall_map.insert(make_pair(POSITIVE, make_pair(0, 0)));
	Recall_map.insert(make_pair(NEGETIVE, make_pair(0, 0)));
	Pricision_map.insert(make_pair(POSITIVE, make_pair(0, 0)));
	Pricision_map.insert(make_pair(NEGETIVE, make_pair(0, 0)));

	map<size_t, set<size_t>> first_RecNE_ms;
	map<size_t, set<size_t>> second_RecNE_ms;
	map<size_t, set<size_t>> first_PreNE_ms;
	map<size_t, set<size_t>> second_PreNE_ms;

	Performance_of_Max_Min_CRF(Max_CRFResult_v, first_RecNE_ms, second_RecNE_ms, first_PreNE_ms, second_PreNE_ms, Recall_map, Pricision_map);

	Performance_of_Max_Min_CRF(Min_CRFResult_v, first_RecNE_ms, second_RecNE_ms, first_PreNE_ms, second_PreNE_ms, Recall_map, Pricision_map);


	MAXEN::Display_Performance_for_MAXEN(true, true, Pricision_map, Recall_map, "\n****************************************\n\nThe Max, Min Performance...\n");

}

void CCRF::Performance_of_Max_Min_CRF(vector<pair<string, string>*>& pmCRFResult_v, 
									  map<size_t, set<size_t>>& first_RecNE_ms, map<size_t, set<size_t>>& second_RecNE_ms, 
									  map<size_t, set<size_t>>& first_PreNE_ms, map<size_t, set<size_t>>& second_PreNE_ms,
									  map<string, pair<size_t, size_t>>& Recall_map, map<string, pair<size_t, size_t>>& Pricision_map)
{
	bool NEs_Recall_Flag = false;
	bool NEs_Precison_Flag = false;
	bool preR_Flag = false;
	bool preP_Flag = false;
	size_t pri_B_Rel = 0;
	size_t pri_B_Pre = 0;
	for(size_t i = 0; i < pmCRFResult_v.size(); i++){
		//---Recall
		if(pmCRFResult_v[i]->first == "B"){
			if(!NEs_Recall_Flag){//-----------
				pri_B_Rel = i;
			}
			NEs_Recall_Flag = true;
			for(size_t j = i+1; j < pmCRFResult_v.size(); j++){
				if(pmCRFResult_v[j]->first == "I"){
					continue;
				}
				if(!NE_Is_Regested(second_RecNE_ms, pri_B_Rel, j-1)){
					Recall_map[POSITIVE].second++;
				}
				break;
			}
			if(preR_Flag && (pmCRFResult_v[i]->second != "I")){
				if(!NE_Is_Regested(first_RecNE_ms, pri_B_Rel, i-1)){
					Recall_map[POSITIVE].first++;
				}
				pri_B_Rel = i;
			}
		}
		if(NEs_Recall_Flag){
			if((pmCRFResult_v[i]->first == "B") && (pmCRFResult_v[i]->second != "B") ){
				NEs_Recall_Flag = false;
			}
			else if((pmCRFResult_v[i]->first == "I") && (pmCRFResult_v[i]->second != "I") ){
				NEs_Recall_Flag = false;
			}
			else if(pmCRFResult_v[i]->first == "O" && (pmCRFResult_v[i]->second != "I")){
				if(!NE_Is_Regested(first_RecNE_ms, pri_B_Rel, i-1)){
					Recall_map[POSITIVE].first++;
				}
				NEs_Recall_Flag = false;
			}
			else if(pmCRFResult_v[i]->first == "O") {
				NEs_Recall_Flag = false;
			}
		}
		//---Precision
		if(pmCRFResult_v[i]->second == "B"){
			if(!NEs_Precison_Flag){//-----------
				pri_B_Pre = i;
			}
			NEs_Precison_Flag = true;
			for(size_t j = i+1; j < pmCRFResult_v.size(); j++){
				if(pmCRFResult_v[j]->second == "I"){
					continue;
				}
				if(!NE_Is_Regested(second_PreNE_ms, pri_B_Pre, j-1)){
					Pricision_map[POSITIVE].second++;
				}
				break;
			}
			if(preP_Flag && (pmCRFResult_v[i]->first != "I")){
				if(!NE_Is_Regested(first_PreNE_ms, pri_B_Pre, i-1)){
					Pricision_map[POSITIVE].first++;
				}
				pri_B_Pre = i;
			}
		}
		if(NEs_Precison_Flag){
			if((pmCRFResult_v[i]->second == "B") && (pmCRFResult_v[i]->first != "B")){
				NEs_Precison_Flag = false;
			}
			else if((pmCRFResult_v[i]->second == "I") && (pmCRFResult_v[i]->first != "I")){
				NEs_Precison_Flag = false;
			}
			else if(pmCRFResult_v[i]->second == "O" && (pmCRFResult_v[i]->first != "I")){
				if(!NE_Is_Regested(first_PreNE_ms, pri_B_Pre, i-1)){
					Pricision_map[POSITIVE].first++;
				}
				NEs_Precison_Flag = false;
			}
			else if(pmCRFResult_v[i]->second == "O"){
				NEs_Precison_Flag = false;
			}
		}
		preR_Flag = NEs_Recall_Flag;
		preP_Flag = NEs_Precison_Flag;
	}
}

bool CCRF::NE_Is_Regested(map<size_t, set<size_t>>& TotalNE_ms, size_t first, size_t second)
{
	if(TotalNE_ms.find(first) == TotalNE_ms.end()){
		TotalNE_ms[first];
	}
	if(TotalNE_ms[first].find(second) == TotalNE_ms[first].end()){
		TotalNE_ms[first].insert(second);
		return false;
	}
	else{
		return true;
	}
}
void CCRF::Performance_of_Max_Min_CRF(vector<pair<string, string>*>& pmCRFResult_v, map<size_t, set<size_t>>& TotalNE_ms, size_t& CorrectCnt)
{

	bool NEs_Flag = false;
	bool preR_Flag = false;
	size_t pri_B = 0;
	size_t i = 0;
	for(size_t i = 0; i < pmCRFResult_v.size(); i++){
		//---Recall
		if(pmCRFResult_v[i]->first == "B"){
			if(!NEs_Flag){//-----------
				pri_B = i;
			}
			NEs_Flag = true;
			if(preR_Flag && (pmCRFResult_v[i]->second != "I")){
				if(TotalNE_ms.find(pri_B) == TotalNE_ms.end()){
					TotalNE_ms[pri_B];
				}
				if(TotalNE_ms[pri_B].find(i-1) == TotalNE_ms[pri_B].end()){
					TotalNE_ms[pri_B].insert(i-1);
					CorrectCnt++;
				}
				pri_B = i;
			}
		}
		if(NEs_Flag){
			if((pmCRFResult_v[i]->first == "B") && (pmCRFResult_v[i]->second != "B") ){
				NEs_Flag = false;
			}
			else if((pmCRFResult_v[i]->first == "I") && (pmCRFResult_v[i]->second != "I") ){
				NEs_Flag = false;
			}
			else if(pmCRFResult_v[i]->first == "O" && (pmCRFResult_v[i]->second != "I")){
				NEs_Flag = false;
				if(TotalNE_ms.find(pri_B) == TotalNE_ms.end()){
					TotalNE_ms[pri_B];
				}
				if(TotalNE_ms[pri_B].find(i-1) == TotalNE_ms[pri_B].end()){
					TotalNE_ms[pri_B].insert(i-1);
					CorrectCnt++;
				}
				pri_B = i;
			}
			else if(pmCRFResult_v[i]->first == "O") {
				NEs_Flag = false;
			}
		}
		preR_Flag = NEs_Flag;
	}
}

void CCRF::Output_Global_CRF_Results_with_Erasing()
{
	ostringstream ostream;
	size_t File_ID = 0;
	while(true){
		ostream.str("");
		ostream << CRF_FOLDER << "G_CRF_Results_" << File_ID << ".txt";
		if(NLPOP::Exist_of_This_File(ostream.str())){
			File_ID++;
		}
		else{
			break;
		}
	}
	ofstream out(ostream.str().c_str());
	if(out.bad()){
		return;
	}
	out.clear();
	out.seekp(0, ios::beg);
	for(size_t i = 0; i < G_CRFResult_v.size(); i++){
		out << G_CRFResult_v[i]->first << " " << G_CRFResult_v[i]->second << endl;
		delete G_CRFResult_v[i];
	}
	G_CRFResult_v.clear();
	out.close();
}

void CCRF::Input_Global_CRF_Results(const char* pFile, vector<pair<string, string>*>& pmG_CRFResult_v)
{
	string sFirst;
	string sSecond;
	char getlineBuf[MAX_SENTENCE];
	ifstream in(pFile);
	if(in.bad())
		return;
	in.clear();
	in.seekg(0, ios::beg);
	if(in.peek() == EOF){
		in.close();
		return;
	}
	while(true){
		if(in.peek() == EOF){	
			break;
		}
		in.getline(getlineBuf, MAX_SENTENCE, '\n');
		istringstream instream(getlineBuf);
		while(EOF != instream.peek()){
			instream.getline(getlineBuf, MAX_SENTENCE,' ');
			sFirst = getlineBuf;
			instream.getline(getlineBuf, MAX_SENTENCE,'\n');
			sSecond = getlineBuf;
			pair<string, string>* pG_Rlt = new pair<string, string>;
			pG_Rlt->first = sFirst;
			pG_Rlt->second = sSecond;
			pmG_CRFResult_v.push_back(pG_Rlt);
		}
	}
}


void CCRF::Read_Result_of_Consensus_CRF_Port(const char* pathchar, vector<pair<string, string>*>& CRFResult_v)
{
	string sFirst;
	string sSecond;
	char getlineBuf[MAX_SENTENCE];
	ifstream in(pathchar);
	if(in.bad())
		return;
	in.clear();
	in.seekg(0, ios::beg);
	if(in.peek() == EOF){
		in.close();
		return;
	}
	while(true){
		if(in.peek() == EOF){	
			break;
		}
		in.getline(getlineBuf, MAX_SENTENCE, '\n');
		istringstream instream(getlineBuf);
		while(EOF != instream.peek()){
			//instream.getline(getlineBuf, MAX_SENTENCE,'\t');
			//if(CRF_POS_Flag || CEDT_Word_Boundary_Flag){
			//	instream.getline(getlineBuf, MAX_SENTENCE,'\t');
			//}
			for(size_t i = 0; i < 9; i++){
				instream.getline(getlineBuf, MAX_SENTENCE,'\t');
			}
			instream.getline(getlineBuf, MAX_SENTENCE,'\t');
			sFirst = getlineBuf;
			instream.getline(getlineBuf, MAX_SENTENCE,'\n');
			sSecond = getlineBuf;
			pair<string, string>* pG_Rlt = new pair<string, string>;
			pG_Rlt->first = sFirst;
			pG_Rlt->second = sSecond;
			CRFResult_v.push_back(pG_Rlt);
		}
	}
}

void CCRF::Performance_of_Consensus_CRF_Port_with_Multi_TYPE_Tags(vector<pair<string, string>*>& CRFResult_v)
{
	map<string, pair<size_t, size_t>> Recall_map;
	map<string, pair<size_t, size_t>> Pricision_map;

	//Recall_map.insert(make_pair(POSITIVE, make_pair(0, 0)));
	//Recall_map.insert(make_pair(NEGETIVE, make_pair(0, 0)));
	//Pricision_map.insert(make_pair(POSITIVE, make_pair(0, 0)));
	//Pricision_map.insert(make_pair(NEGETIVE, make_pair(0, 0)));

	map<size_t, map<size_t, string>> Origin_mm;
	map<size_t, map<size_t, string>> Pridic_mm;
	bool Pre_BEGIN = false;
	size_t START, END;
	char getlineBuf[58];
	string BTags;
	for(size_t i = 0; i < CRFResult_v.size(); i++){
		if(CRFResult_v[i]->first.c_str()[0] == 'B'){
			if(Pre_BEGIN){
				Origin_mm[START];
				Origin_mm[START].insert(make_pair(END, BTags));
			}
			istringstream instream(CRFResult_v[i]->first.c_str());
			instream.getline(getlineBuf, 58, '-');
			instream >> BTags;
			Pre_BEGIN = true;
			START = i;
		}
		else if(CRFResult_v[i]->first.c_str()[0] == 'O'){
			if(Pre_BEGIN){
				Origin_mm[START];
				Origin_mm[START].insert(make_pair(END, BTags));
			}
			Pre_BEGIN = false;
		}
		END = i;
	}
	Pre_BEGIN = false;
	for(size_t i = 0; i < CRFResult_v.size(); i++){
		if(CRFResult_v[i]->second.c_str()[0] == 'B'){
			if(Pre_BEGIN){
				Pridic_mm[START];
				Pridic_mm[START].insert(make_pair(END, BTags));
			}
			istringstream instream(CRFResult_v[i]->second.c_str());
			instream.getline(getlineBuf, 58, '-');
			instream >> BTags;
			Pre_BEGIN = true;
			START = i;
		}
		else if(CRFResult_v[i]->second.c_str()[0] == 'O'){
			if(Pre_BEGIN){
				Pridic_mm[START];
				Pridic_mm[START].insert(make_pair(END, BTags));
			}
			Pre_BEGIN = false;
		}
		END = i;
	}
	string PredTag, OriginTag; 
	for(map<size_t, map<size_t, string>>::iterator mmite = Pridic_mm.begin(); mmite != Pridic_mm.end(); mmite++){
		for(map<size_t, string>::iterator mite = mmite->second.begin(); mite != mmite->second.end(); mite++){ 
			if(Origin_mm.find(mmite->first) != Origin_mm.end()){
				if(Origin_mm[mmite->first].find(mite->first) != Origin_mm[mmite->first].end()){
					PredTag = Pridic_mm[mmite->first][mite->first];
					OriginTag = Origin_mm[mmite->first][mite->first];
					if(!strcmp(PredTag.c_str(), OriginTag.c_str())){
						if(Recall_map.find(OriginTag) == Recall_map.end()){
							Recall_map.insert(make_pair(OriginTag, make_pair(0, 0)));
							Pricision_map.insert(make_pair(OriginTag, make_pair(0, 0)));
						}
						Recall_map[OriginTag].first++;
						Pricision_map[OriginTag].first++;
					}
				}
			}
		}
	}
	for(map<size_t, map<size_t, string>>::iterator mmite = Pridic_mm.begin(); mmite != Pridic_mm.end(); mmite++){
		for(map<size_t, string>::iterator mite = mmite->second.begin(); mite != mmite->second.end(); mite++){
			PredTag = mite->second;
			if(Pricision_map.find(PredTag) == Pricision_map.end()){
				Pricision_map.insert(make_pair(PredTag, make_pair(0, 0)));
			}
			Pricision_map[PredTag].second++;
		}
	}
	for(map<size_t, map<size_t, string>>::iterator mmite = Origin_mm.begin(); mmite != Origin_mm.end(); mmite++){
		for(map<size_t, string>::iterator mite = mmite->second.begin(); mite != mmite->second.end(); mite++){
			OriginTag = mite->second;
			if(Recall_map.find(OriginTag) == Recall_map.end()){
				Recall_map.insert(make_pair(OriginTag, make_pair(0, 0)));
			}
			Recall_map[OriginTag].second++;
		}
	}

	MAXEN::Display_Performance_for_MAXEN(true, true, Pricision_map, Recall_map, "\n****************************************\n\nThe n Cross Performance...\n");
	
	MAXEN::Collect_nGross_Performances(Pricision_map, Recall_map, P_nCrossRtn_m, R_nCrossRtn_m);

}

void CCRF::Performance_of_Consensus_CRF_Port(vector<pair<string, string>*>& CRFResult_v)
{
	map<string, pair<size_t, size_t>> Recall_map;
	map<string, pair<size_t, size_t>> Pricision_map;

	Recall_map.insert(make_pair(POSITIVE, make_pair(0, 0)));
	Recall_map.insert(make_pair(NEGETIVE, make_pair(0, 0)));
	Pricision_map.insert(make_pair(POSITIVE, make_pair(0, 0)));
	Pricision_map.insert(make_pair(NEGETIVE, make_pair(0, 0)));

	map<size_t, size_t> Origin_m;
	map<size_t, size_t> Pred_m;
	bool Pre_BEGIN = false;
	size_t START, END;
	for(size_t i = 0; i < CRFResult_v.size(); i++){
		if(!strcmp(CRFResult_v[i]->first.c_str(), "B")){
			if(Pre_BEGIN){
				Origin_m.insert(make_pair(START, END));
			}
			Pre_BEGIN = true;
			START = i;
		}
		else if(!strcmp(CRFResult_v[i]->first.c_str(), "O")){
			if(Pre_BEGIN){
				Origin_m.insert(make_pair(START, END));
			}
			Pre_BEGIN = false;
		}
		END = i;
	}
	Pre_BEGIN = false;
	for(size_t i = 0; i < CRFResult_v.size(); i++){
		if(!strcmp(CRFResult_v[i]->second.c_str(), "B")){
			if(Pre_BEGIN){
				Pred_m.insert(make_pair(START, END));
			}
			Pre_BEGIN = true;
			START = i;
		}
		else if(!strcmp(CRFResult_v[i]->second.c_str(), "O")){
			if(Pre_BEGIN){
				Pred_m.insert(make_pair(START, END));
			}
			Pre_BEGIN = false;
		}
		END = i;
	}
	for(map<size_t, size_t>::iterator mite = Pred_m.begin(); mite != Pred_m.end(); mite++){
		if(Origin_m.find(mite->first) != Origin_m.end()){
			if(Origin_m[mite->first] == mite->second){
				Recall_map[POSITIVE].first++;
				Pricision_map[POSITIVE].first++;
			}
		}
	}
	Recall_map[POSITIVE].second = Origin_m.size();
	Pricision_map[POSITIVE].second = Pred_m.size();

	//YPench Test
	/*ostringstream outstream;
	size_t cnt = 0;
	outstream << "Count: " << Pred_m.size() << endl;
	for(map<string, pair<size_t, size_t>>::iterator mite = Pricision_map.begin(); mite != Pricision_map.end(); mite++){
		outstream << mite->first << "  " << mite->second.first << "    " << mite->second.second << endl;
	}
	AppCall::Secretary_Message_Box(outstream.str().c_str());*/


	MAXEN::Display_Performance_for_MAXEN(true, true, Pricision_map, Recall_map, "\n****************************************\n\nThe n Cross Performance...\n");
	
	MAXEN::Collect_nGross_Performances(Pricision_map, Recall_map, P_nCrossRtn_m, R_nCrossRtn_m);

	/*bool NEs_Recall_Flag = false;
	bool NEs_Precison_Flag = false;
	bool preR_Flag = false;
	bool preP_Flag = false;
	for(vector<pair<string, string>*>::iterator vite = CRFResult_v.begin(); vite != CRFResult_v.end(); vite++){
		//---Recall
		pair<string, string>& loc_pair = **vite;
		if(loc_pair.first == "B"){
			Recall_map[POSITIVE].second++;
			NEs_Recall_Flag = true;
			if(preR_Flag && (loc_pair.second != "I")){
				Recall_map[POSITIVE].first++;
			}
		}
		if(NEs_Recall_Flag){
			if((loc_pair.first == "B") && (loc_pair.second != "B") ){
				NEs_Recall_Flag = false;
			}
			else if((loc_pair.first == "I") && (loc_pair.second != "I") ){
				NEs_Recall_Flag = false;
			}
			else if(loc_pair.first == "O" && (loc_pair.second != "I")){
				Recall_map[POSITIVE].first++;
				NEs_Recall_Flag = false;
			}
			else if(loc_pair.first == "O") {
				NEs_Recall_Flag = false;
			}
		}
		//---Precision
		if(loc_pair.second == "B"){
			Pricision_map[POSITIVE].second++;
			NEs_Precison_Flag = true;
			if(preP_Flag && (loc_pair.first != "I")){
				Pricision_map[POSITIVE].first++;
			}
		}
		if(NEs_Precison_Flag){
			if((loc_pair.second == "B") && (loc_pair.first != "B")){
				NEs_Precison_Flag = false;
			}
			else if((loc_pair.second == "I") && (loc_pair.first != "I")){
				NEs_Precison_Flag = false;
			}
			else if(loc_pair.second == "O" && (loc_pair.first != "I")){
				Pricision_map[POSITIVE].first++;
				NEs_Precison_Flag = false;
			}
			else if(loc_pair.second == "O"){
				NEs_Precison_Flag = false;
			}
		}
		preR_Flag = NEs_Recall_Flag;
		preP_Flag = NEs_Precison_Flag;
	}*/

}
void CCRF::Generate_Word_Boundary_Info(const char* inputchar, vector<string>& outpos, const char* cSegmentType)
{
	if(strlen(inputchar) == 0){
		return;
	}
	string inputstr = ace_op::Delet_0AH_and_20H_in_string(inputchar);
	vector<string> character_v;
	NLPOP::charseq_to_vector(inputchar, character_v);

	if(strlen(inputstr.c_str()) == 0){
		size_t i = 0;
		while(!strcmp(character_v[i].c_str(), "\n") || !strcmp(character_v[i].c_str(), " ")){
			outpos.push_back("");
			i++;
			if(!(i < character_v.size())){
				return;
			}
		}
	}
	vector<string> SegRtn_v;
	m_CMaxmatch.Maximum_Match_Segmentation_Traditional(inputstr.c_str(), SegRtn_v, cSegmentType, m_Lexicon_set);
	 
	vector<string>::iterator wordvite = SegRtn_v.begin();
	size_t i = 0;
	for( ; wordvite != SegRtn_v.end(); ){
		vector<string> chars_v;
		NLPOP::charseq_to_vector(wordvite->data(), chars_v);

		for(vector<string>::iterator charvite = chars_v.begin(); charvite != chars_v.end(); charvite++){
			if(!(i < character_v.size())){
				AppCall::Secretary_Message_Box("Data Error: in CCRF::Generate_CRF_POS() 1", MB_OK);
				break;
			}
			while(!strcmp(character_v[i].c_str(), "\n") || !strcmp(character_v[i].c_str(), " ")){
				outpos.push_back("");
				i++;
				if(!(i < character_v.size())){
					AppCall::Secretary_Message_Box("Data Error: in CCRF::Generate_CRF_POS() 2", MB_OK);
					break;
				}
			}
			if(strcmp(character_v[i].c_str(), charvite->data())){
				stringstream ostream;
				ostream << character_v[i].c_str() << endl;
				ostream << charvite->data() << endl;
				AppCall::Secretary_Message_Box(ostream.str(), MB_OK);
				return;
			}
		
			if(SegRtn_v.empty() || wordvite == SegRtn_v.end()){
				outpos.push_back("nops");
				AppCall::Secretary_Message_Box("Data Error: in CCRF::Generate_CRF_POS() 6", MB_OK);
			}
			else if(charvite == chars_v.begin()){
				outpos.push_back("wB");
			}
			else if((charvite+1) == chars_v.end()){
				outpos.push_back("wE");
			}
			else{
				outpos.push_back("wM");
			}
			i++;
		}
		wordvite++;
	}
	if(i < character_v.size()){
		while(!strcmp(character_v[i].c_str(), "\n") || !strcmp(character_v[i].c_str(), " ")){
			outpos.push_back("");
			i++;
			if(!(i < character_v.size())){
				break;
			}
		}
	}
	if(outpos.size() != character_v.size()){
		stringstream ostream;
		ostream << "Data Error: in CCRF::Generate_Word_Boundary_Info()\noutpos.size() != character_v.size()" << endl;
		ostream << "|" << inputstr << "|" << ": " << strlen(inputstr.c_str()) << endl;
		ostream <<  "|" << inputchar <<  "|" << ": " << strlen(inputchar) << endl;
		for(size_t i = 0; i < SegRtn_v.size(); i++){
			ostream << SegRtn_v[i] << "|";
		}
		ostream << endl;
		for(size_t i = 0; i < outpos.size(); i++){
			ostream << outpos[i];
		}
		AppCall::Secretary_Message_Box(ostream.str(), MB_OK);
		return;
	}
}



void CCRF::Generate_CRF_POS(const char* inputchar, vector<string>& outpos)
{
	if(strlen(inputchar) == 0){
		return;
	}

	string inputstr = ace_op::Delet_0AH_and_20H_in_string(inputchar);
	vector<string> character_v;
	NLPOP::charseq_to_vector(inputchar, character_v);

	if(strlen(inputstr.c_str()) == 0){
		size_t i = 0;
		while(!strcmp(character_v[i].c_str(), "\n") || !strcmp(character_v[i].c_str(), " ")){
			outpos.push_back("");
			i++;
			if(!(i < character_v.size())){
				return;
			}
		}
	}
	while(inputstr.find('/') != string::npos){
		char inputBuf[MAX_SENTENCE];
		strcpy_s(inputBuf, MAX_SENTENCE, inputstr.c_str());
		inputBuf[inputstr.find('/')] = '.';
		inputstr = inputBuf;
	}

	CLAUSEPOS loc_Seg;
	m_CSegmter.ICTCLAS_Segmention_Port(inputstr.c_str(), loc_Seg);
	 
	if(loc_Seg.Clause.size() != loc_Seg.POS.size()){
		AppCall::Secretary_Message_Box("Data Error: in CCRF::Generate_CRF_POS()\n(loc_Seg.Clause.size() != loc_Seg.POS.size())", MB_OK);
		return;
	}
	for(size_t i = 0; i < loc_Seg.POS.size(); i++){
		if(loc_Seg.POS[i].length() == 0){
			continue;
		}
		if(loc_Seg.POS[i].c_str()[0] == 'n'){
			loc_Seg.POS[i] = "n";
		}
	}
	vector<string>::iterator wordvite = loc_Seg.Clause.begin();
	vector<string>::iterator posvite = loc_Seg.POS.begin();

	size_t i = 0;
	for( ; wordvite != loc_Seg.Clause.end(); ){
		vector<string> chars_v;
		NLPOP::charseq_to_vector(wordvite->data(), chars_v);

		for(vector<string>::iterator charvite = chars_v.begin(); charvite != chars_v.end(); charvite++){
			if(!(i < character_v.size())){
				AppCall::Secretary_Message_Box("Data Error: in CCRF::Generate_CRF_POS() 1", MB_OK);
				break;
			}
			while(!strcmp(character_v[i].c_str(), "\n") || !strcmp(character_v[i].c_str(), " ")){
				outpos.push_back("");
				i++;
				if(!(i < character_v.size())){
					AppCall::Secretary_Message_Box("Data Error: in CCRF::Generate_CRF_POS() 2", MB_OK);
					break;
				}
			}
			if(strcmp(character_v[i].c_str(), charvite->data())){
				stringstream ostream;
				ostream << character_v[i].c_str() << endl;
				ostream << charvite->data() << endl;
				if(strcmp(character_v[i].c_str(), "/")){
					ostream << "|" << inputstr << "|" << ": " << strlen(inputstr.c_str()) << endl;
					ostream <<  "|" << inputchar <<  "|" << ": " << strlen(inputchar) << endl;
					for(size_t i = 0; i < loc_Seg.Clause.size(); i++){
						ostream << loc_Seg.Clause[i] << "|";
					}
					ostream << endl;
					for(size_t i = 0; i < loc_Seg.POS.size(); i++){
						ostream << loc_Seg.POS[i] << "|";
					}
					ostream << endl;
					for(size_t i = 0; i < outpos.size(); i++){
						ostream << outpos[i];
					}
					AppCall::Secretary_Message_Box(ostream.str(), MB_OK);
					return;
				}
			}
			/*while(strcmp(character_v[i].c_str(), charvite->data())){
				if(!(i < character_v.size())){
					AppCall::Secretary_Message_Box("Data Error: in CCRF::Generate_CRF_POS() 3", MB_OK);
					break;
				}
				while(!strcmp(character_v[i].c_str(), "\n") || !strcmp(character_v[i].c_str(), " ")){
					outpos.push_back("");
					i++;
					if(!(i < character_v.size())){
						AppCall::Secretary_Message_Box("Data Error: in CCRF::Generate_CRF_POS() 4", MB_OK);
						break;
					}
				}
				if(loc_Seg.POS.empty()){
					outpos.push_back("EMP");
					AppCall::Secretary_Message_Box("Data Error: in CCRF::Generate_CRF_POS() 5", MB_OK);
				}
				else if(posvite != loc_Seg.POS.begin()){
					vector<string>::iterator loc_vite = posvite;
					loc_vite--;
					outpos.push_back("E_"+*posvite);
				}
				else{
					outpos.push_back("B_"+*posvite);
				}
				i++;
			}*/
		
			if(loc_Seg.POS.empty() || posvite == loc_Seg.POS.end()){
				outpos.push_back("nops");
				AppCall::Secretary_Message_Box("Data Error: in CCRF::Generate_CRF_POS() 6", MB_OK);
			}
			else if(charvite == chars_v.begin()){
				outpos.push_back("B_"+*posvite);
			}
			else if((charvite+1) == chars_v.end()){
				outpos.push_back("E_"+*posvite);
			}
			else{
				outpos.push_back("M_"+*posvite);
			}
			i++;
		}
		wordvite++;
		posvite++;
	}
	if(i < character_v.size()){
		while(!strcmp(character_v[i].c_str(), "\n") || !strcmp(character_v[i].c_str(), " ")){
			outpos.push_back("");
			i++;
			if(!(i < character_v.size())){
				break;
			}
		}
	}
	if(outpos.size() != character_v.size()){
		stringstream ostream;
		ostream << "Data Error: in CCRF::Generate_CRF_POS()\noutpos.size() != character_v.size()" << endl;
		//for(size_t i = 0; i < character_v.size(); i++){
		//	ostream << character_v[i] << endl;
		//}
		ostream << "|" << inputstr << "|" << ": " << strlen(inputstr.c_str()) << endl;
		ostream <<  "|" << inputchar <<  "|" << ": " << strlen(inputchar) << endl;
		for(size_t i = 0; i < loc_Seg.Clause.size(); i++){
			ostream << loc_Seg.Clause[i] << "|";
		}
		ostream << endl;
		for(size_t i = 0; i < loc_Seg.POS.size(); i++){
			ostream << loc_Seg.POS[i] << "|";
		}
		ostream << endl;
		for(size_t i = 0; i < outpos.size(); i++){
			ostream << outpos[i];
		}
		AppCall::Secretary_Message_Box(ostream.str(), MB_OK);
		return;
	}
}

void CCRF::Output_NE_Surround_for_CRF_Reverse_Sequence(string charpathstr, vector<NE_Surround*>& Surround_v, bool nomial_Flag)
{

	ofstream out(charpathstr.c_str());
	if(out.bad()){
		return;
	}
	out.clear();
	out.seekp(0, ios::beg);

	for(vector<NE_Surround*>::iterator ne_vite = Surround_v.begin(); ne_vite != Surround_v.end(); ne_vite++){
		vector<string> character_v;
		set<size_t> START_s;
		set<size_t> END_s;
		string& insurRef = (*ne_vite)->insur;
		NLPOP::charseq_to_vector(insurRef.c_str(), character_v);

		vector<string> SentPOS_v;
		if(CRF_POS_Flag){
			Generate_CRF_POS(insurRef.c_str(), SentPOS_v);
		}
		else if(CEDT_Word_Boundary_Flag){
			Generate_Word_Boundary_Info(insurRef.c_str(), SentPOS_v, "BMM");
		}
		//:head
		map<size_t, map<size_t, string>> mention_mm;
		map<size_t, map<size_t, string>> TYPE_mm;
		if(nomial_Flag){
			Get_Entity_Mention_TYPE_Map((*ne_vite)->entity_mention_v, mention_mm);
		}
		else if(CEDT_Head_Flag){
			Get_Entity_Mention_head_Map((*ne_vite)->entity_mention_v, mention_mm);
		}
		else{
			Get_Entity_Mention_extend_Map((*ne_vite)->entity_mention_v, mention_mm);
		}

		Get_Entity_Mention_TYPE_Map((*ne_vite)->entity_mention_v, mention_mm, TYPE_mm);

		if(character_v.size() == 0){
			continue;
		}
		//---------------reverse---------------------------------------------
		vector<string> r_character_v;
		vector<string> r_SentPOS_v;
		for(vector<string>::reverse_iterator rvite = character_v.rbegin(); rvite != character_v.rend(); rvite++){
			r_character_v.push_back(rvite->data());
		}
		for(vector<string>::reverse_iterator rvite = SentPOS_v.rbegin(); rvite != SentPOS_v.rend(); rvite++){
			r_SentPOS_v.push_back(rvite->data());
		}
		map<size_t, map<size_t, string>> r_mention_mm;
		size_t SLength = NLPOP::Get_Chinese_Sentence_Length_Counter(insurRef.c_str());

		for(map<size_t, map<size_t, string>>::iterator mmite = mention_mm.begin(); mmite != mention_mm.end(); mmite++){
			for(map<size_t, string>::iterator mite = mmite->second.begin(); mite != mmite->second.end(); mite++){
				size_t START = SLength-mmite->first-1;
				size_t END = SLength-mite->first-1;
				if(r_mention_mm.find(END) == r_mention_mm.end()){
					r_mention_mm[END];
				}
				if(r_mention_mm[END].find(START) == r_mention_mm[END].end()){
					r_mention_mm[END].insert(make_pair(START, mite->second));
				}
				else{
					AppCall::Secretary_Message_Box("Duplicated Data in: Output_NE_Surround_for_CRF_Reverse_Sequence()", MB_OK);
				}
			}
		}
		///-----------------------------------------------
		string tempstr;
		bool Outputed_Flag = false;
		for(size_t i = 0; i < r_character_v.size(); ){
			if(!strcmp(r_character_v[i].c_str(), "\n") || !strcmp(r_character_v[i].c_str(), " ")){
				i++;
				continue;
			}
			if(r_mention_mm.find(i) != r_mention_mm.end()){

				vector<string> mention_v;
				if(CRF_Max_Flag){
					NLPOP::charseq_to_vector(r_mention_mm[i].rbegin()->second.c_str(), mention_v);
				}
				else{
					NLPOP::charseq_to_vector(r_mention_mm[i].begin()->second.c_str(), mention_v);
				}
				//----------------reverse
				vector<string> r_mention_v;
				for(vector<string>::reverse_iterator rvite = mention_v.rbegin(); rvite != mention_v.rend(); rvite++){
					r_mention_v.push_back(rvite->data());
				}

				size_t x = 0;
				if(!r_mention_mm.empty()){
					tempstr += r_mention_v[x];//-------
					if(!(CRF_POS_Flag || CEDT_Word_Boundary_Flag)){
						out << r_mention_v[x] << "\t" << 'B' << endl;
					}
					else{
						if(r_SentPOS_v[i+x].length() == 0){
							AppCall::Secretary_Message_Box("Missing POS tag: in CCRF::Output_NE_Surround_for_CRF() 1", MB_OK);
							r_SentPOS_v[i+x] == "POS";
						}
						out << r_mention_v[x] << "\t" << r_SentPOS_v[i+x] << "\t" << 'B' << endl;
					}
					if(!Outputed_Flag){
						Outputed_Flag = true;
					}
					x++;
				}
				for( ; x < r_mention_v.size(); x++){
					if(!strcmp(r_mention_v[x].c_str(), "\n") || !strcmp(r_mention_v[x].c_str(), " ")){
						continue;
					}
					tempstr += r_mention_v[x];//-------
					if(!(CRF_POS_Flag || CEDT_Word_Boundary_Flag)){
						out << r_mention_v[x] << "\t" << 'I' << endl;
					}
					else{
						if(r_SentPOS_v[i+x].length() == 0){
							AppCall::Secretary_Message_Box("Missing POS tag: in CCRF::Output_NE_Surround_for_CRF() 2", MB_OK);
							r_SentPOS_v[i+x] == "POS";
						}
						out << r_mention_v[x] << "\t" << r_SentPOS_v[i+x] << "\t" << 'I' << endl;
					}
					if(!Outputed_Flag){
						Outputed_Flag = true;
					}
				}
				i += x;
			}
			else{
				tempstr += r_character_v[i];//-------
				if(!(CRF_POS_Flag || CEDT_Word_Boundary_Flag)){
					out << r_character_v[i] << "\t" << 'O' << endl;
				}
				else{
					if(r_SentPOS_v[i].length() == 0){
						AppCall::Secretary_Message_Box("Missing POS tag: in CCRF::Output_NE_Surround_for_CRF() 3", MB_OK);
						r_SentPOS_v[i] == "POS";
					}
					out << r_character_v[i] << "\t" << r_SentPOS_v[i] << "\t" << 'O' << endl;
				}
				if(!Outputed_Flag){
					Outputed_Flag = true;
				}
				i++;
			}
		}
		if(Outputed_Flag){
			out << endl;
		}
	}
	out.close();
}
void CCRF::Local_Feature_Extracting_for_CRF(const char* mentionchar, vector<vector<string>*>& pmFeatur_vv)
{
	string inputstr = ace_op::Delet_0AH_and_20H_in_string(mentionchar);
	if(strlen(inputstr.c_str()) == 0){
		return;
	}

	vector<string> character_v;
	vector<string> Seg_v;

	Generate_Word_Boundary_Info(mentionchar, Seg_v, "FMM");
	NLPOP::charseq_to_vector(mentionchar, character_v);

	if(Seg_v.size() != character_v.size()){
		AppCall::Secretary_Message_Box("Local_Feature_Extracting_for_CRF", MB_OK);
	}

	string prixchar, proxchar;
	string C0, C10, C01;
	string leftseg, rightseg;
	for(size_t i = 0; i < character_v.size(); ){
		vector<string>* ploc_v = new vector<string>;
		pmFeatur_vv.push_back(ploc_v);
		if(!strcmp(character_v[i].c_str(), "\n") || !strcmp(character_v[i].c_str(), " ")){
			i++;
			continue;
		}
		for(size_t j = i+1; j < character_v.size(); j++){
			if(!strcmp(character_v[j].c_str(), "\n") || !strcmp(character_v[j].c_str(), " ")){
				continue;
			}
			proxchar = character_v[j];
			rightseg = Seg_v[j];
			break;
		}
		C0 = character_v[i];
		C10 = prixchar + C0;
		C01 = C0 +proxchar;
		//---Is_Sur(C_0)-------------------------------------------------------1
		if(Surname_s.find(C0) != Surname_s.end()){
			ploc_v->push_back("1");
		}
		else{
			ploc_v->push_back("0");
		}
		//---Is_Pro(C_0)-------------------------------------------------------2
		if(Pronoun_s.find(C0) != Pronoun_s.end()){
			ploc_v->push_back("1");
		}
		else{
			ploc_v->push_back("0");
		}
		//---Is_Pro(C_10)-------------------------------------------------------3
		if(Pronoun_s.find(C10) != Pronoun_s.end()){
			ploc_v->push_back("1");
		}
		else{
			ploc_v->push_back("0");
		}
		//---Is_Pro(C_01)-------------------------------------------------------4
		if(Pronoun_s.find(C01) != Pronoun_s.end()){
			ploc_v->push_back("1");
		}
		else{
			ploc_v->push_back("0");
		}
		bool C10_Flag = false;
		bool C01_Flag = false;

		//---In_Loc(C_10)-------------------------------------------------------5
		if(Locname_s.find(C10) != Locname_s.end()){
			ploc_v->push_back("1");
		}
		else{
			ploc_v->push_back("0");
		}
		//---In_Loc(C_01)-------------------------------------------------------6
		if(Locname_s.find(C01) != Locname_s.end()){
			ploc_v->push_back("1");
		}
		else{
			ploc_v->push_back("0");
		}
		//---Seg wE------------------------------------------------------------7
		if(leftseg == "wE"){
			ploc_v->push_back("1");
		}
		else{
			ploc_v->push_back("0");
		}
		//---Seg Begin----------------------------------------------------------8
		if(rightseg == "wB"){
			ploc_v->push_back("1");
		}
		else{
			ploc_v->push_back("0");
		}
		//------------------------------------------------
		prixchar = C0;
		leftseg = Seg_v[i];
		i++;

	}

	if(pmFeatur_vv.size() != character_v.size()){
		AppCall::Secretary_Message_Box("Local_Feature_Extracting_for_CRF", MB_OK);
		return;
	}

}


void CCRF::Output_NE_Surround_for_CRF_Reverse_with_Local_Feature(string charpathstr, vector<NE_Surround*>& Surround_v)
{

	ofstream out(charpathstr.c_str());
	if(out.bad()){
		return;
	}
	out.clear();
	out.seekp(0, ios::beg);

	for(vector<NE_Surround*>::iterator ne_vite = Surround_v.begin(); ne_vite != Surround_v.end(); ne_vite++){
		vector<string> character_v;
		set<size_t> START_s;
		set<size_t> END_s;
		string& insurRef = (*ne_vite)->insur;
		NLPOP::charseq_to_vector(insurRef.c_str(), character_v);

		vector<vector<string>*> locFeatur_vv;
		Local_Feature_Extracting_for_CRF(insurRef.c_str(), locFeatur_vv);

		//:head
		map<size_t, map<size_t, string>> mention_mm;
		if(CEDT_Head_Flag){
			Get_Entity_Mention_head_Map((*ne_vite)->entity_mention_v, mention_mm);
		}
		else{
			Get_Entity_Mention_extend_Map((*ne_vite)->entity_mention_v, mention_mm);
		}
		if(character_v.size() == 0){
			continue;
		}
		//---------------reverse---------------------------------------------
		vector<vector<string>*> r_locFeatur_vv;
		for(vector<vector<string>*>::reverse_iterator rvite = locFeatur_vv.rbegin(); rvite != locFeatur_vv.rend(); rvite++){
			r_locFeatur_vv.push_back(*rvite);
		}
		
		vector<string> r_character_v;
		for(vector<string>::reverse_iterator rvite = character_v.rbegin(); rvite != character_v.rend(); rvite++){
			r_character_v.push_back(rvite->data());
		}

		map<size_t, map<size_t, string>> r_mention_mm;
		size_t SLength = NLPOP::Get_Chinese_Sentence_Length_Counter(insurRef.c_str());

		for(map<size_t, map<size_t, string>>::iterator mmite = mention_mm.begin(); mmite != mention_mm.end(); mmite++){
			for(map<size_t, string>::iterator mite = mmite->second.begin(); mite != mmite->second.end(); mite++){
				size_t START = SLength-mmite->first-1;
				size_t END = SLength-mite->first-1;
				if(r_mention_mm.find(END) == r_mention_mm.end()){
					r_mention_mm[END];
				}
				if(r_mention_mm[END].find(START) == r_mention_mm[END].end()){
					r_mention_mm[END].insert(make_pair(START, mite->second));
				}
				else{
					AppCall::Secretary_Message_Box("Duplicated Data in: Output_NE_Surround_for_CRF_Reverse_with_Local_Feature()", MB_OK);
				}
			}
		}
		///-----------------------------------------------
		string tempstr;
		bool Outputed_Flag = false;
		for(size_t i = 0; i < r_character_v.size(); ){
			if(!strcmp(r_character_v[i].c_str(), "\n") || !strcmp(r_character_v[i].c_str(), " ")){
				i++;
				continue;
			}
			if(r_mention_mm.find(i) != r_mention_mm.end()){

				vector<string> mention_v;
				if(CRF_Max_Flag){
					NLPOP::charseq_to_vector(r_mention_mm[i].rbegin()->second.c_str(), mention_v);
				}
				else{
					NLPOP::charseq_to_vector(r_mention_mm[i].begin()->second.c_str(), mention_v);
				}
				//----------------reverse
				vector<string> r_mention_v;
				for(vector<string>::reverse_iterator rvite = mention_v.rbegin(); rvite != mention_v.rend(); rvite++){
					r_mention_v.push_back(rvite->data());
				}

				size_t x = 0;
				if(!r_mention_mm.empty()){
					tempstr += r_mention_v[x];//-------

					if(r_locFeatur_vv[i+x]->size() != 8){
						AppCall::Secretary_Message_Box("Missing POS tag: in CCRF::Output_NE_Surround_for_CRF() 1", MB_OK);
					}
					out << r_mention_v[x] << "\t";
					for(vector<string>::iterator rfite = r_locFeatur_vv[i+x]->begin(); rfite != r_locFeatur_vv[i+x]->end(); rfite++){
						out << rfite->data() << "\t";
					}
					out <<  'B' << endl;

					if(!Outputed_Flag){
						Outputed_Flag = true;
					}
					x++;
				}
				for( ; x < r_mention_v.size(); x++){
					if(!strcmp(r_mention_v[x].c_str(), "\n") || !strcmp(r_mention_v[x].c_str(), " ")){
						continue;
					}
					tempstr += r_mention_v[x];//-------

					if(r_locFeatur_vv[i+x]->size() != 8){
						AppCall::Secretary_Message_Box("Missing POS tag: in CCRF::Output_NE_Surround_for_CRF() 1", MB_OK);
					}
					out << r_mention_v[x] << "\t";
					for(vector<string>::iterator rfite = r_locFeatur_vv[i+x]->begin(); rfite != r_locFeatur_vv[i+x]->end(); rfite++){
						out << rfite->data() << "\t";
					}
					out <<  'I' << endl;//-------------------------'I', 'O'

					if(!Outputed_Flag){
						Outputed_Flag = true;
					}
				}
				i += x;
			}
			else{
				tempstr += r_character_v[i];//-------

				if(r_locFeatur_vv[i]->size() != 8){
					AppCall::Secretary_Message_Box("Missing POS tag: in CCRF::Output_NE_Surround_for_CRF() 1", MB_OK);
				}
				out << r_character_v[i] << "\t";
				for(vector<string>::iterator rfite = r_locFeatur_vv[i]->begin(); rfite != r_locFeatur_vv[i]->end(); rfite++){
					out << rfite->data() << "\t";
				}
				out <<  'O' << endl;

				if(!Outputed_Flag){
					Outputed_Flag = true;
				}
				i++;
			}
		}
		if(Outputed_Flag){
			out << endl;
		}
		for(size_t i = 0; i < locFeatur_vv.size(); i++){
			delete locFeatur_vv[i];
		}
		locFeatur_vv.clear();
	}
	out.close();
}
void CCRF::START_or_END_Boundary_Detection_Perforamnce(vector<NE_Surround*>& Surround_v, CEDTInfo& pmCEDTInfo)
{
	map<size_t, map<size_t, double>*>& START_mm = CEDT_Head_Flag?pmCEDTInfo.Head_START_mm:pmCEDTInfo.Extend_START_mm;
	map<size_t, map<size_t, double>*>& END_mm = CEDT_Head_Flag?pmCEDTInfo.Head_END_mm:pmCEDTInfo.Extend_END_mm;
	map<size_t, map<size_t, double>*>& Bound_mm =  CRF_START_or_END_DetBound_Flag?START_mm:END_mm;

	map<string, pair<size_t, size_t>> Recall_map;
	map<string, pair<size_t, size_t>> Pricision_map;

	Recall_map.insert(make_pair(POSITIVE, make_pair(0, 0)));
	Recall_map.insert(make_pair(NEGETIVE, make_pair(0, 0)));
	Pricision_map.insert(make_pair(POSITIVE, make_pair(0, 0)));
	Pricision_map.insert(make_pair(NEGETIVE, make_pair(0, 0)));

	for(vector<NE_Surround*>::iterator ne_vite = Surround_v.begin(); ne_vite != Surround_v.end(); ne_vite++){
		vector<string> character_v;
		NLPOP::charseq_to_vector((*ne_vite)->insur.c_str(), character_v);
		set<size_t> START_s;
		set<size_t> END_s;
		if(character_v.size() == 0){
			continue;
		}
		if(Bound_mm.find((*ne_vite)->SENID) == Bound_mm.end()){
			continue;
		}
		map<size_t, map<size_t, string>> mention_mm;
		if(CEDT_Head_Flag){
			Get_Entity_Mention_head_Map((*ne_vite)->entity_mention_v, mention_mm);
		}
		else{
			Get_Entity_Mention_extend_Map((*ne_vite)->entity_mention_v, mention_mm);
		}
		for(map<size_t, map<size_t, string>>::iterator mmite = mention_mm.begin(); mmite != mention_mm.end(); mmite++){
			START_s.insert(mmite->first);
			for(map<size_t, string>::iterator mite = mmite->second.begin(); mite != mmite->second.end(); mite++){
				END_s.insert(mite->first);
			}
		}
		/*Get_Entity_Mention_head_Map((*ne_vite)->entity_mention_v, mention_mm);
		for(map<size_t, map<size_t, string>>::iterator mmite = mention_mm.begin(); mmite != mention_mm.end(); mmite++){
			START_s.insert(mmite->first);
			for(map<size_t, string>::iterator mite = mmite->second.begin(); mite != mmite->second.end(); mite++){
				END_s.insert(mite->first);
			}
		}
		mention_mm.clear();
		Get_Entity_Mention_extend_Map((*ne_vite)->entity_mention_v, mention_mm);
		for(map<size_t, map<size_t, string>>::iterator mmite = mention_mm.begin(); mmite != mention_mm.end(); mmite++){
			START_s.insert(mmite->first);
			for(map<size_t, string>::iterator mite = mmite->second.begin(); mite != mmite->second.end(); mite++){
				END_s.insert(mite->first);
			}
		}*/

		set<size_t>& Bound_s = CRF_START_or_END_DetBound_Flag?START_s:END_s;
		Recall_map[POSITIVE].second += Bound_s.size();

		map<size_t, double>& SENID_Bound_m = *Bound_mm[(*ne_vite)->SENID];
		for(size_t i = 0; i < character_v.size(); ){
			if(!strcmp(character_v[i].c_str(), "\n") || !strcmp(character_v[i].c_str(), " ")){
				i++;
				continue;
			}
			if(SENID_Bound_m[i] > 0.5){
				Pricision_map[POSITIVE].second++;
				if(Bound_s.find(i) != Bound_s.end()){
					Pricision_map[POSITIVE].first++;
					Recall_map[POSITIVE].first++;
				}
			}
			i++;
		}
	}
	if(CRF_START_or_END_DetBound_Flag){
		AppCall::Maxen_Responce_Message("\n\nCRF START Detection...\n\n");
	}
	else{
		AppCall::Maxen_Responce_Message("\n\nCRF END Detection...\n\n");
	}

	MAXEN::Display_Performance_for_MAXEN(true, true, Pricision_map, Recall_map, "\n****************************************\n\nThe n Cross Performance...\n");
	
	MAXEN::Collect_nGross_Performances(Pricision_map, Recall_map, P_nCrossRtn_m, R_nCrossRtn_m);

}

void CCRF::Output_NE_Surround_for_START_or_END_Boundary_with_Local_Feature(string charpathstr, vector<NE_Surround*>& Surround_v)
{
	ofstream out(charpathstr.c_str());
	if(out.bad()){
		return;
	}
	out.clear();
	out.seekp(0, ios::beg);

	for(vector<NE_Surround*>::iterator ne_vite = Surround_v.begin(); ne_vite != Surround_v.end(); ne_vite++){
		vector<string> character_v;
		set<size_t> START_s;
		set<size_t> END_s;
		string& insurRef = (*ne_vite)->insur;
		NLPOP::charseq_to_vector(insurRef.c_str(), character_v);
		if(character_v.size() == 0){
			continue;
		}
		vector<vector<string>*> locFeatur_vv;
		Local_Feature_Extracting_for_CRF(insurRef.c_str(), locFeatur_vv);
		//:head
		map<size_t, map<size_t, string>> mention_mm;
		if(CEDT_Head_Flag){
			Get_Entity_Mention_head_Map((*ne_vite)->entity_mention_v, mention_mm);
		}
		else{
			Get_Entity_Mention_extend_Map((*ne_vite)->entity_mention_v, mention_mm);
		}
		for(map<size_t, map<size_t, string>>::iterator mmite = mention_mm.begin(); mmite != mention_mm.end(); mmite++){
			START_s.insert(mmite->first);
			for(map<size_t, string>::iterator mite = mmite->second.begin(); mite != mmite->second.end(); mite++){
				END_s.insert(mite->first);
			}
		}
		/*Get_Entity_Mention_head_Map((*ne_vite)->entity_mention_v, mention_mm);
		for(map<size_t, map<size_t, string>>::iterator mmite = mention_mm.begin(); mmite != mention_mm.end(); mmite++){
			START_s.insert(mmite->first);
			for(map<size_t, string>::iterator mite = mmite->second.begin(); mite != mmite->second.end(); mite++){
				END_s.insert(mite->first);
			}
		}
		mention_mm.clear();
		Get_Entity_Mention_extend_Map((*ne_vite)->entity_mention_v, mention_mm);
		for(map<size_t, map<size_t, string>>::iterator mmite = mention_mm.begin(); mmite != mention_mm.end(); mmite++){
			START_s.insert(mmite->first);
			for(map<size_t, string>::iterator mite = mmite->second.begin(); mite != mmite->second.end(); mite++){
				END_s.insert(mite->first);
			}
		}*/

		//string tempstr;
		bool Outputed_Flag = false;
		for(size_t i = 0; i < character_v.size(); ){
			if(!strcmp(character_v[i].c_str(), "\n") || !strcmp(character_v[i].c_str(), " ")){
				i++;
				continue;
			}
			out << character_v[i] << '\t';
			if(locFeatur_vv[i]->size() != 8){
				AppCall::Secretary_Message_Box("Missing POS tag: in CCRF::Output_NE_Surround_for_CRF_Routine_Sequence() 2", MB_OK);
			}
			for(vector<string>::iterator fite = locFeatur_vv[i]->begin(); fite != locFeatur_vv[i]->end(); fite++){
				out << fite->data() << "\t";
			}
			if(CRF_START_or_END_DetBound_Flag && (START_s.find(i) != START_s.end())){
				out <<  'B' << endl;//------------'I' or 'O'
			}
			else if(!CRF_START_or_END_DetBound_Flag && END_s.find(i) != END_s.end()){
				out <<  'B' << endl;//------------'I' or 'O'
			}
			else{
				out <<  'O' << endl;
			}
			if(!Outputed_Flag){
				Outputed_Flag = true;
			}
			i++;
		}
		if(Outputed_Flag){
			out << endl;
		}
		for(size_t i = 0; i < locFeatur_vv.size(); i++){
			delete locFeatur_vv[i];
		}
		locFeatur_vv.clear();
	}
	out.close();
}

void CCRF::Output_NE_Surround_for_CRF_with_Local_Feature(string charpathstr, vector<NE_Surround*>& Surround_v)
{
	ofstream out(charpathstr.c_str());
	if(out.bad()){
		return;
	}
	out.clear();
	out.seekp(0, ios::beg);

	for(vector<NE_Surround*>::iterator ne_vite = Surround_v.begin(); ne_vite != Surround_v.end(); ne_vite++){
		vector<string> character_v;
		set<size_t> START_s;
		set<size_t> END_s;
		string& insurRef = (*ne_vite)->insur;
		NLPOP::charseq_to_vector(insurRef.c_str(), character_v);
		if(character_v.size() == 0){
			continue;
		}
		vector<vector<string>*> locFeatur_vv;
		Local_Feature_Extracting_for_CRF(insurRef.c_str(), locFeatur_vv);
		//:head
		map<size_t, map<size_t, string>> mention_mm;
		map<size_t, map<size_t, string>> TYPE_mm;
		//if(nomial_Flag){
		//	Get_Entity_Mention_TYPE_Map((*ne_vite)->entity_mention_v, mention_mm);
		//}
		//else 
		if(CEDT_Head_Flag){
			Get_Entity_Mention_head_Map((*ne_vite)->entity_mention_v, mention_mm);
		}
		else{
			Get_Entity_Mention_extend_Map((*ne_vite)->entity_mention_v, mention_mm);
		}
		if(CRF_Multi_TYPE_Flag){
			Get_Entity_Mention_TYPE_Map((*ne_vite)->entity_mention_v, mention_mm, TYPE_mm);
		}
		//string tempstr;
		bool Outputed_Flag = false;
		for(size_t i = 0; i < character_v.size(); ){
			if(!strcmp(character_v[i].c_str(), "\n") || !strcmp(character_v[i].c_str(), " ")){
				i++;
				continue;
			}
			if(mention_mm.find(i) != mention_mm.end()){
				vector<string> mention_v;
				if(CRF_Max_Flag){
					NLPOP::charseq_to_vector(mention_mm[i].rbegin()->second.c_str(), mention_v);
				}
				else{
					NLPOP::charseq_to_vector(mention_mm[i].begin()->second.c_str(), mention_v);
				}
				size_t x = 0;
				//-----for end detection
				if(!mention_mm.empty()){
					//tempstr += mention_v[x];//-------
					if(locFeatur_vv[i+x]->size() != 8){
						AppCall::Secretary_Message_Box("Missing POS tag: in CCRF::Output_NE_Surround_for_CRF() 1", MB_OK);
					}
					out << mention_v[x] << "\t";
					for(vector<string>::iterator fite = locFeatur_vv[i+x]->begin(); fite != locFeatur_vv[i+x]->end(); fite++){
						out << fite->data() << "\t";
					}
					out <<  'B'; 
					if(CRF_Multi_TYPE_Flag){
						if(TYPE_mm.find(i) == TYPE_mm.end()){
							AppCall::Secretary_Message_Box("Missing POS tag: in CCRF::Output_NE_Surround_for_CRF() 1-1", MB_OK);
						}
						size_t offset = i+mention_v.size()-1;
						if(TYPE_mm[i].find(offset) == TYPE_mm[i].end()){
							AppCall::Secretary_Message_Box("Missing POS tag: in CCRF::Output_NE_Surround_for_CRF() 1-2", MB_OK);
						}
						out << "-" << "NON";//TYPE_mm[i][offset];
					}
					out << endl;;
						
					if(!Outputed_Flag){
						Outputed_Flag = true;
					}
					x++;
				}
				for( ; x < mention_v.size(); x++){
					if(!strcmp(mention_v[x].c_str(), "\n") || !strcmp(mention_v[x].c_str(), " ")){
						continue;
					}
					//tempstr += mention_v[x];//-------
					if(locFeatur_vv[i+x]->size() != 8){
						AppCall::Secretary_Message_Box("Missing POS tag: in CCRF::Output_NE_Surround_for_CRF_Routine_Sequence() 2", MB_OK);
					}
					out << mention_v[x] << "\t";
					for(vector<string>::iterator fite = locFeatur_vv[i+x]->begin(); fite != locFeatur_vv[i+x]->end(); fite++){
						out << fite->data() << "\t";
					}
					out <<  'I';//------------'I' or 'O'
					if(CRF_Multi_TYPE_Flag){
						if(TYPE_mm.find(i) == TYPE_mm.end()){
							AppCall::Secretary_Message_Box("Missing POS tag: in CCRF::Output_NE_Surround_for_CRF() 1-1", MB_OK);
						}
						size_t offset = i+mention_v.size()-1;
						if(TYPE_mm[i].find(offset) == TYPE_mm[i].end()){
							AppCall::Secretary_Message_Box("Missing POS tag: in CCRF::Output_NE_Surround_for_CRF() 1-2", MB_OK);
						}
						out << "-" << "NON";//TYPE_mm[i][offset];
					}
					out << endl;;

					if(!Outputed_Flag){
						Outputed_Flag = true;
					}
				}
				i += x;
			}
			else{
				//tempstr += character_v[i];//
				if(locFeatur_vv[i]->size() != 8){
					AppCall::Secretary_Message_Box("Missing POS tag: in CCRF::Output_NE_Surround_for_CRF_Routine_Sequence() 3", MB_OK);
				}
				out << character_v[i] << "\t";
				for(vector<string>::iterator fite = locFeatur_vv[i]->begin(); fite != locFeatur_vv[i]->end(); fite++){
					out << fite->data() << "\t";
				}
				out <<  'O' << endl;
				
				if(!Outputed_Flag){
					Outputed_Flag = true;
				}
				i++;
			}
		}
		if(Outputed_Flag){
			out << endl;
		}
		for(size_t i = 0; i < locFeatur_vv.size(); i++){
			delete locFeatur_vv[i];
		}
		locFeatur_vv.clear();
	}
	out.close();
}

/*void CCRF::Output_NE_Surround_for_CRF_Routine_Sequence(string charpathstr, vector<NE_Surround*>& Surround_v, bool nomial_Flag)
{
	ofstream out(charpathstr.c_str());
	if(out.bad()){
		return;
	}
	out.clear();
	out.seekp(0, ios::beg);

	for(vector<NE_Surround*>::iterator ne_vite = Surround_v.begin(); ne_vite != Surround_v.end(); ne_vite++){
		vector<string> character_v;
		set<size_t> START_s;
		set<size_t> END_s;
		string& insurRef = (*ne_vite)->insur;
		NLPOP::charseq_to_vector(insurRef.c_str(), character_v);
		if(character_v.size() == 0){
			continue;
		}

		vector<string> SentPOS_v;
		if(CRF_POS_Flag){
			Generate_CRF_POS(insurRef.c_str(), SentPOS_v);
		}
		else if(CEDT_Word_Boundary_Flag){
			Generate_Word_Boundary_Info(insurRef.c_str(), SentPOS_v, "FMM");
		}
		//:head
		map<size_t, map<size_t, string>> mention_mm;
		if(nomial_Flag){
			Get_Entity_Mention_TYPE_Map((*ne_vite)->entity_mention_v, mention_mm);
		}
		else if(CEDT_Head_Flag){
			Get_Entity_Mention_head_Map((*ne_vite)->entity_mention_v, mention_mm);
		}
		else{
			Get_Entity_Mention_extend_Map((*ne_vite)->entity_mention_v, mention_mm);
		}

		
		string tempstr;
		bool Outputed_Flag = false;
		for(size_t i = 0; i < character_v.size(); ){
			if(!strcmp(character_v[i].c_str(), "\n") || !strcmp(character_v[i].c_str(), " ")){
				i++;
				continue;
			}
			if(mention_mm.find(i) != mention_mm.end()){
				vector<string> mention_v;
				if(CRF_Max_Flag){
					NLPOP::charseq_to_vector(mention_mm[i].rbegin()->second.c_str(), mention_v);
				}
				else{
					NLPOP::charseq_to_vector(mention_mm[i].begin()->second.c_str(), mention_v);
				}
				size_t x = 0;
				if(!mention_mm.empty()){
					tempstr += mention_v[x];//-------
					if(!(CRF_POS_Flag || CEDT_Word_Boundary_Flag)){
						out << mention_v[x] << "\t" << 'B' << endl;
					}
					else{
						if(SentPOS_v[i+x].length() == 0){
							AppCall::Secretary_Message_Box("Missing POS tag: in CCRF::Output_NE_Surround_for_CRF() 1", MB_OK);
							SentPOS_v[i+x] == "POS";
						}
						out << mention_v[x] << "\t" << SentPOS_v[i+x] << "\t" << 'B' << endl;
					}
					if(!Outputed_Flag){
						Outputed_Flag = true;
					}
					x++;
				}
				for( ; x < mention_v.size(); x++){
					if(!strcmp(mention_v[x].c_str(), "\n") || !strcmp(mention_v[x].c_str(), " ")){
						continue;
					}
					tempstr += mention_v[x];//-------
					if(!(CRF_POS_Flag || CEDT_Word_Boundary_Flag)){
						out << mention_v[x] << "\t" << 'I' << endl;
					}
					else{
						if(SentPOS_v[i+x].length() == 0){
							AppCall::Secretary_Message_Box("Missing POS tag: in CCRF::Output_NE_Surround_for_CRF_Routine_Sequence() 2", MB_OK);
							SentPOS_v[i+x] == "POS";
						}
						out << mention_v[x] << "\t" << SentPOS_v[i+x] << "\t" << 'I' << endl;
					}
					if(!Outputed_Flag){
						Outputed_Flag = true;
					}
				}
				i += x;
			}
			else{
				tempstr += character_v[i];//-------
				if(!(CRF_POS_Flag || CEDT_Word_Boundary_Flag)){
					out << character_v[i] << "\t" << 'O' << endl;
				}
				else{
					if(SentPOS_v[i].length() == 0){
						AppCall::Secretary_Message_Box("Missing POS tag: in CCRF::Output_NE_Surround_for_CRF_Routine_Sequence() 3", MB_OK);
						SentPOS_v[i] == "POS";
					}
					out << character_v[i] << "\t" << SentPOS_v[i] << "\t" << 'O' << endl;
				}
				if(!Outputed_Flag){
					Outputed_Flag = true;
				}
				i++;
			}
		}
		if(Outputed_Flag){
			out << endl;
		}
	}
	out.close();
}*/

void CCRF::Get_Entity_Mention_extend_Map(vector<ACE_entity_mention>& EntityMention_v, map<size_t, map<size_t, string>>& mention_mm)
{
	for(vector<ACE_entity_mention>::iterator pvite = EntityMention_v.begin(); pvite != EntityMention_v.end(); pvite++){	
		if((Detect_Single_NE_TYPE.length() != 0) && strcmp(pvite->Entity_TYPE.c_str(), Detect_Single_NE_TYPE.c_str())){
			continue;
		}
		if(mention_mm.find(pvite->extent.START) == mention_mm.end()){
			mention_mm[pvite->extent.START];
		}
		if(mention_mm[pvite->extent.START].find(pvite->extent.END) == mention_mm[pvite->extent.START].end()){
			mention_mm[pvite->extent.START][pvite->extent.END];
		}
		else{
			//AppCall::Secretary_Message_Box("Duplicated Data in CCRF::Get_Entity_Mention_extent_Map()", MB_OK);
			continue;
		}
		mention_mm[pvite->extent.START][pvite->extent.END] = pvite->extent.charseq;
	}
}


void CCRF::Get_Entity_Mention_head_Map(vector<ACE_entity_mention>& EntityMention_v, map<size_t, map<size_t, string>>& mention_mm)
{
	for(vector<ACE_entity_mention>::iterator pvite = EntityMention_v.begin(); pvite != EntityMention_v.end(); pvite++){	
		if((Detect_Single_NE_TYPE.length() != 0) && strcmp(pvite->Entity_TYPE.c_str(), Detect_Single_NE_TYPE.c_str())){
			continue;
		}
		if(mention_mm.find(pvite->head.START) == mention_mm.end()){
			mention_mm[pvite->head.START];
		}
		if(mention_mm[pvite->head.START].find(pvite->head.END) == mention_mm[pvite->head.START].end()){
			mention_mm[pvite->head.START][pvite->head.END];
		}
		else{
			//AppCall::Secretary_Message_Box("Duplicated Data in CCRF::Get_Entity_Mention_head_Map()", MB_OK);
			continue;
		}
		mention_mm[pvite->head.START][pvite->head.END] = pvite->head.charseq;
	}
}
void CCRF::Get_Entity_Mention_TYPE_Map(vector<ACE_entity_mention>& EntityMention_v, map<size_t, map<size_t, string>>& TYPE_mm)
{
	for(vector<ACE_entity_mention>::iterator pvite = EntityMention_v.begin(); pvite != EntityMention_v.end(); pvite++){	
		if((Detect_Single_NE_TYPE.length() != 0) && strcmp(pvite->Entity_TYPE.c_str(), Detect_Single_NE_TYPE.c_str())){
			continue;
		}
		if(strcmp(pvite->TYPE.c_str(), "NOM")){
			continue;
		}
		if(TYPE_mm.find(pvite->head.START) == TYPE_mm.end()){
			TYPE_mm[pvite->head.START];
		}
		if(TYPE_mm[pvite->head.START].find(pvite->head.END) == TYPE_mm[pvite->head.START].end()){
			TYPE_mm[pvite->head.START][pvite->head.END];
		}
		else{
			//AppCall::Secretary_Message_Box("Duplicated Data in CCRF::Get_Entity_Mention_head_Map()", MB_OK);
			continue;
		}
	
		TYPE_mm[pvite->head.START][pvite->head.END] = pvite->head.charseq;
	}
}
void CCRF::Get_Entity_Mention_TYPE_Map(vector<ACE_entity_mention>& EntityMention_v, map<size_t, map<size_t, string>>& mention_mm, map<size_t, map<size_t, string>>& TYPE_mm)
{
	for(vector<ACE_entity_mention>::iterator pvite = EntityMention_v.begin(); pvite != EntityMention_v.end(); pvite++){
		size_t START = CEDT_Head_Flag?pvite->head.START:pvite->extent.START;
		size_t END = CEDT_Head_Flag?pvite->head.END:pvite->extent.END;

		if(mention_mm.find(START) == mention_mm.end()){
			continue;
		}
		TYPE_mm[START];
		if(mention_mm[START].find(END) == mention_mm[START].end()){
			continue;
		}
		TYPE_mm[START][END];
		TYPE_mm[START][END] = pvite->Entity_TYPE;
	}
}



//**********************************************************************************************************
//*************************************************** PFR
void CCRF::PFR_Port()
{
	NEs_s.insert("nt");
	NEs_s.insert("ns");
	NEs_s.insert("nr");

	PBreakpoint_s.insert("¡£");
	PBreakpoint_s.insert("£¡");
	PBreakpoint_s.insert("£¿");
	PBreakpoint_s.insert("£»");
	PBreakpoint_s.insert("£¬");

	DelHowRate = 0.1;
	DelHighRate = 0.1;
	CRF_POS_Flag = false;
	CEDT_Word_Boundary_Flag = false;

	vector<vector<pair<string, string>>*> loc_PFRcorpus_v;

	string sPRF_Path = CRF_PFR_FOLDER;
	sPRF_Path += "199801.txt";
	Open_PFR(sPRF_Path.c_str(), loc_PFRcorpus_v);
	
	Merging_Nested_NE_of_PFR(loc_PFRcorpus_v, mergedPFRcorpus_v);
	//random_shuffle(mergedPFRcorpus_v.begin(), mergedPFRcorpus_v.end());

	//sPRF_Path = CRF_PFR_FOLDER;
	//sPRF_Path += "context_info.txt";
	Collecting_Named_Entity_Info(sPRF_Path.c_str(), mergedPFRcorpus_v);

	size_t step = mergedPFRcorpus_v.size()/5;
	for(size_t i = 0; i < 5; i++){
		size_t Range = i!=4?(i+1)*step:mergedPFRcorpus_v.size();
		vector<vector<pair<string, string>>*> training_v;
		vector<vector<pair<string, string>>*> testing_v;

		for(size_t j = 0; j < mergedPFRcorpus_v.size(); j++){
			if((j >= i*step) && (j < Range) && (step != 0)){
				testing_v.push_back(mergedPFRcorpus_v[j]);
			}
			else{
				training_v.push_back(mergedPFRcorpus_v[j]);
			}
		}
		ostringstream outputFilestream;
		ostringstream inputFilestream;
		ostringstream parameterstream;

		//-------Training
		inputFilestream << CRF_PFR_FOLDER << "Training.txt";

		//Output_Words_PFR_for_CRF(inputFilestream.str().c_str(), training_v.begin(), training_v.end());
		//Output_Characters_PFR_BEGIN_for_CRF(inputFilestream.str().c_str(), training_v.begin(), training_v.end());
		Output_Characters_PFR_END_for_CRF(inputFilestream.str().c_str(), training_v.begin(), training_v.end());

		string Templatestr = "templatePFR";
		parameterstream << Templatestr.c_str() << " " << inputFilestream.str() << " " << "PFR\\Model";
		CRF_Training_Port(parameterstream.str().c_str(), inputFilestream.str().c_str());

		//-------Testing
		outputFilestream.str("");
		inputFilestream.str("");
		parameterstream.str("");

		inputFilestream << CRF_PFR_FOLDER << "Testing.txt";

		//Output_Words_PFR_for_CRF(inputFilestream.str().c_str(), testing_v.begin(), testing_v.end());
		//Output_Characters_PFR_BEGIN_for_CRF(inputFilestream.str().c_str(), testing_v.begin(), testing_v.end());
		Output_Characters_PFR_END_for_CRF(inputFilestream.str().c_str(), testing_v.begin(), testing_v.end());

		parameterstream << CRF_FOLDER << "crf_test.exe" << " " << "-m PFR\\Model" << " " << inputFilestream.str().c_str();
		outputFilestream << CRF_PFR_FOLDER << "output.txt";
		AppCall::Console_Running_with_Output_Redirection(CRF_FOLDER, parameterstream.str().c_str(), outputFilestream.str().c_str());

		vector<pair<string, string>*> CRFResult_v;
		Read_Result_of_Consensus_CRF_Port(outputFilestream.str().c_str(), CRFResult_v);
		Performance_of_Consensus_CRF_Port(CRFResult_v);

		training_v.clear();
		testing_v.clear();
		for(size_t i = 0; i < CRFResult_v.size(); i++){
			delete CRFResult_v[i];
		}
		CRFResult_v.clear();

		//break;
	}

	MAXEN::Display_Performance_for_MAXEN(true, true, P_nCrossRtn_m, R_nCrossRtn_m, "\n****************************************\n\nThe 5 Cross Performance...\n");
}


void CCRF::Collecting_Named_Entity_Info(const char* pathchar, vector<vector<pair<string, string>>*>& pmPFRcorpus_v)
{
	ostringstream outputFilestream;
	outputFilestream << "Pieces of Corpus: " <<  pmPFRcorpus_v.size() << endl;
	size_t nt_cnt = 0;
	size_t ns_cnt = 0;
	size_t nr_cnt = 0;

	for(size_t i = 0; i < pmPFRcorpus_v.size(); i++){
		for(vector<pair<string,string>>::iterator vite = pmPFRcorpus_v[i]->begin(); vite != pmPFRcorpus_v[i]->end(); vite++){
			if(vite->second.find("nt") != -1){
				nt_cnt++;
			}
			else if(vite->second.find("ns") != -1){
				ns_cnt++;
			}
			else if(vite->second.find("nr") != -1){
				nr_cnt++;
			}
		}
	}
	outputFilestream << "nt: " << nt_cnt << endl;
	outputFilestream << "ns: " << ns_cnt << endl;
	outputFilestream << "nr: " << nr_cnt << endl;
	outputFilestream << "Total: " << nt_cnt+ns_cnt+nr_cnt << endl;

	AppCall::Secretary_Message_Box(outputFilestream.str().c_str(), MB_OK);

}



void CCRF::Open_PFR(const char* pathchar, vector<vector<pair<string, string>>*>& pmPFRcorpus_v)
{
	string phrasestr;
	string morphstr;
	char getlineBuf[MAX_SENTENCE];
	ifstream in(pathchar);
	if(in.bad())
		return;
	in.clear();
	in.seekg(0, ios::beg);
	if(in.peek() == EOF){
		in.close();
		return;
	}
	while(true){
		if(in.peek() == EOF){	
			break;
		}
		in.getline(getlineBuf, MAX_SENTENCE, '\n');
		istringstream instream(getlineBuf);
		vector<pair<string, string>>* plocPair_v = new vector<pair<string, string>>;
		while(EOF != instream.peek()){
			while(instream.peek() == ' '){
				instream.getline(getlineBuf, MAX_SENTENCE,' ');
			}
			if(instream.peek() != '['){
				instream.getline(getlineBuf, MAX_SENTENCE,'/');
				phrasestr = getlineBuf;
				instream.getline(getlineBuf, MAX_SENTENCE,' ');
				morphstr = getlineBuf;
				plocPair_v->push_back(make_pair(phrasestr, morphstr));
			}
			else{
				instream.getline(getlineBuf, MAX_SENTENCE,']');
				phrasestr = getlineBuf;
				phrasestr += "]";
				instream.getline(getlineBuf, MAX_SENTENCE,' ');
				morphstr = getlineBuf;
				plocPair_v->push_back(make_pair(phrasestr, morphstr));
			}
		}
		pmPFRcorpus_v.push_back(plocPair_v);
	}
}



void CCRF::Merging_Nested_NE_of_PFR(vector<vector<pair<string, string>>*>& PFRcorpus_v, vector<vector<pair<string, string>>*>& mergedPFRcorpus_v)
{
	//NEs_s.insert("nt");
	//NEs_s.insert("ns");
	//NEs_s.insert("nr");
	char getlineBuf[MAX_SENTENCE];
	string phrasestr;
	string morphstr;
	bool Pre_NEs_Flag = false;
	for(size_t i = 0; i < PFRcorpus_v.size(); i++){
		vector<pair<string, string>>* loc_pair_v = new vector<pair<string, string>>;
		for(vector<pair<string, string>>::iterator vite = PFRcorpus_v[i]->begin(); vite != PFRcorpus_v[i]->end(); vite++){
			if(vite->first.length() == 0){
				//AppCall::Secretary_Message_Box("Data Error: in CGPFR::Output_Words_PFR_for_CRF()", MB_OK);
				continue;
			}
			if(Pre_NEs_Flag && (strcmp(vite->second.c_str(), morphstr.c_str()) || (vite->first.c_str()[0] == '['))){
				loc_pair_v->push_back(make_pair(phrasestr, morphstr));
				phrasestr = "";
				morphstr = "";
			}
			if(vite->first.c_str()[0] != '['){
				if(strcmp(vite->second.c_str(), "nr")){//NEs_s.find(vite->second) == NEs_s.end()){
					loc_pair_v->push_back(make_pair(vite->first, vite->second));
					Pre_NEs_Flag = false;
				}
				else{
					if(!Pre_NEs_Flag){
						phrasestr = vite->first;
						morphstr = vite->second;
						Pre_NEs_Flag = true;
					}
					else{
						phrasestr += vite->first;
					}
				}
			}
			else{
				istringstream instream(vite->first.c_str());
				instream.getline(getlineBuf, MAX_SENTENCE,'[');
				while(EOF != instream.peek()){
					while(instream.peek() == ' '){
						instream.getline(getlineBuf, MAX_SENTENCE,' ');
					}
					instream.getline(getlineBuf, MAX_SENTENCE,'/');
					phrasestr += getlineBuf;
					instream.getline(getlineBuf, MAX_SENTENCE,' ');
				}
				morphstr = vite->second;
				loc_pair_v->push_back(make_pair(phrasestr, morphstr));
				phrasestr = "";
				morphstr = "";
			}
		}
		mergedPFRcorpus_v.push_back(loc_pair_v);
		delete PFRcorpus_v[i];
	}
	PFRcorpus_v.clear();
}

void CCRF::Output_Characters_PFR_BEGIN_for_CRF(const char* pathchar, PFRite STARTite, PFRite ENDite)
{
	ofstream out(pathchar);
	if(out.bad()){
		return;
	}
	out.clear();
	out.seekp(0, ios::beg);

	bool Pre_NEs_Flag = false;
	string pre_morph;
	for( ; STARTite != ENDite; STARTite++){
		for(vector<pair<string, string>>::iterator vite = (*STARTite)->begin(); vite != (*STARTite)->end(); vite++){
			if(vite->first.length() == 0){
				//AppCall::Secretary_Message_Box("Data Error: in CGPFR::Output_Words_PFR_for_CRF()", MB_OK);
				out << endl;
				continue;
			}
			vector<string> char_v;
			NLPOP::charseq_to_vector(vite->first.c_str(), char_v);
			if(NEs_s.find(vite->second) == NEs_s.end()){
				for(size_t i = 0; i < char_v.size(); i++){
					out << char_v[i] << '\t' << "O" << endl;
				}
				Pre_NEs_Flag = false;
				pre_morph = "";
			}
			else{
				if(!Pre_NEs_Flag || pre_morph != vite->second){
					vector<string>::iterator cite = char_v.begin();
					if(cite != char_v.end()){
						out << cite->data() << '\t' << "B" << endl;
						cite++;
					}
					while(cite != char_v.end()){
						out << cite->data() << '\t' << "O" << endl;		//------------O or I
						cite++;
					}
					Pre_NEs_Flag = true;
					pre_morph = vite->second;
				}
				else{
					for(size_t i = 0; i < char_v.size(); i++){
						out <<  char_v[i] << '\t' << "O" << endl;		//------------O or I
					}
				}
			}
			if(PBreakpoint_s.find(vite->first) != PBreakpoint_s.end()){
				out << endl;
			}
		}
	}
	out.close();
}
void CCRF::Output_Characters_PFR_END_for_CRF(const char* pathchar, PFRite STARTite, PFRite ENDite)
{
	bool Output_Begin_or_End_Flag = false;
	ofstream out(pathchar);
	if(out.bad()){
		return;
	}
	out.clear();
	out.seekp(0, ios::beg);
	for( ; STARTite != ENDite; STARTite++){
		for(vector<pair<string, string>>::iterator vite = (*STARTite)->begin(); vite != (*STARTite)->end(); vite++){
			if(vite->first.length() == 0){
				//AppCall::Secretary_Message_Box("Data Error: in CGPFR::Output_Words_PFR_for_CRF()", MB_OK);
				out << endl;
				continue;
			}
			vector<string> char_v;
			NLPOP::charseq_to_vector(vite->first.c_str(), char_v);
			if(NEs_s.find(vite->second) == NEs_s.end()){
				for(size_t i = 0; i < char_v.size(); i++){
					out << char_v[i] << '\t' << "O" << endl;
				}
			}
			else{
				if(Output_Begin_or_End_Flag){
					out << char_v[0] << '\t' << "B" << endl;
					for(size_t i = 1; i < char_v.size(); i++){
						out << char_v[i] << '\t' << "I" << endl;
					}
				}
				else{
					vector<string>::iterator endite = char_v.end();
					if(endite != char_v.begin()){
						endite--;
					}
					for(vector<string>::iterator vite = char_v.begin(); vite != endite; vite++){
						out << vite->data() << '\t' << "O" << endl;
					}
					out << endite->data() << '\t' << "B" << endl;
				}
			}
			if(PBreakpoint_s.find(vite->first) != PBreakpoint_s.end()){
				out << endl;
			}
		}
	}
	out.close();
}
void CCRF::Output_Words_PFR_for_CRF(const char* pathchar, PFRite STARTite, PFRite ENDite)
{
	ofstream out(pathchar);
	if(out.bad()){
		return;
	}
	out.clear();
	out.seekp(0, ios::beg);

	string prixmorph = "";
	bool Pre_NEs_Flag = false;
	for( ; STARTite != ENDite; STARTite++){
		for(vector<pair<string, string>>::iterator vite = (*STARTite)->begin(); vite != (*STARTite)->end(); vite++){
			if(vite->first.length() == 0){
				//AppCall::Secretary_Message_Box("Data Error: in CGPFR::Output_Words_PFR_for_CRF()", MB_OK);
				out << endl;
				continue;
			}
			out << vite->first << '\t';
			if(NEs_s.find(vite->second) == NEs_s.end()){
				out << "O" << endl;
				Pre_NEs_Flag = false;
				prixmorph = "";
			}
			else{
				if(!Pre_NEs_Flag || prixmorph != vite->second){
					out << "B" << endl;
					Pre_NEs_Flag = true;
					prixmorph = vite->second;
				}
				else{
					out << "I" << endl;
				}
			}
		}
	}
	out.close();
}

void CCRF::Context_Statistical_Info(const char* pathchar, vector<vector<pair<string, string>>*>& pmPFRcorpus_v)
{//This function collects the distribution of words.

	map<string, map<string, size_t>*> PrixContext_mm;
	map<string, map<string, size_t>*> ProxContext_mm;
	for(set<string>::iterator site = NEs_s.begin(); site != NEs_s.end(); site++){
		PrixContext_mm.insert(make_pair(site->data(), new map<string, size_t>));
		ProxContext_mm.insert(make_pair(site->data(), new map<string, size_t>));
	}
	string prixstr;
	string mophstr;
	bool Det_NEs_Flag = false;
	for(size_t i = 0; i < pmPFRcorpus_v.size(); i++){
		for(vector<pair<string, string>>::iterator vite = pmPFRcorpus_v[i]->begin(); vite != pmPFRcorpus_v[i]->end(); vite++){
			if(NEs_s.find(vite->second) != NEs_s.end()){
				if((prixstr.length() != 0) && !Det_NEs_Flag){
					if(PrixContext_mm[vite->second]->find(prixstr) == PrixContext_mm[vite->second]->end()){
						PrixContext_mm[vite->second]->insert(make_pair(prixstr, 0));
					}
					(*PrixContext_mm[vite->second])[prixstr]++;
				}
				Det_NEs_Flag = true;
			}
			else if(Det_NEs_Flag){
				if(ProxContext_mm[mophstr]->find(vite->first) == ProxContext_mm[mophstr]->end()){
					ProxContext_mm[mophstr]->insert(make_pair(vite->first, 0));
				}
				(*ProxContext_mm[mophstr])[vite->first]++;
				Det_NEs_Flag = false;
			}
			prixstr = vite->first;
			mophstr = vite->second;
		}
	}
	ofstream out(pathchar);
	if(out.bad()){
		return;
	}
	out.clear();
	out.seekp(0, ios::beg);

	out << "#PrixContext" << endl;
	for(map<string, map<string, size_t>*>::iterator mmite = PrixContext_mm.begin(); mmite != PrixContext_mm.end(); mmite++){
		multimap<size_t, string> contex_mm;
		size_t DeletSize;
		for(map<string, size_t>::iterator mite = mmite->second->begin(); mite != mmite->second->end(); mite++){
			if(mite->second < 10){
				continue;
			}
			contex_mm.insert(make_pair(mite->second, mite->first));
		}
		DeletSize = (size_t)(contex_mm.size()*(1-DelHighRate));
		for(multimap<size_t, string>::iterator mite = contex_mm.begin(); mite != contex_mm.end(); ){
			if(DeletSize > 0){
				mite++;
				DeletSize--;
			}
			else{
				mite = contex_mm.erase(mite);
			}
		}
		for(multimap<size_t, string>::reverse_iterator rmite = contex_mm.rbegin(); rmite != contex_mm.rend(); rmite++){
			out << rmite->second << ',' << rmite->first << ',';
		}
		out << endl;
	}
	out << "#ProxContext" << endl;
	for(map<string, map<string, size_t>*>::iterator mmite = ProxContext_mm.begin(); mmite != ProxContext_mm.end(); mmite++){
		multimap<size_t, string> contex_mm;
		size_t DeletSize;
		for(map<string, size_t>::iterator mite = mmite->second->begin(); mite != mmite->second->end(); mite++){
			if(mite->second < 10){
				continue;
			}
			contex_mm.insert(make_pair(mite->second, mite->first));
		}
		DeletSize = (size_t)(contex_mm.size()*(1-DelHighRate));
		for(multimap<size_t, string>::iterator mite = contex_mm.begin(); mite != contex_mm.end(); ){
			if(DeletSize > 0){
				mite++;
				DeletSize--;
			}
			else{
				mite = contex_mm.erase(mite);
			}
		}
		for(multimap<size_t, string>::reverse_iterator rmite = contex_mm.rbegin(); rmite != contex_mm.rend(); rmite++){
			out << rmite->second << ',' << rmite->first << ',';
		}
		out << endl;
	}
	out.close();
}
	/*map<size_t, set<size_t>> TotalNE_ms;
	size_t Total_NE_Cnt;

	Total_NE_Cnt = 0;
	TotalNE_ms.clear();
	bool Begin_Flag = false;
	bool pri_Begin_Flag = false;
	size_t pri_B = 0;
	
	for(size_t i = 0; i < Max_CRFResult_v.size(); i++){
		if(Max_CRFResult_v[i]->first == "B"){
			if(!Begin_Flag){
				pri_B = i;
			}
			Begin_Flag = true;
			if(pri_Begin_Flag){
				if(TotalNE_ms.find(pri_B) == TotalNE_ms.end()){
					TotalNE_ms[pri_B];
				}
				if(TotalNE_ms[pri_B].find(i-1) == TotalNE_ms[pri_B].end()){
					TotalNE_ms[pri_B].insert(i-1);
					Total_NE_Cnt++;
				}
				pri_B = i;
			}
		}
		if(Begin_Flag){
			if(Max_CRFResult_v[i]->first == "O"){
				if(TotalNE_ms.find(pri_B) == TotalNE_ms.end()){
					TotalNE_ms[pri_B];
				}
				if(TotalNE_ms[pri_B].find(i-1) == TotalNE_ms[pri_B].end()){
					TotalNE_ms[pri_B].insert(i-1);
					Total_NE_Cnt++;
				}
				Begin_Flag = false;
			}
		}
		pri_Begin_Flag = Begin_Flag;
	}

	Begin_Flag = false;
	pri_Begin_Flag = false;
	//pri_B = 0;
	for(size_t i = 0; i < Min_CRFResult_v.size(); i++){
		if(Min_CRFResult_v[i]->first == "B"){
			if(!Begin_Flag){
				pri_B = i;
			}
			Begin_Flag = true;
			if(pri_Begin_Flag){
				if(TotalNE_ms.find(pri_B) == TotalNE_ms.end()){
					TotalNE_ms[pri_B];
				}
				if(TotalNE_ms[pri_B].find(i-1) == TotalNE_ms[pri_B].end()){
					TotalNE_ms[pri_B].insert(i-1);
					Total_NE_Cnt++;
				}
				pri_B = i;
			}
		}
		if(Begin_Flag){
			if(Min_CRFResult_v[i]->first == "O"){
				if(TotalNE_ms.find(pri_B) == TotalNE_ms.end()){
					TotalNE_ms[pri_B];
				}
				if(TotalNE_ms[pri_B].find(i-1) == TotalNE_ms[pri_B].end()){
					TotalNE_ms[pri_B].insert(i-1);
					Total_NE_Cnt++;
				}
				Begin_Flag = false;
			}

		}
		pri_Begin_Flag = Begin_Flag;
	}

	
	ostringstream ostream;
	ostream << "Total:\t" << Total_NE_Cnt << endl;

	Total_NE_Cnt = 0;
	TotalNE_ms.clear();
	Performance_of_Max_Min_CRF(Max_CRFResult_v, TotalNE_ms, Total_NE_Cnt);

	Performance_of_Max_Min_CRF(Min_CRFResult_v, TotalNE_ms, Total_NE_Cnt);

	ostream << "Correct:\t" <<  Total_NE_Cnt << endl;
	AppCall::Secretary_Message_Box(ostream.str(), MB_OK);*/