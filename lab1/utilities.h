#ifndef UTILITIES_H
#define UTILITIES_H


#include <stdio.h>
#include <stdlib.h>


#define SIGN_MASK (1U<<((sizeof(int)<<3)-1))
#define BASE (0U-1)


typedef struct BigInt {
	int high_digit;
	unsigned int* koefs;
} BigInt;                      //он же - длинное целое число


int MaxInt(int num1, int num2);
unsigned int MaxUnInt(unsigned int num1, unsigned int num2);
int MinInt(int num1, int num2);
unsigned int MinUnInt(unsigned int num1, unsigned int num2);
void SetSign(BigInt* big_int, int sign);
int GetSign(BigInt* big_int);
int AbsCompare(BigInt* num1, BigInt* num2);
int hiword(int value);
int loword(int value);
unsigned int uhiword(unsigned int value);
unsigned int uloword(unsigned int value);
int CopyBigInt(BigInt* source, BigInt* target);
int Normolize(BigInt* bigint);
int LShiftBigInt(BigInt* bigint, unsigned int shift);
int SplitBigint(BigInt* bigint, BigInt* low, BigInt* high, unsigned int shift);
void PrintBigInt(BigInt* big_int);


#endif