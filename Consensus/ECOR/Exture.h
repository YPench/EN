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
#include "ECCom.h"


class CECOR;




class CExtrue
{
public:

	CECOR* m_pCECOR;



public:
	CExtrue();
	~CExtrue();


	void Extracting_Mention_Pair_Feature_Port(pair<ACE_entity_mention*, ACE_entity_mention*>& MentionPairCandit, vector<string>& pmFeatureVector);

	void Acronym_Matching(const char* mentionTYPE, const char* cFirst, const char* cSecond, vector<string>& pmFeatureVector);
	void Prefixion_and_Suffixation_and_Nested_Words_Matching_Feature(const char* mentionTYPE, const char* cFirst, const char* cSecond, vector<string>& pmFeatureVector);
};
