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

//#define CONSENSUS_CONFIG

#ifdef CONSENSUS_CONFIG
	#define CONSENSUS_CEDT
	#define CONSENSUS_CRDC
	#define CONSENSUS_ECOR
	#define CONSENSUS_CDOC
	#define CONSENSUS_ENET
	//#define CONSENSUS_OTHER
	//#define CONSENSUS_CRDC

#else
	//#undef CONSENSUS_CEDT
	//#undef CONSENSUS_CRDC

#endif

//#define CONSENSUS_CDOC
//#define CONSENSUS_CRDC
//#define CONSENSUS_ECOR
#define CONSENSUS_CRDC
//#define CONSENSUS_ENET

//#define ECOR_DATA_CHECKING

