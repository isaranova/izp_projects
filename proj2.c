//Soubor:  proj2.c
//Datum:   13.11.2017
//Autor:   Ivana Saranova, xsaran02@stud.fit.vutbr.cz
//Projekt: Iteracni vypocty - Projekt 2 pro predmet IZP
//Popis:   Program vypocitava hodnotu tan(x) bez vyuziti matematicke knihovny
//         vzdalenost a vysku mereneho objektu

//funkce pro vstup a vystup
#include <stdio.h>
//obecne funkce jazyka C
#include <stdlib.h>
//matematicke funkce - pouze pro tan(x) na porovnani
#include <math.h>
//pro funkci strcmp
#include <string.h>

#define amax 1.4
#define cmax 100
#define nmmax 13

//textovy retezec pro help
const char* HELPMSG =
  "////////////////////////////////////////////////////////////\n"
  "Program: Project2 Iterative calculations\n"
  "Author: Ivana Saranova 2017\n"
  "Description: Program calculates tan(x), distance\n"
  "             and height of measured object\n"
  "To run this program successfully use parameters:\n"
  "   1) --tan A N M (A = angle in radians,\n"
  "                   N and M = integer of iterations)\n"
  "       - get results of math.h tan(A), taylor tan(A),\n"
  "         chained fraction tan(A) and divergence between them\n"
  "   2) [-c X] -m A [B] ([..] means you don't have to use it,\n"
  "                       A and B = angle in radians,\n"
  "                       X = height of the measuring device (double))\n"
  "       - get distance (and height) of measured object\n"
  "         from chained fraction tan(x)\n"
  "       - number of iterations used: 13\n"
  "////////////////////////////////////////////////////////////\n";

//struktura pro promenne
struct variables
{
  double A, B; //uhly v radianech
  unsigned int N, M; //pocty iteraci
  double X; //vyska merice
};

//PROTOTYPY VSECH PROMENNYCH
//Error vypisy
void err(int argc, char** argv);
int err_c(char** argv);
int err_tan(char** argv);
int err_m(char** argv);
//Nacitani z argumentu a vypis vysledku
struct variables var_load(struct variables var, char** argv, int argc);
void result(struct variables var, char** argv, int argc);
//Pomocne matematicke a vyhledavaci funkce
int check_err(char** argv, char* par, int pos, char* n1, char* n2, char* n3);
int is_interval(double numb, double low,double up);
int is_numb(char* numb);
int is_arg(char** argv, char* find, int argc);
//vypocet tan(x)
double cfrac_tan(double x, unsigned int n);
double taylor_tan(double x, unsigned int n);

//HLAVNI TELO PROGRAMU
int main(int argc, char** argv)
{
  err(argc, argv);
  if(strcmp(argv[1], "--help") == 0)
  {
    printf("%s", HELPMSG);
    exit(0);
  }

  struct variables var;
  var = var_load(var, argv, argc);

  result(var, argv, argc);
  return 0;
}

//FUNKCE PRO VYPOCET TAN(X)
//vypocet tan(x) pomoci Taylorova rozvoje
double taylor_tan(double x, unsigned int n)
{
  long long nom[] = {1, 1, 2, 17, 62, 1382, 21844, 929569, 6404582,
  443861162, 18888466084, 113927491862, 58870668456604, 8374643517010684};
  long long den[] = {1, 3, 15, 315, 2835, 155925, 6081075, 638512875,
  10854718875, 1856156927625, 194896477400625, 49308808782358125, 3698160658676859375};
  double a, a0, s;
  a0 = x;
  a = a0;
  s = a;
  for(int i = 1; i < (int)n; i++)
  {
    a = a * a0 * a0 * (double)nom[i]/(double)den[i] * (double)den[i-1] / (double)nom[i-1];
    s = s + a;
  }
  return s;
}

//vypocet tan(x) pomoci vzorce zretezenehe zlomku
double cfrac_tan(double x, unsigned int n)
{
  double b, b0, a, frac;
  b0 = x;
  b = x*x;
  frac = 1.0;
  for(int i = (int)n; i >= 1; i--)
  {
    a = ((i+1)*2-1);
    frac = b/(a - frac);
  }
  double tanx = b0/(1.0-frac);
  return tanx;
}

//zkontroluje, zda je zadany argument cislo
int is_numb(char* numb)
{
  char *p;
  strtod(numb, &p);
  if (*p != 0)
    return 0;
  return 1;
}

//zkontroluje, zda cislo lezi v zadanem intervalu
int is_interval(double numb, double low,double up)
{
  if (numb > low && numb <= up)
    return 0;
  return 1;
}

//zkontroluje, zda se mezi argumenty nachazi urcity retezec
int is_arg(char** argv, char* find, int argc)
{
  for(int i = 0; i <argc; i++)
    if(strcmp(argv[i], find) == 0)
      return 1;
  return 0;
}

//funkce pro kontrolu erroru - umisteni parametru a zda byla zadana cisla
int check_err(char** argv, char* par, int pos, char* n1, char* n2, char* n3)
{
  if(strcmp(argv[pos], par) != 0)
    return 1;
  if(n1 == '\0' || is_numb(n1) == 0)
    return 1;
  if(n2 == '\0' || is_numb(n2) == 0)
    return 1;
  if(n3 == '\0' || is_numb(n3) == 0)
    return 1;
  return 0;
}

