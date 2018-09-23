/*
===============================================================================

  FILE:  floatcompressornew.cpp
  
  CONTENTS:

    see corresponding header file

  PROGRAMMERS:
  
    martin isenburg@cs.unc.edu
  
  COPYRIGHT:
  
    Copyright (C) 2000  Martin Isenburg (isenburg@cs.unc.edu)
    
    This software is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

  CHANGE HISTORY:
  
    see corresponding header file
  
===============================================================================
*/

#include "floatcompressornew.h"

#include <stdlib.h>

#define BITS_HIGH 12

FloatCompressorNew::FloatCompressorNew()
{
  ae_mantissa_none = (RangeEncoder**)malloc(sizeof(RangeEncoder*)*3);
  ae_mantissa_last = (RangeEncoder**)malloc(sizeof(RangeEncoder*)*3);
  ae_mantissa_across = (RangeEncoder**)malloc(sizeof(RangeEncoder*)*3);

  ad_mantissa_none = (RangeDecoder**)malloc(sizeof(RangeDecoder*)*3);
  ad_mantissa_last = (RangeDecoder**)malloc(sizeof(RangeDecoder*)*3);
  ad_mantissa_across = (RangeDecoder**)malloc(sizeof(RangeDecoder*)*3);
}

FloatCompressorNew::~FloatCompressorNew()
{
  free(ae_mantissa_none);
  free(ae_mantissa_last);
  free(ae_mantissa_across);

  free(ad_mantissa_none);
  free(ad_mantissa_last);
  free(ad_mantissa_across);
}

void FloatCompressorNew::SetupCompressor(RangeEncoder* re)
{
  last_float = 0;

  if (re == 0)
  {
    // none predictions

	RangeEncoder ren(0,0);
    ae_signexponent_none = new RangeEncoder(0);
    ae_mantissa_none[0] = new RangeEncoder(0);
    ae_mantissa_none[1] = new RangeEncoder(0);
    ae_mantissa_none[2] = new RangeEncoder(0);

    // last predictions
    ae_signexponent_last = new RangeEncoder(0);
    ae_mantissa_last[0] = new RangeEncoder(0);
    ae_mantissa_last[1] = new RangeEncoder(0);
    ae_mantissa_last[2] = new RangeEncoder(0);

    // across predictions
    ae_signexponent_across = new RangeEncoder(0);
    ae_mantissa_across[0] = new RangeEncoder(0);
    ae_mantissa_across[1] = new RangeEncoder(0);
    ae_mantissa_across[2] = new RangeEncoder(0);
  }
  else
  {
    // none predictions
    ae_signexponent_none = re;
    ae_mantissa_none[0] = re;
    ae_mantissa_none[1] = re;
    ae_mantissa_none[2] = re;

    // last predictions
    ae_signexponent_last = re;
    ae_mantissa_last[0] = re;
    ae_mantissa_last[1] = re;
    ae_mantissa_last[2] = re;

    // across predictions
    ae_signexponent_across = re;
    ae_mantissa_across[0] = re;
    ae_mantissa_across[1] = re;
    ae_mantissa_across[2] = re;
  }
  alloc_range_tables();
}

