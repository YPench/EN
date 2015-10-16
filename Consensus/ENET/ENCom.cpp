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
#include "stdafx.h"
#include "ENCom.h"
#include "ConvertUTF.h"

//=====================PAJEK
void PAJEK::Save_Pajek_by_Vertex_and_Edges_Info(const char* cFilePath, list<pajekvertex*>& pajekvertex_l, list<pajekedge*>& pakekedge_l, list<pair<string, size_t>>& partition_l)
{
	map<size_t, size_t> ReOrdered_map;
	FILE * fout;
	if(fopen_s(&fout, cFilePath, "wb")){
		AppCall::Secretary_Message_Box("Error to Open File in: ENCOM::Save_Pajek_by_Vertex_and_Edges_Info()");
	}

	//-------Output Vertex
	fprintf(fout, "*Network Merged Event Network\n");
	fprintf(fout, "*Vertices %d\n", pajekvertex_l.size());
	size_t vertex_id = 1;
	for(list<pajekvertex*>::iterator lite = pajekvertex_l.begin(); lite != pajekvertex_l.end(); lite++){
		pajekvertex& loc_vex = **lite;
		ReOrdered_map.insert(make_pair(loc_vex.vex, vertex_id));
		fprintf(fout, "%d \"", vertex_id);
		fprintf(fout, loc_vex.name.c_str());
		fprintf(fout, "\"    %f    %f    %f    T ", loc_vex.x, loc_vex.y, loc_vex.weight);
		fprintf(fout, loc_vex.TYPE.c_str());
		fprintf(fout, "\n");
		vertex_id++;
	}

	//-------Output Edges
	fprintf(fout, "*Edges\n");
	for(list<pajekedge*>::iterator lite = pakekedge_l.begin(); lite != pakekedge_l.end(); lite++){
		pajekedge& loc_edge = **lite;
		fprintf(fout, "%d %d    %f    l ", ReOrdered_map[loc_edge.vex1], ReOrdered_map[loc_edge.vex2], loc_edge.value);
		fprintf(fout, loc_edge.TYPE.c_str());
		fprintf(fout, "\n");
	}

	//-------Output Partition Info
	if(!partition_l.empty()){
		map<string, size_t> name2partition_m;
		for(list<pair<string, size_t>>::iterator lite = partition_l.begin(); lite != partition_l.end(); lite++){
			name2partition_m.insert(make_pair(lite->first, lite->second));
		}
		fprintf(fout, "*Partition Named Entity TYPE\n");
		fprintf(fout, "*Vertices %d\n", pajekvertex_l.size());
		for(list<pajekvertex*>::iterator lite = pajekvertex_l.begin(); lite != pajekvertex_l.end(); lite++){
			pajekvertex& loc_vex = **lite;
			if(name2partition_m.find(loc_vex.name) == name2partition_m.end()){
				AppCall::Secretary_Message_Box("Data Error in: PAJEK::Save_Pajek_by_Vertex_and_Edges_Info()");
				return;
			}
			fprintf(fout, "%d\n", name2partition_m[loc_vex.name]);
		}
	}
	fclose(fout);

}
void PAJEK::Save_Igraph_to_Pajek(const char* cFilePath, igraph_t& event_network, string memo)
{
	FILE * fout;
	if(fopen_s(&fout, cFilePath, "wb")){
		AppCall::Secretary_Message_Box("Error to Open File in: PAJEK::Save_Igraph_to_Pajek()");
	}
	fprintf(fout, "*Network "); 
	fprintf(fout, memo.c_str());
	fprintf(fout, " Event Network\n");
	igraph_write_graph_pajek(&event_network, fout);
	fclose(fout);

}
void PAJEK::Load_Pajek_Partition_Info(const char* cFilePath,  list<pair<string, size_t>>& partition_l)
{
	char SentenceBuf[MAX_SENTENCE];
	size_t vertex_cnt;
	ifstream in(cFilePath);
	if(in.bad())
		return;
	in.clear();
	in.seekg(0, ios::beg);
	if(in.peek() == EOF){
		in.close();
		return;
	}
	if(in.peek() != '*'){
		AppCall::Secretary_Message_Box("Data Error in: ENCOM::Load_Pajek_Partition_Info()");
		return;
	}
	in.getline(SentenceBuf, MAX_SENTENCE, '\n');;
	in.getline(SentenceBuf, MAX_SENTENCE, '\n');
	istringstream instream(SentenceBuf);
	instream.getline(SentenceBuf, MAX_SENTENCE, ' ');
	instream >> vertex_cnt;
	for(size_t i = 0; i < vertex_cnt; i++){
		in.getline(SentenceBuf, MAX_SENTENCE, '\n');
		istringstream instream(SentenceBuf);
		size_t vertex_id;
		while(instream.peek() == ' '){
			instream.getline(SentenceBuf, MAX_SENTENCE, ' ');
		}
		instream >> vertex_id;
		instream.getline(SentenceBuf, MAX_SENTENCE, '\"');
		instream.getline(SentenceBuf, MAX_SENTENCE, '\"');
		partition_l.push_back(make_pair(SentenceBuf, 0));
	}
	while(in.peek() != '*'){
		in.getline(SentenceBuf, MAX_SENTENCE, '\n');
	}
	in.getline(SentenceBuf, MAX_SENTENCE, '\n');
	istringstream edgestream(SentenceBuf);
	edgestream.getline(SentenceBuf, MAX_SENTENCE, '*');
	edgestream.getline(SentenceBuf, MAX_SENTENCE, '\n');
	if(strcmp(SentenceBuf, "Edges")){
		AppCall::Secretary_Message_Box("Data Error in: ENCOM::Load_Pajek_Partition_Info()");
		return;
	}
	while(in.peek() != '*' && in.peek() != EOF){
		in.getline(SentenceBuf, MAX_SENTENCE, '\n');
	}
	if(in.peek() == EOF){
		in.close();
		return;
	}
	while(in.peek() != '*'){
		AppCall::Secretary_Message_Box("Data Error in: ENCOM::Load_Pajek_Partition_Info()");
		return;
	}
	in.getline(SentenceBuf, MAX_SENTENCE, '\n');
	in.getline(SentenceBuf, MAX_SENTENCE, '\n');
	istringstream partstream(SentenceBuf);
	partstream.getline(SentenceBuf, MAX_SENTENCE, '*');
	partstream.getline(SentenceBuf, MAX_SENTENCE, ' ');
	if(strcmp(SentenceBuf, "Vertices")){
		AppCall::Secretary_Message_Box("Data Error in: ENCOM::Load_Pajek_Partition_Info()");
		return;
	}
	size_t par_vertex_num;
	partstream >> par_vertex_num;
	if(vertex_cnt != par_vertex_num){
		AppCall::Secretary_Message_Box("Data Error in: ENCOM::Load_Pajek_Partition_Info()");
		return;
	}
	for(list<pair<string, size_t>>::iterator lite = partition_l.begin(); lite != partition_l.end(); lite++){
		in >> lite->second;
		in.getline(SentenceBuf, MAX_SENTENCE, '\n');
	}
	in.close();
}


