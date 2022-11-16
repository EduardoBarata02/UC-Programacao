/*======================================================*/
/*Program file: dataprint.c                             */
/*Brief: File and terminal data print                   */
/*                                                      */
/*Description: This file is responsible for printing    */
/*             a single element or the entire list to   */
/*             the user terminal and to a .csv or .dat  */
/*             file.                                    */
/*                                                      */
/*Dependencies: Struct creation, head of the list and   */
/*              declaration by commandline.c of:        */
/*                                -Output file name     */
/*                                -type of print(whole  */
/*                                 or single element)   */
/*                                                      */
/*Authorship: Eduardo Filipe Braz Barata 99930, IST-MEEC*/
/*          Nuno Filipe Trigo Fernandes 100695, IST-MEEC*/
/*                                                      */
/*                                     Last mod:23/05/21*/
/*======================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "header.h"

void printwhole(country* head);
void printelement(country *print);
void printCSV(country *print, char filename[]);
void printBinary(country *head, char filename[]);

/******************************************************************************
* Function name: printselection()
*
* Arguments: country* element - element to be printed or head of the list
*            short type - type of print (whole or a single element)
*            char output[] - Name of the output file
*
* Return: void
*
* Side-effects: This will permanently delete the entire list from memory
*
* Description: This function will remove from memory the everything that
*              is in the linked list, including the secondary lists
*
*******************************************************************************/
void printselection(country* element,short type,char output[]){
    switch(type){
        case 1://print whole
            printwhole(element);//prints to terminal whole list
            if(strstr(output,".csv")!=NULL)//Output file name ends in .csv
                printCSV(element,output);
            else if(strstr(output,".dat")!=NULL) //Output file name ends in .dat
                printBinary(element,output);
            else{//Output file name doesn't end neither in .csv nor in .dat
                fprintf(stderr,"Invalid output file type. Please try again with a different format.\n");
                exit(EXIT_FAILURE);
            }
            break;
        case 2://print single
            printelement(element);
            break;
    }
}

/******************************************************************************
* Function name: printwhole()
*
* Arguments: country* head - head of the list
*
* Return: void
*
* Side-effects: none to report
*
* Description: This function will go through the entire main list and call
*              printelement to print each element
*
*******************************************************************************/
void printwhole(country* head){
    country *nextelement =head;

    while(nextelement!=NULL){//This loop will go through the entire main list
        printelement(nextelement);
        nextelement = nextelement->next;
    }
}

/******************************************************************************
* Function name: printelement()
*
* Arguments: country* print - element of the main list to be printed
*
* Return: void
*
* Side-effects: none to report
*
* Description: This function will receive a country of the main list and print to
*              the terminal all the sub-list elements.
*
*******************************************************************************/
void printelement(country *print){
    countryinfo *infoprint = print->head;
    while(infoprint != NULL){//This loop will go through the entire secondary list
        printf("%s %s %s %d %s %d %s %f %d\n",print->country_name,
               print->country_code,print->continent,print->population,
               infoprint->indicator, infoprint->weekly_count,infoprint->year_week,
               infoprint->rate_14_day, infoprint->cumulative_count
               );//prints every information of the country
        infoprint=infoprint->nexti;
    }
}

/******************************************************************************
* Function name: printCSV()
*
* Arguments: country* print - Head of the main list
*            char filename[] - Output file name
*
* Return: void
*
* Side-effects: none to report
*
* Description: This function will receive the head of the main list and print
*              the entire list to a CSV file.
*
*******************************************************************************/
void printCSV(country *print, char filename[]){

    FILE *fp=NULL;
    countryinfo *infoprint=NULL;

    if((fp=fopen(filename,"w"))==NULL){//Opens file for writing, if it fails, show error message
        fprintf(stderr,"Couldn't create output file! Please try again.\n");
        return;
    }
    //1st line of the file (contains info of the columns)
    fprintf(fp,"country,country_code,continent,population,indicator,weekly_count,year_week,rate_14_day,cumulative_count\n");
    while(print!=NULL){//This loop will go through the entire main list
        infoprint=print->head;
        while(infoprint!=NULL){//This loop will go through the entire secondary list
            fprintf(fp,"%s,%s,%s,%d,%s,%d,%s,%f,%d\n",print->country_name,
               print->country_code,print->continent,print->population,
               infoprint->indicator, infoprint->weekly_count,infoprint->year_week,
               infoprint->rate_14_day, infoprint->cumulative_count
               );//prints every information of the country to the file
            infoprint=infoprint->nexti;
        }
    print=print->next;
    }
    fclose(fp);//Closes file
}

/******************************************************************************
* Function name: printBinary()
*
* Arguments: country* head - Head of the main list
*            char filename[] - Output file name
*
* Return: void
*
* Side-effects: none to report
*
* Description: This function will receive the head of the main list and print
*              the entire list to a binary file.
*
*******************************************************************************/
void printBinary(country *head, char filename[]){

    char countryname[32]="\0";
    FILE *fp=NULL;
    countryinfo *aux=NULL;

    if((fp=fopen(filename,"wb"))==NULL){//Opens file for writing binary, if it fails, show error message
        fprintf(stderr,"Couldn't create output file! Please try again.\n");
        return;
    }

    while(head!=NULL){//This loop will go through the entire main list
        aux=head->head;
        while(aux!=NULL){//This loop will go through the entire secondary list
            //prints every information of the country to the file
            strcpy(countryname,head->country_name);
            fwrite(countryname,32,1,fp);//Every country_name will be written in a 32 byte form
            fwrite(head->country_code,sizeof(head->country_code),1,fp);
            fwrite(head->continent,sizeof(head->continent),1,fp);
            fwrite(&(head->population),sizeof(int),1,fp);
            fwrite(aux->indicator,sizeof(aux->indicator),1,fp);
            fwrite(&(aux->weekly_count),sizeof(int),1,fp);
            fwrite(aux->year_week,sizeof(aux->year_week),1,fp);
            fwrite(&(aux->rate_14_day),sizeof(double),1,fp);
            fwrite(&(aux->cumulative_count),sizeof(int),1,fp);
            aux=aux->nexti;
        }
    head=head->next;
    }

    fclose(fp);//Closes file
}

