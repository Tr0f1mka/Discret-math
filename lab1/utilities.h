#ifndef UTILITIES_H
#define UTILITIES_H


#include <stdio.h>
#include <stdlib.h>


#define SIGN_MASK (1U<<31)
#define BASE (256U*256U*256U*256U)


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


#endif