void PAJEK::Load_Pajek_Vertex_Info(const char* cFilePath, list<pajekvertex*>& pajekvertex_l)
{
	char SentenceBuf[MAX_SENTENCE];
	size_t vertex_cnt;
	ifstream in(cFilePath);
	if(in.bad())
		return;
	in.clear();
	in.seekg(0, ios::beg);
	if(in.peek() == EOF){
		in.close();
		return;
	}
	if(in.peek() != '*'){
		AppCall::Secretary_Message_Box("Data Error in: ENCOM::Load_Pajek_Vertex_Info()");
		return;
	}
	in.getline(SentenceBuf, MAX_SENTENCE, '\n');;
	in.getline(SentenceBuf, MAX_SENTENCE, '\n');
	istringstream instream(SentenceBuf);
	instream.getline(SentenceBuf, MAX_SENTENCE, ' ');
	instream >> vertex_cnt;
	for(size_t i = 0; i < vertex_cnt; i++){
		pajekvertex* ploc_pajekvertex = new pajekvertex;
		in.getline(SentenceBuf, MAX_SENTENCE, '\n');
		istringstream instream(SentenceBuf);
		instream >> ploc_pajekvertex->vex;
		instream.getline(SentenceBuf, MAX_SENTENCE, '\"');
		instream.getline(SentenceBuf, MAX_SENTENCE, '\"');
		ploc_pajekvertex->name = SentenceBuf;
		instream.getline(SentenceBuf, MAX_SENTENCE, ' ');
		instream.getline(SentenceBuf, MAX_SENTENCE, ' ');
		instream.getline(SentenceBuf, MAX_SENTENCE, ' ');
		instream.getline(SentenceBuf, MAX_SENTENCE, ' ');
		instream >> ploc_pajekvertex->x;
		instream.getline(SentenceBuf, MAX_SENTENCE, ' ');
		instream.getline(SentenceBuf, MAX_SENTENCE, ' ');
		instream.getline(SentenceBuf, MAX_SENTENCE, ' ');
		instream.getline(SentenceBuf, MAX_SENTENCE, ' ');
		instream >> ploc_pajekvertex->y;
		instream.getline(SentenceBuf, MAX_SENTENCE, ' ');
		instream.getline(SentenceBuf, MAX_SENTENCE, ' ');
		instream.getline(SentenceBuf, MAX_SENTENCE, ' ');
		instream.getline(SentenceBuf, MAX_SENTENCE, ' ');
		instream >> ploc_pajekvertex->weight;
		instream.getline(SentenceBuf, MAX_SENTENCE, ' ');
		instream.getline(SentenceBuf, MAX_SENTENCE, ' ');
		instream.getline(SentenceBuf, MAX_SENTENCE, ' ');
		instream.getline(SentenceBuf, MAX_SENTENCE, ' ');
		instream.getline(SentenceBuf, MAX_SENTENCE, ' ');
		if(!strcmp(SentenceBuf, "T")){
			instream.getline(SentenceBuf, MAX_SENTENCE, '\n');
			ploc_pajekvertex->TYPE = SentenceBuf;
		}
		pajekvertex_l.push_back(ploc_pajekvertex);
	}
	in.close();
}