void FloatCompressorNew::SaveChars(string numF,string dataF){

	int number=0;
	unsigned char *chars;

	ofstream *of=new ofstream(numF.c_str());

	FILE *fp=fopen(dataF.c_str(),"wb");

	
	chars=ae_signexponent_none->getChars();
	number=ae_signexponent_none->getNumberChars();
	*of <<number <<endl;
	//fputc(number,fp);
	for(int i=0;i<number;i++){
		
		fputc(chars[i], fp);
	}


	chars=ae_mantissa_none[0]->getChars();
	number=ae_mantissa_none[0]->getNumberChars();
	*of <<number <<endl;
	//fputc(number,fp);
	for(int i=0;i<number;i++){
		
		fputc(chars[i], fp);
	}


	chars=ae_mantissa_none[1]->getChars();
	number=ae_mantissa_none[1]->getNumberChars();
	*of <<number <<endl;
	//fputc(number,fp);
	for(int i=0;i<number;i++){
		
		fputc(chars[i], fp);
	}
	

	chars=ae_mantissa_none[2]->getChars();
	number=ae_mantissa_none[2]->getNumberChars();
	*of <<number <<endl;
	//fputc(number,fp);
	for(int i=0;i<number;i++){
		
		fputc(chars[i], fp);
	}
	

	chars=ae_signexponent_last->getChars();
	number=ae_signexponent_last->getNumberChars();
	*of <<number <<endl;
	//fputc(number,fp);
	for(int i=0;i<number;i++){
		
		fputc(chars[i], fp);
	}
	

	chars=ae_mantissa_last[0]->getChars();
	number=ae_mantissa_last[0]->getNumberChars();
	*of <<number <<endl;
	//fputc(number,fp);
	for(int i=0;i<number;i++){
		
		fputc(chars[i], fp);
	}
	

	chars=ae_mantissa_last[1]->getChars();
	number=ae_mantissa_last[1]->getNumberChars();
	*of <<number <<endl;
	//fputc(number,fp);
	for(int i=0;i<number;i++){
		
		fputc(chars[i], fp);
	}
	

	chars=ae_mantissa_last[2]->getChars();
	number=ae_mantissa_last[2]->getNumberChars();
	*of <<number <<endl;
	//fputc(number,fp);
	for(int i=0;i<number;i++){
		
		fputc(chars[i], fp);
	}
	

	chars=ae_signexponent_across->getChars();
	number=ae_signexponent_across->getNumberChars();
	*of <<number <<endl;
	//fputc(number,fp);
	for(int i=0;i<number;i++){
		
		fputc(chars[i], fp);
	}
	

	chars=ae_mantissa_across[0]->getChars();
	number=ae_mantissa_across[0]->getNumberChars();
	*of <<number <<endl;
	//fputc(number,fp);
	for(int i=0;i<number;i++){
		
		fputc(chars[i], fp);
	}
	

	chars=ae_mantissa_across[1]->getChars();
	number=ae_mantissa_across[1]->getNumberChars();
	*of <<number <<endl;
	//fputc(number,fp);
	for(int i=0;i<number;i++){
		
		fputc(chars[i], fp);
	}
	

	chars=ae_mantissa_across[2]->getChars();
	number=ae_mantissa_across[2]->getNumberChars();
	*of <<number <<endl;
	//fputc(number,fp);
	for(int i=0;i<number;i++){
		
		fputc(chars[i], fp);
	}
	
	
	of->close();
	fclose(fp);
}

void FloatCompressorNew::NewSetupDC(string numF,string dataF){

  last_float = 0;


	int number=0;
	unsigned char *chars;

//	int c;
	//unsigned char c;

	ifstream *inf=new ifstream(numF.c_str());

	FILE *fp=fopen(dataF.c_str(),"rb");


	*inf >>number;
	//number=fgetc(fp);
	chars = (unsigned char*) malloc(sizeof(unsigned char)*number);
	for(int i=0;i<number;i++){

		chars[i] = fgetc(fp);
	}
	ad_signexponent_none = new RangeDecoder(chars, number);


	*inf >>number;
	//number=fgetc(fp);
	chars = (unsigned char*) malloc(sizeof(unsigned char)*number);
	for(int i=0;i<number;i++){

		chars[i] = fgetc(fp);
	}
	ad_mantissa_none[0] = new RangeDecoder(chars, number);


	*inf >>number;
	//number=fgetc(fp);
	chars = (unsigned char*) malloc(sizeof(unsigned char)*number);
	for(int i=0;i<number;i++){

		chars[i] = fgetc(fp);
	}
	ad_mantissa_none[1] = new RangeDecoder(chars, number);


	*inf >>number;
	//number=fgetc(fp);
	chars = (unsigned char*) malloc(sizeof(unsigned char)*number);
	for(int i=0;i<number;i++){

		chars[i] = fgetc(fp);
	}
	ad_mantissa_none[2] = new RangeDecoder(chars, number);


	*inf >>number;
	//number=fgetc(fp);
	chars = (unsigned char*) malloc(sizeof(unsigned char)*number);
	for(int i=0;i<number;i++){

		chars[i] = fgetc(fp);
	}
	ad_signexponent_last = new RangeDecoder(chars, number);


	*inf >>number;
	//number=fgetc(fp);
	chars = (unsigned char*) malloc(sizeof(unsigned char)*number);
	for(int i=0;i<number;i++){

		chars[i] = fgetc(fp);
	}
	ad_mantissa_last[0] = new RangeDecoder(chars, number);


	*inf >>number;
	//number=fgetc(fp);
	chars = (unsigned char*) malloc(sizeof(unsigned char)*number);
	for(int i=0;i<number;i++){

		chars[i] = fgetc(fp);
	}
	ad_mantissa_last[1] = new RangeDecoder(chars, number);


	*inf >>number;
	//number=fgetc(fp);
	chars = (unsigned char*) malloc(sizeof(unsigned char)*number);
	for(int i=0;i<number;i++){

		chars[i] = fgetc(fp);
	}
	ad_mantissa_last[2] = new RangeDecoder(chars, number);


	*inf >>number;
	//number=fgetc(fp);
	chars = (unsigned char*) malloc(sizeof(unsigned char)*number);
	for(int i=0;i<number;i++){

		chars[i] = fgetc(fp);
	}
	ad_signexponent_across = new RangeDecoder(chars, number);


	*inf >>number;
	//number=fgetc(fp);
	chars = (unsigned char*) malloc(sizeof(unsigned char)*number);
	for(int i=0;i<number;i++){

		chars[i] = fgetc(fp);
	}
	ad_mantissa_across[0] = new RangeDecoder(chars, number);


	*inf >>number;
	//number=fgetc(fp);
	chars = (unsigned char*) malloc(sizeof(unsigned char)*number);
	for(int i=0;i<number;i++){

		chars[i] = fgetc(fp);
	}
	ad_mantissa_across[1] = new RangeDecoder(chars, number);


	*inf >>number;
	//number=fgetc(fp);
	chars =new unsigned char[number]; 
		//(unsigned char*) malloc(sizeof(unsigned char)*number);
	if(chars==0) cout <<"error" <<endl;
	for(int i=0;i<number;i++){

		chars[i] = fgetc(fp);
	}
	ad_mantissa_across[2] = new RangeDecoder(chars, number);

	inf->close();
	fclose(fp);

  alloc_range_tables();
}

