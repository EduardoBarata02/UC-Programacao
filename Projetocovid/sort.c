/*======================================================*/
/*Program file: sort.c                                  */
/*Brief: Sort of data list                              */
/*                                                      */
/*Description: This file is responsible for sorting the */
/*             country data list according to a         */
/*             previously defined option.               */
/*                                                      */
/*Dependencies: Struct creation, declaration by         */
/*              commandline.c of: -Input file name      */
/*                                -Reading option       */
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

/*  Credit for the sorting algorithm code goes to the website:
    https://www.geeksforgeeks.org/quicksort-on-singly-linked-list/
    some changes have been made in order to adapt it to our problem*/

country* lastElement(country* current);
int sorttype(country* a, country* b, char type[]);
int isvalid(char *type,country *head);

/******************************************************************************
* Function name: partition()
*
* Arguments: country* head - Head of the list
*            country* Tail - Tail of the list
*            country** newHead - New Head for the list
*            country** newTail - New Tail for the list
*            char type[] - type of sort
*
* Return: country * - Returns the pivot for the list
*
* Side-effects: During partition, both the head and tail of the list might change
                which is updated in the newHead and newEnd variables
*
* Description: This function will partition the list using the tail as the
*              pivot, making a "sub list" to be analyzed
*
*******************************************************************************/
country* partition(country* head, country* Tail ,country** newHead,country** newTail, char type[]){

    country *pivot=Tail,*previous=NULL,*current=head,*tail=pivot, *aux=NULL;

    while(current!=pivot){
        if(sorttype(current,pivot,type)){//Node is greater/lower (depends on sort type) then the pivot
            if((*newHead)==NULL)//The first node that has a value lower then the pivot becomes the new head
                (*newHead)=current;

            previous=current;
            aux=current->next;
        }
        else{//Node is lower/greater then the pivot, according to the type
            //We have to move the node to the end of the list and update the Tail
            if(previous!=NULL)
                previous->next = current->next;
            aux=current->next;
            current->next=NULL;
            tail->next=current;
            tail=current;
        }
        current = aux;
    }
    if((*newHead)==NULL)//Checks if the pivot was the smallest element in the list, if so, it becomes the new head
        (*newHead)=pivot;
    *newTail = tail;

    return pivot;
}

/******************************************************************************
* Function name: quickSortRecur()
*
* Arguments: country* head - Head of the list
*            country* Tail - Tail of the list
*            char type[] - type of sort
*
* Return: country * - Returns the new head after sorting
*
* Side-effects: This will permanently change the order of the list
*
* Description: This function will sort the list, excluding the tail
*
*******************************************************************************/
country* quickSortRecur(country* head,country* tail, char type[]){

    country *newHead=NULL, *newTail=NULL, *pivot=NULL,*aux=NULL;

    //Base condition, when head hits the tail or the head equals to NULL we reached the end
    if((head==NULL)||(head==tail))
        return head;

    //Partitions the list, updating the newHead and the newTail
    pivot=partition(head,tail,&newHead,&newTail,type);
    //If the pivot is equal to the newHead then its the smallest element so there is no need
    //to sort the left part of the partition
    if(newHead!=pivot){
        aux=newHead;
        //sets the node previous the the pivot as NULL
        while(aux->next!=pivot)
            aux=aux->next;
        aux->next=NULL;
        //Recur for the left part of the pivot
        newHead=quickSortRecur(newHead,aux,type);
        //reestablish the link of the list, reputing the pivot in the list
        aux=lastElement(newHead);
        aux->next = pivot;
    }
    //Recur for the right part of the pivot
    pivot->next=quickSortRecur(pivot->next,newTail,type);

    return newHead;
}

/******************************************************************************
* Function name: lastElement()
*
* Arguments: country* current - Head of the list
*
* Return: country * - Returns the tail of the list
*
* Side-effects: None to report
*
* Description: This function will go through the list and return
*              it's last element
*
*******************************************************************************/
country* lastElement(country* current){
    if (current!=NULL){
        while (current->next != NULL)//This loop will go trough the entire list
            current = current->next;
    }
    return current;
}

