/*=======================================================*/
/*Program file: read.c                                   */
/*Brief: List memory clear                               */
/*                                                       */
/*Description: This file is responsible for reading      */
/*             .csv and .dat files and dynamically       */
/*             allocate such data in a singly linked     */
/*             nested list.                              */
/*                                                       */
/*Dependencies: Struct creation, declaration by          */
/*              commandline.c of: -Input file name       */
/*                                -Reading option        */
/*                                                       */
/*Autorship: Eduardo Filipe Braz Barata 99930, IST-MEEC  */
/*           Nuno Filipe Trigo Fernandes 100695, IST-MEEC*/
/*                                                       */
/*                                      Last mod:23/05/21*/
/*=======================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "header.h"

country *addelement(country *previous,char line[],country* head, FILE *fp);
country* checkifisavailable (char* thiscountry,country *head,country **previous);
int linecheck(char *line,country* head);

/******************************************************************************
* Function name: readcsv()
*
* Arguments: char read[] - Type of reading the user wants (all or a continent)
*            char ifile[] - Input file name
*
* Return: country * - Returns a pointer to the head of the list
*
*
* Side-effects: none to report
*
* Description: This function reads data from csv files respecting the type of
*              reading the user requested and sends that information to the
*              addelement function for it to be allocated accordingly
*
*******************************************************************************/
country* readcsv(char read[], char ifile[]){

    country *head=NULL, *tail=NULL, *element=NULL;
    char line[200];

    FILE *fp = fopen(ifile, "r"); //opens csv file for reading
    if (fp==NULL){//fp==NULL when its not able to open it
        fprintf(stderr,"Input file doesn't exit. Please try again with a new one.\n");
        return NULL;
    }
    fgets(line,sizeof(line),fp);//discards the 1st line
    while(fgets(line,sizeof(line),fp)){//fgets will return NULL when unable to read a line, breaking the cycle
        if(strcmp(read,"all")!=0){//Lines are only read if the read mode is "all" or the continent name typed
            if(strstr(line,read)==NULL)
                continue;
        }
        if(head==NULL){//If its the 1st iteration of this cycle
            if((element = addelement(NULL,line,head,fp))==NULL)//invalid line
                return NULL;

            head = element;
            tail = element;//head is the 1st and last element of the list
        }
        else{
            if((element = addelement(tail,line,head,fp))==NULL){//invalid line
                clearmemory(head);
                return NULL;
            }
            tail=element;
        }
    }

    fclose(fp);//closes file
    if(head==NULL)
        fprintf(stderr,"Read type invalid or file doesn't contain information with said type.\nPlease consider using a different one.\n"
               "For more help rerun the program with the -h option.\n");
    return head;
}

/******************************************************************************
* Function name: addelement()
*
* Arguments: country *previous - Previous node of the main list
*            char line[] - Line read from the csv file
*            country *head - Head of the main list
*            FILE *fp - Pointer to the open file
*
* Return: country * - Returns a pointer to the new country created
*
* Side-effects: loses the line info
*
* Description: This function separates the data from the line read in the csv
*              file and, if everything is looking good, dynamically allocates it
*              in a singly connected nested list.
*
*******************************************************************************/
country* addelement(country *previous,char line[],country*head, FILE *fp){

    char *targ,*targ2;
    country *newcountry=NULL;
    countryinfo *newinfo=NULL,*previousinfo=NULL,*currentinfo=NULL;

    if(linecheck(line,head)==0){
        fprintf(stderr,"-1 Erro de Leitura ...\n");
        fclose(fp);
        return NULL;
    }

    //targ will receive the information in each column (separated by commas) of the line
    targ=strtok(line,",");
    //Dynamically allocates a memory block for the variable data
    if((newinfo=(countryinfo *) calloc(1, sizeof(countryinfo)))==NULL){
        fprintf(stderr,"There was an error when trying to allocate memory\n");//prints error message in case something goes wrong
        fclose(fp);
        return NULL;
    }
    newinfo->nexti=NULL;//Points the next position in the recently created block to NULL
    //Dynamically allocates a memory block for the static country information
    if((newcountry=checkifisavailable(targ,head,&previous))==NULL){
        //In case the country found still doesn't have a block of memory allocated for it,
        //the program dynamically allocates a memory block for the static country information
        if((newcountry =(country *) calloc(1,sizeof(country)))== NULL){
            fprintf(stderr,"There was an error when trying to allocate memory\n");//prints error message in case something goes wrong
            fclose(fp);
            return NULL;
        }
        //The 1st variable data position for this country will be the recently
        //allocated memory for the variable country data
        newcountry->head=newinfo;
        if(previous!=NULL){//previous will be NULL if its the 1st ever country allocated in memory
            previous->next=newcountry;
        }
        //Dynamically allocates memory for the country's name
        if((newcountry->country_name=calloc(1,strlen(targ)+1))== NULL){
            fprintf(stderr,"There was an error when trying to allocate memory\n");//prints error message in case something goes wrong
            fclose(fp);
            return NULL;
        }
        strcpy(newcountry->country_name,targ);
    }
    else{//The country already exits
        currentinfo=newcountry->head;//Gets the 1st position for the variable data
        //Finds where the last position of the variable data is so we can add the new block after it
        do{
            previousinfo=currentinfo;
            currentinfo=currentinfo->nexti;
        }
        while(currentinfo!=NULL);
        currentinfo=previousinfo;
        currentinfo->nexti=newinfo;
    }
    //Associates the data to their respective place
    targ=strtok(NULL,",");//For simplicity sake we overwrite the fixed data
    strcpy(newcountry->country_code, targ);
    targ=strtok(NULL,",");
    strcpy(newcountry->continent, targ);
    targ=strtok(NULL,",");
    newcountry->population=atoi(targ);
    targ=strtok(NULL,",");
    strcpy(newinfo->indicator, targ);
    targ=strtok(NULL,",");
    newinfo->weekly_count=atoi(targ);
    targ=strtok(NULL,",");
    strcpy(newinfo->year_week, targ);
    targ=strtok(NULL,",");
    targ2=strtok(NULL,",");
    if(targ2==NULL){
        //targ2 will be NULL if the rate_14_day field is NULL.
        //therefore targ will pickup the cumulative count information
        newinfo->rate_14_day=0;
        newinfo->cumulative_count=atoi(targ);
    }
    else{//Information is as expected
        newinfo->rate_14_day=atof(targ);
        newinfo->cumulative_count=atoi(targ2);
    }

    return newcountry;
}

