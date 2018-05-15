//Soubor:  proj1.c
//Datum:   20.10.2017
//Autor:   Ivana Saranová, xsaran02@stud.fit.vutbr.cz
//Projekt: Práce s textem, projekt è. 10 pro pøedmìt IZP
//Popis:   Program, který vypoèítává povolení klávesy pøi práci s adresami

//funkce pro vstup/výstup
#include <stdio.h>
//obecné funkce jazyka C
#include <stdlib.h>
//pro funkce strcmp, strcpy a strlen
#include <string.h>

//maximální délka pole adresy
#define MAXP 101
//maximální délka pole enabled
#define MAXE 95

//Návod k užívání programu
const char *HELP =
  "Program: Projekt 1\n"
  "Author: Ivana Saranova 2017\n"
  "Description: This program calculates enabled keys when working\n"
  "with multiple addresses from auxiliary text file\n"
  "To run this program successfully you need to write:\n"
  "    1) ./proj1\n"
  "    2) one or no argument(e.g.: b, ada, lomnice)\n"
  "        - if you need to write two words use quotation marks\n"
  "        - more than one argument will cause the program to stop\n"
  "    3) <textfile.txt (textfile = any text file with addresses in it)\n"
  "        - addresses in text file should be up to 100 characters long\n"
  "        and must contain only letters, numbers or character '-'\n"
  "        - characters except for letters and numbers have to be written in quotation marks\n"
  "        - address longer than 100 character will be trimmed to 100 characters\n"
  "        - any unsupported characters will cause the program to stop\n"
  "Note: This program supports only first 128 ASCII characters\n";

//DEKLARACE PROMĚNNÝCH VE STRUKTUŘE
struct NACITANI
{
  char adresa [MAXP+1]; //pole znaků pro uložení načtených dat ze stdin
  char found [MAXP]; //pole znaků pro uložení adresy na výpis Found
  char found2 [MAXP]; //pole znaků pro uložení adresy na kombinovaný výpis Enable a Found
  char enable [MAXE]; //pole znaků pro uložení a seřazení znaků ASCII na výpis Enable
  char arg[MAXP]; //pole znaků pro uložení argumentu
  int delka_arg; //počet znaků v argumentu
  int moznosti; //počet možností výpisu na Enable
  int combo; //proměnná pro určení kombinovaného výpisu Enable a Found
};

///////////////////////////////
//PROTOTYPY VŠECH FUNKCÍ
//popisy funkcí jsou vypsány nad jednotlivými funkcemi
void vymazat_zbytek_nactena();
int obsahuje_charpole_znak(char pole[], char znak);
int kontrola_znaky_arg(char pole[]);
char* vycisteni_pole(char pole[], int max);
struct NACITANI vse_vycistit(struct NACITANI foo);
char* mala_na_velka(char pole[], int max);
void vypis(struct NACITANI foo);
char* zapis_enable(char* enable, char* adresa, int delka_arg);
int nacteni_shoda(int shoda, char* arg, char* adresa, int delka_arg);
char* zkraceni_adresy(char* adresa);
void err_format(char* adresa);
void err_arg(int argc);
void show_help(char** argv);
char* prace_arg(char* arg, int argc, char** argv);
struct NACITANI bez_argumentu(struct NACITANI foo);
struct NACITANI s_argumentem(struct NACITANI foo);
struct NACITANI nacitaci_cyklus(int argc, struct NACITANI foo);

///////////////////////////////
//HLAVNÍ TĚLO PROGRAMU
int main(int argc, char *argv[])
{
  struct NACITANI foo;
  foo.moznosti = 0;
  foo.combo = 0;

  foo = vse_vycistit(foo);

  if (argc > 1)
    foo.delka_arg = (int)strlen(argv[1]);
  strcpy(foo.arg, prace_arg(foo.arg, argc, argv));

  vypis(nacitaci_cyklus(argc, foo));
  return 0;
}