void FloatCompressorNew::SetupDecompressor(RangeDecoder* rd)
{
  last_float = 0;

  if (rd == 0)
  {
    // none predictions
    ad_signexponent_none = new RangeDecoder(ae_signexponent_none->getChars(), ae_signexponent_none->getNumberChars());
    ad_mantissa_none[0] = new RangeDecoder(ae_mantissa_none[0]->getChars(), ae_mantissa_none[0]->getNumberChars());
    ad_mantissa_none[1] = new RangeDecoder(ae_mantissa_none[1]->getChars(), ae_mantissa_none[1]->getNumberChars());
    ad_mantissa_none[2] = new RangeDecoder(ae_mantissa_none[2]->getChars(), ae_mantissa_none[2]->getNumberChars());

    // last predictions
    ad_signexponent_last = new RangeDecoder(ae_signexponent_last->getChars(), ae_signexponent_last->getNumberChars());
    ad_mantissa_last[0] = new RangeDecoder(ae_mantissa_last[0]->getChars(), ae_mantissa_last[0]->getNumberChars());
    ad_mantissa_last[1] = new RangeDecoder(ae_mantissa_last[1]->getChars(), ae_mantissa_last[1]->getNumberChars());
    ad_mantissa_last[2] = new RangeDecoder(ae_mantissa_last[2]->getChars(), ae_mantissa_last[2]->getNumberChars());

    // across predictions
    ad_signexponent_across = new RangeDecoder(ae_signexponent_across->getChars(), ae_signexponent_across->getNumberChars());
    ad_mantissa_across[0] = new RangeDecoder(ae_mantissa_across[0]->getChars(), ae_mantissa_across[0]->getNumberChars());
    ad_mantissa_across[1] = new RangeDecoder(ae_mantissa_across[1]->getChars(), ae_mantissa_across[1]->getNumberChars());
    ad_mantissa_across[2] = new RangeDecoder(ae_mantissa_across[2]->getChars(), ae_mantissa_across[2]->getNumberChars());
  }
  else
  {
    // none predictions
    ad_signexponent_none = rd;
    ad_mantissa_none[0] = rd;
    ad_mantissa_none[1] = rd;
    ad_mantissa_none[2] = rd;

    // last predictions
    ad_signexponent_last = rd;
    ad_mantissa_last[0] = rd;
    ad_mantissa_last[1] = rd;
    ad_mantissa_last[2] = rd;

    // across predictions
    ad_signexponent_across = rd;
    ad_mantissa_across[0] = rd;
    ad_mantissa_across[1] = rd;
    ad_mantissa_across[2] = rd;
  }
  alloc_range_tables();
}

