#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utilities.h"


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
    new_big_int->koefs = (unsigned int*)malloc(sizeof(unsigned int)*2);
    if (new_big_int->koefs == NULL) {
        free(new_big_int);
        return NULL;
    }

    new_big_int->koefs[0] = 1;
    new_big_int->koefs[1] = 0;
    new_big_int->high_digit = 0;
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
    if (big_int->koefs == NULL) {
        return 1;
    }

    printf("Input number of digits: ");
    scanf("%u", &big_int->koefs[0]);
    
    if (big_int->koefs[0] == 0) {
        return 1;
    }

    big_int->koefs = (unsigned int*)realloc(big_int->koefs, sizeof(unsigned int)*(1+big_int->koefs[0]));
    if (big_int->koefs == NULL) {
        return 1;
    }
    
    printf("Input the digits from MOST significant to LEAST significant (e.g., for 12345 input: 1 2 3 4 5):\n");
    for (unsigned int i =  big_int->koefs[0]; i > 0; i--) {
        scanf("%u", &big_int->koefs[i]);
    }
    
    int sign;
    big_int->high_digit = (int)big_int->koefs[big_int->koefs[0]];
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
    
    if (big_int == NULL || big_int->koefs == NULL) {
        return;
    }
    printf("%s", (GetSign(big_int)) ? "-" : "");
    for (unsigned int i = big_int->koefs[0]; i > 0; i--) {
        printf("%u ", big_int->koefs[i]);
    }
    printf("\n");
}


BigInt* SumTwo(BigInt* bigint1, BigInt* bigint2) {
    /*
    Сумма двух длинных чисел в новое длинное целое число
    Вход: 2 длинных целых числа
    Возврат: длинное целое число - сумма
    */

    if (bigint1 == NULL || bigint1->koefs == NULL || bigint2 == NULL || bigint2->koefs == NULL) {
        return NULL;
    }

    int sign1 = GetSign(bigint1);
    int sign2 = GetSign(bigint2);

    if (sign1 == sign2) {              //одинаковые знаки - выполняем сложение

        BigInt* result = Init();
        if (result == NULL) {
            return NULL;
        }

        unsigned long long buffer = 0;
        unsigned int length = MaxUnInt(bigint1->koefs[0], bigint2->koefs[0]);
        unsigned int min_length = MinUnInt(bigint1->koefs[0], bigint2->koefs[0]);

        result->koefs = (unsigned int*)realloc(result->koefs, sizeof(unsigned int)*(2+length));
        if (result->koefs == NULL) {
            DeInit(result);
            return NULL;
        }

        for (unsigned int i = 1; i <= min_length; i++) {
            buffer += ((unsigned long long)bigint1->koefs[i] + (unsigned long long)bigint2->koefs[i]);
            result->koefs[i] = buffer & (BASE-1);
            buffer >>= 32;
        }

        if (min_length != length) {

            if (min_length == bigint1->koefs[0]) {

                for (unsigned int i = min_length+1; i <= length; i++) {
                    buffer += (unsigned long long)bigint2->koefs[i];
                    result->koefs[i] = buffer & (BASE-1);
                    buffer >>= 32;
                }

            }
            else {

                for (unsigned int i = min_length+1; i <= length; i++) {
                    buffer += (unsigned long long)bigint1->koefs[i];
                    result->koefs[i] = buffer & (BASE-1);
                    buffer >>= 32;
                }

            }

        }

        if (buffer) {
            length++;
            result->koefs[length] = buffer;
        }

        result->koefs[0] = length;
        result->high_digit = (int)result->koefs[length];
        SetSign(result, sign1);
        return result;
    }

    //разные знаки - выполняем вычитание
    if (sign1) {
        SetSign(bigint1, 0);
        BigInt* result = DiffTwo(bigint2, bigint1);
        SetSign(bigint1, 1);
        if (result == NULL) {
            return NULL;
        }
        return result;
    }
    
    SetSign(bigint2, 0);
    BigInt* result = DiffTwo(bigint1, bigint2);
    SetSign(bigint2, 1);
    if (result == NULL) {
        return NULL;
    }
    return result;
}