void PAJEK::Load_Pajek_Edges_Info(const char* cFilePath, list<pajekedge*>& pakekedge_l)
{
	char SentenceBuf[MAX_SENTENCE];
	size_t vertex_cnt;
	ifstream in(cFilePath);
	if(in.bad())
		return;
	in.clear();
	in.seekg(0, ios::beg);
	if(in.peek() == EOF){
		in.close();
		return;
	}
	if(in.peek() != '*'){
		AppCall::Secretary_Message_Box("Data Error in: ENCOM::Load_Pajek_Edges_Info()");
		return;
	}
	in.getline(SentenceBuf, MAX_SENTENCE, '\n');;
	in.getline(SentenceBuf, MAX_SENTENCE, '\n');
	istringstream instream(SentenceBuf);
	instream.getline(SentenceBuf, MAX_SENTENCE, ' ');
	instream >> vertex_cnt;
	for(size_t i = 0; i < vertex_cnt; i++){
		in.getline(SentenceBuf, MAX_SENTENCE, '\n');
	}
	while(in.peek() != '*'){
		in.getline(SentenceBuf, MAX_SENTENCE, '\n');
	}
	in.getline(SentenceBuf, MAX_SENTENCE, '\n');
	istringstream edgestream(SentenceBuf);
	edgestream.getline(SentenceBuf, MAX_SENTENCE, '*');
	edgestream.getline(SentenceBuf, MAX_SENTENCE, '\n');
	if(strcmp(SentenceBuf, "Edges")){
		AppCall::Secretary_Message_Box("Data Error in: ENCOM::Load_Pajek_Edges_Info()");
		return;
	}
	size_t EdgesID = 0;
	while(in.peek() != '\n' && in.peek() != EOF){
		pajekedge* ploc_edge = new pajekedge;
		in.getline(SentenceBuf, MAX_SENTENCE, '\n');
		istringstream instream(SentenceBuf);
		instream >> ploc_edge->vex1;
		instream.getline(SentenceBuf, MAX_SENTENCE, ' ');
		instream >> ploc_edge->vex2;
		instream.getline(SentenceBuf, MAX_SENTENCE, ' ');
		instream.getline(SentenceBuf, MAX_SENTENCE, ' ');
		instream.getline(SentenceBuf, MAX_SENTENCE, ' ');
		instream.getline(SentenceBuf, MAX_SENTENCE, ' ');
		instream >> ploc_edge->value;

		instream.getline(SentenceBuf, MAX_SENTENCE, ' ');
		instream.getline(SentenceBuf, MAX_SENTENCE, ' ');
		instream.getline(SentenceBuf, MAX_SENTENCE, ' ');
		instream.getline(SentenceBuf, MAX_SENTENCE, ' ');
		instream.getline(SentenceBuf, MAX_SENTENCE, ' ');
		instream.getline(SentenceBuf, MAX_SENTENCE, ' ');
		ploc_edge->TYPE = SentenceBuf;
		pakekedge_l.push_back(ploc_edge);
		if(in.peek() == '*'){
			break;
		}
	}
	in.close();
}

void PAJEK::Appending_Info_to_Edges_in_Pajek(const char* cFilePath, vector<string>& EdgesInfo_v)
{
	char SentenceBuf[MAX_SENTENCE];
	vector<string> Ogirin_v;
	size_t vertex_cnt;
	string sentstr;

	ifstream in(cFilePath);
	if(in.bad())
		return;
	in.clear();
	in.seekg(0, ios::beg);
	if(in.peek() == EOF){
		in.close();
		return;
	}
	if(in.peek() != '*'){
		AppCall::Secretary_Message_Box("Data Error in: ENCOM::Appending_Info_to_Edges_in_Pajek()");
		return;
	}
	in.getline(SentenceBuf, MAX_SENTENCE, '\n');
	Ogirin_v.push_back(SentenceBuf);
	in.getline(SentenceBuf, MAX_SENTENCE, '\n');
	Ogirin_v.push_back(SentenceBuf);
	istringstream instream(SentenceBuf);
	instream.getline(SentenceBuf, MAX_SENTENCE, ' ');
	instream >> vertex_cnt;
	for(size_t i = 0; i < vertex_cnt; i++){
		in.getline(SentenceBuf, MAX_SENTENCE, '\n');
		Ogirin_v.push_back(SentenceBuf);
	}
	while(in.peek() != '*'){
		in.getline(SentenceBuf, MAX_SENTENCE, '\n');
		Ogirin_v.push_back(SentenceBuf);
	}
	in.getline(SentenceBuf, MAX_SENTENCE, '\n');
	Ogirin_v.push_back(SentenceBuf);
	istringstream edgestream(SentenceBuf);
	edgestream.getline(SentenceBuf, MAX_SENTENCE, '*');
	edgestream.getline(SentenceBuf, MAX_SENTENCE, '\n');
	if(strcmp(SentenceBuf, "Edges")){
		AppCall::Secretary_Message_Box("Data Error in: ENCOM::Appending_Info_to_Edges_in_Pajek()");
		return;
	}
	size_t EdgesID = 0;
	while(in.peek() != '\n' && in.peek() != EOF){
		ostringstream ostream;
		in.getline(SentenceBuf, MAX_SENTENCE, '\n');
		ostream << SentenceBuf;
		ostream << "    " << "l " << EdgesInfo_v[EdgesID++];
		Ogirin_v.push_back(ostream.str());
	}
	while(in.peek() != EOF){
		in.getline(SentenceBuf, MAX_SENTENCE, '\n');
		Ogirin_v.push_back(SentenceBuf);

	}
	in.close();

	Save_Vector_String_to_Pajek(cFilePath, Ogirin_v);
}

