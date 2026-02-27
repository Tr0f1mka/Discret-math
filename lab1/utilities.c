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

    target->koefs = (unsigned int*)realloc(target->koefs, sizeof(unsigned int) * (source->koefs[0]+1));
    if (target->koefs == NULL) {
        return 1;
    }

    for (unsigned int i = 0; i < source->koefs[0]+1; i++) {
        target->koefs[i] = source->koefs[i];
    }

    return 0;
}