///////////////////////////////////////
//JEDNOTLIVÉ FUNKCE A JEJICH POPISY
//hlavní načítací cyklus pro načítání a práci s adresou a ukládáním povolených kláves do enable
struct NACITANI nacitaci_cyklus(int argc, struct NACITANI foo)
{
  int nacteno = scanf(" %101[^\n]s", foo.adresa);
  while (nacteno == 1)
  {
    err_format(foo.adresa);
    zkraceni_adresy(foo.adresa);
    strcpy(foo.adresa, mala_na_velka(foo.adresa, MAXP));

    if (strcmp(foo.adresa, foo.found) == 0)
    {
      nacteno = scanf(" %101[^\n]s", foo.adresa);
      continue;
    }

    if(argc == 1)
      foo = bez_argumentu(foo);
    else
      foo = s_argumentem(foo);

    nacteno = scanf(" %101[^\n]s", foo.adresa);
  }
  return foo;
}

//vypíše možné znaky do enable, pocet moznosti a nalezene adresy
struct NACITANI s_argumentem(struct NACITANI foo)
{
  int shoda = 0;
  if (strcmp(foo.adresa, foo.arg) == 0)
  {
    foo.combo++;
    strcpy(foo.found2, foo.adresa);
  }
  shoda = nacteni_shoda(shoda, foo.arg, foo.adresa, foo.delka_arg);
  if (shoda == foo.delka_arg)
  {
    foo.moznosti++;
    strcpy(foo.found, foo.adresa);
    strcpy(foo.enable, zapis_enable(foo.enable, foo.adresa, foo.delka_arg));
  }
  return foo;
}

//vypíše první písmena všech adres do proměnné enable
struct NACITANI bez_argumentu(struct NACITANI foo)
{
  foo.moznosti++;
  strcpy(foo.found, foo.adresa);
  strcpy(foo.enable, zapis_enable(foo.enable, foo.adresa, 0));
  return foo;
}

//provede základní úkony se zadaným argumentem (zobrazení help, error při více argumentech, převod písmen)
char* prace_arg(char* arg, int argc, char** argv)
{
  if (argc > 1)
  {
    show_help(argv);
    err_arg(argc);
    strcpy(arg, argv[1]);
    strcpy(arg, mala_na_velka(arg, MAXP));
  }
  return arg;
}

//zobrazení pomocné funkce help
void show_help(char** argv)
{
  if (strcmp(argv[1], "help") == 0)
  {
    printf("%s", HELP);
    exit(0);
  }
}

//výpis na stderr při větším počtu argumentů
void err_arg(int argc)
{
  if (argc > 2)
  {
    fprintf(stderr, "(Error) - too many arguments! (use 'help' argument)\n");
    exit(-1);
  }
}

//výpis na stderr při použití řídících znaků nebo znaků vyšších než 127
void err_format(char* adresa)
{
  if (kontrola_znaky_arg(adresa) == 0)
  {
    fprintf(stderr, "(Error) - wrong adress format!\n");
    exit(-1);
  }
}

//v případě, že adresa obsahuje více než 100 znaků, dojde k jejímu "osekání" na sto znaků
//program pokračuje normálně v běhu dál
char* zkraceni_adresy(char* adresa)
{
  if ((int)strlen(adresa) > 100)
  {
    vymazat_zbytek_nactena();
    fprintf(stderr, "(Error) - address '%.5s...' too long! (shortened to 100 characters)\n", adresa);
    adresa[101] = '\0';
    adresa[100] = '\0';
  }
  return adresa;
}

//cyklus kontroluje shodu mezi znaky argumentu a načtenou adresou,
//pokud se shodují, proměnná shoda se zvětší o 1
int nacteni_shoda(int shoda, char* arg, char* adresa, int delka_arg)
{
  for (int i = 0; i < delka_arg; i++)
    if ((int)adresa[i] == (int)arg[i])
      shoda++;
  return shoda;
}