void FloatCompressorNew::alloc_range_tables()
{
  int i;

  rmSignExponentNone = (RangeModel**)malloc(sizeof(RangeModel*)*256);
  rmMantissaBitsNone = (RangeModel**)malloc(sizeof(RangeModel*)*256);

  rmSignExponentLast = (RangeModel**)malloc(sizeof(RangeModel*)*256);
  rmMantissaBitsLast = (RangeModel**)malloc(sizeof(RangeModel*)*256);

  rmSignExponentAcross = (RangeModel**)malloc(sizeof(RangeModel*)*256);
  rmMantissaBitsAcross = (RangeModel**)malloc(sizeof(RangeModel*)*256);

  for (i = 0; i < 256; i++)
  {
    rmSignExponentNone[i] = 0;
    rmSignExponentLast[i] = 0;
    rmSignExponentAcross[i] = 0;
    rmMantissaBitsNone[i] = 0;
    rmMantissaBitsLast[i] = 0;
    rmMantissaBitsAcross[i] = 0;
  }

  rmExponentOther = 0;

  // entries [0] to [BITS_HIGH-1] are for the lower corrector bits
  // while [BITS_HIGH] to [22]  are used for corrector's high bits

  rmMantissa = (RangeModel**)malloc(sizeof(RangeModel*)*23); 

  for (i = 0; i < 23; i++)
  {
    rmMantissa[i] = 0;
  }
}

void FloatCompressorNew::dealloc_range_tables()
{
  int i;

	for (i = 0; i < 256; i++)
	{
		if (rmSignExponentNone[i]) delete rmSignExponentNone[i];
		if (rmSignExponentLast[i]) delete rmSignExponentLast[i];
		if (rmSignExponentAcross[i]) delete rmSignExponentAcross[i];
		if (rmMantissaBitsNone[i]) delete rmMantissaBitsNone[i];
		if (rmMantissaBitsLast[i]) delete rmMantissaBitsLast[i];
		if (rmMantissaBitsAcross[i]) delete rmMantissaBitsAcross[i];
	}

  free(rmSignExponentNone);
  free(rmSignExponentLast);
  free(rmSignExponentAcross);

  free(rmMantissaBitsNone);
  free(rmMantissaBitsLast);
  free(rmMantissaBitsAcross);

  if (rmExponentOther) delete rmExponentOther;

	for (i = 0; i < 23; i++)
	{
		if (rmMantissa[i]) delete rmMantissa[i];
	}
  free(rmMantissa);
}

void FloatCompressorNew::FinishCompressor()
{
  if (ae_signexponent_none != ae_signexponent_last)
  {
    ae_signexponent_none->done();
    ae_signexponent_last->done();
	ae_signexponent_across->done();
	ae_mantissa_none[0]->done();
	ae_mantissa_last[0]->done();
	ae_mantissa_across[0]->done();
	ae_mantissa_none[1]->done();
	ae_mantissa_last[1]->done();
	ae_mantissa_across[1]->done();
	ae_mantissa_none[2]->done();
	ae_mantissa_last[2]->done();
	ae_mantissa_across[2]->done();
  }
  dealloc_range_tables();
}

void FloatCompressorNew::FinishDecompressor()
{
  if (ad_signexponent_none != ad_signexponent_last)
  {
    ad_signexponent_none->done();
    ad_signexponent_last->done();
    ad_signexponent_across->done();

    ad_mantissa_none[0]->done();
    ad_mantissa_last[0]->done();
    ad_mantissa_across[0]->done();

    ad_mantissa_none[1]->done();
    ad_mantissa_last[1]->done();
    ad_mantissa_across[1]->done();

    ad_mantissa_none[2]->done();
    ad_mantissa_last[2]->done();
    ad_mantissa_across[2]->done();
  }
  dealloc_range_tables();
}

int FloatCompressorNew::GetTotalBytes()
{
  return GetSignExponentBytes()+GetMantissaBytes();
}

int FloatCompressorNew::GetSignExponentBytes()
{
  return ae_signexponent_none->getNumberChars()+ae_signexponent_last->getNumberChars()+ae_signexponent_across->getNumberChars();
}

int FloatCompressorNew::GetMantissaBytes()
{
  return GetMantissaBytes(0)+GetMantissaBytes(1)+GetMantissaBytes(2);
}

int FloatCompressorNew::GetMantissaBytes(int i)
{
  return ae_mantissa_none[i]->getNumberChars()+ae_mantissa_last[i]->getNumberChars()+ae_mantissa_across[i]->getNumberChars();
}

