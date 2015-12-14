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
#include "stdafx.h"
#include "RCCom.h"
#include "maxen.h"
#include "CSegmter.h"

void RCCOM::Delet_0AH_and_20H_in_RelationContext(RelationContext& pm_context)
{
	ace_op::Delet_0AH_and_20H_in_string(pm_context.L_str);
	ace_op::Delet_0AH_and_20H_in_string(pm_context.M_str);
	ace_op::Delet_0AH_and_20H_in_string(pm_context.R_str);
}
void RCCOM::Delet_0AH_and_20H_in_Relation_Case(Relation_Case& pmRelCase)
{
	ace_op::Delet_0AH_and_20H_in_string(pmRelCase.first_entity.extent.charseq);
	ace_op::Delet_0AH_and_20H_in_string(pmRelCase.sencond_entity.extent.charseq);
	ace_op::Delet_0AH_and_20H_in_string(pmRelCase.relatin_mention.extent.charseq);
			
	ace_op::Delet_0AH_and_20H_in_string(pmRelCase.first_entity.head.charseq);
	ace_op::Delet_0AH_and_20H_in_string(pmRelCase.sencond_entity.head.charseq);
}


string RCCOM::Get_Entity_Positional_Structures(Relation_Case& pm_RCase)
{
	//Nested, Adjacent and Separated
	if((pm_RCase.first_entity.extent.START >= pm_RCase.sencond_entity.extent.START) && (pm_RCase.first_entity.extent.END <= pm_RCase.sencond_entity.extent.END)){
		return "1_Nest_2";//Nested
	}
	if((pm_RCase.sencond_entity.extent.START >= pm_RCase.first_entity.extent.START) && (pm_RCase.sencond_entity.extent.END <= pm_RCase.first_entity.extent.END)){
		return "2_Nest_1";
	}
	if((pm_RCase.sencond_entity.extent.START <= pm_RCase.first_entity.extent.END) && (pm_RCase.sencond_entity.extent.END > pm_RCase.first_entity.extent.END)){
		//AppCall::Secretary_Message_Box("数据出错：1_Adjacent_2 is hit", MB_OK);
		//return "1_Adj_2";
	}
	if((pm_RCase.first_entity.extent.START <= pm_RCase.sencond_entity.extent.END) && (pm_RCase.first_entity.extent.END > pm_RCase.sencond_entity.extent.END)){
		//AppCall::Secretary_Message_Box("数据出错：2_Adjacent_1 is hit", MB_OK);
		//return "2_Adj_1";
	}
	if(pm_RCase.sencond_entity.extent.START > pm_RCase.first_entity.extent.END){
		return "1_Sep_2";//Separated
	}
	if(pm_RCase.first_entity.extent.START > pm_RCase.sencond_entity.extent.END){
		return "2_Sep_1";
	}
	//AppCall::Secretary_Message_Box("数据出错：ace_op::Get_Entity_Positional_Structures()", MB_OK);
	return "";
}

void RCCOM::Adjacent_Words_POS_Feature_Extracting(CSegmter& m_CSegmter, const char* sentchar, map<string, float>& WordsCnt_map, string prix = "", string prox = "")
{
	CLAUSEPOS loc_Seg;
	m_CSegmter.ICTCLAS_Segmention_Port(sentchar, loc_Seg);
	if(prix.length() > 0){
		if(!loc_Seg.POS.empty()){
			WordsCnt_map.insert(make_pair(prix + loc_Seg.POS[0], (float)1));
		}
	}
	if(prox.length() > 0){
		if(!loc_Seg.POS.empty()){
			WordsCnt_map.insert(make_pair(loc_Seg.POS[loc_Seg.POS.size() - 1] + prox, (float)1));
		}
	}
}
void RCCOM::Local_Adjacent_Words_Singleton_POS_Feature_Extracting(CSegmter& m_CSegmter, const char* sentchar, map<string, float>& WordsCnt_map, string prix = "", string prox = "")
{
	CLAUSEPOS loc_Seg;
	m_CSegmter.ICTCLAS_Segmention_Port(sentchar, loc_Seg);
	if(prix.length() > 0){
		if(!loc_Seg.POS.empty()){
			WordsCnt_map.insert(make_pair(loc_Seg.POS[0], (float)1));
		}
	}
	if(prox.length() > 0){
		if(!loc_Seg.POS.empty()){
			WordsCnt_map.insert(make_pair(loc_Seg.POS[loc_Seg.POS.size() - 1], (float)1));
		}
	}
}
//</Generate Training Cases>
void RCCOM::Get_Entity_Mention_extent_Map(map<string, ACE_entity>& ACE_Entity_Info_map, map<string, ACE_entity_mention>& EntityMention_map)
{
	for(map<string, ACE_entity>::iterator mite = ACE_Entity_Info_map.begin(); mite != ACE_Entity_Info_map.end(); mite++){
		for(vector<ACE_entity_mention>::iterator vite = mite->second.entity_mention_v.begin(); vite != mite->second.entity_mention_v.end(); vite++){
			if(EntityMention_map.find(vite->ID) == EntityMention_map.end()){
				EntityMention_map.insert(make_pair(vite->ID, *vite));
			}
		}
	}
}