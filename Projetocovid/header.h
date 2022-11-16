/*===========================================================*/
/*Program: Covid-19 data analysis                            */
/*                                                           */
/*Description: This program has the objective of analyzing   */
/*             various .csv or .dat files containing various */
/*             data informations about the delevoplement     */
/*             of the pandemic in various countries.         */
/*                                                           */
/*Files:                                                     */
/*  (1) commandline.c: main, reads command line options      */
/*  (2) read.c: dynamically allocates .csv and .dat files    */
/*  (3) dataprint.c: prints data to console and to files     */
/*  (4) datares.c: restricts data to selected condition      */
/*  (5) datasel.c: selects data from selected condition      */
/*  (6) memoryclear.c: clears the dynamically allocated data */
/*  (7) sort.c: sorts data accordingly                       */
/*                                                           */
/* UC: Programação - MEEC - 1º semestre                      */
/*                                                           */
/*Autorship: Eduardo Filipe Braz Barata 99930, IST-MEEC      */
/*           Nuno Filipe Trigo Fernandes 100695, IST-MEEC    */
/*                                                           */
/*                                          Last mod:23/05/21*/
/*===========================================================*/
#ifndef covid_h_included
#define covid_h_included

typedef struct countries{ //Main list - contains all the countries and their static info
    char *country_name;
    char country_code[4];
    char continent[8];
    int population;
    struct countries *next;
    struct country_info *head;
} country;

typedef struct country_info{ //Secondary list - contains all the variable info of each country
    char indicator[6];
    int weekly_count;
    char year_week[7];
    double rate_14_day;
    int cumulative_count;
    struct country_info *nexti;
}countryinfo;

//Global functions declaration
country* readtype(char inputname[], char type[]);
void printselection(country* element,short type,char output[]);
country* quicksortThis(country *head,char type[]);
void clearmemory(country *start);
void dataselection(country **Current, char seltype[]);
country* dataRestriction(country *head,char condition[]);
int weekref(country* headref,char week[]);

#endif // covid_h
