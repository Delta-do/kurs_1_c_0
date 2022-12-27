#define _CRT_SECURE_NO_WARNINGS
#define _USE_MATH_DEFINES
#include <stdio.h>
#include <io.h> //_setmode
#include <fcntl.h> //_O_U16TEXT
#include <locale.h>
#include <stdlib.h> 
#include <math.h>

#define SCREENW 60 //размеры поля вывода в символах
#define SCREENH 40

typedef double (*pF)(double);

typedef struct D_arr {
	double* p;
	int size;
} D_arr;

double F1(double x)
{
	double y = 0;
	y = cos(M_PI * x) / x + x * sin(M_PI * x);
	return y;
}

double F2(double x)
{
	double y = 0;
	if (x <= 0)				// x <= 0
		y = 1 + ((3 + x) / (1 + pow(x, 2)));
	else
		if (x > 0 && x < 1) // 0 < x < 1
			y = pow(1 + pow(1 - x, 2), 0.5);
		else				// x >= 1
			y = (1 + x) / (1 + pow(cos(x), 2));
	return y;
}


int rand_value(pF pf, double x1, double x2, int N)
{
	double x;
	x1 += 0.001; //интервал

	long lt = time(NULL);
	int st = (unsigned)lt / 2;
	srand(st);

	for (int i = 0; i < N; i++)
	{
		printf("%2d) ", i + 1);
		x = x1 + 1. * (x2 - x1) * rand() / RAND_MAX;
		printf("F(%3.1lf) = %5.1lf\n", x, pf(x));
	}

	return 0;
}

int count_negative(D_arr* arr)
{
	int k = 0;
	puts("Отрицательные значения функции при табулировании");
	for (int i = 0; i < arr->size; i++)
	{
		if (arr->p[i] < 0)
		{
			printf("%8.2lf\n", arr->p[i]);
			k++;
		}
	}
	printf("Количество отрицательных: %d\n", k);
	return 0;
}


D_arr* tab_F(pF pf, double x1, double x2, double step, int isfile)
{
	D_arr arr_y;
	arr_y.p = (double*)calloc((x2 - x1 + 2) / step, sizeof(double));
	int i = 0;
	FILE *f = NULL;

	if (isfile)
	{
		f = fopen("Tab_Func.txt", "w");
		if (f == NULL)
		{
			puts("Ошибка при создании файла");
			return NULL;
		}
		fprintf(f, "%5c %8s\n", 'x', "F(x)");
	}

	puts("======================");
	printf("||   x   |   F(x)   ||\n");
	puts("||------------------||");

	for (double x = x1; x < x2; x += step)
	{
		if (pf == F1 && x == 0)
		{
			printf("|| %5.2lf |     -    ||\n", x);
		}
		else
		{
			arr_y.p[i] = pf(x);
			printf("|| %5.2lf | %8.2lf ||\n", x, arr_y.p[i]);
			if (isfile)
				fprintf(f, "%5.2lf %8.2lf\n", x, arr_y.p[i]);
			i++;
		}
	}
	puts("======================");
	arr_y.size = i;

	if (isfile)
	{
		if (fclose(f) == EOF)
		{
			puts("Ошибка при закрытии файла");
			return NULL;
		}
	}
	
	return &arr_y;
}

int draw_F(pF pf, double x1, double x2)
{
	char screen[SCREENW][SCREENH];
	double x, y[SCREENW];
	double ymin = 0, ymax = 0;
	double hx, hy;
	int i, j;
	int xz, yz;

	hx = (x2 - x1) / (SCREENW - 1);

	for (i = 0, x = x1; i < SCREENW; ++i, x += hx)
	{
		if (pf == F1 && x == 0) //учитываем область допустимых значений
			y[i] = pf(x + 0.1);
		else
			y[i] = pf(x); //расчет значений функции для каждой точки поля вывода графика

		if (y[i] < ymin) ymin = y[i];
		if (y[i] > ymax) ymax = y[i];
	}

	hy = (ymax - ymin) / (SCREENH - 1);
	yz = (int)floor(ymax / hy + 0.5);
	xz = (int)floor((0. - x1) / hx + 0.5);

	//построение осей и заполнение массива отображения пробелами

	for (j = 0; j < SCREENH; ++j)
		for (i = 0; i < SCREENW; ++i)
		{
			if (j == yz && i == xz) screen[i][j] = '+';
			else if (j == yz) screen[i][j] = '-';
			else if (i == xz) screen[i][j] = '|';
			else screen[i][j] = ' ';
		}

	//определение положения значения функции на поле вывода

	for (i = 0; i < SCREENW; ++i)
	{
		j = (int)floor((ymax - y[i]) / hy + 0.5);
		screen[i][j] = '*';
	}

	//печать массива символов

	for (j = 0; j < SCREENH; ++j)
	{
		for (i = 0; i < SCREENW; ++i)  putchar(screen[i][j]);
		putchar('\n');
	}

	return 0;
}


