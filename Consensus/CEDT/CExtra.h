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

class CExtra
{
public:
	CCEDT* pCCEDT;


public:
	CExtra();
	~CExtra();



	void ACE_Extra_Information_Extraction_Port(vector<DismCase*>& TrainingDism_v, vector<DismCase*>& TestingDism_v);
	void Load_Entity_TYPE_Info(vector<DismCase*>& pmDismCase_v);

	void Entity_CLASS_Recognition(vector<DismCase*>& TrainingDism_v, vector<DismCase*>& TestingDism_v);
	void Mention_TYPE_Recognition(vector<DismCase*>& TrainingDism_v, vector<DismCase*>& TestingDism_v);

	void Entity_SUBTYPE_Recognition(vector<DismCase*>& TrainingDism_v, vector<DismCase*>& TestingDism_v);
	void Each_Entity_SUBTYPE_Recognition(vector<DismCase*>& TrainingDism_v, vector<DismCase*>& TestingDism_v);

};
