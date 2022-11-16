/*======================================================*/
/*Program file: datares.c                               */
/*Brief: Restrict data list                             */
/*                                                      */
/*Description: This file is responsible for restricting */
/*             the data on the list according to a      */
/*             previously defined option.               */
/*                                                      */
/*Dependencies: Struct creation, declaration by         */
/*              commandline.c of: -Restriction option   */
/*                                                      */
/*Authorship: Eduardo Filipe Braz Barata 99930, IST-MEEC*/
/*          Nuno Filipe Trigo Fernandes 100695, IST-MEEC*/
/*                                                      */
/*                                     Last mod:25/05/21*/
/*======================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "header.h"

int isConditionTrue(country* a,char condition1[],char condition2[],char condition3[]);
int isresvalid(char* type1,char* type2,char* type3,country* head);
int weekref(country* headref,char week[]);


/******************************************************************************
* Function name: dataRestriction()
*
* Arguments: country *head - The head of the data list
*            char condition[] - condition by which we should
                                restrict the data list
*
* Return: country* - Pointer to the new head of the list
*
* Side-effects: This will permanently remove elements from the list.
*
* Description: This function will go through the entire list and
*              call the function isConditionTrue to check if the node should
*              be removed or not according to the restriction the user set.
*
*******************************************************************************/
country* dataRestriction(country *head,char condition[]){
    int flag=0;
    char *token=NULL,*token1=NULL, *token2=NULL, aux[24];
    country *newhead=head, *hold=NULL, *previous=NULL;
    //This will separate the condition in 3 strings since we can receive 3 arguments
    token=strtok(condition,",");
    token1=strtok(NULL,",");
    token2=strtok(NULL,",");

    if(isresvalid(token,token1,token2,head)==0){//This function returns 0 if the restriction condition inserted is invalid
        fprintf(stderr,"Invalid type of restriction.\nPlease rerun the program with the option -h for more help\n");
        clearmemory(head);
        return NULL;
    }

    if(strcmp(token,"dates")==0){
        //when using dates restriction, the order of the weeks shouldn't matter. Therefore we make sure they are ordered
        if(strcmp(token1,token2)>0){//Week 1 is greater than 2
            strcpy(aux,token1);
            strcpy(token1,token2);
            strcpy(token2,aux);
        }
    }
    //This loop will go through the entire list
    while(head!=NULL){
        if(isConditionTrue(head,token,token1,token2)==1){//if is true maintain node
            if(flag==0)//Is it the 1st note to respect condition? If yes, store it as the new head
                newhead=head;
            previous=head;
            head=head->next;
            flag=1;
            continue;
            }
        //else free this node
        hold=head->next;
        if(flag!=0)//This means that previous (correspondent to the last valid position) exits
            previous->next=head->next;
        free(head->country_name);//frees node
        free(head);
        head=hold;
    }
    if(flag==0){//If there was no valid position returns a NULL head
        fprintf(stderr,"No data was left after the data treatment\nConsider reviewing criteria used and rerun the program with the option -h for more help\n");
        return NULL;
    }
    return newhead;
}

/****************************************************************************************************
* Function name: isConditionTrue()
*
* Arguments: country *a - country to be analyzed
*            char condition1[] - 1st part of the condition by which we should restrict the data list
*            char condition2[] - 2nd part of the condition by which we should restrict the data list
*            char condition3[] - 3rd part of the condition by which we should restrict the data list
*
* Return: int - 1 if condition is true and node should be maintained 0 if not
*
* Side-effects: none to report
*
* Description: This function will compare the value of the country a and
*              check if it respects or not the condition.
*
****************************************************************************************************/
int isConditionTrue(country* a,char condition1[],char condition2[], char condition3[]){
    int nhabitantes=0, flag=0, flag1=0;
    countryinfo *current=NULL, *hold=NULL,*aux=NULL,*aux2=NULL;
    //Analyze what type of restriction the user wants
    if(strstr(condition1,"min")!=NULL){//Restriction by min
        nhabitantes=(atoi(condition2)*1000);//n will come in the thousands units
        if(a->population>nhabitantes)//a passes the restriction and should be kept
            flag=1;
        else{//If a is lower then the restriction then remove all his secondary list nodes
            current=a->head;//Gets the 1st element of the secondary list
            while(current!=NULL){//Goes through the secondary list
                hold=current->nexti;//Deletes secondary list
                free(current);
                current=hold;
            }
            flag=0;
        }
    }
    else if(strstr(condition1,"max")!=NULL){//Restriction by max
        nhabitantes=(atoi(condition2)*1000);//n will come in the thousands units
        if(a->population<nhabitantes)
            flag=1;
        else{//If a is bigger then the restriction then remove all his secondary list nodes
            current=a->head;//Gets the 1st element of the secondary list
            while(current!=NULL){//Goes through the secondary list
                hold=current->nexti;//Deletes secondary list
                free(current);
                current=hold;
            }
            flag=0;
        }
    }
    else if(strcmp(condition1,"date")==0){//Restriction by date
        current=a->head;//Gets the 1st element of the secondary list
            while(current!=NULL){//Goes through the secondary list
                if(strcmp(current->year_week,condition2)==0){
                    //If the week found corresponds to the one we are looking for
                    if(flag1==0)//1st time the week appears
                        a->head=current;
                    else//2nd time (there can only be 2 weeks, one for cases, one for deaths)
                        a->head->nexti=current;
                    flag1++;
                    current=current->nexti;//Gets next secondary list position
                    continue;
                }
                else{//If the week found doesn't correspond to the one we want, free it
                hold=current->nexti;//Gets next secondary list position
                free(current);
                current=hold;
                }
            }
            if(flag1==0)
                flag=0;//No week corresponding to the one typed was found
            else{
                if(flag1==2)//A week for cases and deaths was found
                    a->head->nexti->nexti=NULL;
                else if(flag1==1)//Only one week was found
                    a->head->nexti=NULL;
                flag=1;
            }
    }
    else if(strstr(condition1,"dates")){//Restriction by dates
        current=a->head;//Gets the 1st element of the secondary lis
            while(current!=NULL){//Goes through the secondary list
                if((strcmp(current->year_week,condition2)>=0)&&(strcmp(current->year_week,condition3)<=0)){
                    //If the week found is contained in the interval between the ones typed by the user
                    if(flag1==0){//1st time a valid week appears
                        a->head=current;
                        aux=current;
                        flag1++;
                    }
                    else{
                        aux->nexti=current;//adds the node found to the next "valid list" position
                        aux=current;
                    }
                    aux2=current->nexti;//Gets next secondary list position
                    current->nexti=NULL;
                    current=aux2;
                    continue;
                }
                else{//If the week found doesn't correspond to the one we want, free it
                hold=current->nexti;//Gets next secondary list position
                free(current);
                current=hold;
                }
            }
            if(flag1==0)
                flag=0;//No week corresponding to the one typed was found
            else{
                flag=1;
            }
    }
return flag;
}

