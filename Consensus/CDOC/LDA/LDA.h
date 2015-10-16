#pragma once
#include "CGCom.h"
#include "ace.h"

class CDOC;
class CLDA
{
public:
	CDOC* m_p_CDOC;
	CLDA();
	~CLDA();
	int LDA_main_Port(int argc, char* argv[]);
	int LDA_estimate_Port();
	void show_help();

};
