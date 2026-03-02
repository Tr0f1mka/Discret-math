#include <stdio.h>
#include <stdlib.h>

#include "utilities.h"


BigInt* Init();
void DeInit(BigInt* big_int);
BigInt* SumTwo(BigInt* bigint1, BigInt* bigint2);
BigInt* DiffTwo(BigInt* bigint1, BigInt* bigint2);
BigInt* MultTwo(BigInt* bigint1, BigInt* bigint2);
BigInt* Karatsuba(BigInt* bigint1, BigInt* bigint2);
int SumTwoWithoutNew(BigInt* bigint1, BigInt* bigint2);
int DiffTwoWithoutNew(BigInt* bigint1, BigInt* bigint2);
int MultTwoWithoutNew(BigInt* bigint1, BigInt* bigint2);
int KaratsubaWithoutNew(BigInt* bigint1, BigInt* bigint2);
int Af(unsigned int n, int (*f)(BigInt* bigint1, BigInt* bigint2), BigInt* result);
int F(unsigned int n, int (*f)(BigInt* bigint1, BigInt* bigint2), BigInt* result);


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
    big_int = NULL;
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

    if (bigint2->high_digit == 0 && bigint2->koefs == NULL) {
        return 0;
    }

    if (bigint1->high_digit == 0 && bigint1->koefs == NULL) {
        return CopyBigInt(bigint2, bigint1);
    }

    unsigned int high1 = (unsigned int)bigint1->high_digit & (~SIGN_MASK),
        high2 = (unsigned int)bigint2->high_digit & (~SIGN_MASK),
        len1 = (bigint1->koefs == NULL) ? 1 : (bigint1->koefs[0] + (high1 != 0)),
        len2 = (bigint2->koefs == NULL) ? 1 : (bigint2->koefs[0] + (high2 != 0)),
        res_len = MaxUnInt(len1, len2) + 1;

    unsigned int* t = (unsigned int*)realloc(bigint1->koefs, sizeof(int) * (res_len + 1));     //сумма всегда помещается максимум из длин слагаемых + 1
    if (t == NULL) {
        return 1;
    }
    bigint1->koefs = t;
    bigint1->koefs[0] = res_len;
    bigint1->high_digit = 0;
    bigint1->koefs[res_len] = 0;

    //printf("LENS  %u %u %u\n", res_len, len1, len2);

    unsigned int buffer = 0, s1, s2;
    for (unsigned int i = 1; i <= res_len; i++) {
        //Первое слагаемое
        if (i > len1) {
            s1 = 0;
        }
        else if (i == len1) {
            if (high1 != 0) {
                s1 = high1;
            }
            else {
                s1 = bigint1->koefs[i];
            }
        }
        else {
            s1 = bigint1->koefs[i];
        }

        //Второе слагаемое
        if (i > len2) {
            s2 = 0;
        }
        else if (i == len2) {
            if (high2 != 0) {
                s2 = high2;
            }
            else {
                s2 = bigint2->koefs[i];
            }
        }
        else {
            s2 = bigint2->koefs[i];
        }

        //printf("%u) %u %u\n", i, s1, s2);

        unsigned int ls1 = loword(s1),
            hs1 = hiword(s1),
            ls2 = loword(s2),
            hs2 = hiword(s2);

        s1 = ls1 + ls2 + buffer;
        s2 = hs1 + hs2;

        s2 += hiword(s1);
        buffer = hiword(s2);

        s1 = loword(s1);
        s2 = loword(s2);

        bigint1->koefs[i] = (s2 << (sizeof(int) << 2)) + s1;
    }

    SetSign(bigint1, sign1);
    return Normolize(bigint1);
}