void PAJEK::Appending_Attribute_to_Edges_in_Pajek(const char* cFilePath, igraph_vector_t& attr_v)
{
	char SentenceBuf[MAX_SENTENCE];
	vector<string> Ogirin_v;
	size_t vertex_cnt;
	string sentstr;

	ifstream in(cFilePath);
	if(in.bad())
		return;
	in.clear();
	in.seekg(0, ios::beg);
	if(in.peek() == EOF){
		in.close();
		return;
	}
	if(in.peek() != '*'){
		AppCall::Secretary_Message_Box("Data Error in: ENCOM::Appending_Attribute_to_Edges_in_Pajek()");
		return;
	}
	in.getline(SentenceBuf, MAX_SENTENCE, '\n');
	Ogirin_v.push_back(SentenceBuf);
	in.getline(SentenceBuf, MAX_SENTENCE, '\n');
	Ogirin_v.push_back(SentenceBuf);
	istringstream instream(SentenceBuf);
	instream.getline(SentenceBuf, MAX_SENTENCE, ' ');
	instream >> vertex_cnt;
	for(size_t i = 0; i < vertex_cnt; i++){
		in.getline(SentenceBuf, MAX_SENTENCE, '\n');
		Ogirin_v.push_back(SentenceBuf);
	}
	while(in.peek() != '*'){
		in.getline(SentenceBuf, MAX_SENTENCE, '\n');
		Ogirin_v.push_back(SentenceBuf);
	}
	in.getline(SentenceBuf, MAX_SENTENCE, '\n');
	Ogirin_v.push_back(SentenceBuf);
	istringstream edgestream(SentenceBuf);
	edgestream.getline(SentenceBuf, MAX_SENTENCE, '*');
	edgestream.getline(SentenceBuf, MAX_SENTENCE, '\n');
	if(strcmp(SentenceBuf, "Edges")){
		AppCall::Secretary_Message_Box("Data Error in: ENCOM::Appending_Attribute_to_Edges_in_Pajek()");
		return;
	}
	size_t EdgesID = 0;
	while(in.peek() != '\n' && in.peek() != EOF){
		ostringstream ostream;
		in.getline(SentenceBuf, MAX_SENTENCE, '\n');
		ostream << SentenceBuf;
		ostream << "    " << VECTOR(attr_v)[EdgesID++];
		Ogirin_v.push_back(ostream.str());
	}
	while(in.peek() != EOF){
		in.getline(SentenceBuf, MAX_SENTENCE, '\n');
		Ogirin_v.push_back(SentenceBuf);

	}
	in.close();

	Save_Vector_String_to_Pajek(cFilePath, Ogirin_v);
}

void PAJEK::Appending_Vertex_Info_to_Pajek(const char* cFilePath, map<size_t, string>& VertexInfo_m)
{
	char SentenceBuf[MAX_SENTENCE];
	vector<string> Ogirin_v;
	size_t vertex_cnt;
	string sentstr;

	ifstream in(cFilePath);
	if(in.bad())
		return;
	in.clear();
	in.seekg(0, ios::beg);
	if(in.peek() == EOF){
		in.close();
		return;
	}
	if(in.peek() != '*'){
		AppCall::Secretary_Message_Box("Data Error in: ENCOM::Appending_Vertex_Info_to_Pajek()");
		return;
	}
	in.getline(SentenceBuf, MAX_SENTENCE, '\n');
	Ogirin_v.push_back(SentenceBuf);
	in.getline(SentenceBuf, MAX_SENTENCE, '\n');
	Ogirin_v.push_back(SentenceBuf);
	istringstream instream(SentenceBuf);
	instream.getline(SentenceBuf, MAX_SENTENCE, ' ');
	instream >> vertex_cnt;
	if(vertex_cnt != VertexInfo_m.size()){
		AppCall::Secretary_Message_Box("Data Error in: ENCOM::Appending_Vertex_Info_to_Pajek()");
		return;
	}

	for(size_t i = 0; i < vertex_cnt; i++){
		ostringstream ostream;
		in.getline(SentenceBuf, MAX_SENTENCE, '\n');
		ostream << SentenceBuf;
		ostream << "    " << "T" << " " << VertexInfo_m[i];
		Ogirin_v.push_back(ostream.str());
	}
	while(in.peek() != EOF){
		in.getline(SentenceBuf, MAX_SENTENCE, '\n');
		Ogirin_v.push_back(SentenceBuf);

	}
	in.close();
	Save_Vector_String_to_Pajek(cFilePath, Ogirin_v);


}

