#include "utilities.h"


int MaxInt(int num1, int num2) {
    /*
    Максимум двух целых чисел
    Вход: два целых числа
    Возврат: максимум двух целых чисел
    */

    if (num1 > num2) {
        return num1;
    }
    return num2;
}


unsigned int MaxUnInt(unsigned int num1, unsigned int num2) {
    /*
    Максимум двух беззнаковых целых чисел
    Вход: два беззнаковых целых числа
    Возврат: максимум двух беззнаковых целых чисел
    */

    if (num1 > num2) {
        return num1;
    }
    return num2;
}


int MinInt(int num1, int num2) {
    /*
    Минимум двух целых чисел
    Вход: два целых числа
    Возврат: минимум двух целых чисел
    */

    if (num1 < num2) {
        return num1;
    }
    return num2;
}


unsigned int MinUnInt(unsigned int num1, unsigned int num2) {
    /*
    Минимум двух беззнаковых целых чисел
    Вход: два беззнаковых целых числа
    Возврат: минимум двух беззнаковых целых чисел
    */

    if (num1 < num2) {
        return num1;
    }
    return num2;
}


void SetSign(BigInt* big_int, int sign) {
    /*
    Запись знака в длинном целом числе
    Вход: длинное целое число и код нужного знака: 0 - "+", не 0 - "-"
    Возврат: ничего
    */

    if (big_int == NULL) {
        return;
    }

    if (sign) {                             //0 - "+", не 0 - "-"
        big_int->high_digit |= SIGN_MASK;
    }
    else {
        big_int->high_digit &= ~SIGN_MASK;
    }
}


int GetSign(BigInt* big_int) {
    /*
    Получение знака длинного целого числа
    Вход: длинного целого числа
    Возврат: 0 - "+", 1 - "-", 2 - ошибка
    */

    if (big_int == NULL) {
        return 2;
    }

    return (big_int->high_digit & SIGN_MASK) != 0;
}


int AbsCompare(BigInt* num1, BigInt* num2) {
    /*
    Сравнение длинных целых чисел по модулю
    Вход: 2 длинных целых числа
    Возврат: 1 - первое число больше, 0 - числа равны, -1 - второе число больше, 2 - ошибка
    */

    if (num1 == NULL || num2 == NULL) {
        return 2;
    }

    int high1 = num1->high_digit & (~SIGN_MASK),
        high2 = num2->high_digit & (~SIGN_MASK);

    if (num1->koefs == NULL && num2->koefs == NULL) {
        if (high1 > high2) {
            return 1;
        }
        else if (high1 == high2) {
            return 0;
        }
        else {
            return -1;
        }
    }
    else if (num1->koefs != NULL && num2->koefs == NULL) {
        return 1;
    }

    else if (num1->koefs == NULL && num2->koefs != NULL) {
        return -1;
    }

    else {
        if (num1->koefs[0] > num2->koefs[0]) {
            return 1;
        }
        else if (num1->koefs[0] < num2->koefs[0]) {
            return -1;
        }
        else {
            if (high1 > high2) {
                return 1;
            }
            else if (high1 < high2) {
                return -1;
            }
            else {
                for (unsigned int i = num1->koefs[0]; i > 0; i--) {
                    if (num1->koefs[i] > num2->koefs[i]) {
                        return 1;
                    }
                    else if (num1->koefs[i] < num2->koefs[i]) {
                        return -1;
                    }
                }
            }
        }
        return 0;
    }
}


int hiword(int value) {
    return (value >> (sizeof(value) << 2)) & ((1 << (sizeof(value) << 2)) - 1);
}


int loword(int value) {
    return value & ((1 << (sizeof(value) << 2)) - 1);
}

unsigned int uhiword(unsigned int value) {
    return (value >> (sizeof(value) << 2)) & ((1 << (sizeof(value) << 2)) - 1);
}


unsigned int uloword(unsigned int value) {
    return value & ((1 << (sizeof(value) << 2)) - 1);
}


int CopyBigInt(BigInt* source, BigInt* target) {
    /*
    Копирование длинного целого числа в другое длинное целое число
    Вход: 2 длинных целых числа - источник и приёмник
    Возврат: 0 - успех, 1 - ошибка
    */

    if (source == NULL || target == NULL) {
        return 1;
    }

    target->high_digit = source->high_digit;
    if (source->koefs == NULL) {
        free(target->koefs);
        target->koefs = NULL;
        return 0;
    }

    unsigned int* t = (unsigned int*)realloc(target->koefs, sizeof(unsigned int) * (source->koefs[0] + 1));
    if (t == NULL) {
        return 1;
    }
    target->koefs = t;

    for (unsigned int i = 0; i < source->koefs[0] + 1; i++) {
        target->koefs[i] = source->koefs[i];
    }

    return 0;
}