BigInt* DiffTwo(BigInt* bigint1, BigInt* bigint2) {
    /*
    Разность двух длинных целых чисел в новое длинное целое число
    Вход: 2 длинных целых числа
    Возврат: длинное целое число - разность
    */
    
    if (bigint1 == NULL || bigint1->koefs == NULL || bigint2 == NULL || bigint2->koefs == NULL) {
        return NULL;
    }

    int sign1 = GetSign(bigint1);
    int sign2 = GetSign(bigint2);

    //разные знаки - выполняем сложение
    if (sign1 != sign2) {
        
        SetSign(bigint2, sign1);
        BigInt* result = SumTwo(bigint1, bigint2);
        SetSign(bigint2, !sign1);
        if (result == NULL) {
            return NULL;
        }
        return result;
    }

    //одинаковые знаки - выполняем вычитание
    BigInt* result = Init();
    if (result == NULL) {
        return NULL;
    }

    int compare_result = AbsCompare(bigint1, bigint2);
    if (compare_result == 0) {       //числа равны - возврат нуля
        return result;
    }

    BigInt *small, *big;

    if (compare_result == 1) {
        big = bigint1;
        small = bigint2;
    }
    else {
        big = bigint2;
        small = bigint1;
        sign1 = !sign1;
    }

    unsigned int overhead = 0;
    unsigned int length = big->koefs[0];
    unsigned int min_length = small->koefs[0];

    result->koefs = (unsigned int*)realloc(result->koefs, sizeof(unsigned int)*(1+length));
    if (result->koefs == NULL) {
        DeInit(result);
        return NULL;
    }

    for (unsigned int i = 1; i <= min_length; i++) {

        result->koefs[i] = big->koefs[i] - overhead - small->koefs[i];
        if (big->koefs[i] < small->koefs[i] + overhead) {
            overhead = 1;
        }
        else {
            overhead = 0;
        }

    }

    for (unsigned int i = min_length+1; i <= length; i++) {

        result->koefs[i] = big->koefs[i] - overhead;
        if (big->koefs[i] < overhead) {
            overhead = 1;
        }
        else {
            overhead = 0;
        }

    }
    if (overhead != 0) {
        DeInit(result);
        return NULL;
    }

    while (length > 1 && result->koefs[length] == 0) {
        length--;
    }
    result->koefs[0] = length;
    result->high_digit = (int)result->koefs[length];
    SetSign(result, sign1);

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

    if (bigint1->high_digit == 0 || bigint2->high_digit == 0) {
        return result;
    }

    unsigned int len1 = bigint1->koefs[0],
                 len2 = bigint2->koefs[0],
                 res_len = len1 + len2;

    result->koefs = (unsigned int*)realloc(result->koefs, sizeof(unsigned int)*(len1 + len2));
    if (result->koefs == NULL) {
        DeInit(result);
        return NULL;
    }
    result->koefs = (unsigned int*)memset(result->koefs, 0, sizeof(unsigned int)*(len1 + len2));

    for (unsigned int i = 1; i <= len1; i++) {
        unsigned long long buffer = 0;
        for  (unsigned int j = 1; j <= len2; j++) {
            buffer = (unsigned long long)bigint1->koefs[i]
                     * (unsigned long long)bigint2->koefs[j]
                     + (unsigned long long)result->koefs[i+j-1]
                     + buffer;
            result->koefs[i+j-1] = (unsigned int)(buffer & (BASE - 1));
            buffer >>= 32;
        }
        if (buffer) {
            unsigned int k = i+len2;
            while (buffer && k <= res_len) {
                buffer = (unsigned long long)result->koefs[k] + buffer;
                result->koefs[k] = (unsigned int)(buffer & (BASE-1));
                buffer >>= 32;
                k++;
            }
        }
    }

    while (res_len > 1 && result->koefs[res_len] == 0) {
        res_len--;
    }
    result->koefs[0] = res_len;

    result->high_digit = result->koefs[res_len];
    int sign1 = GetSign(bigint1), sign2 = GetSign(bigint2);
    SetSign(result, sign1 != sign2);
    return result;
}