/******************************************************************************
* Function name: readBinary()
*
* Arguments: char filename[] - Input file name
*
* Return: country * - Returns a pointer to the head of the list
*
* Side-effects: none
*
* Description: This function reads data from dat files and dynamically
*              allocates it in a singly connected nested list.
*
*******************************************************************************/
country* readBinary(char filename[]){

    char buffer[32];
    FILE *fp=NULL;
    country *previous=NULL, *newcountry=NULL, *head=NULL;
    countryinfo* newinfo=NULL, *previousinfo=NULL;

    if((fp=fopen(filename,"rb"))==NULL){//fp=NULL if the program is unable to open the file
        fprintf(stderr,"Input file doesn't exit. Please try again with a new one.\n");
        return NULL;
    }

    while((fread(&buffer,32,1,fp))!=0){//fread will return 0 at the end of the file
            //Dynamically allocates a memory block for the static country information
            if((newinfo=(countryinfo *) calloc(1, sizeof(countryinfo)))==NULL){
                fprintf(stderr,"There was an error when trying to allocate memory\n");//prints error message in case something goes wrong
                clearmemory(head);
                fclose(fp);
                return NULL;
            }
            if((previous==NULL)||(strcmp(previous->country_name,buffer)!=0)){
                //If its the 1st time running the cycle or the previous country ain't equal to the one being read
                //the program dynamically allocates a memory block for the static country information
                if((newcountry =(country *) calloc(1,sizeof(country)))== NULL){
                    fprintf(stderr,"There was an error when trying to allocate memory\n");//prints error message in case something goes wrong
                    clearmemory(head);
                    fclose(fp);
                    return NULL;
                }
                if(head==NULL)//If its the 1st country created, it will be the head
                    head=newcountry;
                else//else make the next pointer in the previous country point to this new country
                    previous->next=newcountry;
                newcountry->next=NULL;
                //Dynamically allocates memory for the country's name
                newcountry->country_name=calloc(1,strlen(buffer)+1);
                strcpy(newcountry->country_name,buffer);
                //Associates the new info block to the head of
                //the variable data of the country
                newcountry->head=newinfo;
                previousinfo=newinfo;
            }
            previous=newcountry;
            //reads the binary data from the file and places it in the correct place
            //For the country static info, for simplicity sake, we overwrite the information
            fread(newcountry->country_code,sizeof(newcountry->country_code),1,fp);
            fread(newcountry->continent,sizeof(newcountry->continent),1,fp);
            fread(&(newcountry->population),sizeof(int),1,fp);
            fread(newinfo->indicator,sizeof(newinfo->indicator),1,fp);
            fread(&(newinfo->weekly_count),sizeof(int),1,fp);
            fread(newinfo->year_week,sizeof(newinfo->year_week),1,fp);
            fread(&(newinfo->rate_14_day),sizeof(double),1,fp);
            fread(&(newinfo->cumulative_count),sizeof(int),1,fp);
            previousinfo->nexti=newinfo;
            previousinfo=newinfo;
            newinfo->nexti=NULL;
    }
    fclose(fp);//closes file
return head;
}