void PAJEK::Appending_Vertex_Weighting_to_Pajek(const char* cFilePath, igraph_vector_t& weight)
{
	char SentenceBuf[MAX_SENTENCE];
	vector<string> Ogirin_v;
	size_t vertex_cnt;
	string sentstr;

	ifstream in(cFilePath);
	if(in.bad())
		return;
	in.clear();
	in.seekg(0, ios::beg);
	if(in.peek() == EOF){
		in.close();
		return;
	}
	if(in.peek() != '*'){
		AppCall::Secretary_Message_Box("Data Error in: ENCOM::Appending_Vertex_Weighting_to_Pajek()");
		return;
	}
	in.getline(SentenceBuf, MAX_SENTENCE, '\n');
	Ogirin_v.push_back(SentenceBuf);
	in.getline(SentenceBuf, MAX_SENTENCE, '\n');
	Ogirin_v.push_back(SentenceBuf);
	istringstream instream(SentenceBuf);
	instream.getline(SentenceBuf, MAX_SENTENCE, ' ');
	instream >> vertex_cnt;
	if(vertex_cnt != igraph_vector_size(&weight)){
		AppCall::Secretary_Message_Box("Data Error in: ENCOM::Appending_Vertex_Weighting_to_Pajek()");
		return;
	}

	for(size_t i = 0; i < vertex_cnt; i++){
		ostringstream ostream;
		in.getline(SentenceBuf, MAX_SENTENCE, '\n');
		ostream << SentenceBuf;
		ostream << "    " << VECTOR(weight)[i];
		Ogirin_v.push_back(ostream.str());
	}
	while(in.peek() != EOF){
		in.getline(SentenceBuf, MAX_SENTENCE, '\n');
		Ogirin_v.push_back(SentenceBuf);

	}
	in.close();
	Save_Vector_String_to_Pajek(cFilePath, Ogirin_v);
}

void PAJEK::Appending_Layout_to_Pajek(const char* cFilePath, igraph_matrix_t& layout_matrix)
{
	char SentenceBuf[MAX_SENTENCE];
	vector<string> Ogirin_v;
	size_t vertex_cnt;
	string sentstr;

	ifstream in(cFilePath);
	if(in.bad())
		return;
	in.clear();
	in.seekg(0, ios::beg);
	if(in.peek() == EOF){
		in.close();
		return;
	}
	if(in.peek() != '*'){
		AppCall::Secretary_Message_Box("Data Error in: ENCOM::Appending_Layout_to_Pajek()");
		return;
	}
	in.getline(SentenceBuf, MAX_SENTENCE, '\n');
	Ogirin_v.push_back(SentenceBuf);
	in.getline(SentenceBuf, MAX_SENTENCE, '\n');
	Ogirin_v.push_back(SentenceBuf);
	istringstream instream(SentenceBuf);
	instream.getline(SentenceBuf, MAX_SENTENCE, ' ');
	instream >> vertex_cnt;
	if(vertex_cnt != igraph_matrix_nrow(&layout_matrix)){
		AppCall::Secretary_Message_Box("Data Error in: ENCOM::Appending_Layout_to_Pajek()");
		return;
	}
	if(2 != igraph_matrix_ncol(&layout_matrix)){
		AppCall::Secretary_Message_Box("Data Error in: ENCOM::Appending_Layout_to_Pajek()");
		return;
	}
	for(size_t i = 0; i < vertex_cnt; i++){
		ostringstream ostream;
		in.getline(SentenceBuf, MAX_SENTENCE, '\n');
		ostream << SentenceBuf;
		ostream << "    " << LAYOUT_REARRANGE(MATRIX(layout_matrix,i,0)) << "    " << LAYOUT_REARRANGE(MATRIX(layout_matrix,i,1));
		Ogirin_v.push_back(ostream.str());
	}
	while(in.peek() != EOF){
		in.getline(SentenceBuf, MAX_SENTENCE, '\n');
		Ogirin_v.push_back(SentenceBuf);
	}
	in.close();

	Save_Vector_String_to_Pajek(cFilePath, Ogirin_v);
}