/******************************************************************************
* Function name: sorttype()
*
* Arguments: country* a - one of the countries to be analyzed
*            country* b - the other countries to be analyzed
*            char type[] - type of sort
*
* Return: int - Boolean result between the comparison of the 2 countries
*
* Side-effects: None to report
*
* Dependency: Sort type should be well defined
*
* Description: This function will compare the 2 values of each country
*              and return the Boolean result of this comparison.
*
*******************************************************************************/
int sorttype(country* a, country* b, char type[]){

    countryinfo *aux=NULL,*hold1=NULL,*hold2=NULL;
    int result1=0,result2=0;

    if(strcmp(type,"alfa,")==0){//alfa sort type
        if((strcmp((a->country_name),(b->country_name)))<0)//Country name a is lower in the alphabet then b
            return 1;
        else
            return 0;
    }
    else if(strcmp(type,"pop,")==0){//pop sort type
         if(a->population==b->population){//for equal populations the 2nd criteria is alfa sort
            if(strcmp(a->country_name,b->country_name)<0)
                return 1;
            else
                return 0;
         }
         else if(a->population>b->population)//Compares a population with b population
         return 1;
         else
         return 0;
    }
    else if(strstr(type,"inf,")){//inf sort type
        aux=a->head;
        while(aux!=NULL){//Goes through the secondary list of the country to find the week we are looking for
            if(strcmp(aux->indicator,"deaths")!=0)
                if(strstr(type,aux->year_week))
                   hold1=aux;
            aux=aux->nexti;
        }
        aux=b->head;
        while(aux!=NULL){//Goes through the secondary list of the country to find the week we are looking for
            if(strcmp(aux->indicator,"deaths")!=0)
                if(strstr(type,aux->year_week))
                   hold2=aux;
            aux=aux->nexti;
        }
        //gets weekly count if the week is available, otherwise we will consider the value of 0
        if(hold1!=NULL)
            result1=hold1->weekly_count;
        if(hold2!=NULL)
            result2=hold2->weekly_count;
        if(result1==result2){//for equal weekly counts the 2nd criteria is alfa sort
            if(strcmp(a->country_name,b->country_name)<0)
                return 1;
            else
                return 0;
         }
        if(result2<result1)
            return 1;
        else
            return 0;
    }
    else if(strstr(type,"dea,")){//dea sort type
        aux=a->head;
        while(aux!=NULL){//Goes through the secondary list of the country to find the week we are looking for
            if(strcmp(aux->indicator,"cases")!=0)
                if(strstr(type,aux->year_week))
                   hold1=aux;
            aux=aux->nexti;
        }
        aux=b->head;
        while(aux!=NULL){//Goes through the secondary list of the country to find the week we are looking for
            if(strcmp(aux->indicator,"cases")!=0)
                if(strstr(type,aux->year_week))
                   hold2=aux;
            aux=aux->nexti;
        }
        //gets weekly count if the week is available, otherwise we will consider the value of 0
        if(hold1!=NULL)
            result1=hold1->weekly_count;
        if(hold2!=NULL)
            result2=hold2->weekly_count;
        if(result1==result2){//for equal weekly counts the 2nd criteria is alfa sort
            if(strcmp(a->country_name,b->country_name)<0)
                return 1;
            else
                return 0;
         }
        if(result2<result1)
            return 1;
        else
            return 0;
    }
    return 0;
}

/******************************************************************************
* Function name: quicksortThis()
*
* Arguments: country* head- Head of the list
*            char type[] - type of sort
*
* Return: country* - Head of the sorted list
*
* Side-effects: None to report
*
* Description: This function will confirm if the sort type chosen is valid,
*              if it is, it will call quickSortRecur and start the sorting
*              process
*
*******************************************************************************/
country* quicksortThis(country *head,char type[]){
    if(isvalid(type,head)==0){
        fprintf(stderr,"Invalid sort type. Please rerun the program with the option -h for more help\n");
        clearmemory(head);
        return NULL;
    }
    head=quickSortRecur(head,lastElement(head),type);
    return head;
}

/******************************************************************************
* Function name: isvalid()
*
* Arguments: char *type - type of sort
*            country* head- Head of the list
*
* Return: int - Boolean value (1- valid, 0- invalid)
*
* Side-effects: None to report
*
* Description: This function will analyze if the sort type chosen is valid
*
*******************************************************************************/
int isvalid(char *type,country *head){

    char aux[24]="\0",*token=NULL;
    int i=0;
    for( ;type[i]!='\0';i++)//copies type to aux variable
        aux[i]=type[i];
    aux[i+1]='\0';

    if(strcmp(type,"alfa,")==0){//sort type is alfa
        return 1;
    }
    else if(strcmp(type,"pop,")==0){//sort type is pop
        return 1;
    }
    else if(strncmp(type,"inf",3)==0){//sort type is inf
        if(strlen(type)!=(strlen("inf,")+8))//user only inserted inf yyyy-ww
            return 0;
        token=strtok(aux,",");
        token=strtok(NULL,",");
        if(token==NULL)
            return 0;
        if(weekref(head,token))//user inserted a week that appears at least once
            return 1;
        return 0;
    }
    else if(strncmp(type,"dea",3)==0){
        if(strlen(type)!=(strlen("dea,")+8))//user only inserted inf yyyy-ww
            return 0;
        token=strtok(aux,",");
        token=strtok(NULL,",");
        if(token==NULL)
            return 0;
        if(weekref(head,token))//user inserted a week that appears at least once
            return 1;
        return 0;
    }
return 0;//None of the above
}