int DiffTwoWithoutNew(BigInt* bigint1, BigInt* bigint2) {
    if (bigint1 == NULL || bigint2 == NULL) {
        return 1;
    }

    int sign1 = GetSign(bigint1),
        sign2 = GetSign(bigint2);

    if (sign1 == 2 || sign2 == 2) {
        return 1;
    }

    //разные знаки - сложение
    if (sign1 != sign2) {
        SetSign(bigint2, sign1);
        if (SumTwoWithoutNew(bigint1, bigint2)) {
            SetSign(bigint2, sign2);
            return 1;
        }
        SetSign(bigint2, sign2);
        return 0;
    }

    int compare_result = AbsCompare(bigint1, bigint2);
    if (compare_result == 2) {
        return 1;
    }

    if (compare_result == 0) {
        free(bigint1->koefs);
        bigint1->koefs = NULL;
        bigint1->high_digit = 0;
        return 0;
    }

    BigInt* big, * small;
    int res_sign;

    if (compare_result == 1) {
        big = bigint1;
        small = bigint2;
        res_sign = sign1;
    }
    else {
        big = Init();
        if (big == NULL) {
            return 1;
        }
        if (CopyBigInt(bigint2, big)) {
            DeInit(big);
            return 1;
        }
        small = bigint1;
        res_sign = !sign1;
    }

    unsigned int big_high = (unsigned int)big->high_digit & (~SIGN_MASK),
        small_high = (unsigned int)small->high_digit & (~SIGN_MASK),
        big_len = (big->koefs == NULL) ? 0 : big->koefs[0],
        small_len = (small->koefs == NULL) ? 0 : small->koefs[0];

    if (big->koefs == NULL) {               //small->koefs равен только NULL (по AbsCompare)
        big->high_digit = (int)(big_high - small_high);
    }
    else {
        if (small->koefs == NULL) {
            unsigned int buffer = small_high;
            for (unsigned int i = 1; i <= big_len; i++) {
                if (big->koefs[i] < buffer) {
                    big->koefs[i] -= buffer;
                    buffer = 1;
                }
                else {
                    big->koefs[i] -= buffer;
                    buffer = 0;
                    break;
                }
            }
            big->high_digit = (int)(big_high - buffer);
        }
        else {
            unsigned long long int buffer = 0;
            for (unsigned int i = 1; i <= small_len; i++) {
                buffer += (unsigned long long int)small->koefs[i];
                if (big->koefs[i] < buffer) {
                    big->koefs[i] -= buffer;
                    buffer = 1;
                }
                else {
                    big->koefs[i] -= buffer;
                    buffer = 0;
                }
            }
            buffer += (unsigned long long int)small_high;
            for (unsigned int i = small_len + 1; i <= big_len; i++) {
                if (big->koefs[i] < buffer) {
                    big->koefs[i] -= buffer;
                    buffer = 1;
                }
                else {
                    big->koefs[i] -= buffer;
                    buffer = 0;
                }
            }
            if (buffer) {
                big->high_digit = (int)(big_high - (unsigned int)buffer);
            }
        }
    }

    if (Normolize(big)) {
        if (compare_result == -1) {
            DeInit(big);
        }
        return 1;
    }

    SetSign(big, res_sign);

    if (compare_result == -1) {
        free(bigint1->koefs);
        bigint1->high_digit = big->high_digit;
        bigint1->koefs = big->koefs;
    }
    return 0;
}