void PAJEK::Appending_Vertex_Partition_to_Pajek(const char* cFilePath, map<string, size_t>& Partition_m)
{
	map<string, size_t> loc_EncodedVex2ID_m;
	char cEntityBuf[MAX_CLAUSE_LENGTH];
	char sChar[3];
	sChar[2]=0;
	for(map<string, size_t>::iterator mite = Partition_m.begin(); mite != Partition_m.end(); mite++){
		strcpy_s(cEntityBuf, MAX_CLAUSE_LENGTH, "");
		const char* locChar = mite->first.c_str();
		size_t charlen = mite->first.length();
		for(size_t i = 0; i < charlen; ){
			sChar[0] = locChar[i++];
			sChar[1] = 0;
			if(sChar[0] < 0 ){
				sChar[1] = locChar[i++];
			}
			if(!strcmp(sChar, "%")){
				continue;
			}
			strcat_s(cEntityBuf, MAX_CLAUSE_LENGTH, sChar);
		}
		loc_EncodedVex2ID_m.insert(make_pair(ENCOM::EncodeHtml(SCONVERT::StringToWString(cEntityBuf)), mite->second));
	}

	char SentenceBuf[MAX_SENTENCE];
	vector<string> FileLine_v;
	size_t vertex_cnt;
	string sentstr;
	vector<size_t> VertexPartition_v;

	ifstream in(cFilePath);
	if(in.bad())
		return;
	in.clear();
	in.seekg(0, ios::beg);
	if(in.peek() == EOF){
		in.close();
		return;
	}
	if(in.peek() != '*'){
		AppCall::Secretary_Message_Box("Data Error in: ENCOM::Appending_Vertex_Partition_to_Pajek() 0");
		return;
	}
	in.getline(SentenceBuf, MAX_SENTENCE, '\n');
	FileLine_v.push_back(SentenceBuf);
	in.getline(SentenceBuf, MAX_SENTENCE, '\n');
	FileLine_v.push_back(SentenceBuf);
	istringstream instream(SentenceBuf);
	instream.getline(SentenceBuf, MAX_SENTENCE, ' ');
	instream >> vertex_cnt;
	if(vertex_cnt != Partition_m.size()){
		AppCall::Secretary_Message_Box("Data Error in: ENCOM::Appending_Vertex_Partition_to_Pajek() 1");
		return;
	}
	for(size_t i = 0; i < vertex_cnt; i++){
		in.getline(SentenceBuf, MAX_SENTENCE, '\n');
		FileLine_v.push_back(SentenceBuf);
		istringstream instream(SentenceBuf);
		size_t vertex_id;
		while(instream.peek() == ' '){
			instream.getline(SentenceBuf, MAX_SENTENCE, ' ');
		}
		instream >> vertex_id;
		instream.getline(SentenceBuf, MAX_SENTENCE, '\"');
		instream.getline(SentenceBuf, MAX_SENTENCE, '\"');
		if(loc_EncodedVex2ID_m.find(SentenceBuf) == loc_EncodedVex2ID_m.end()){
			AppCall::Secretary_Message_Box("Data Error in: ENCOM::Appending_Vertex_Partition_to_Pajek() 2");
			return;
		}
		VertexPartition_v.push_back(loc_EncodedVex2ID_m[SentenceBuf]);
	}
	while(in.peek() != EOF){
		in.getline(SentenceBuf, MAX_SENTENCE, '\n');
		FileLine_v.push_back(SentenceBuf);
	}
	in.close();
	FileLine_v.push_back("");
	FileLine_v.push_back("*Partition Named Entity TYPE");
	ostringstream ostream;
	ostream << "*Vertices " << loc_EncodedVex2ID_m.size();
	FileLine_v.push_back(ostream.str());
	for(size_t i = 0; i < VertexPartition_v.size(); i++){
		ostream.str("");
		ostream << VertexPartition_v[i];
		FileLine_v.push_back(ostream.str());
	}

	Save_Vector_String_to_Pajek(cFilePath, FileLine_v);
}

void PAJEK::Save_Vector_String_to_Pajek(const char* cFilePath, vector<string>& FileLine_v)
{
	FILE * fout;
	if(fopen_s(&fout, cFilePath, "wb")){
		AppCall::Secretary_Message_Box("Error to Open File in: ENCOM::Save_Vector_String_to_Pajek()");
	}
	for(size_t i = 0; i < FileLine_v.size(); i++){
		fprintf(fout, FileLine_v[i].c_str());
		fprintf(fout, "\n");
	}
	fclose(fout);
}


//=====================ENCOM
void ENCOM::Generating_iGraph_by_Vertex_Set(igraph_t& event_network, map<string, size_t>& vertex_m)
{
	igraph_strvector_t names;

	igraph_i_set_attribute_table(&igraph_cattribute_table);
	igraph_empty(&event_network, vertex_m.size(), 0);

    igraph_strvector_init(&names, 0);
	for(map<string, size_t>::iterator mite = vertex_m.begin(); mite != vertex_m.end(); mite++){
		igraph_strvector_add(&names, ENCOM::EncodeHtml(SCONVERT::StringToWString(mite->first.c_str())).c_str());
	}
	SETVASV(&event_network, "id", &names);
	igraph_strvector_destroy(&names);
}
void ENCOM::Generating_iGraph_by_Vertex_Set(igraph_t& event_network, map<string, double>& NmaedEntity_m, map<string, string>& HtmlDecode_m)
{
	igraph_strvector_t names;

	igraph_i_set_attribute_table(&igraph_cattribute_table);
	igraph_empty(&event_network, NmaedEntity_m.size(), 0);

    igraph_strvector_init(&names, 0);
	for(map<string, double>::iterator mite = NmaedEntity_m.begin(); mite != NmaedEntity_m.end(); mite++){
		HtmlDecode_m.insert(make_pair(ENCOM::EncodeHtml(SCONVERT::StringToWString(mite->first.c_str())).c_str(), mite->first.c_str()));
		igraph_strvector_add(&names, ENCOM::EncodeHtml(SCONVERT::StringToWString(mite->first.c_str())).c_str());
	}
	SETVASV(&event_network, "id", &names);
	igraph_strvector_destroy(&names);
}




