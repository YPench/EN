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
#pragma once

#include "..\\Include\\ace.h"
#include "maxen.h"

class CCEDT;
class CEDTInfo;

class CDetCandit
{
public:
	CCEDT* pCCEDT;

	Maxen_Rtn_map ProcesedCasesInfor_m;
public:
	CDetCandit();
	~CDetCandit();

public:
	//==============================================
	void Candidates_Recognization_Port(const char* inputchar, MaxentModel& pm_maxen, CEDTInfo& pmCEDTInf);
	void Candidate_Training_and_Testing_Port(vector<NE_Surround*>& training_Surround_v, vector<NE_Surround*>& testing_Surround_v);
	
	//==============================================
	void Training_Named_Entity_Candidates(vector<NE_Surround*>& training_Surround_v, MaxentModel& pm_maxent, CEDTInfo& pmTrainingCEDTInfo);
	void Testing_Named_Entity_Candidates(vector<NE_Surround*>& testing_Surround_v, MaxentModel& pm_maxent, CEDTInfo& pmTestingCEDTInfo);

	//==============================================
	void Gen_Candit_Training_Case_By_FeedBack_Info_Erasing(vector<NE_Surround*>& Surround_v, vector<CanditCase*>& CanditCase_v, CEDTInfo& pmTrainingCEDTInfo);
	void Gen_Candit_Testing_Case_By_FeedBack_Info(vector<NE_Surround*>& Surround_v, vector<CanditCase*>& Cand_DismCase_v, CEDTInfo& pmTestingCEDTInfo);
	void Gen_Candit_Inputchar_Case_By_FeedBack_Info(const char* inputchar, vector<CanditCase*>& Cand_DismCase_v, CEDTInfo& pmCEDTInfo);
	
	void Testing_Named_Entity_Candidates_By_Greedy_Matching(vector<NE_Surround*>& testing_Surround_v, CEDTInfo& pmTestingCEDTInfo);
	void Gen_Greedy_Right_or_Left_Testing_Match_Candit(vector<NE_Surround*>& Surround_v, vector<CanditCase*>& Cand_DismCase_v, CEDTInfo& pmTestingCEDTInfo);
	void Greedy_Left_to_Right_matching(set<size_t>& START_s, set<size_t>& END_s, int Curposit, int limit, size_t Match_Distance, set<size_t>& Match_s);

	void Creating_DismCase_Cases(const char* inputchar, size_t i, size_t SENID, size_t&  CASID, set<size_t>& START_s, set<size_t>& END_s, map<size_t, double>& START_Rtn_map, 
	map<size_t, double>& END_Rtn_map, map<size_t, set<size_t>>& pmSavedPair, map<size_t, map<size_t, ACE_entity_mention*>>& EntityMention_mm, vector<CanditCase*>& pmCandit_v);

	void Generate_Candit_Cases_Feature(vector<CanditCase*>& CanditCase_v, FeatureVctor& Training_v);

	void Greedy_Right_to_Left_matching(set<size_t>& START_s, set<size_t>& END_s, int Curposit, int limit, size_t Match_Distance, set<size_t>& Match_s);
	//==============================================
	int Get_Left_Outer_Feature_Range_For_START(set<size_t>& START_s, set<size_t>& END_s, int Curposit, int limit);
	int Get_Right_Outer_Feature_Range_For_END(set<size_t>& START_s, set<size_t>& END_s, int Curposit, int limit);
	
	int Get_END_Boundary_Range_For_START(set<size_t>& START_s, set<size_t>& END_s, int Curposit, int limit);
	int Get_START_Boundary_Range_For_END(set<size_t>& START_s, set<size_t>& END_s, int Curposit, int limit);
	
	//==============================================
	bool Exist_Detection_and_Updata(map<size_t, set<size_t>>& SavedPair_m, size_t START, size_t END);
	void Erasing_Prescribed_Candidates(multimap<double, size_t>& FeedbackCase_mm);
	void Generated_Candidate_Testing_Cases_Information(map<string, size_t>& pmPositive_Cases, vector<CanditCase*>& pmCandit_v);
	

};