int MultTwoWithoutNew(BigInt* bigint1, BigInt* bigint2) {
    if (bigint1 == NULL || bigint2 == NULL) {
        return 1;
    }

    int sign1 = GetSign(bigint1),
        sign2 = GetSign(bigint2);

    if (sign1 == 2 || sign2 == 2) {
        return 1;
    }

    unsigned int high1 = (unsigned int)bigint1->high_digit & (~SIGN_MASK),
        high2 = (unsigned int)bigint2->high_digit & (~SIGN_MASK),
        len1 = ((bigint1->koefs == NULL) ? 1 : bigint1->koefs[0] + (high1 != 0)),
        len2 = ((bigint2->koefs == NULL) ? 1 : bigint2->koefs[0] + (high2 != 0));

    if (high1 == 0 && bigint1->koefs == NULL) {        //0 в первом множителе
        return 0;
    }
    if (high2 == 0 && bigint2->koefs == NULL) {        //0 во втором множителе
        bigint1->high_digit = 0;
        free(bigint1->koefs);
        bigint1->koefs = NULL;
        return 0;
    }

    if (high2 == 1 && bigint2->koefs == NULL) {        //1 во втором множителе
        SetSign(bigint1, sign1 != sign2);
        return 0;
    }

    if (high1 == 1 && bigint1->koefs == NULL) {        //1 в первом множителе
        if (CopyBigInt(bigint2, bigint1)) {
            return 1;
        }
        SetSign(bigint1, sign1 != sign2);
        return 0;
    }

    unsigned int res_len = len1 + len2;
    unsigned int* result = (unsigned int*)calloc(res_len + 1, sizeof(int) * (res_len + 1));
    if (result == NULL) {
        return 1;
    }

    unsigned long long int buffer;
    unsigned int mul1, mul2;

    //printf("res_len: %u len1: %u len2: %u\n", res_len, len1, len2);
    for (unsigned int i = 1; i <= res_len; i++) {
        for (unsigned int j = 1; j <= len1; j++) {
            if ((i + 1 - j <= len2) && (i + 1 - j > 0)) {
                //printf("%u %u\n", i, j);
                if (j == len1 && high1 != 0) {
                    mul1 = high1;
                }
                else {
                    mul1 = bigint1->koefs[j];
                }
                if ((i + 1 - j) == len2 && high2 != 0) {
                    mul2 = high2;
                }
                else {
                    mul2 = bigint2->koefs[i + 1 - j];
                }

                //mul1 = ((j == len1) ? high1 : bigint1->koefs[j]);
                //mul2 = (((i + 1 - j) == len2) ? high2 : bigint2->koefs[i + 1 - j]);
                unsigned int m1 = uloword(mul1) * uloword(mul2),
                    m2 = uloword(mul1) * uhiword(mul2),
                    m3 = uhiword(mul1) * uloword(mul2),
                    m4 = uhiword(mul1) * uhiword(mul2);
                buffer = 0;
                buffer += (unsigned long long int)m4;
                buffer <<= (sizeof(int) << 2);
                buffer += (unsigned long long int)m3;
                buffer += (unsigned long long int)m2;
                buffer <<= (sizeof(int) << 2);
                buffer += (unsigned long long int)m1;
                //printf("%u * %u = %llu\n", mul1, mul2, buffer);
                //printf("%u BEFORE) %u %u %llu %u %u %u %u %u %u %u\n", i, mul1, mul2, buffer, j, i + 1 - j, result[i], m1, m2, m3, m4);
                //printf("RESULT: ");
                //for (unsigned int o = 1; o <= res_len; o++) {
                    //printf("%u ", result[o]);
                //}
                //printf("\n");
                unsigned int k = i;
                while (buffer > 0 && k <= res_len) {
                    //printf("WHILE BUFFER == %llu %u\n", buffer, result[k]);
                    buffer = buffer + ((unsigned long long int)result[k]);
                    result[k] = (unsigned int)(buffer & BASE);
                    buffer >>= (sizeof(int) << 3);
                    //printf("WHILE AFTER == %llu %u\n", buffer, result[k]);
                    k++;
                }
                /*printf("RESULT: ");
                for (unsigned int o = 1; o <= res_len; o++) {
                    printf("%u ", result[o]);
                }
                printf("\n");*/
                //printf("%u AFTER) %u %u %llu %u %u %u %u %u %u %u\n", i, mul1, mul2, buffer, j, i + 1 - j, result[i], m1, m2, m3, m4);
            }
        }
    }

    mul1 = res_len;
    while (result[mul1] == 0 && mul1 > 0) {
        mul1--;
    }

    free(bigint1->koefs);
    bigint1->koefs = NULL;

    if (mul1 != res_len) {
        if (mul1 == 1) {
            if (result[1] <= (BASE >> 1)) {
                bigint1->high_digit = (int)result[1];
                free(result);
                bigint1->koefs = NULL;
            }
            else {
                result = (unsigned int*)realloc(result, sizeof(int) << 1);
                if (result == NULL) {
                    return 1;
                }
                bigint1->high_digit = 0;
                bigint1->koefs = result;
                bigint1->koefs[0] = 1;
            }
        }
        else {
            if (result[mul1] <= (BASE >> 1)) {
                bigint1->high_digit = (int)result[mul1];
                result = (unsigned int*)realloc(result, sizeof(int) * (mul1));
                if (result == NULL) {
                    return 1;
                }
                bigint1->koefs = result;
                bigint1->koefs[0] = mul1 - 1;
            }
            else {
                result = (unsigned int*)realloc(result, sizeof(int) * (mul1 + 1));
                if (result == NULL) {
                    return 1;
                }
                bigint1->high_digit = 0;
                bigint1->koefs = result;
                bigint1->koefs[0] = mul1;
            }
        }
    }
    else {
        if (result[mul1] <= (BASE >> 1)) {
            bigint1->high_digit = (int)result[mul1];
            result = (unsigned int*)realloc(result, sizeof(int) * mul1);
            if (result == NULL) {
                return 1;
            }
            bigint1->koefs = result;
            bigint1->koefs[0] = mul1 - 1;
        }
        else {
            bigint1->high_digit = 0;
            bigint1->koefs = result;
            bigint1->koefs[0] = mul1;
        }
    }

    SetSign(bigint1, sign1 != sign2);
    return 0;
}