void ENCOM::Load_Extracted_Relations_Result(const char* cFilePath, vector<relationinfo*>& pmRelations_v)
{
	char SentenceBuf[MAX_SENTENCE];
	ifstream in(cFilePath);
	if(in.bad())
		return;
	in.clear();
	in.seekg(0, ios::beg);
	if(in.peek() == EOF){
		in.close();
		return;
	}
	string Sent;
	string DOCID;
	size_t SENID;
	while(in.peek() != EOF){
		vector<relationinfo*> loc_SentRelation_v;
		while(in.peek() == '\t'){
			relationinfo* ploc_relationinfo = new relationinfo;
			in.getline(SentenceBuf, MAX_SENTENCE, '\t');
			in.getline(SentenceBuf, MAX_SENTENCE, '\n');
			istringstream instream(SentenceBuf);
			instream.getline(SentenceBuf, MAX_SENTENCE,'\t');
			ploc_relationinfo->arg1 = SentenceBuf;
			instream.getline(SentenceBuf, MAX_SENTENCE,'\t');
			ploc_relationinfo->arg2 = SentenceBuf;
			instream.getline(SentenceBuf, MAX_SENTENCE,'\t');
			ploc_relationinfo->TYPE = SentenceBuf;
			instream >> ploc_relationinfo->value;
			instream.getline(SentenceBuf, MAX_SENTENCE,'\t');
			instream >> ploc_relationinfo->Arg1_ID;
			instream >> ploc_relationinfo->Arg2_ID;
			instream.getline(SentenceBuf, MAX_SENTENCE,'\t');

			pmRelations_v.push_back(ploc_relationinfo);
			loc_SentRelation_v.push_back(ploc_relationinfo);
		}	
		in.getline(SentenceBuf, MAX_SENTENCE, '\n');
		in.getline(SentenceBuf, MAX_SENTENCE, '\n');
		DOCID = SentenceBuf;
		in >> SENID;
		in.getline(SentenceBuf, MAX_SENTENCE, '\n');
		for(size_t i = 0; i < loc_SentRelation_v.size(); i++){
			loc_SentRelation_v[i]->DOCID = DOCID;
			loc_SentRelation_v[i]->SENID = SENID;
		}
	}
	in.close();

}



string ENCOM::EncodeHtml(const wstring& html)
{
	ostringstream result;
	wstring::const_iterator i;
	for (i = html.begin(); i != html.end(); ++i){
		if (*i >= 128)
			result << "&#" << static_cast<int>(*i) << ";";
		else if (*i == '<')
			result << "&lt;";
		else if (*i == '>')
			result << "&gt;";
		else if (*i == '&')
			result << "&amp;";
		else if (*i == '"')
			result << "&quot;";
		else
			result << static_cast<char>(*i); // Copy untranslated
		}
		return result.str();
	}

void ENCOM::Load_Extracted_Entities_Result(const char* cFilePath, vector<entity_mention*>& pmEntities_v)
{

	char SentenceBuf[MAX_SENTENCE];
	ifstream in(cFilePath);
	if(in.bad())
		return;
	in.clear();
	in.seekg(0, ios::beg);
	if(in.peek() == EOF){
		in.close();
		return;
	}
	string Sent;
	while(in.peek() != EOF){
		in.getline(SentenceBuf, MAX_SENTENCE, '\t');
		Sent = SentenceBuf;
		size_t m_size;
		in >> m_size;
		in.getline(SentenceBuf, MAX_SENTENCE, '\n');
		for(size_t k = 0; k < m_size; k++){
			if(in.peek() == EOF){
				return;
			}
			in.getline(SentenceBuf, MAX_SENTENCE, '\n');
			istringstream instream(SentenceBuf);

			entity_mention* p_loc_mention = new entity_mention;
			entity_mention& loc_mention = *p_loc_mention;
			instream.getline(SentenceBuf, MAX_SENTENCE,'\t');
			loc_mention.DOCID = SentenceBuf;
			instream >> loc_mention.CASID; 
			instream.getline(SentenceBuf, MAX_SENTENCE,'\t');
			instream >> loc_mention.SENID;
			instream.getline(SentenceBuf, MAX_SENTENCE,'\t');
			instream.getline(SentenceBuf, MAX_SENTENCE,'\t');
			loc_mention.TYPE = SentenceBuf;
			instream.getline(SentenceBuf, MAX_SENTENCE,'\t');
			loc_mention.SUBTYPE = SentenceBuf;

			instream >> loc_mention.TYPE_V;
			instream.getline(SentenceBuf, MAX_SENTENCE,'\t');
			instream >> loc_mention.SUBTYPE_V;
			instream.getline(SentenceBuf, MAX_SENTENCE,'\t');
				
			instream >> loc_mention.extent.START;
			instream.getline(SentenceBuf, MAX_SENTENCE,'\t');
			instream >> loc_mention.extent.END;
			instream.getline(SentenceBuf, MAX_SENTENCE,'\t');
			instream.getline(SentenceBuf, MAX_SENTENCE,'\t');
			loc_mention.extent.charseq = SentenceBuf;

			instream >> loc_mention.head.START;
			instream.getline(SentenceBuf, MAX_SENTENCE,'\t');
			instream >> loc_mention.head.END;
			instream.getline(SentenceBuf, MAX_SENTENCE,'\t');
			instream.getline(SentenceBuf, MAX_SENTENCE,'\t');
			loc_mention.head.charseq = SentenceBuf;

			pmEntities_v.push_back(p_loc_mention);
		}
	}
	in.close();

}