#define DIFFERENT_SIGN    0
#define EXPONENT_MINUS_3  1
#define EXPONENT_MINUS_2  2
#define EXPONENT_MINUS_1  3
#define SAME_EXPONENT     4
#define EXPONENT_PLUS_1   5
#define EXPONENT_PLUS_2   6
#define EXPONENT_PLUS_3   7
#define OTHER_EXPONENT    8

void FloatCompressorNew::compress_signexponent(int signexponentPred, int signexponentReal, RangeEncoder* re_signexponent, RangeModel** rmSignExponent)
{
/*
	int 

	if (rmSignExponent[signexponentPred&0xFF] == 0)
  {
    rmSignExponent[signexponentPred&0xFF] = new RangeModel(9, 0, 1, 8192, 16);
  }
  if (signexponentPred == signexponentReal)
  {
    re_signexponent->encode(rmSignExponent[signexponentPred&0xFF],SAME_EXPONENT);
  }
  else
  {
    if ((signexponentPred & 0x100) == (signexponentReal & 0x100)) // same sign ?
    {
      int diff = signexponentReal - signexponentPred; // exponent difference
      if ((-3 <= diff) && (diff <= 3))
      {
        re_signexponent->encode(rmSignExponent[signexponentPred&0xFF],SAME_EXPONENT+diff);
      }
      else
      {
        re_signexponent->encode(rmSignExponent[signexponentPred&0xFF],OTHER_EXPONENT);
        if (rmExponentOther == 0)
        {
          rmExponentOther = new RangeModel(255, 0, 1, 8192, 16);
        }
        re_signexponent->encode(rmExponentOther,signexponentReal&0xFF);
      }
    }
    else
    {
      re_signexponent->encode(rmSignExponent[signexponentPred&0xFF],DIFFERENT_SIGN);
      if (rmExponentOther == 0)
      {
        rmExponentOther = new RangeModel(255, 0, 1, 8192, 16);
      }
      re_signexponent->encode(rmExponentOther,signexponentReal&0xFF);
    }
  }
*/

  if (rmSignExponent[signexponentPred&0xFF] == 0)
  {
    rmSignExponent[signexponentPred&0xFF] = new RangeModel(9, 0, 1, 8192, 16);
  }
  
  //判断符号位、指数是否相同
  if (signexponentPred == signexponentReal){

    re_signexponent->encode(rmSignExponent[signexponentPred&0xFF],SAME_EXPONENT);
  }
  
  //不相同
  else{

	  //判断符号位是否相同
    if ((signexponentPred & 0x100) == (signexponentReal & 0x100)){

		//指数的差值
      int diff = signexponentReal - signexponentPred;
      
	  //符号位相同，判断指数的差值是否在区间[-3, 3]内
	  if ((-3 <= diff) && (diff <= 3)){

        re_signexponent->encode(rmSignExponent[signexponentPred&0xFF],SAME_EXPONENT+diff);
      }

      //指数的差值不在区间内
	  else{

        re_signexponent->encode(rmSignExponent[signexponentPred&0xFF],OTHER_EXPONENT);
        if (rmExponentOther == 0){

          rmExponentOther = new RangeModel(255, 0, 1, 8192, 16);
        }
        re_signexponent->encode(rmExponentOther,signexponentReal&0xFF);
      }
    }
	
	//符号位不同
    else{

      re_signexponent->encode(rmSignExponent[signexponentPred&0xFF],DIFFERENT_SIGN);

      if (rmExponentOther == 0){

        rmExponentOther = new RangeModel(255, 0, 1, 8192, 16);
      }
      re_signexponent->encode(rmExponentOther,signexponentReal&0xFF);
    }
  }
}

int FloatCompressorNew::decompress_signexponent(int signexponentPred, RangeDecoder* rd_signexponent, RangeModel** rmSignExponent)
{


	if (rmSignExponent[signexponentPred&0xFF] == 0){

		rmSignExponent[signexponentPred&0xFF] = new RangeModel(9, 0, 0, 8192, 16);
	}
  
	int signexponentReal = rd_signexponent->decode(rmSignExponent[signexponentPred&0xFF]);

	if (signexponentReal == SAME_EXPONENT){

		return signexponentPred;
	}
  else
  {
    if ((EXPONENT_MINUS_3 <= signexponentReal) && (signexponentReal <= EXPONENT_PLUS_3))
    {
      return signexponentPred - (SAME_EXPONENT-signexponentReal);
    }
    else
    {
      if (rmExponentOther == 0)
      {
        rmExponentOther = new RangeModel(255, 0, 0, 8192, 16);
      }
      if (signexponentReal == OTHER_EXPONENT) // but same sign
      {
        return (signexponentPred & 0x100) | rd_signexponent->decode(rmExponentOther);
      }
      else  // ... but different sign
      {
		return ((~signexponentPred) & 0x100) | rd_signexponent->decode(rmExponentOther);
      }
    }
  }
}