int KaratsubaWithoutNew(BigInt* bigint1, BigInt* bigint2) {
    /*
    Умножение двух длинных целых чисел алгоритмом Карацубы (результат в первом множителе)
    Вход: 2 длинных целых числа
    Возврат: 0 - успех, 1 - ошибка
    */

    if (bigint1 == NULL || bigint2 == NULL) {
        return 1;
    }

    unsigned int high1 = bigint1->high_digit & (BASE >> 1),
        high2 = bigint2->high_digit & (BASE >> 1),
        len1 = ((bigint1->koefs == NULL) ? 1 : (bigint1->koefs[0] + (high1 != 0))),
        len2 = ((bigint2->koefs == NULL) ? 1 : (bigint2->koefs[0] + (high2 != 0))),
        sign1 = GetSign(bigint1),
        sign2 = GetSign(bigint2),
        res_split = MaxUnInt(len1, len2) >> 1;

    SetSign(bigint1, 0);
    SetSign(bigint2, 0);

    if (high1 == 0 && bigint1->koefs == NULL) {
        return 0;
    }

    if (high2 == 0 && bigint2->koefs == NULL) {
        bigint1->high_digit = 0;
        free(bigint1->koefs);
        bigint1->koefs = NULL;
        return 0;
    }

    if ((len1 == 1) && (len2 == 1)) {
        if (MultTwoWithoutNew(bigint1, bigint2)) {
            SetSign(bigint2, sign2);
            return 1;
        }
        SetSign(bigint1, sign1 != sign2);
        SetSign(bigint2, sign2);
        return 0;
    }

    BigInt* low_part1 = Init(),
        * high_part1 = Init(),
        * low_part2 = Init(),
        * high_part2 = Init();

    //пилим числа
    if (SplitBigint(bigint1, low_part1, high_part1, MinUnInt(res_split, len1))
        || SplitBigint(bigint2, low_part2, high_part2, MinUnInt(res_split, len2))) {

        DeInit(low_part1);
        DeInit(high_part1);
        DeInit(low_part2);
        DeInit(high_part2);
        SetSign(bigint2, sign2);
        return 1;
    }

    /*printf("%u\n", MinUnInt(res_split, len1));
    PrintBigInt(low_part1);
    PrintBigInt(high_part1);*/

    //нормализуем части
    if (Normolize(low_part1)
        || Normolize(high_part1)
        || Normolize(low_part2)
        || Normolize(high_part2)) {

        DeInit(low_part1);
        DeInit(high_part1);
        DeInit(low_part2);
        DeInit(high_part2);
        SetSign(bigint2, sign2);
        return 1;
    }

    BigInt* res_high = Init(),
        * res_low = Init();

    //копируем старшую и младшую половины в другие переменные
    if (CopyBigInt(high_part1, res_high)
        || CopyBigInt(low_part1, res_low)) {

        DeInit(low_part1);
        DeInit(high_part1);
        DeInit(low_part2);
        DeInit(high_part2);
        DeInit(res_high);
        DeInit(res_low);
        SetSign(bigint2, sign2);
        return 1;
    }

    //перемножаем старшие и младшие части
    if (KaratsubaWithoutNew(res_high, high_part2)
        || KaratsubaWithoutNew(res_low, low_part2)) {

        DeInit(low_part1);
        DeInit(high_part1);
        DeInit(low_part2);
        DeInit(high_part2);
        DeInit(res_high);
        DeInit(res_low);
        SetSign(bigint2, sign2);
        return 1;
    }

    /*printf("CUR:\n");
    PrintBigInt(bigint1);
    PrintBigInt(bigint2);
    PrintBigInt(res_high);
    PrintBigInt(res_low);
    printf("END CUR\n");*/

    //складываем части одного числа
    BigInt* medium = SumTwo(low_part1, high_part1);

    if (medium == NULL
        || SumTwoWithoutNew(low_part2, high_part2)) {

        DeInit(low_part1);
        DeInit(high_part1);
        DeInit(low_part2);
        DeInit(high_part2);
        DeInit(res_high);
        DeInit(res_low);
        DeInit(medium);
        SetSign(bigint2, sign2);
        return 1;
    }

    //перемножаем суммы
    if (KaratsubaWithoutNew(medium, low_part2)) {

        DeInit(low_part1);
        DeInit(high_part1);
        DeInit(low_part2);
        DeInit(high_part2);
        DeInit(res_high);
        DeInit(res_low);
        DeInit(medium);
        SetSign(bigint2, sign2);
        return 1;
    }

    /*printf("\nCUR:\n");
    PrintBigInt(bigint1);
    PrintBigInt(bigint2);
    PrintBigInt(res_high);
    PrintBigInt(res_low);
    PrintBigInt(low_part1);
    PrintBigInt(high_part1);
    PrintBigInt(low_part2);
    PrintBigInt(high_part2);
    PrintBigInt(medium);
    printf("END CUR\n");*/

    //вычисление среднего члена
    if (DiffTwoWithoutNew(medium, res_high)
        || DiffTwoWithoutNew(medium, res_low)) {

        DeInit(low_part1);
        DeInit(high_part1);
        DeInit(low_part2);
        DeInit(high_part2);
        DeInit(res_high);
        DeInit(res_low);
        DeInit(medium);
        SetSign(bigint2, sign2);
        return 1;
    }

    //вычисление результата
    if (LShiftBigInt(res_high, res_split)
        || SumTwoWithoutNew(res_high, medium)
        || LShiftBigInt(res_high, res_split)
        || SumTwoWithoutNew(res_high, res_low)) {

        DeInit(low_part1);
        DeInit(high_part1);
        DeInit(low_part2);
        DeInit(high_part2);
        DeInit(res_high);
        DeInit(res_low);
        DeInit(medium);
        SetSign(bigint2, sign2);
        return 1;
    }

    //копирование результата в первый аргумент
    if (CopyBigInt(res_high, bigint1)) {
        printf("AZAZA\n");
        DeInit(low_part1);
        DeInit(high_part1);
        DeInit(low_part2);
        DeInit(high_part2);
        DeInit(res_high);
        DeInit(res_low);
        DeInit(medium);
        SetSign(bigint2, sign2);
        return 1;
    }

    DeInit(low_part1);
    DeInit(high_part1);
    DeInit(low_part2);
    DeInit(high_part2);
    DeInit(res_high);
    DeInit(res_low);
    DeInit(medium);
    SetSign(bigint1, sign1 != sign2);
    SetSign(bigint2, sign2);
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
    //printf("COPY1: ");/*
    //PrintBigInt(result);*/
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
    if (MultTwoWithoutNew(result, bigint2)) {
        DeInit(result);
        return NULL;
    }
    return result;
}