int Normolize(BigInt* bigint) {
    /*
    Убирает ведущие нули
    Вход: длинное целое число
    Возврат: 0 - успех, 1 - ошибка
    */

    //printf("%u)\n", bigint->koefs[0]);

    if (bigint == NULL) {
        return 1;
    }

    if (bigint->koefs == NULL) {
        return 0;
    }

    if (bigint->high_digit == 0 && bigint->koefs[0] == 0) {
        free(bigint->koefs);
        bigint->koefs = NULL;
        return 0;
    }

    unsigned int high = bigint->high_digit & (~SIGN_MASK),
        sign = GetSign(bigint);

    if (high != 0) {
        return 0;
    }

    unsigned i = bigint->koefs[0];
    while (bigint->koefs[i] == 0 && i > 0) {
        i--;
    }
    if (i == 0) {
        bigint->high_digit = 0;
        free(bigint->koefs);
        bigint->koefs = NULL;
        return 0;
    }

    //if (i != bigint->koefs[0]) {
    if (i == 1) {
        if (bigint->koefs[1] <= (BASE >> 1)) {
            bigint->high_digit = (int)bigint->koefs[1];
            free(bigint->koefs);
            bigint->koefs = NULL;
        }
        else {
            bigint->high_digit = 0;
            unsigned int* t = (unsigned int*)realloc(bigint->koefs, sizeof(int) * 2);
            if (t == NULL) {
                return 1;
            }
            bigint->koefs = t;
            bigint->koefs[0] = 1;
        }
    }
    else {
        if (bigint->koefs[i] <= (BASE >> 1)) {
            bigint->high_digit = (int)bigint->koefs[i];
            unsigned int* t = (unsigned int*)realloc(bigint->koefs, sizeof(int) * i);
            if (t == NULL) {
                return 1;
            }
            bigint->koefs = t;
            bigint->koefs[0] = i - 1;
        }
        else {
            bigint->high_digit = 0;
            unsigned int* t = (unsigned int*)realloc(bigint->koefs, sizeof(int) * (i + 1));
            if (t == NULL) {
                return 1;
            }
            bigint->koefs = t;
            bigint->koefs[0] = i;
        }
    }
    SetSign(bigint, sign);

    return 0;
}


int LShiftBigInt(BigInt* bigint, unsigned int shift) {
    /*
    Сдвиг влево длинного целого числа bigint на shift цифр
    Вход: длинное целое число и обычное целое число
    Возврат: 0 - успех, 1 - ошибка
    */

    if (bigint == NULL) {
        return 1;
    }

    if (shift == 0) {
        return 0;
    }

    if ((bigint->high_digit & (~SIGN_MASK)) == 0 && bigint->koefs == NULL) {
        return 0;
    }

    if (bigint->koefs == NULL) {
        if (bigint->high_digit == 0) {
            return 0;
        }
        bigint->koefs = (unsigned int*)calloc(shift + 1, sizeof(int) * (shift + 1));
        if (bigint->koefs == NULL) {
            return 1;
        }
        bigint->koefs[0] = shift;
        return 0;
    }
    else {

        bigint->koefs = (unsigned int*)realloc(bigint->koefs, sizeof(int) * (bigint->koefs[0] + shift + 1));
        if (bigint->koefs == NULL) {
            return 1;
        }
        for (unsigned int i = bigint->koefs[0]; i > 0; i--) {
            bigint->koefs[i + shift] = bigint->koefs[i];
        }
        for (unsigned int i = 1; i <= shift; i++) {
            bigint->koefs[i] = 0;
        }
        bigint->koefs[0] += shift;
        return 0;
    }
}


int SplitBigint(BigInt* bigint, BigInt* low, BigInt* high, unsigned int shift) {
    /*
    Пилит длинное целое число bigint пополам и записывает половины в low и high
    Вход: 3 длинных целых числа: истоник и ответы
    Возврат: 0 - успех, 1 - ошибка
    */

    if (bigint == NULL || low == NULL || high == NULL) {
        return 1;
    }

    unsigned int sign = GetSign(bigint);

    SetSign(bigint, 0);

    if (shift == 0) {
        if (CopyBigInt(bigint, high)) {
            SetSign(bigint, sign);
            return 1;
        }
        low->high_digit = 0;
        free(low->koefs);
        low->koefs = NULL;
        SetSign(bigint, sign);
        return 0;
    }

    unsigned int high_d = bigint->high_digit & (~SIGN_MASK),
        len = ((bigint->koefs == NULL) ? 1 : (bigint->koefs[0] + (high_d != 0)));
    
    if (shift >= len) {
        if (CopyBigInt(bigint, low)) {
            SetSign(bigint, sign);
            return 1;
        }
        high->high_digit = 0;
        free(high->koefs);
        high->koefs = NULL;
        SetSign(bigint, sign);
        return 0;
    }

    high->high_digit = 0;
    unsigned int* t = (unsigned int*)realloc(high->koefs, sizeof(int) * (len - shift + 1));
    if (t == NULL) {
        SetSign(bigint, sign);
        return 1;
    }
    high->koefs = t;
    low->high_digit = 0;
    t = (unsigned int*)realloc(low->koefs, sizeof(int) * (shift + 1));
    if (t == NULL) {
        SetSign(bigint, sign);
        return 1;
    }
    low->koefs = t;

    low->koefs[0] = shift;
    high->koefs[0] = len - shift;

    for (unsigned int i = 1; i <= shift; i++) {
        low->koefs[i] = bigint->koefs[i];
    }
    for (unsigned int i = shift + 1; i <= bigint->koefs[0]; i++) {
        high->koefs[i - shift] = bigint->koefs[i];
    }
    if (high_d != 0) {
        high->koefs[len - shift] = high_d;
    }

    if (Normolize(low) || Normolize(high)) {
        SetSign(bigint, sign);
        return 1;
    }

    SetSign(bigint, sign);
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