#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>

#include "utilities.h"


BigInt* Init();
void DeInit(BigInt* big_int);
int InputBigInt(BigInt* big_int);
void PrintBigInt(BigInt* big_int);
BigInt* SumTwo(BigInt* bigint1, BigInt* bigint2);
BigInt* DiffTwo(BigInt* bigint1, BigInt* bigint2);
BigInt* MultTwo(BigInt* bigint1, BigInt* bigint2);
int SumTwoWithoutNew(BigInt* bigint1, BigInt* bigint2);
int DiffTwoWithoutNew(BigInt* bigint1, BigInt* bigint2);
int MultTwoWithoutNew(BigInt* bigint1, BigInt* bigint2);


BigInt* Init() {
    /*
    Инициализация длинного целого числа (с присвоением 0)
    Вход: ничего
    Возврат: новое длинное целое число, равное 0
    */

    BigInt* new_big_int = (BigInt*)malloc(sizeof(BigInt));
    if (new_big_int == NULL) {
        return NULL;
    }

    new_big_int->high_digit = 0;
    new_big_int->koefs = NULL;

    return new_big_int;
}


void DeInit(BigInt* big_int) {
    /*
    Деинициализация длинного целого числа
    Вход: длинное целое число
    Возврат: ничего
    */

    if (big_int == NULL) {
        return;
    }
    if (big_int->koefs != NULL) {
        free(big_int->koefs);
    }
    free(big_int);
}


int InputBigInt(BigInt* big_int) {
    /*
    Ввод длинного целого числа через консоль
    Вход: длинное целое число
    Возврат: код работы: 0 - успех, 1 - ошибка
    */

    if (big_int == NULL) {
        return 1;
    }

    unsigned int size;
    printf("Input number of digits: ");
    scanf("%u", &size);

    if (size == 0) {
        return 1;
    }

    if (size == 1) {
        printf("Input the digit WITHOUT sign:\n");
        scanf("%d", &big_int->high_digit);
    }
    else {
        big_int->koefs = (unsigned int*)realloc(big_int->koefs, sizeof(unsigned int) * size);
        if (big_int->koefs == NULL) {
            return 1;
        }
        big_int->koefs[0] = size - 1;
        printf("Input the digits from MOST significant to LEAST significant WITHOUT sign (e.g., for -12345 input: 1 2 3 4 5):\n");
        scanf("%d", &big_int->high_digit);
        for (unsigned int i = size - 1; i > 0; i--) {
            scanf("%u", &big_int->koefs[i]);
        }
    }

    int sign;
    printf("Input \"0\" for \"+\" or something other than \"0\" for \"-\": ");
    scanf("%d", &sign);
    SetSign(big_int, sign);

    return 0;
}


void PrintBigInt(BigInt* big_int) {
    /*
    Вывод длинного целого числа в консоль
    Вход: длинное целое число
    Возврат: ничего
    */

    if (big_int == NULL) {
        return;
    }

    printf("%s", (GetSign(big_int)) ? "-" : "");

    if (big_int->koefs == NULL || ((big_int->high_digit) & (~SIGN_MASK)) != 0) {
        printf("%d ", big_int->high_digit & (~SIGN_MASK));
    }

    if (big_int->koefs != NULL) {
        for (unsigned int i = big_int->koefs[0]; i > 0; i--) {
            printf("%u ", big_int->koefs[i]);
        }
    }

    printf("\n");
}