BigInt* Karatsuba(BigInt* bigint1, BigInt* bigint2) {
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
    //printf("COPY1: ");/*
    //PrintBigInt(result);*/
    if (KaratsubaWithoutNew(result, bigint2)) {
        DeInit(result);
        return NULL;
    }
    return result;
}


int Af(unsigned int n, int (*mult)(BigInt* bigint1, BigInt* bigint2), BigInt* result) {
    /*
    Вычисляет функцию суммы по i от 1 до n ((-1)^(n-i)*(n!))
    Вход: натуральное число n, указатель на функцию умножения, поле для записи результата
    Возврат: 0 - успех, 1 - ошибка
    */

    if (result->koefs != NULL) {
        free(result->koefs);
        result->koefs = NULL;
    }
    result->high_digit = 0;

    if (n & 1 == 0) {     //по чётным n функция равна нулю
        return 0;
    }

    BigInt* one = Init();
    BigInt* cur_mult = Init();
    one->high_digit = 1;
    cur_mult->high_digit = 1;
    //по нечётным - n!
    result->high_digit = 1;
    for (unsigned int i = 2; i <= n; i++) {
        if (SumTwoWithoutNew(cur_mult, one)) {
            DeInit(one);
            DeInit(cur_mult);
            return 1;
        }
        if (mult(result, cur_mult)) {
            DeInit(one);
            DeInit(cur_mult);
            return 1;
        }
    }
    DeInit(one);
    DeInit(cur_mult);
    return 0;
}


