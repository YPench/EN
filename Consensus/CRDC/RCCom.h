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
#include "CSegmter.h"


namespace RCCOM{
	string Get_Entity_Positional_Structures(Relation_Case& pm_RCase);
	void Local_Adjacent_Words_Singleton_POS_Feature_Extracting(CSegmter& m_CSegmter, const char* sentchar, map<string, float>& WordsCnt_map, string prix, string prox);
	void Adjacent_Words_POS_Feature_Extracting(CSegmter& m_CSegmter, const char* sentchar, map<string, float>& WordsCnt_map, string prix, string prox);
	void Get_Entity_Mention_extent_Map(map<string, ACE_entity>& ACE_Entity_Info_map, map<string, ACE_entity_mention>& EntityMention_map);
	
	void Delet_0AH_and_20H_in_Relation_Case(Relation_Case& pmRelCase);
	void Delet_0AH_and_20H_in_RelationContext(RelationContext& pm_context);
	
};



