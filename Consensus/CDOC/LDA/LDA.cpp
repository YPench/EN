// LDA.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "LDA.h"
#include "model.h"
#include "..\\DOCCom.h"
#include "..\\CDOC.h"

CLDA::CLDA()
{

}
CLDA::~CLDA()
{

}
int CLDA::LDA_main_Port(int argc, char* argv[])
{
	model lda;

    if (lda.init(argc, argv)) {
	show_help();
	return 1;
    }
    
    if (lda.model_status == MODEL_STATUS_EST || lda.model_status == MODEL_STATUS_ESTC) {
	// parameter estimation
	lda.estimate();
    }
    
    if (lda.model_status == MODEL_STATUS_INF) {
	// do inference
	lda.inference();
    }
    
    return 0;
}
int CLDA::LDA_estimate_Port()
{
	//src/lda -est -alpha 0.5 -beta 0.1 -ntopics 100 -niters 1000 -savestep 100 -twords 20 -dfile models/casestudy/trndocs.dat

	char str[25];
	char* argvector[MAX_PARAMETERS];
	if(m_p_CDOC->m_DebugDoc_Flag){
		m_p_CDOC->m_niters = 10;
	}
	vector<string> parameter_v;
	parameter_v.push_back("-est");
	parameter_v.push_back("-alpha");
	_gcvt_s(str, 25, m_p_CDOC->m_alpha, 10);
	parameter_v.push_back(str);
	parameter_v.push_back("-beta");
	_gcvt_s(str, 25, m_p_CDOC->m_beta, 10);
	parameter_v.push_back(str);
	parameter_v.push_back("-ntopics");
	_itoa_s(m_p_CDOC->m_Events, str, 25, 10);
	parameter_v.push_back(str);
	parameter_v.push_back("-niters");
	_itoa_s(m_p_CDOC->m_niters, str, 25, 10);
	parameter_v.push_back(str);
	parameter_v.push_back("-savestep");
	_itoa_s(m_p_CDOC->m_niters+5, str, 25, 10);
	parameter_v.push_back(str);
	parameter_v.push_back("-twords");
	_itoa_s(m_p_CDOC->m_twords, str, 25, 10);
	parameter_v.push_back(str);
	parameter_v.push_back("-dfile");
	parameter_v.push_back("Matrix.DAT");

	for(size_t i = 0; i < MAX_PARAMETERS; i++){
		argvector[i] = new char[256];
	}
	for(size_t i = 0; i < parameter_v.size(); i++){
		strcpy_s(argvector[i], 256, parameter_v[i].c_str());
	}
	
	LDA_main_Port(parameter_v.size(), argvector);

	for(size_t i = 0; i < MAX_PARAMETERS; i++){
		delete argvector[i];
	}
	return 0;
}






void CLDA::show_help() {
	if(LDA_PRINTF_ENABLE){
		DOCCOM::LDA_printf("Command line usage:\n");
		DOCCOM::LDA_printf("\tlda -est -alpha <double> -beta <double> -ntopics <int> -niters <int> -savestep <int> -twords <int> -dfile <string>\n");
		DOCCOM::LDA_printf("\tlda -estc -dir <string> -model <string> -niters <int> -savestep <int> -twords <int>\n");
		DOCCOM::LDA_printf("\tlda -inf -dir <string> -model <string> -niters <int> -twords <int> -dfile <string>\n");
	}
	else{
		printf("Command line usage:\n");
		printf("\tlda -est -alpha <double> -beta <double> -ntopics <int> -niters <int> -savestep <int> -twords <int> -dfile <string>\n");
		printf("\tlda -estc -dir <string> -model <string> -niters <int> -savestep <int> -twords <int>\n");
		printf("\tlda -inf -dir <string> -model <string> -niters <int> -twords <int> -dfile <string>\n");
		// printf("\tlda -inf -dir <string> -model <string> -niters <int> -twords <int> -dfile <string> -withrawdata\n");
	}
}
