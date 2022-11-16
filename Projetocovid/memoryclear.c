/*======================================================*/
/*Program file: memoryclear.c                           */
/*Brief: List memory clear                              */
/*                                                      */
/*Description: This file is responsible for clearing    */
/*             the data dynamically allocated on the    */
/*             singly linked nested list.               */
/*                                                      */
/*Dependencies: Receiving the head of the list          */
/*                                                      */
/*Authorship: Eduardo Filipe Braz Barata 99930, IST-MEEC*/
/*          Nuno Filipe Trigo Fernandes 100695, IST-MEEC*/
/*                                                      */
/*                                     Last mod:17/05/21*/
/*======================================================*/
#include <stdio.h>
#include <stdlib.h>

#include "header.h"

/******************************************************************************
* Function name: clearmemory()
*
* Arguments: country *start - Head of the list
*
* Return: void
*
* Side-effects: This will permanently delete the entire list from memory
*
* Description: This function will remove from memory the everything that
*              is in the linked list, including the secondary lists
*
*******************************************************************************/
void clearmemory(country *start){

    country *nextfree = start;
    country *hold = NULL;
    countryinfo *infoDel,*infoHold;

    while(nextfree != NULL){//This loop will go through the entire list
        infoDel = nextfree->head;//Gets the 1st node of the secondary list
        while(infoDel != NULL){//This loop will go through the entire secondary list
            infoHold = infoDel->nexti;//Deletes memory for the secondary list
            free(infoDel);
            infoDel = infoHold;
        }
        hold = nextfree->next;//Deletes memory for the main list
        free(nextfree->country_name);//country_name was dynamically allocated
        free(nextfree);
        nextfree = hold;
    }
}