bool ENCOM::Get_Next_Entity_Mention(ifstream& in, vector<entity_mention>& entity_mention_v, string& Sent)
{
	if(in.peek() == EOF){
		return false;
	}
	char getlineBuf[MAX_SENTENCE];
	in.getline(getlineBuf, MAX_SENTENCE, '\t');
	Sent = getlineBuf;
	size_t m_size;
	in >> m_size;
	in.getline(getlineBuf, MAX_SENTENCE, '\n');
	for(size_t k = 0; k < m_size; k++){
		if(in.peek() == EOF){
			return false;
		}
		in.getline(getlineBuf, MAX_SENTENCE, '\n');
		istringstream instream(getlineBuf);

		entity_mention loc_mention;
		instream.getline(getlineBuf, MAX_SENTENCE,'\t');
		loc_mention.DOCID = getlineBuf;
		instream >> loc_mention.CASID; 
		instream.getline(getlineBuf, MAX_SENTENCE,'\t');
		instream >> loc_mention.SENID;
		instream.getline(getlineBuf, MAX_SENTENCE,'\t');
		instream.getline(getlineBuf, MAX_SENTENCE,'\t');
		loc_mention.TYPE = getlineBuf;
		instream.getline(getlineBuf, MAX_SENTENCE,'\t');
		loc_mention.SUBTYPE = getlineBuf;

		instream >> loc_mention.TYPE_V;
		instream.getline(getlineBuf, MAX_SENTENCE,'\t');
		instream >> loc_mention.SUBTYPE_V;
		instream.getline(getlineBuf, MAX_SENTENCE,'\t');
			
		instream >> loc_mention.extent.START;
		instream.getline(getlineBuf, MAX_SENTENCE,'\t');
		instream >> loc_mention.extent.END;
		instream.getline(getlineBuf, MAX_SENTENCE,'\t');
		instream.getline(getlineBuf, MAX_SENTENCE,'\t');
		loc_mention.extent.charseq = getlineBuf;

		instream >> loc_mention.head.START;
		instream.getline(getlineBuf, MAX_SENTENCE,'\t');
		instream >> loc_mention.head.END;
		instream.getline(getlineBuf, MAX_SENTENCE,'\t');
		instream.getline(getlineBuf, MAX_SENTENCE,'\t');
		loc_mention.head.charseq = getlineBuf;

		entity_mention_v.push_back(loc_mention);
	}
	if(in.peek() == EOF){
		return false;
	}
	else{
		return true;
	}
}


void ENCOM::Generating_Relation_Case(const char* cSent, entity_mention& arg1, entity_mention& arg2, Relation_Case& pmRelation_Case)
{
	if(arg1.CASID == arg2.CASID){
		pmRelation_Case.Faulted_Flag = true;
		return;
	}
	if(arg1.SENID != arg2.SENID){
		pmRelation_Case.Faulted_Flag = true;
		return;
	}
	pmRelation_Case.DOCID = arg1.DOCID;
	pmRelation_Case.SENID = arg1.SENID;
	pmRelation_Case.Faulted_Flag = false;
	pmRelation_Case.TYPE_V = 0;
	pmRelation_Case.relatin_mention.extent.charseq = cSent;
	pmRelation_Case.relatin_mention.extent.START = 0;
	pmRelation_Case.relatin_mention.extent.END = NLPOP::Get_Chinese_Sentence_Length_Counter(cSent);
	
	pmRelation_Case.first_entity.CASID = arg1.CASID;
	pmRelation_Case.first_entity.Entity_TYPE = arg1.TYPE;
	pmRelation_Case.first_entity.Entity_SUBSTYPE = arg1.SUBTYPE;
	pmRelation_Case.first_entity.extent = arg1.extent;
	pmRelation_Case.first_entity.head = arg1.head;

	pmRelation_Case.sencond_entity.CASID = arg2.CASID;
	pmRelation_Case.sencond_entity.Entity_TYPE = arg2.TYPE;
	pmRelation_Case.sencond_entity.Entity_SUBSTYPE = arg2.SUBTYPE;
	pmRelation_Case.sencond_entity.extent = arg2.extent;
	pmRelation_Case.sencond_entity.head = arg2.head;
}
size_t ENCOM::Get_Event_Num(const char* cEVENT_Path)
{
	map<string, size_t> DocID2Event_m;
	AppCall::Load_Document_Events_Info(cEVENT_Path, DocID2Event_m);
	set<size_t> Event_s;
	for(map<string, size_t>::iterator mite = DocID2Event_m.begin(); mite != DocID2Event_m.end(); mite++){
		Event_s.insert(mite->second);
	}
	return Event_s.size();
}

size_t ENCOM::Get_ENTITY_Num(const char* cENTITY_Path)
{
	size_t Entities_Size = 0;
	vector<entity_mention> entity_mention_v;
	string SentMention;
	ifstream in(cENTITY_Path);
	if(in.bad())
		return 0;
	in.clear();
	while(ENCOM::Get_Next_Entity_Mention(in, entity_mention_v, SentMention)){
		Entities_Size += entity_mention_v.size();
		entity_mention_v.clear();
	}
	in.close();
	return Entities_Size;
}
size_t ENCOM::Get_RELATION_Num(const char* cRELATION_Path)
{
	char SentenceBuf[MAX_SENTENCE];
	size_t Relation_Size = 0;
	ifstream in(cRELATION_Path);
	if(in.bad())
		return 0;
	in.clear();
	while(in.peek() != EOF){
		if(in.peek() == '\t'){
			Relation_Size++;
		}
		in.getline(SentenceBuf, MAX_SENTENCE, '\n');
	}
	in.close();
	return Relation_Size;
}