//načtení příslušného znaku do pole enable
char* zapis_enable(char* enable, char* adresa, int delka_arg)
{
  if (obsahuje_charpole_znak(enable, adresa[0]) == 0)
    enable[(int)adresa[delka_arg] - 32]= adresa[delka_arg];
  return enable;
}

//výpis na stdout, proměnné moznosti, combo a argc určují typ výstupu
//Not found pokud se v souboru nenachází žádná adresa nebo pro zadaný argument neexistuje žádná shoda (moznosti je 0)
//Found pokud existuje právě jedna shoda (moznosti je 1) nebo je argument prázdný, využita proměnná found
//Enable pokud existuje více shod (moznosti jsou větší než 1), využita proměnná enable
//u výpisu Enable je ještě kombinovaná možnost Enable a Found, pokud se v v souboru nachází dvě adresy
//se stejným základem (např. Lom a Lomnice) - zde je využita proměnná found2 a combo
void vypis(struct NACITANI foo)
{
  if (foo.moznosti == 0)
    printf("Not found\n");
  else if(foo.moznosti == 1)
    printf("Found: %s\n", foo.found);
  else if(foo.moznosti > 1)
  {
    printf("Enable: ");
    //výpis všech znaků uložených v poli enable bez prázdných znaků
    for (int i = 0; i < MAXE; i++)
      if (foo.enable[i] != '\0')
        printf("%c", foo.enable[i]);
    printf("\n");
    //případ, kdy se v souboru.txt nachází dvě adresy se stejným základem (např. Lom a Lomnice)
    //výpis adresy, která celá odpovídá zadanému argumentu
    if (foo.combo == 1)
      printf("Found: %s\n", foo.found2);
  }
}

//Vymaže přebytečné znaky ze vstupu tak, aby se znovu nenačetly
void vymazat_zbytek_nactena()
{
  char znak;
  while ((znak = getchar() != '\n') && (znak != EOF));
}

//Zkontroluje, zda se v poli znakù nenachází daný znak (pokud ano, vrátí 1, jinak 0)
//vstupní argumenty: prohledávané pole znakù, vyhledávaný znak
int obsahuje_charpole_znak(char pole[], char znak)
{
  for (int i = 0; i < 26; i++)
    if (pole[i] == znak)
      return 1;
  return 0;
}

//Zkontroluje, zda pole znakù obsahuje pouze tisknutelné znaky (pokud ano, vrátí 1, jinak 0)
//vstupní argumenty: kontrolované pole znakù
int kontrola_znaky_arg(char pole[])
{
  for (int i = 1; i < 256; i++)
  {
    if(obsahuje_charpole_znak(pole, (char)i) == 1)
      return 0;
    if (i == 31)
      i = 126;
  }
  return 1;
}
//vycisteni vsech poli najednou
struct NACITANI vse_vycistit(struct NACITANI foo)
{
  strcpy(foo.adresa, vycisteni_pole(foo.adresa, MAXP+1));
  strcpy(foo.enable, vycisteni_pole(foo.enable, MAXE));
  strcpy(foo.found, vycisteni_pole(foo.found, MAXP));
  strcpy(foo.found2, vycisteni_pole(foo.found2, MAXP));
  strcpy(foo.arg, vycisteni_pole(foo.arg, MAXP));
  return foo;
}

//Naplni pole znakù prázdnými znaky
//vstupní argumenty: pole znakù na vyèištìní, délka pole (je nutné vyèistit všechny znaky)
char* vycisteni_pole(char pole[], int max)
{
  for (int i = 0; i < max; i++)
    pole[i] = '\0';
  return pole;
}

//Pøevede všechna malá písmena v poli na velká
//vstupní argumenty: pole znakù na úpravu, délka pole
char* mala_na_velka(char pole[], int max)
{
  for (int i = 0; i < max; i++)
    if ((char)pole[i] >= 'a' && (char)pole[i] <= 'z')
      pole[i] = (char)pole[i] - 'a' + 'A';
  return pole;
}


