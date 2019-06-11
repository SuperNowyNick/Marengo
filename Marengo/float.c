#include "float.h"


float_t itof(int a)
{
    float_t b;
    if (a < 0)
        b.signum = -1;
    else
        b.signum = 1;
    b.character = abs(a);
    b.mantisa = 0;
    b.precision = 0;
}
float_t idiv(int a, int b, signed char precision)
{
    float_t c;
    c.character = a / b;
    c.signum = (a < 0) ? -1 : 1;
    c.signum *= (b < 0) ? -1 : 1;
    c.precision = precision;
    if (precision > 0)
    {
        int rest = a % b;
        while (precision) {
            rest *= 10;
            precision--;
        }
        c.mantisa = rest / b;
    }
    else
        c.mantisa = 0;
    return c;
}

float_t fidiv(float_t a, int b, signed char precision)
{
    float_t c;
    c = a;
    c.signum *= (b < 0) ? -1 : 1;
    b = abs(b);
    c.character /= b;
    c.precision = precision;
    while (a.precision < precision)
    {
        a.mantisa *= 10;
        a.precision++;
    }
    while (a.precision > precision)
    {
        a.mantisa /= 10;
        a.precision--;
    }
    if (precision > 0)
    {
        int rest = a.character % b;
        while (precision) {
            rest *= 10;
            precision--;
        }
        c.mantisa = rest / b + a.mantisa/b;
    }
    else
        c.mantisa = 0;
    return c;
}
float_t fadd(float_t a, float_t b)
{
    float_t c;
    char maxprec;
    while (b.precision < a.precision) {
        b.mantisa *= 10;
        b.precision++;
    }
    while (a.precision < b.precision) {
        a.mantisa *= 10;
        a.precision++;
    }
    if (a.signum == b.signum) {
        c = a;
        c.character += b.character;
        c.mantisa += b.mantisa;
        int checker = 1;
        for (int i = 0; i < c.precision; i++)
            checker *= 10;
        if (c.mantisa / checker != 0) {
            c.mantisa %= checker;
            c.character++;
        }
    }
    else
    {
        if (a.character == b.character)
        {
            // compare fractional parts
            if (a.mantisa > b.mantisa)
                c = a;
            else {
                c = b;
                b = a;
            }
            c.character = 0;
            c.mantisa -= b.mantisa;
        }
        else {
            if (a.character > b.character) {
                c = a;
            }
            else {
                c = b;
                b = a;
            }
            c.character -= b.character;
            if (c.mantisa >= b.mantisa)
                c.mantisa - b.mantisa;
            // TODO: cut zeroes at end
            else {
                if (c.character > 0)
                    c.character--;
                int checker = 1;
                for (int i = 0; i < c.precision; i++)
                    checker *= 10;
                c.mantisa += checker - b.mantisa;
            }
        }
    }
    return c;
}
float_t fsub(float_t a, float_t b)
{
    b.signum *= -1;
    return fadd(a, b);
}

float_t myatof(char* str)
{
    float_t result = { 1,0,0,0 };
    int i = 0;
    if (str[i] == '-')
    {
        result.signum = -1;
        i++;
    }
    while (isdigit(str[i]))
    {
        result.character = result.character * 10 + ((int)str[i] - 48);
        i++;
    }
    if (str[i] == '.' || str[i] == ',')
    {
        i++;
        while (isdigit(str[i]))
        {
            result.mantisa = result.mantisa * 10 + ((int)str[i] - 48);
            i++;
            if (++result.precision >= FLOAT_PRECISION)
                break;
        }
        // cut the zeroes
        i--;
        while (str[i] == '0')
        {
            result.mantisa /= 10;
            result.precision--;
            i--;
        }
    }
    else if (str[i] == 'e' || str[i] == 'E')
    {
        i++;
        int sgn = 1;
        if (str[i] == '-') {
            sgn = -1;
            i++;
        }
        int exp = 0;
        while (isdigit(str[i])) {
            exp = exp * 10 + ((int)str[i] - 48);
            i++;
        }
        if (sgn > 0)
        {
            for (int i = 0; i < exp; i++)
                result.character *= 10;
        }
        else if (sgn < 0)
        {
            if (exp > FLOAT_PRECISION) {
                result.character = 0;
            }
            else {
                for (int i = 0; i < exp; i++) {
                    result.mantisa = result.mantisa * 10 + result.character % 10;
                    result.character /= 10;
                    result.precision++;
                }
            }
        }
    }
    return result;
}
int myftoa(float_t f, char* str)
{
    char buf[16];
    char *p = buf;

    while (f.character || p==buf) {
        *p++ = f.character % 10 + 48;
        f.character /= 10;
    }
    int len = p - buf;

    if (f.signum == -1) {
        *str++ = '-';
        len++;
    }
    while (p > buf)
        *str++ = *--p;

    if (f.mantisa>0)
    {
        *str++ = '.';
        len++;
        while (f.precision) {
            *p++ = f.mantisa % 10 + 48;
            f.mantisa /= 10;
            f.precision--;
        }
        len += p - buf;
        while (p > buf)
            *str++ = *--p;
    }
    return len;
}

void printFloat(float_t f)
{
    if (f.signum < 0)
        consPrintf("-");
    consPrintf("%d", f.character);
    if (f.mantisa > 0)
        consPrintf(".");
    char* c = calloc(f.precision + 1, sizeof(char));
    c[f.precision] = 0;
    int tempman = f.mantisa;
    for (int i = f.precision; i > 0; i--)
    {
        c[i - 1] = (tempman % 10) + 48;
        tempman /= 10;
    }
    printf("%s", c);
}