//NACITANI PROMENNYCH Z ARGUMENTU A VYPIS VYSLEDKU
//vypis vysledku pro jednotlive parametry
void result(struct variables var, char** argv, int argc)
{
  if(strcmp(argv[1], "--tan") == 0)
  {
    for(int i = (int)var.N; i <= (int)var.M; i++)
      printf("%d %e %e %e %e %e\n", i, tan(var.A), taylor_tan(var.A, i),
             fabs(tan(var.A) - taylor_tan(var.A, i)), cfrac_tan(var.A, i),
             fabs(tan(var.A) - cfrac_tan(var.A, i)));
  }
  if(is_arg(argv, "-m", argc) == 1)
  {
    printf("%.10e\n", var.X/cfrac_tan(var.A, 13));
    if (var.B != '\0')
      printf("%.10e\n", var.X/cfrac_tan(var.A, 13)*cfrac_tan(var.B, 13)+var.X);
  }
}

//nacteni argumentu do promennych podle typu parametru
struct variables var_load(struct variables var, char** argv, int argc)
{
  switch(argc)
  {
    case 3: //-m A
      sscanf(argv[2],"%lf", &var.A);
      var.B = '\0';
      var.X = 1.5;
      break;
    case 4: //-m A B
      sscanf(argv[2],"%lf", &var.A);
      sscanf(argv[3],"%lf", &var.B);
      var.X = 1.5;
      break;
    case 5:
      if (strcmp(argv[1], "--tan") ==0) //--tan A N M
      {
        sscanf(argv[2],"%lf", &var.A);
        sscanf(argv[3],"%u", &var.N);
        sscanf(argv[4],"%u", &var.M);
        var.X = 1.5;
        break;
      }
      if(strcmp(argv[1], "-c") ==0) //-c X -m A
      {
        sscanf(argv[4],"%lf", &var.A);
        sscanf(argv[2],"%lf", &var.X);
        break;
      }
    case 6: //-c X -m A B
      sscanf(argv[4],"%lf", &var.A);
      sscanf(argv[5],"%lf", &var.B);
      sscanf(argv[2],"%lf", &var.X);
      break;
  }
  return var;
}

//Errorove hlasky
//kontrola zadani parametru -m, pokud je spatne umisten parametr, nejsou tam spravna cisla, nacte se chyba
int err_m(char** argv)
{
  int fault = 0;
  fault += check_err(argv, "-m", 1, argv[2], "", "");
  fault += is_interval(strtod(argv[2], NULL), 0, amax);
  return fault;
}

//kontrola zadani parametru -tan
int err_tan(char** argv)
{
  int fault = 0;
  fault += check_err(argv, "--tan", 1, argv[2], argv[3], argv[4]);
  if(strtod(argv[2], NULL) == INFINITY || strtod(argv[2], NULL) == NAN || strcmp(argv[2],"") == 0)
    fault++;
  fault += is_interval(strtod(argv[3], NULL), 0, nmmax);
  fault += is_interval(strtod(argv[4], NULL), 0, nmmax);
  if(strtod(argv[3], NULL) > strtod(argv[4], NULL))
    fault++;
  return fault;
}

//kontrola zadani parametru -c
int err_c(char** argv)
{
  int fault = 0;
  fault += check_err(argv, "-c", 1, argv[2], "","");
  fault += is_interval(strtod(argv[2], NULL), 0, cmax);
  fault += check_err(argv, "-m", 3, argv[4], "","");
  fault += is_interval(strtod(argv[4], NULL), 0, amax);
  return fault;
}

//hlavni kontrola erroru u argumentu pro vsechny situace
void err(int argc, char** argv)
{
  int fault = 0, faulta, faultb;
  switch(argc)
  {
    case 1:
      fprintf(stderr, "Error - No argument used! (try --help)\n");
      exit(-1);
      break;
    case 2:
      if(strcmp(argv[1], "--help") != 0)
      {
        fprintf(stderr, "Error - Wrong --help argument!\n");
        exit(-1);
      }
      break;
    case 3: //-m A
      fault = err_m(argv);
      break;
    case 4: //-m A B
      fault = err_m(argv);
      fault += is_interval(strtod(argv[3], NULL), 0, amax);
      break;
    case 5: //--tan A N M ... -c X -m A
      faulta = err_tan(argv);
      faultb = err_c(argv);
      if((faulta != 0 && faultb == 0) || (faulta == 0 && faultb != 0))
        fault = 0;
      else fault = 1;
      break;
    case 6: //-c X -m A B
      fault = err_c(argv);
      fault += is_interval(strtod(argv[5],NULL), 0, amax);
      break;
    default:
      fprintf(stderr, "Error - Wrong amount of arguments! (try --help)\n");
      exit(-1);
  }
  if(fault != 0 && (argc != 1 || argc > 6))
  {
    fprintf(stderr, "Error - Wrong use of arguments! (try --help)\n");
    exit(-1);
  }
}