int SumTwoWithoutNew(BigInt* bigint1, BigInt* bigint2) {
    if (bigint1 == NULL || bigint2 == NULL) {
        return 1;
    }

    int sign1 = GetSign(bigint1),
        sign2 = GetSign(bigint2);

    if (sign1 == 2 || sign2 == 2) {
        return 1;
    }

    //разные знаки - вычитание
    if (sign1 != sign2) {
        if (sign2) {
            SetSign(bigint2, 0);
            if (DiffTwoWithoutNew(bigint1, bigint2)) {
                SetSign(bigint2, 1);
                return 1;
            }
            SetSign(bigint2, 1);
            return 0;
        }
        else {
            BigInt* copy_bigint2 = Init();
            if (copy_bigint2 == NULL) {
                return 1;
            }
            if (CopyBigInt(bigint2, copy_bigint2)) {
                DeInit(copy_bigint2);
                return 1;
            }
            SetSign(bigint1, 0);
            if (DiffTwoWithoutNew(copy_bigint2, bigint1)) {
                DeInit(copy_bigint2);
                SetSign(bigint1, 1);
                return 1;
            }
            SetSign(bigint1, 1);
            if (CopyBigInt(copy_bigint2, bigint1)) {
                DeInit(copy_bigint2);
                return 1;
            }
            return 0;
        }
    }

    unsigned int high1 = (unsigned int)bigint1->high_digit & (~SIGN_MASK),
        high2 = (unsigned int)bigint2->high_digit & (~SIGN_MASK),
        len1 = (bigint1->koefs == NULL) ? 0 : bigint1->koefs[0],
        len2 = (bigint2->koefs == NULL) ? 0 : bigint2->koefs[0];

    //PrintBigInt(bigint1);

    if (bigint1->koefs == NULL && bigint2->koefs == NULL) {                  //2 малых числа
        if (high1 + high2 < BASE >> 1) {
            bigint1->high_digit = high1 + high2;
            SetSign(bigint1, sign1);
            return 0;
        }
        else {
            bigint1->koefs = (unsigned int*)malloc(sizeof(unsigned int) * 2);
            if (bigint1->koefs == NULL) {
                return 1;
            }
            bigint1->koefs[1] = (high1 + high2) & (BASE - 1);
            bigint1->high_digit = ((high1 + high2) >> (sizeof(int) * 8));
            bigint1->koefs[0] = 1;
            SetSign(bigint1, sign1);
            return 0;
        }
    }

    if (bigint1->koefs != NULL && bigint2->koefs == NULL) {                  //большое и малое число

        unsigned int i = 1;
        unsigned int buffer = high2;
        while (buffer != 0 && i <= len1) {
            if (bigint1->koefs[i] + buffer < BASE) {
                bigint1->koefs[i] += buffer;
                break;
            }
            bigint1->koefs[i] = (bigint1->koefs[i] + buffer) & (BASE - 1);
            buffer = (bigint1->koefs[i] + buffer) >> (sizeof(int) * 8);
            i++;
        }

        if (buffer != 0) {
            if (high1 + buffer < (BASE >> 1)) {
                bigint1->high_digit = (int)(high1 + buffer);
            }
            else {
                bigint1->koefs = (unsigned int*)realloc(bigint1->koefs, sizeof(unsigned int) * (len1 + 2));
                if (bigint1->koefs == NULL) {
                    return 1;
                }
                bigint1->koefs[len1 + 1] = (high1 + buffer) & (BASE - 1);
                bigint1->high_digit = (int)((high1 + buffer) >> (sizeof(int) * 8));
                bigint1->koefs[0] = len1 + 1;
            }
            SetSign(bigint1, sign1);
        }

        return 0;
    }

    if (bigint1->koefs == NULL && bigint2->koefs != NULL) {                   //малое и большое число
        bigint1->koefs = (unsigned int*)malloc(sizeof(int) * (len2 + 1));
        if (bigint1->koefs == NULL) {
            return 1;
        }

        unsigned int buffer = high1;
        for (unsigned int i = 1; i <= len2; i++) {
            bigint1->koefs[i] = ((buffer + bigint2->koefs[i]) & (BASE - 1));
            buffer = (buffer + bigint2->koefs[i]) >> (sizeof(int) * 8);
        }

        if (buffer != 0) {
            if (buffer + high2 < (BASE >> 1)) {
                bigint1->high_digit = buffer + high2;
                len1 = len2;
            }
            else {
                bigint1->koefs = (unsigned int*)realloc(bigint1->koefs, sizeof(int) * (len2 + 2));
                if (bigint1->koefs == NULL) {
                    return 1;
                }
                len1 = len2 + 1;
                bigint1->koefs[len2] = ((buffer + high2) & (BASE - 1));
                bigint1->high_digit = ((buffer + high2) >> (sizeof(int) * 8));
            }
        }
        else {
            bigint1->high_digit = high2;
            len1 = len2;
        }

        bigint1->koefs[0] = len1;
        SetSign(bigint1, sign1);
        return 0;
    }

    if (bigint1->koefs != NULL && bigint2->koefs != NULL) {                   //2 больших числа
        //printf("0) ");
        //PrintBigInt(bigint1);
        unsigned long long int buffer = 0;
        for (unsigned int i = 1; i <= ((len1 < len2) ? len1 : len2); i++) {
            buffer = (unsigned long long)(bigint1->koefs[i] + bigint2->koefs[i]) + buffer;
            bigint1->koefs[i] = (unsigned int)(buffer & (BASE - 1));
            buffer = (buffer >> (sizeof(int) * 8));
        }

        if (len1 == len2) {
            if (high1 + high2 + buffer < (BASE >> 1)) {
                bigint1->high_digit = (int)(high1 + high2 + buffer);
                SetSign(bigint1, sign1);
                return 0;
            }
            else {
                bigint1->koefs = (unsigned int*)realloc(bigint1->koefs, sizeof(int) * (len1 + 2));
                if (bigint1->koefs == NULL) {
                    return 1;
                }
                bigint1->koefs[len1 + 1] = ((high1 + high2 + buffer) & (BASE - 1));
                bigint1->high_digit = ((high1 + high2 + buffer) >> (sizeof(int) * 8));
                bigint1->koefs[0] = len1 + 1;
                SetSign(bigint1, sign1);
                return 0;
            }
        }

        else if (len1 > len2) {
            for (unsigned int i = len2 + 1; i <= len1; i++) {
                bigint1->koefs[i] = ((bigint1->koefs[i] + buffer) & (BASE - 1));
                buffer = ((bigint1->koefs[i] + buffer) >> (sizeof(int) * 8));
                bigint1->koefs[i] = ((bigint1->koefs[i] + high2) & (BASE - 1));
                high2 = ((bigint1->koefs[i] + high2) >> (sizeof(int) * 8));
            }
            if (buffer || high2) {
                if ((high1 + high2 + buffer) < (BASE >> 1)) {
                    bigint1->high_digit = (int)(high1 + high2 + buffer);
                    bigint1->koefs[0] = len1;
                    SetSign(bigint1, sign1);
                    return 0;
                }
                else {
                    bigint1->koefs = (unsigned int*)realloc(bigint1->koefs, sizeof(int) * (len1 + 2));
                    if (bigint1->koefs == NULL) {
                        return 1;
                    }
                    bigint1->koefs[len1 + 1] = ((high1 + high2 + buffer) & (BASE - 1));
                    bigint1->high_digit = (int)((high1 + high2 + buffer) >> (sizeof(int) * 8));
                    bigint1->koefs[0] = len1 + 1;
                    SetSign(bigint1, sign1);
                    return 0;
                }
            }
            else {
                bigint1->high_digit = 0;
                SetSign(bigint1, sign1);
                return 0;
            }
        }

        else {
            bigint1->koefs = (unsigned int*)realloc(bigint1->koefs, sizeof(unsigned int) * (len2+1));
            if (bigint1->koefs == NULL) {
                return 1;
            }
            buffer += (unsigned long long int)high1;
            for (unsigned int i = len1 + 1; i <= len2; i++) {
                buffer += (unsigned long long int)bigint2->koefs[i];
                printf("1 BEFORE) %llu %u %u %u %u %u\n", buffer, bigint1->koefs[i], bigint2->koefs[i], len1, len2, i);
                PrintBigInt(bigint1);
                bigint1->koefs[i] = (unsigned int)(buffer & (BASE - 1));
                buffer >>= (sizeof(int) * 8);
                printf("1) %llu %u %u %u %u %u\n", buffer, bigint1->koefs[i], bigint2->koefs[i], len1, len2, i);
                //printf("2) ");
                PrintBigInt(bigint1);
            }
            if (buffer) {
                printf("A\n");
                if ((high2 + buffer) < (BASE >> 1)) {
                    printf("B\n");
                    bigint1->high_digit = (int)(high2 + buffer);
                    bigint1->koefs[0] = len2;
                    SetSign(bigint1, sign1);
                    return 0;
                }
                else {
                    printf("C\n");
                    bigint1->koefs = (unsigned int*)realloc(bigint1->koefs, sizeof(int) * (len2 + 2));
                    if (bigint1->koefs == NULL) {
                        return 1;
                    }
                    bigint1->koefs[len1 + 1] = ((high2 + buffer) & (BASE - 1));
                    bigint1->high_digit = (int)((high2 + buffer) >> (sizeof(int) * 8));
                    bigint1->koefs[0] = len2 + 1;
                    SetSign(bigint1, sign1);
                    return 0;
                }
            }
            else {
                printf("D\n");
                bigint1->high_digit = high2;
                bigint1->koefs[0] = len2;
                SetSign(bigint1, sign1);
                return 0;
            }
        }
    }
}


