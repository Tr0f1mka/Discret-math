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
    Возврат: 0 - "+", 1 - "-"
    */
    
    return (big_int->high_digit & SIGN_MASK) != 0;
}


int AbsCompare(BigInt* num1, BigInt* num2) {
    /*
    Сравнение длинных целых чисел по модулю
    Вход: 2 длинных целых числа
    Возврат: 1 - первое число больше, 0 - числа равны, -1 - второе число больше, 2 - ошибка
    */

    if (num1 == NULL || num1->koefs == NULL || num2 == NULL || num2->koefs == NULL) {
        return 2;
    }

    unsigned int len1 = num1->koefs[0], len2 = num2->koefs[0];

    if (len1 != len2) {
        return (len1 > len2) ? 1 : -1;
    }

    for (int i = 1; i <= len1; i++) {

        if (num1->koefs[i] > num2->koefs[i]) {
            return 1;
        }
        if (num1->koefs[i] < num2->koefs[i]) {
            return -1;
        }

    }

    return 0;
}