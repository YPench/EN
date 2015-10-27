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


class CDetBound
{
public:
	CCEDT* pCCEDT;
	
public:
	CDetBound();
	~CDetBound();

public:
	//=====================Boundary Port
	void START_or_END_Boundary_Detection_Port(bool START_Flag, const char* inputstr, MaxentModel& pm_maxent, map<size_t, map<size_t, double>*>& boundout_m, size_t SENID);
	void START_Boundary_Training_and_Testing_Port(vector<NE_Surround*>& training_Surround_v, vector<NE_Surround*>& testing_Surround_v);
	void END_Boundary_Training_and_Testing_Port(vector<NE_Surround*>& training_Surround_v, vector<NE_Surround*>& testing_Surround_v);
	
	//========================================
	void Training_START_or_END_Boundary(bool START_Flag, vector<NE_Surround*>& training_Surround_v, MaxentModel& pm_maxent, CEDTInfo& pmTrainingCEDTInfo);
	void Testing_START_or_END_Boundary(bool START_Flag, vector<NE_Surround*>& testing_Surround_v, MaxentModel& pm_maxent, CEDTInfo& pmTestingCEDTInfo);
	void Generate_START_or_END_Cases_Feature(bool START_Flag, vector<BoundCase*>& START_BoundCase_v, FeatureVctor& Training_v);

	//========================================
	void Gen_END_Boundary_Cases(vector<NE_Surround*>& Surround_v, vector<BoundCase*>& END_BoundCase_v);
	void Gen_START_Boundary_Cases(vector<NE_Surround*>& Surround_v, vector<BoundCase*>& START_BoundCase_v);
	void Gen_START_Boundary_Cases(const char* inputchar, vector<BoundCase*>& START_BoundCase_v, size_t SENID);
	void Gen_END_Boundary_Cases(const char* inputchar, vector<BoundCase*>& END_BoundCase_v, size_t SENID);
};
