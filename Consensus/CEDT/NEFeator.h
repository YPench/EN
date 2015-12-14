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

#include "CGCom.h"
#include "ace.h"

class CNEFeator
{
private:
	set<string> lexicon_set;
	set<string> Number_Set;
	set<string> English_Set;
	int maxtoken_size;

public:

	CNEFeator();
	~CNEFeator();

	void Generate_Candit_Cases_Feature(vector<CanditCase*>& CanditCase_v, FeatureVctor& Training_v);
	void Generate_START_or_END_Cases_Feature(bool START_Flag, vector<BoundCase*>& START_BoundCase_v, FeatureVctor& Training_v);

	bool Init_Named_Entity_Featrue_Extractor(const char* pmDataFolder);
	void Combined_Feature_In_Candit_Two_Boundary(const CanditCase& pmCandit, vector<string>& pmFeatureVector, string attach);
	void Extract_Local_Feature_Vector_of_Boundary_with_nGram(const BoundCase& pmBoundary, vector<string>& pmFeatureVector, bool START_Flag, string attach);

	//-------Feature Extraction Port
	void Extract_Feature_Vector_of_Candit(const CanditCase& pmCandit, vector<string>& pmFeatureVector, string attach);
	void Extract_Feature_Vector_of_Deterimer(const DismCase& pmDismCase, vector<string>& pmFeatureVector);

	//----Feature Extraction Function--------------
	void Adjacent_Left_Border_n_Gram(const char* strchar, int Distance, vector<string>& outstr_v, string attach);
	void Adjacent_Right_Border_n_Gram(const char* strchar, int Distance, vector<string>& outstr_v, string attach);
	void Internal_n_Gram(const char* strchar, int Distance, vector<string>& outstr_v, string attach);

};

