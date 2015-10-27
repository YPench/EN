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

class CECOR;

class CTest
{
public:
	CECOR* m_pCECOR;
	string m_TestFolder;

public:
	CTest();
	~CTest();
	void Generating_Testing_Reference_Documents(ACE_Corpus& pmACE_Corpus, vector<string>& RefTest_v, map<string, map<string, set<ACE_entity_mention*>*>>& CoferenceSet_mm);
	void ECOR_Model_Testing_Port(ACE_Corpus& pmACE_Corpus, vector<string>& pmTesting_v, MaxentModel& ECOR_Maxent);
	void Output_Coference_Mention_Set_with_Erasing(const char* parthchar, map<string, ACE_DocInfo>& DocInfo_m, map<string, map<string, set<ACE_entity_mention*>*>>& CoferenceSet_mm);
	void Output_Mentions_Documents_List(const char* clistpath, const char* outputfolder, map<string, map<string, set<ACE_entity_mention*>*>>& CoferenceSet_mm);

	void Generate_Reference_Coreference_Data(ACE_Corpus& pmACE_Corpus, vector<string>& pmTesting_v);

	void ECOR_Model_Testing_Parting_Port(ACE_Corpus& pmACE_Corpus, vector<string>& pmTesting_v);
};
