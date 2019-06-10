#include "float.h"

// TODO: Write myftoa
// TODO: Write idivide

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
				}
			}
		}
	}
	return result;
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