/******************************************************************************
* Function name: isresvalid()
*
* Arguments: char condition1[] - 1st part of the condition by which we should restrict the data list
*            char condition2[] - 2nd part of the condition by which we should restrict the data list
*            char condition3[] - 3rd part of the condition by which we should restrict the data list
*            country *head - head of the list
*
* Return: int - 1 if the restriction typed is valid, 0 if not
*
* Side-effects: none to report
*
* Description: This function will compare for each type of restriction the
*              input the user typed and check if its in the right format.
*              For the date and dates restriction it will also check if the
*              week exists in the data that we stored in list.
*
*******************************************************************************/
int isresvalid(char* type1,char* type2,char* type3,country* head){

    char *ptr=NULL;

    if(strcmp(type1,"min")==0){//restriction is by min
        if((type2==NULL)||(type3!=NULL))//user didn't write any value or value has a space
            return 0;
        strtol(type2,&ptr,10);//ptr will receive any "garbage" left when assigning the string to a int
        if((strcmp(ptr,"")!=0)||(type2==NULL))//true if ptr received "garbage"
            return 0;
        return 1;
    }
    else if(strcmp(type1,"max")==0){//restriction is by max
        if((type2==NULL)||(type3!=NULL))//user didn't write any value or value has a space
            return 0;
        strtol(type2,&ptr,10);//ptr will receive any "garbage" left when assigning the string to a int
        if(strcmp(ptr,"")!=0)//true if ptr received "garbage"
            return 0;
        return 1;
    }
    else if(strncmp(type1,"date",5)==0){//restriction is by date
        if(type3!=NULL)//user typed more then just the week
            return 0;
        if(strlen(type2)!=(strlen("yyyy-ww")))//check if week is the correct format
            return 0;
        if(weekref(head,type2)==0)//check if the week exists in memory
            return 0;
        return 1;
    }
    else if(strncmp(type1,"dates",6)==0){//restriction is by dates
        if((strlen(type2)!=(strlen("yyyy-ww")))||(strlen(type3)!=(strlen("yyyy-ww"))))//check if week is the correct format
            return 0;
        //Because dates uses an interval of weeks it doesn't make sense to check if the bounds appear in data or not
        return 1;
    }
return 0;
}

/******************************************************************************
* Function name: weekref()
*
* Arguments: country* headref - Reference to the head of the list
*            char week[] - week to find in memory
*
* Return: int - 1 if the week was found, 0 if not
*
* Side-effects: none to report
*
* Description: This function will go through the entire list that is on
*              memory and check if the week the user typed for restricting
*              is in fact a valid week or not.
*
*******************************************************************************/
int weekref(country* headref,char week[]){
    int flag=0;
    countryinfo *info=NULL;

    while(headref != NULL){//This loop will go through the entire list
        info = headref->head;//Gets the 1st node of the secondary list
        while(info != NULL){//This loop will go through the entire secondary list
            if(strcmp(info->year_week,week)==0)//Week typed is equal to the one in memory
                flag++;
            info=info->nexti;
        }
        headref=headref->next;
    }
    if(flag==0){//No week was found matching the one typed
        fprintf(stderr,"Invalid week. This week doesn't appear at least 1 time in the data inserted:\n");
        return 0;
    }
    return 1;//At least one matching week was found
}
