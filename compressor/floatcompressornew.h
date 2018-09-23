/*
===============================================================================

  FILE:	floatcompressornew.h
	
	CONTENTS:

    first codes the signexponent, then the mantissa
    
    - codes the signexponent in dependence of the predicted signexponent
    
    - codes a corrector between predicted and actual mantissa in dependence
      of the actual exponent.
      for this it first encodes the number of correctly predicted mantissa
      bits using the exponent as the context (or rather the number of leading
      corrector bits that are zero). the remaining incorrectly predicted bits
      are then compressed in one or two chunks.
	
    the compressor has three contexts to compress (none, last, across) in case
    you have predictions of differing quality. i use them like this:

    none for floats without prediction (uses the last float it saw)
    last for floats with delta predictions
    across for floats with the best predictions

    offering three contexts is somewhat arbitrary and a leftover from the
    fact that there were three position predictions of different quality
    (none, last, across) ... but it should should work for now ...

  PROGRAMMERS:
	
		martin isenburg@cs.unc.edu
	
	COPYRIGHT:
	
		Copyright (C) 2000  Martin Isenburg (isenburg@cs.unc.edu)
		
		This software is distributed in the hope that it will be useful,
		but WITHOUT ANY WARRANTY; without even the implied warranty of
		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
	
	CHANGE HISTORY:
	
    30 June 2004 -- complete redone of sign and exponent compression
    30 June 2004 -- now for k >= HIGHBITS-1 (combine tables into rmMantissa)
    29 June 2004 -- use single rmMantissaHigh table for all k >= HIGHBITS
    28 June 2004 -- share rmMantissaHigh and rmMantissaLow tables 
    28 June 2004 -- now using the last float for none predictions 
		26 June 2004 -- created after a good Masman Thai curry with Henna
	
===============================================================================
*/
#ifndef FLOAT_COMPRESSOR_NEW_H
#define FLOAT_COMPRESSOR_NEW_H

#include "mydefs.h"

#include "rangeencoder.h"
#include "rangedecoder.h"
#include "rangemodel.h"

#include <stdio.h>

#include <iostream>
#include <fstream>
using namespace std;

class FloatCompressorNew
{
public:

	// SetupCompressor:
	void SetupCompressor(RangeEncoder* re);

	void SaveChars(string numF,string dataF);

	void FinishCompressor();

	// Compress:
	F32 CompressNone(F32 fReal);
	F32 CompressLast(F32 fPred, F32 fReal);
	F32 CompressAcross(F32 fPred, F32 fReal);


	void NewSetupDC(string numF,string dataF);



	// Decompress:
	void SetupDecompressor(RangeDecoder* rd);

	void FinishDecompressor();
	
	// Compress:
	F32 DecompressNone();
	F32 DecompressLast(F32 fPred);
	F32 DecompressAcross(F32 fPred);


	// Constructor:
	FloatCompressorNew();
	// Destructor:
  ~FloatCompressorNew();

  // for experiments
  int GetTotalBytes();
  int GetSignExponentBytes();
  int GetMantissaBytes();
  int GetMantissaBytes(int);

private:

//  int sum_of_k[256];
//  int num_of_k[256];

  // Private Functions

  void alloc_range_tables();
	void dealloc_range_tables();

	void compress_signexponent(int signexponentPred, int signexponentReal, 
		RangeEncoder* re_signexponent, RangeModel** rmSignExponent);

	int decompress_signexponent(int signexponentPred, RangeDecoder* rd_signexponent,
		RangeModel** rmSignExponent);

	int compress_mantissa(int exponent, int mantissaPred, 
		int mantissaReal, RangeEncoder** re_mantissa, RangeModel** rmCorrectorBits);

	int decompress_mantissa(int exponent, int mantissaPred,
		RangeDecoder** rd_mantissa, RangeModel** rmCorrectorBits);


  // Private Variables

  RangeEncoder* ae_signexponent_none;
	RangeEncoder** ae_mantissa_none;

	RangeEncoder* ae_signexponent_last;
	RangeEncoder** ae_mantissa_last;

	RangeEncoder* ae_signexponent_across;
	RangeEncoder** ae_mantissa_across;

  RangeDecoder* ad_signexponent_none;
	RangeDecoder** ad_mantissa_none;

	RangeDecoder* ad_signexponent_last;
	RangeDecoder** ad_mantissa_last;

	RangeDecoder* ad_signexponent_across;
	RangeDecoder** ad_mantissa_across;

	RangeModel** rmSignExponentNone;
  RangeModel** rmMantissaBitsNone;

	RangeModel** rmSignExponentLast;
  RangeModel** rmMantissaBitsLast;

	RangeModel** rmSignExponentAcross;
  RangeModel** rmMantissaBitsAcross;

  RangeModel* rmExponentOther;

  RangeModel** rmMantissa;

  F32 last_float;
};

#endif