int FloatCompressorNew::compress_mantissa(int exponent, int mantissaPred, int mantissaReal, RangeEncoder* re_mantissa[3], RangeModel** rmMantissaBits)
{
  int c, c1, k, k1;

  // the corrector c will be within the interval [ - (2^23 - 1)  ...  + (2^23 - 1) 

  c = mantissaReal - mantissaPred;

  // we fold c into the interval [ - (2^22 - 1)  ...  + (2^22) ]

  if (c < (1-(1<<22)))
  {
    c += (1<<23);
  }
  else if (c > (1<<22))
  {
    c -= (1<<23); 
  }

  // now we find the tighest interval [ - (2^k - 1)  ...  + (2^k) ] that contains c

  k = 0;

  // we adjust c1 for positive c to eliminate special handling in case c is 2^k

  c1 = (c <= 0 ? -c : c-1);

  // this loop could be replaced with more efficient code

  while (c1)
  {
    c1 = c1 >> 1;
    k = k + 1;
  }

  // now the number k will be between 0 and 22

  if (rmMantissaBits[exponent] == 0)
  {
    rmMantissaBits[exponent] = new RangeModel(23,0,1,512,16);
  }

  // compress (in dependence on the exponent) the number of bits k+1 that we need to store to correct the mantissa
//cout <<exponent <<' ' <<k <<endl;
  re_mantissa[0]->encode(rmMantissaBits[exponent], k);
//cout <<2 <<endl;
  // compress the k+1 bits of the mantissa corrector

  if (k < BITS_HIGH)
  {
    // we translate c into the interval [ 0 ...  + (2^(k+1)-1) ]

    c += ((1<<k) - 1);

    // allocate a new range table if necessary

    if (rmMantissa[k] == 0)
    {
      rmMantissa[k] = new RangeModel((1<<(k+1)),0,1,8192,16);
    }

    // compress c with the range coder

    re_mantissa[1]->encode(rmMantissa[k], c);
  }
  else
  {
    // we need to break up the k+1 bits in two smaller bit patterns

    k1 = k-BITS_HIGH+1;

    // c1 represents the lowest k-BITS_HIGH+1 bits
    // c represents the highest BITS_HIGH bits

    if (c < 0)
    {
      c1 = (-c) & ((1<<k1) - 1);
      c = -((-c) >> k1);
    }
    else
    {
      c1 = c & ((1<<k1) - 1);
      c = c >> k1;
    }

    // we translate c into the interval [ 0 ...  + (2^(BITS_HIGH)-1) ]

    c += ((1<<(BITS_HIGH-1)) - 1);

    // allocate a new range table if necessary

    if (rmMantissa[k] == 0)
    {
      if (rmMantissa[BITS_HIGH-1] == 0)
      {
        rmMantissa[BITS_HIGH-1] = new RangeModel(1<<BITS_HIGH,0,1,8192,16);
      }
//cout <<3 <<endl;
      rmMantissa[k] = new RangeModel(1<<k1,0,1,2048,16);
    }
//cout <<4 <<endl;
    re_mantissa[1]->encode(rmMantissa[BITS_HIGH-1], c);
//cout <<5 <<endl;
    re_mantissa[2]->encode(rmMantissa[k], c1); 
  }
  return mantissaReal;
}