/******************************************************************************
* Function name: checkifisavailable()
*
* Arguments: char* thiscountry - Country name to search for
*            country *head - Head of the list
*            country **previous - Will be updated with the country previous
*                                 to thiscountry
*
* Return: country * - Returns a pointer to the position of the country node
*                     or it returns NULL if it wasn't able to find it
*
* Side-effects: Previous will be updated
*
* Description: This function will search in the entirety of the list for
*              the country name that was requested and return NULL if it's
*              not found or said position
*
*******************************************************************************/
country* checkifisavailable (char* thiscountry,country *head,country **previous){
    //Goes through the file and finds if the country name found is already in memory or not
    while(head!=NULL){
        if(strcmp(head->country_name,thiscountry)==0){
            return head;
           }
        (*previous)=head;
        head=head->next;
    }
    return NULL;
}

/******************************************************************************
* Function name: readtype()
*
* Arguments: char inputname[] - Input file name
*            char type[] - Type of reading from the file the user wants
*
* Return: country * - Returns a pointer to the head of the list
*
* Side-effects: none
*
* Description: This function will call the readcsv function if the input file
*              name ends in .csv or the readBinary function if it ends in .dat.
*              In case of a wrong file format it will close the program with an
*              error message.
*
*******************************************************************************/
country* readtype(char inputname[], char type[]){
    country *head=NULL;
    if(strstr(inputname,".csv")!=NULL)//File ends in .csv
        head=readcsv(type,inputname);
    else if(strstr(inputname,".dat")!=NULL)//File ends in .dat
        head=readBinary(inputname);
    else{//None of the above
        fprintf(stderr,"Invalid input file format. Please try again with a different format\n");
        exit(EXIT_FAILURE);
    }
    return head;

}

/******************************************************************************
* Function name: linecheck()
*
* Arguments: char *line - line read from the file
*            country* head- Head of the list
*
* Return: int - Boolean value (1- valid, 0- invalid)
*
* Side-effects: None to report
*
* Description: This function will analyze if the line read
*              from the file is valid
*
*******************************************************************************/
int linecheck(char *line,country* head){
    int i=0,flag=0;
    char aux[200]="\0",*token=NULL,*token1=NULL;

    for(i=0;line[i]!='\0';i++){//safety copy of line to aux
        aux[i]=line[i];
    }
    aux[i+1]='\0';//adds terminator character to the array to make it a string
    for (i = 0; line[i] != '\0'; ++i) {//Checks for how many commas there are, there should be 8
        if ((line[i]==','))
            flag++;
    }
    if(flag!=8)//If there are not 8 commas, line has an error
        return 0;
    flag=0;
    token=strtok(aux,",");
    for (i=0 ; token[i] != '\0'; ++i) {//Country name - Only letters and space token
        if (((token[i]<64)||(token[i]>122)||((token[i]>90)&&(token[i]<97)))&&(token[i]!=' '))
            flag++;
    }
    token=strtok(NULL,",");
    for (i=0 ; token[i] != '\0'; ++i) {//Country code - Only letters
        if ((token[i]<64)||(token[i]>122)||((token[i]>90)&&(token[i]<97)))
            flag++;
    }
    token=strtok(NULL,",");
    if ((strcmp(token,"Europe")!=0)&&(strcmp(token,"Asia")!=0)//country continent - Predefined words
            &&(strcmp(token,"Africa")!=0)&&(strcmp(token,"America")!=0)&&(strcmp(token,"Oceania")!=0))
            flag++;
    token=strtok(NULL,",");
    for (i=0 ; token[i] != '\0'; ++i) {//Country population - only digits
        if ((token[i]<48)||(token[i]>57))
            flag++;
    }
    token=strtok(NULL,",");
    if ((strcmp(token,"deaths")!=0)&&(strcmp(token,"cases")!=0))//Indicator - only predefined words
        flag++;
    token=strtok(NULL,",");
    for (i=0 ; token[i] != '\0'; ++i) {//weekly count - only digits
        if ((token[i]<48)||(token[i]>57))
            flag++;
    }
    token=strtok(NULL,",");
    if(strlen(token)!=(strlen("yyyy-ww")))//needs to be of the same size as example
        flag++;
    for (i=0 ; token[i] != '\0'; ++i) {//year_week - only digits and a '-'
        if (((token[i]<48)||(token[i]>57))&&(token[i]!='-'))
            flag++;
    }
    token=strtok(NULL,",");//rate 14 days - only digits
    token1=strtok(NULL,",");//cumulative count - only digits
    if(token1==NULL){
        //token1 will be NULL if the rate_14_day field is NULL.
        //therefore token pickup the cumulative count information
        for (i=0 ; token[i] != '\0'; ++i) {//cumulative count - only digits
            if(token[i]=='\r')
                break;
            if ((token[i]<48)||(token[i]>57))
                flag++;
        }
    }
    else{//Information is as expected
        for (i=0 ; token[i] != '\0'; ++i) {//rate 14 days - only digits
            if (((token[i]<48)||(token[i]>57))&&(token[i]!='.'))
            flag++;
        }
        for (i=0 ; token1[i] != '\r'; ++i) {//cumulative count - only digits
            if(token1[i]=='\0')
                break;
            if ((token1[i]<48)||(token1[i]>57))
                flag++;
        }
    }
    if(flag!=0)//There was an error in any part of the line
        return 0;
    //No errors where found
    return 1;
}