int DiffTwoWithoutNew(BigInt* bigint1, BigInt* bigint2) {
    return 0;
}


int MultTwoWithoutNew(BigInt* bigint1, BigInt* bigint2) {
    return 0;
}


BigInt* SumTwo(BigInt* bigint1, BigInt* bigint2) {
    if (bigint1 == NULL || bigint2 == NULL) {
        return NULL;
    }

    BigInt* result = Init();
    if (result == NULL) {
        return NULL;
    }

    if (CopyBigInt(bigint1, result)) {
        DeInit(result);
        return NULL;
    }
    if (SumTwoWithoutNew(result, bigint2)) {
        DeInit(result);
        return NULL;
    }
    return result;
}


BigInt* DiffTwo(BigInt* bigint1, BigInt* bigint2) {
    if (bigint1 == NULL || bigint2 == NULL) {
        return NULL;
    }

    BigInt* result = Init();
    if (result == NULL) {
        return NULL;
    }

    if (CopyBigInt(bigint1, result)) {
        DeInit(result);
        return NULL;
    }
    if (DiffTwoWithoutNew(result, bigint2)) {
        DeInit(result);
        return NULL;
    }
    return result;
}


BigInt* MultTwo(BigInt* bigint1, BigInt* bigint2) {
    if (bigint1 == NULL || bigint1->koefs == NULL || bigint2 == NULL || bigint2->koefs == NULL) {
        return NULL;
    }

    BigInt* result = Init();
    if (result == NULL) {
        return NULL;
    }

    if (CopyBigInt(bigint1, result)) {
        DeInit(result);
        return NULL;
    }
    if (MultTwoWithoutNew(result, bigint2)) {
        DeInit(result);
        return NULL;
    }
    return result;
}