int main()
{
	int v, t, N;
	double x, y, x1, x2, h;
	D_arr arr_y;
	pF pf = F1;

	{
		system("cls");
		setlocale(LC_ALL, "RUS");

		puts("===============================================================");
		puts("||                    Курсовая работа                        ||");
		puts("||   по предмету \"Основы программирования и алгоритмизации\"  ||");
		puts("||     Выполнил студент группы бИСТ-223 Смоленская М.А.      ||");
		puts("===============================================================");
		puts("");
		puts("===============================================================");
		puts("||                    Добро пожаловать!                      ||");
		puts("||         Это программа для расчёта функций F1 и F2         ||");
		puts("||-----------------------------------------------------------||");

		int d = _setmode(_fileno(stdout), _O_U16TEXT); //меняем кодировку на юникод
		wprintf(L"||                 cos(πx)                                   ||\n");
		wprintf(L"||        F1(x) = --------- + x * sin(πx)                    ||\n");
		wprintf(L"||                    x                                      ||\n");
		wprintf(L"||-----------------------------------------------------------||\n");
		wprintf(L"||                 「                                        ||\n");
		wprintf(L"||                 |         3 + x                           ||\n");
		wprintf(L"||                 |   1 + --------- ,    x <= 0             ||\n");
		wprintf(L"||                 |        1 + x²                           ||\n");
		wprintf(L"||                 |                                         ||\n");
		wprintf(L"||        F2(x) = <    1 + (1 - x)² ,     0 < x < 1          ||\n");
		wprintf(L"||                 |                                         ||\n");
		wprintf(L"||                 |       1 + x                             ||\n");
		wprintf(L"||                 |   -------------- ,   x >= 1             ||\n");
		wprintf(L"||                 |    1 + cos²(x)                          ||\n");
		wprintf(L"||                 L                                         ||\n");
		d = _setmode(_fileno(stdout), d); //меняем кодировку обратно
	}
	puts("===============================================================");
	puts("Что вы хотите сделать?");
	puts(" 1 - Вычислить единичное значение функции F1");
	puts(" 2 - Вычислить единичное значение функции F2");
	puts(" 3 - Вычислить значения функции F1 для N случайных значений из интервала (x1, x2)");
	puts(" 4 - Вычислить значения функции F2 для N случайных значений из интервала (x1, x2)");
	puts(" 5 - Протабулировать функцию F1 на промежутке от x1 до x2");
	puts(" 6 - Протабулировать функцию F2 на промежутке от x1 до x2");
	puts(" 7 - Построить график функции F1");
	puts(" 8 - Построить график функции F2");
	puts(" 9 - выход из программы");
	printf("> ");
	scanf("%d", &v);

	if (v % 2 == 0)
		pf = F2;

	while (1)
	{
		if (v == 1 || v == 2)
		{
			puts("Введите значение x");
			printf("x = ");
			scanf("%lg", &x);

			if (v == 1 && x == 0)
			{
				do {
					fseek(stdin, 0, SEEK_END);
					puts("Значение x не из области допустимых значений");
					puts("Введите другое значение");
					printf("x = ");
					scanf("%lg", &x);
				} while (x == 0);
			}

			y = pf(x);
			printf("F(%lg) = %.4lg\n", x, y);
			break;
		}
		else
		{
			if (v == 3 || v == 4)
			{
				puts("Введите границы промежутка");
				printf("x1 = ");
				scanf("%lg", &x1);
				printf("x2 = ");
				scanf("%lg", &x2);
				puts("Введите количество значений");
				printf("N = ");
				scanf("%d", &N);

				while (!(x1 < x2))
				{
					fseek(stdin, 0, SEEK_END);
					puts("Введены некорректные значения: x1 должен быть меньше x2");
					puts("Введите другие значения");
					printf("x1 = ");
					scanf("%lg", &x1);
					printf("x2 = ");
					scanf("%lg", &x2);
				}

				rand_value(pf, x1, x2, N);
				break;
			}
			else
			{
				if (v >= 5 && v <= 8)
				{
					puts("Выберите вид промежутка");
					puts(" 1 - Интервал      (x1, x2)");
					puts(" 2 - Полуинтервал  [x1, x2)");
					puts(" 3 - Полуинтервал  (x1, x2]");
					puts(" 4 - Отрезок       [x1, x2]");
					printf("> ");
					scanf("%d", &t);

					puts("Введите границы промежутка");
					printf("x1 = ");
					scanf("%lg", &x1);
					printf("x2 = ");
					scanf("%lg", &x2);

					while (!(x1 < x2))
					{
						fseek(stdin, 0, SEEK_END);
						puts("Введены некорректные значения: x1 должен быть меньше x2");
						puts("Введите другие значения");
						printf("x1 = ");
						scanf("%lg", &x1);
						printf("x2 = ");
						scanf("%lg", &x2);
					}

					puts("Введите шаг табуляции");
					printf("h = ");
					scanf("%lg", &h);

					switch (t)
					{
					case 1:
						x1 += h;
						break;
					case 2:
						break;
					case 3:
						x1 += h;
						x2 += h;
						break;
					case 4:
						x2 += h;
						break;
					}

					if (v == 5 || v == 6)
					{
						puts("Куда вы хотите вывести значения?");
						puts(" 1 - В файл и на консоль");
						puts(" 0 - Только на консоль");
						printf("> ");
						scanf("%d", &v);
						arr_y = *tab_F(pf, x1, x2, h, v);

						puts("Вывести только отрицательные значения функции?");
						puts(" 1 - Да");
						puts(" 0 - Нет");
						printf("> ");
						scanf("%d", &v);
						if (v)
							count_negative(&arr_y);
					}
					if (v == 7 || v == 8)
						draw_F(pf, x1, x2);

					break;
				}
				else
				{
					if (v == 9)
						return 0;
					else
					{
						do {
							fseek(stdin, 0, SEEK_END); //очищаем поток ввода
							puts("Введено некорректное значение");
							puts("Введите другое значение");
							printf("> ");
							scanf("%d", &v);
						} while (!(v >= 0 && v <= 9));

						if (v % 2 == 0)
							pf = F2;
						else pf = F1;
					}
				}
			}
		}
	}

	puts("Начать заново?");
	puts(" 1 - Да");
	puts(" 0 - Нет (выход)");
	scanf("%d", &v);
	if (v == 1)
	{
		main();
		return 0;
	}
	else
		return 0;

	return 0;
}