int F(unsigned int n, int (*mult)(BigInt* bigint1, BigInt* bigint2), BigInt* result) {
    /*
    Функция поиска значение выражения 115249^4183 mod(2^n)
    Вход: натуральное n, указатель на функцию умножения, поле для записи результата
    Возврат: 0 - успех, 1 - ошибка
    */

    if (result == NULL) {
        return 1;
    }

    if (result->koefs != NULL) {
        free(result->koefs);
        result->koefs = NULL;
    }
    result->high_digit = 1;

    BigInt* base = Init();
    base->high_digit = 115249;

    unsigned int exp = 4183;

    while (exp != 0) {
        if (exp & 1) {
            if (mult(result, base)) {
                DeInit(base);
                return 1;
            }
            if (MaskBigInt(result, n)) {
                DeInit(base);
                return 1;
            }
        }
        if (mult(base, base)) {
            DeInit(base);
            return 1;
        }
        if (MaskBigInt(base, n)) {
            DeInit(base);
            return 1;
        }
        exp >>= 1;
    }

    return 0;
}


int main() {

    unsigned int n = 101;

    BigInt* num1 = Init();
    BigInt* num2 = Init();
    BigInt* num3 = Init();
    BigInt* num4 = Init();
    BigInt* num5 = Init();
    BigInt* num6 = Init();

    num1->koefs = (unsigned int*)realloc(num1->koefs, 4 * sizeof(unsigned int));
    num1->koefs[0] = 3;
    num1->koefs[1] = 4765;
    num1->koefs[2] = 423567898;
    num1->koefs[3] = 4565644;
    num1->high_digit = 98764456;
    //SetSign(num1, 1);

    num2->koefs = (unsigned int*)realloc(num2->koefs, 6 * sizeof(unsigned int));
    num2->koefs[0] = 5;
    num2->koefs[1] = 124567;
    num2->koefs[2] = 134326;
    num2->koefs[3] = 876543234;
    num2->koefs[4] = 45765434;
    num2->koefs[5] = 634;
    num2->high_digit = 6;
    //SetSign(num2, 1);

    BigInt* sum = SumTwo(num1, num2);
    BigInt* diff = DiffTwo(num1, num2);
    BigInt* mult = MultTwo(num1, num2);
    BigInt* karat = Karatsuba(num1, num2);


    Af(n, MultTwoWithoutNew, num3);
    Af(n, MultTwoWithoutNew, num4);
    F(n, KaratsubaWithoutNew, num5);
    F(n, MultTwoWithoutNew, num6);
    

    PrintBigInt(sum);      //6 634 144529890 881108878 423702224 129332
    PrintBigInt(diff);     //-6 633 4241968274 871977589 3871533724 119802
    PrintBigInt(mult);     //592586750 2515569219 1541370835 1417945931 2845549147 700905846 267932998 3844149492 593561755
    PrintBigInt(karat);    //592586750 2515569219 1541370835 1417945931 2845549147 700905846 267932998 3844149492 593561755
    PrintBigInt(num3);     //703019 1293101599 3661011161 472310225 3106540053 2626154124 3293359988 2062927204 23496238 1532496876 1894677346 3880089912 483927649 2016880706 0 0 0
    PrintBigInt(num4);     //703019 1293101599 3661011161 472310225 3106540053 2626154124 3293359988 2062927204 23496238 1532496876 1894677346 3880089912 483927649 2016880706 0 0 0
    PrintBigInt(num5);     //6 4291374985 388989611 2921648977
    PrintBigInt(num6);     //6 4291374985 388989611 2921648977


    DeInit(num1);
    DeInit(num2);
    DeInit(num3);
    DeInit(num4);
    DeInit(num5);
    DeInit(num6);
    DeInit(sum);
    DeInit(diff);
    DeInit(mult);
    DeInit(karat);   

    return 0;
}