int main() {
    BigInt* num1 = Init();
    BigInt* num2 = Init();

    num1->koefs = (unsigned int*)realloc(num1->koefs, 4 * sizeof(unsigned int));
    num1->koefs[0] = 3;
    num1->koefs[1] = 4765;
    num1->koefs[2] = 423567898;
    num1->koefs[3] = 4565644;
    num1->high_digit = 98764456;

    num2->koefs = (unsigned int*)realloc(num2->koefs, 6 * sizeof(unsigned int));
    num2->koefs[0] = 5;
    num2->koefs[1] = 124567;
    num2->koefs[2] = 134326;
    num2->koefs[3] = 876543234;
    num2->koefs[4] = 45765434;
    num2->koefs[5] = 634;
    num2->high_digit = 6;
    // SetSign(num2, 1);

    PrintBigInt(num1);
    PrintBigInt(num2);
    if (SumTwoWithoutNew(num1, num2)) {
        DeInit(num1);
        DeInit(num2);
        return 0;
    };
    
    printf("RESULT: ");
    PrintBigInt(num1);
    PrintBigInt(num2);


    // BigInt* diff1 = DiffTwo(num1, num2);
    // BigInt* mult1 = MultTwo(num1, num2);

    // PrintBigInt(sum1);
    // PrintBigInt(diff1);
    // PrintBigInt(mult1);

    DeInit(num1);
    DeInit(num2);
    // DeInit(sum1);
    // DeInit(diff1);
    // DeInit(mult1);

    return 0;
}