int FloatCompressorNew::decompress_mantissa(int exponent, int mantissaPred, RangeDecoder** rd_mantissa, RangeModel** rmMantissaBits)
{
  int k,k1,c,c1;

  if (rmMantissaBits[exponent] == 0)
  {
    // the number k will be between 0 and 22
    rmMantissaBits[exponent] = new RangeModel(23,0,0,512,16);
  }

  // now we decompress (in dependence on the exponent) the number of bits k+1 that the mantissa corrector has

  k = rd_mantissa[0]->decode(rmMantissaBits[exponent]);

  if (k < BITS_HIGH)
  {
    // allocate a new range table if necessary

    if (rmMantissa[k] == 0)
    {
      rmMantissa[k] = new RangeModel((1<<(k+1)),0,0,8192,16);
    }

    // decode a corrector within the interval [ 0 ... (2^(k+1)-1) ] 

    c = rd_mantissa[1]->decode(rmMantissa[k]);

    // translate c back into the interval [ 2^(k)-1 ...  2^(k) ]

    c -= ((1<<k) - 1); 
  }
  else
  {
    // we need to break it up in two bit patterns

    k1 = k-BITS_HIGH+1;

    // allocate a new range table if necessary

    if (rmMantissa[k] == 0)
    {
//      rmMantissa[k] = new RangeModel(1<<BITS_HIGH,0,0,8192,16); 
      if (rmMantissa[BITS_HIGH-1] == 0)
      {
        rmMantissa[BITS_HIGH-1] = new RangeModel(1<<BITS_HIGH,0,0,8192,16);
      }
      rmMantissa[k] = new RangeModel(1<<k1,0,0,2048,16);
    }

    // c1 represents the lowest k-BITS_HIGH+1 bits
    // c represents the highest BITS_HIGH bits (inclusive of the corrector's sign)

    c = rd_mantissa[1]->decode(rmMantissa[BITS_HIGH-1]);
    c1 = rd_mantissa[2]->decode(rmMantissa[k]);

    // we translate c back into the interval [ 2^(BITS_HIGH-1)-1 ...  2^(BITS_HIGH-1) ]

    c -= ((1<<(BITS_HIGH-1)) - 1);

    // add the lower bits stored in c1 back into the number

    if (c < 0)
    {
      c = -( ((-c) << k1) | c1);
    }
    else
    {
      c = (c << k1) | c1;
    }
  }

  int mantissaReal = c + mantissaPred;

  // we fold the mantissa into the interval [ 0  ...  + (2^23) - 1 ]

  if (mantissaReal < 0)
  {
	return mantissaReal + (1<<23);
  }
  else if (mantissaReal >= (1<<23))
  {
	return mantissaReal - (1<<23);
  }
  else
  {
	return mantissaReal;
  }
}

F32 FloatCompressorNew::CompressNone(F32 fReal)
{
  I32 signexponentPred = (((U32&)last_float) & 0xFF800000) >> 23;
  I32 mantissaPred = (((U32&)last_float) & 0x007FFFFF);

  I32 signexponentReal = (((U32&)fReal) & 0xFF800000) >> 23;
  I32 mantissaReal = (((U32&)fReal) & 0x007FFFFF);

  // compress sign and exponent
  compress_signexponent(signexponentPred,signexponentReal,ae_signexponent_none,rmSignExponentNone);

  // compress mantissa
  if (signexponentPred == signexponentReal) // sign and exponent predicted correctly
  {
    mantissaReal = compress_mantissa(signexponentReal&0xFF,mantissaPred,mantissaReal,ae_mantissa_none,rmMantissaBitsNone);
  }
  else 
  {
    mantissaReal = compress_mantissa(signexponentReal&0xFF,0x0,mantissaReal,ae_mantissa_none,rmMantissaBitsNone);
  }

  last_float = fReal;

  return fReal;
}

F32 FloatCompressorNew::DecompressNone()
{
  F32 fReal;

  I32 signexponentReal;
  I32 mantissaReal;

  I32 signexponentPred = (((U32&)last_float) & 0xFF800000) >> 23;
  I32 mantissaPred = (((U32&)last_float) & 0x007FFFFF);

  // decompress sign and exponent
  signexponentReal = decompress_signexponent(signexponentPred,ad_signexponent_none,rmSignExponentNone);

  // decompress mantissa
  if (signexponentPred == signexponentReal) // sign and exponent predicted correctly
  {
    mantissaReal = decompress_mantissa(signexponentReal&0xFF,mantissaPred,ad_mantissa_none,rmMantissaBitsNone);
  }
  else
  {
    mantissaReal = decompress_mantissa(signexponentReal&0xFF,0x0,ad_mantissa_none,rmMantissaBitsNone);
  }

  // put together decompressed float value
  ((U32&)fReal) = (signexponentReal << 23) | mantissaReal;

  last_float = fReal;

  return fReal;
}

