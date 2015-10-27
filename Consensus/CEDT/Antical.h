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
#pragma once

#include "..\\Include\\ace.h"
#include "maxen.h"

class CCEDT;


typedef struct _BountRet{
	bool START_Flag;
	bool END_Flag;
	string mention;
	string insur;
}BountRet, *pBountRet;

class CAntical
{
public:
	CCEDT* pCCEDT;


public:
	CAntical();
	~CAntical();

	void ACE_Evaluation_Toolkit_Call(const char* toolkitfolder);

	void Corpus_Entity_and_Entity_Mention_Information(vector<NE_Surround*>& Surround_v, bool Output_Flag);
	void ACE_Named_Entity_Recognition_Evaluation_Port(vector<NE_Surround*>& Surround_v, vector<DismCase*>& pmDismCase_v);
	void Output_For_ACE_Named_Entity_Recognition_Evaluation(vector<NE_Surround*>& Surround_v, vector<DismCase*>& pmDismCase_v);

	void Candit_Performance_Evaluation_Output_Result(vector<CanditCase*>& pmCandit_v);
	void Analysizing_Performance_Evaluation_on_Extend_or_Head_Only_Recognition(vector<NE_Surround*>& testing_Surround_v, vector<DismCase*>& pmDismCase_v, Maxen_Rtn_map& P_nCrossRtn_m, Maxen_Rtn_map& R_nCrossRtn_m);
	void Analysizing_Performance_Evaluation_on_Extend_and_Head_Matching(vector<NE_Surround*>& testing_Surround_v, vector<DismCase*>& pmDismCase_v, Maxen_Rtn_map& P_nCrossRtn_m, Maxen_Rtn_map& R_nCrossRtn_m);
	void ACE_Named_Entity_Recognition_Output_Result(vector<NE_Surround*>& testing_Surround_v, vector<DismCase*>& pmDismCase_v);
	void Analysizing_Performance_Evaluation_on_Mention(vector<NE_Surround*>& testing_Surround_v, vector<DismCase*>& pmDismCase_v, Maxen_Rtn_map& P_nCrossRtn_m, Maxen_Rtn_map& R_nCrossRtn_m);
	void Detect_Boundary_Result_Analyser();
	void Named_Entity_Result_Analyser(vector<DismCase>& pmCanditCase_v, vector<bool>& pmNE_TestRet_v);

	void Candidates_Performance_Analysis_Single_Extend_or_Head_Mention(vector<NE_Surround*>& testing_Surround_v, bool CEDT_Head_Flag, Maxen_Rtn_map& P_nCrossRtn_m, Maxen_Rtn_map& R_nCrossRtn_m);
};
