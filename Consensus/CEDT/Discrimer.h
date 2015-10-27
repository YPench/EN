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
class CEDTInfo;

class CDiscrimer
{
public:
	CCEDT* pCCEDT;


public:
	CDiscrimer();
	~CDiscrimer();

public:
//============================================
	void Discrimer_Training_and_Testing_Port(vector<NE_Surround*>& training_Surround_v, vector<NE_Surround*>& testing_Surround_v);
	void Discriminater_Recognization_Port(const char* inputchar, MaxentModel& pm_maxent, CEDTInfo& pmCEDTInfo);

//============================================
	void Training_Named_Entity_Discriminater(vector<NE_Surround*>& training_Surround_v, MaxentModel& pm_maxent);
	void Testing_Named_Entity_Discriminater(vector<NE_Surround*>& testing_Surround_v, MaxentModel& pm_maxent);

//============================================
	void Generate_Discrimer_Training_Case(vector<NE_Surround*>& Surround_v, CEDTInfo& pmCEDTInfo);
	void Combined_Performance_and_Generate_ACE_Testing_for_Discrimer(CEDTInfo& pmCEDTInfo);
	void Combined_Matched_Head_and_Extend_for_Discrimer(CEDTInfo& pmCEDTInfo);
	void Generate_Discrimer_for_Extend_or_Head_Only_Recognition(CEDTInfo& pmCEDTInfo);
//============================================
	void Generate_Named_Entity_Discrimer_Cases_Feature(vector<DismCase*>& DismCase_v, FeatureVctor& Training_v);
	
	
};