F32 FloatCompressorNew::CompressLast(F32 fPred, F32 fReal)
{
  I32 signexponentPred = (((U32&)fPred) & 0xFF800000) >> 23;
  I32 mantissaPred = (((U32&)fPred) & 0x007FFFFF);

  I32 signexponentReal = (((U32&)fReal) & 0xFF800000) >> 23;
  I32 mantissaReal = (((U32&)fReal) & 0x007FFFFF);

  // compress sign and exponent
  compress_signexponent(signexponentPred,signexponentReal,ae_signexponent_last,rmSignExponentLast);

  // compress mantissa
  if (signexponentPred == signexponentReal) // sign and exponent predicted correctly
  {
    mantissaReal = compress_mantissa(signexponentReal&0xFF,mantissaPred,mantissaReal,ae_mantissa_last,rmMantissaBitsLast);
  }
  else 
  {
    mantissaReal = compress_mantissa(signexponentReal&0xFF,0x0,mantissaReal,ae_mantissa_last,rmMantissaBitsLast);
  }

  last_float = fReal;

  return fReal;
}

F32 FloatCompressorNew::DecompressLast(F32 fPred)
{
  F32 fReal;

  I32 signexponentReal;
  I32 mantissaReal;

  I32 signexponentPred = (((U32&)fPred) & 0xFF800000) >> 23;
  I32 mantissaPred = (((U32&)fPred) & 0x007FFFFF);

  // decompress sign and exponent
  signexponentReal = decompress_signexponent(signexponentPred,ad_signexponent_last,rmSignExponentLast);

  // decompress mantissa
  if (signexponentPred == signexponentReal) // sign and exponent predicted correctly
  {
    mantissaReal = decompress_mantissa(signexponentReal&0xFF,mantissaPred,ad_mantissa_last,rmMantissaBitsLast);
  }
  else
  {
    mantissaReal = decompress_mantissa(signexponentReal&0xFF,0x0,ad_mantissa_last,rmMantissaBitsLast);
  }

  // put together decompressed float value
  ((U32&)fReal) = (signexponentReal << 23) | mantissaReal;

  last_float = fReal;

  return fReal;
}

F32 FloatCompressorNew::CompressAcross(F32 fPred, F32 fReal)
{
	//提取预测值和真实值的signexponet及mantissa
  I32 signexponentPred = (((U32&)fPred) & 0xFF800000) >> 23;
  I32 mantissaPred = (((U32&)fPred) & 0x007FFFFF);

  I32 signexponentReal = (((U32&)fReal) & 0xFF800000) >> 23;
  I32 mantissaReal = (((U32&)fReal) & 0x007FFFFF);

  // compress sign and exponent
  compress_signexponent(signexponentPred,signexponentReal,ae_signexponent_across,rmSignExponentAcross);
//cout <<1 <<endl;
  // compress mantissa
  if (signexponentPred == signexponentReal) // sign and exponent predicted correctly
  {
//cout <<2 <<endl;
    mantissaReal = compress_mantissa(signexponentReal&0xFF,mantissaPred,mantissaReal,ae_mantissa_across,rmMantissaBitsAcross);

  }
  else
  {
//cout <<3 <<endl;
    mantissaReal = compress_mantissa(signexponentReal&0xFF,0x0,mantissaReal,ae_mantissa_across,rmMantissaBitsAcross);

  }
  last_float = fReal;

  return fReal;
}

F32 FloatCompressorNew::DecompressAcross(F32 fPred)
{
  F32 fReal;

  I32 signexponentReal;
  I32 mantissaReal;

  I32 signexponentPred = (((U32&)fPred) & 0xFF800000) >> 23;
  I32 mantissaPred = (((U32&)fPred) & 0x007FFFFF);

  // decompress sign and exponent
  signexponentReal = decompress_signexponent(signexponentPred,ad_signexponent_across,rmSignExponentAcross);

  // decompress mantissa
  if (signexponentPred == signexponentReal) // sign and exponent predicted correctly
  {
    mantissaReal = decompress_mantissa(signexponentReal&0xFF,mantissaPred,ad_mantissa_across,rmMantissaBitsAcross);
  }
  else
  {
    mantissaReal = decompress_mantissa(signexponentReal&0xFF,0x0,ad_mantissa_across,rmMantissaBitsAcross);
  }

  // put together decompressed float value
  ((U32&)fReal) = (signexponentReal << 23) | mantissaReal;

  last_float = fReal;

  return fReal;
}