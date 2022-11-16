/*======================================================*/
/*Program file: datasel.c                               */
/*Brief: Select data from list                          */
/*                                                      */
/*Description: This file is responsible for selecting   */
/*             data on the list according to a          */
/*             previously defined option.               */
/*                                                      */
/*Dependencies: Struct creation, declaration by         */
/*              commandline.c of: -Selection option     */
/*                                                      */
/*Authorship: Eduardo Filipe Braz Barata 99930, IST-MEEC*/
/*          Nuno Filipe Trigo Fernandes 100695, IST-MEEC*/
/*                                                      */
/*                                     Last mod:18/05/21*/
/*======================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "header.h"

char* selTypeWord(char type[]);
int comparevalues(countryinfo* a, countryinfo* b,char type[]);
void removeNode(countryinfo **previous,countryinfo **current);

/******************************************************************************
* Function name: dataselection()
*
* Arguments: country **Current - Points to the head of the data list
*            char seltype[] - condition by which we should
*                             select the data list
*
* Return: void
*
* Side-effects: This will permanently remove elements from the list.
*
* Description: This function will go through the entire list and
*              call the function comparevalues to check if the node should
*              be removed or not according to the selection the user set.
*
*******************************************************************************/
void dataselection(country **Current, char seltype[]){

    char* selectionword;
    int flag=0;
    countryinfo *hold,*aux=NULL,*previous=NULL,*aux2=NULL, *infoDel,*infoHold;
    country* head =*Current;

    //selectionword will receive the indicator from the secondary list we should totally ignore
    //or NULL in case the user inserted a wrong type of sort
    if((selectionword=selTypeWord(seltype))==NULL){
        fprintf(stderr,"Invalid type of selection.\nPlease rerun the program with the option -h for more help\n");
        clearmemory(head);
        *Current=NULL;
        return;
    }
    //This loop will go through the entire list
    while((*Current)!=NULL){
        previous=NULL;//country info previous is now NULL
        aux=(*Current)->head;//1st position of the secondary list
        hold=aux;
        flag=0;
        while(aux != NULL){//This loop will go through the entire sub list of the main list
            if(strcmp(aux->indicator,selectionword)==0){//==0 means we should ignore it
                //if the indicator is equal to type we should ignore we will clear it
                if(aux==(*Current)->head){//If its the 1st node in the secondary list to be removed
                    (*Current)->head=aux->nexti;//The main list node head will receive the next position
                    infoDel=aux;//Deletes position from memory
                    previous=aux;
                    aux=aux->nexti;
                    free(infoDel);
                    if(flag==0)//There is still no greater value
                        hold=aux;
                    continue;
                }
                //Only gets here if its not the 1st node in the secondary list
                //Node should be removed from memory
                aux2=aux;
                aux=aux2->nexti;
                removeNode(&previous,&aux2);
                if(flag==0)//There is still no greater value
                    hold=aux;
                continue;
            }
            //Only gets here if the previous if is false
            flag=1;//There is one valid node for the type of selection
            if(comparevalues(aux,hold,seltype)==1)//aux is greater then hold
                hold=aux;//hold keeps the greater value
            previous=aux;
            aux=aux->nexti;
        }
        //End of Loop - Should delete all secondary list nodes except the ones that respect the selection
        infoDel = (*Current)->head;
        while(infoDel != NULL){//Loop goes through secondary list
            infoHold = infoDel->nexti;
            if(infoDel==hold){//infoDel should not be deleted in this case
                (*Current)->head=infoDel;//There will only be one value left in the list so we make it the head
                infoDel->nexti=NULL;
                infoDel=infoHold;//next position
                continue;
            }
            free(infoDel);
            infoDel = infoHold;//next position
        }
        (*Current)=(*Current)->next;//next country position
    }

    *Current=head;//Current gets the new head
}

/******************************************************************************
* Function name: selTypeWord()
*
* Arguments: char type[] - type of selection the user selected
*
* Return: char* returns the indicator that should be ignored
*
* Side-effects: None to report
*
* Dependency: Type should be well defined
*
* Description: This function will evaluate the type of selection the user
*              selected and return the string that corresponds to the indicator
*              that should be ignored by the function that calls it.
*              Example: When selection the most "inf" we should ignore the
*              indicator of deaths
*
*******************************************************************************/
char* selTypeWord(char type[]){
    if (strcmp(type,"inf")==0)
        return "deaths";
    else if (strcmp(type,"dea")==0)
        return "cases";
    else if (strcmp(type,"racioinf")==0)
        return "deaths";
    else if (strcmp(type,"raciodea")==0)
        return "cases";
return NULL;
}

/******************************************************************************
* Function name: comparevalues()
*
* Arguments: countryinfo* a - memory position of one country
*            countryinfo* b - memory position of another country
*            char type[] - type of selection the user selected
*
* Return: int - returns the Boolean value of the various conditions
*
* Side-effects: None to report
*
* Dependency: Type should be well defined, list should only have the indicator
*             that should be evaluated
*
* Description: This function will evaluate for country a and b which of them
*              has a greater value according to the type selected
*
*******************************************************************************/
int comparevalues(countryinfo* a, countryinfo* b,char type[]){
    if (strcmp(type,"inf")==0)//for inf we should compare weekly_count
        return ((a->weekly_count)>=(b->weekly_count));
    else if (strcmp(type,"dea")==0)//for dea we should compare weekly_count
        return ((a->weekly_count)>=(b->weekly_count));
    else if (strcmp(type,"racioinf")==0)//for racioinf we should compare rate_14_day
        return ((a->rate_14_day)>=(b->rate_14_day));
    else if (strcmp(type,"raciodea")==0)//for raciodea we should compare rate_14_day
        return ((a->rate_14_day)>=(b->rate_14_day));
return 0;
}

/******************************************************************************
* Function name: removeNode()
*
* Arguments: countryinfo **previous - previous position in the list
*                                     of the current country
*            countryinfo **current - position in the list of  a country
*                                    to be removed
*
* Return: void
*
* Side-effects: This will permanently delete a node from memory
*
* Dependency: previous and current should not be NULL
*
* Description: This function will remove from memory the "current"
*              country and restore the list links
*
*******************************************************************************/
void removeNode(countryinfo **previous,countryinfo **current){
    countryinfo* temp= *current;//saves position to be freed
    (*previous)->nexti=(*current)->nexti;//gets next position and places it next next to the previous, reestablishing the list order
    free(temp);
}
