/*=======================================================*/
/*Program file: commandline.c                            */
/*Brief: Main() Command line options reader              */
/*                                                       */
/*Description: This file is responsible for reading the  */
/*             command line options and calling the      */
/*             respective functions that make it work.   */
/*                                                       */
/*Dependencies: None to report                           */
/*                                                       */
/*Authorship: Eduardo Filipe Braz Barata 99930, IST-MEEC */
/*          Nuno Filipe Trigo Fernandes 100695, IST-MEEC */
/*                                                       */
/*                                      Last mod:23/05/21*/
/*=======================================================*/
#include <getopt.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "header.h"

/******************************************************************************
* Constant name: availableoptions
*
* Arguments: none
*
* Side-effects: none to report
*
* Description: This constant contains the available options for the program
*              when called it will replace the word availableoptions for
*              the string declared.
*
*******************************************************************************/
#define availableoptions "i:o:L:S:D:P:h"

void HelpMessage (char program_name[]);

/******************************************************************************
* Function name: main()
*
* Arguments: int argc - number of strings pointed to by argv[]
*            char *argv[] - argument vector
*
* Return: int - program exit code
*
* Side-effects: none to report
*
* Description: This function reads the arguments typed by the user in the
*              terminal and stores them accordingly in their designated
*              variables. It will then call the respective functions that
*              make the program run like the user expects.
*
*******************************************************************************/
int main(int argc, char*argv[]){
    int opt=0;
    char *inputname=NULL,*outputname=NULL,*read=NULL,sorting[32]="\0",*datasel=NULL,datares[48]="\0";
    short inputflag[6]={0};
    //inputflag[0]=-i, inputflag[1]=-o, inputflag[2]=-L, inputflag[3]=-S, inputflag[4]=-D,, inputflag[5]=-P
    country* head;

    opterr=0;

    while((opt=getopt(argc,argv,availableoptions))!=-1){
        //getopt will return -1 when there are no more options to be processed
        if((opt!='h')&&(opt!='?'))//Excludes options that don't require arguments
            if(optarg[0]=='-'){//option doesn't have an argument next to it
                fprintf(stderr,"Option -%c, requires an argument.\n",(char)opt);
                return -1;
            }
        if((opt=='L')||(opt=='D')||(opt=='i')||(opt=='o')){
            if((argv[optind]!=NULL)&&(argv[optind][0]!='-')){//Analyze for  -L, -D, -i and -o if arguments were inserted
                fprintf(stderr,"Invalid format for option -%c. This option only requires one argument.\n",opt);
                if(inputflag[0]!=0)//Exiting arguments for -i
                    free(inputname);
                if(inputflag[1]!=0)//Existing arguments for -o
                    free(outputname);
                if(inputflag[2]!=0)//Existing arguments for -L
                    free(read);
                if(inputflag[4]!=0)//Existing arguments for -D
                    free(datasel);
                exit(EXIT_FAILURE);
            }
        }
        switch(opt){//Analyze the option inserted
            case 'i'://Option -i (input file name)
                inputname=calloc(1,strlen(optarg)+1);
                strcpy(inputname,optarg);
                inputflag[0]=1;
                break;
            case 'o'://Option -o (output file name)
                outputname=calloc(1,strlen(optarg)+1);
                strcpy(outputname,optarg);
                inputflag[1]=1;
                break;
            case 'L'://Option -L (Type of reading from the file)
                read=calloc(1,strlen(optarg)+1);
                strcpy(read,optarg);
                inputflag[2]=1;
                break;
            case 'S'://Option -S (Type of sort of data)
                optind--;
                //This loop allows to read every argument that follows -S
                for(;optind < argc && *argv[optind] != '-'; optind++){
                    strcat(sorting,argv[optind]);
                    strcat(sorting,",");
                }
                inputflag[3]=1;
                break;
            case 'D'://Option -D (Type of of data selection)
                datasel=calloc(1,strlen(optarg)+1);
                strcpy(datasel,optarg);
                inputflag[4]=1;
                break;
            case 'P'://Option -P (Type of of data restriction)
                optind--;
                //This loop allows to read every argument that follows -P
                for(;optind < argc && *argv[optind] != '-'; optind++){
                    strcat(datares,argv[optind]);
                    strcat(datares,",");
                }
                inputflag[5]=1;
                break;
            case 'h'://Option -h (Help message)
                HelpMessage(argv[0]);
                exit(EXIT_SUCCESS);
            default://'?': unknown option or invalid format (option expected argument)
                fprintf(stderr,"You typed an invalid option/format.\nFor more help please type: %s -h\n", argv[0]);
                if(inputflag[0]!=0)//Exiting arguments for -i
                    free(inputname);
                if(inputflag[1]!=0)//Existing arguments for -o
                    free(outputname);
                if(inputflag[2]!=0)//Existing arguments for -L
                    free(read);
                if(inputflag[4]!=0)//Existing arguments for -D
                    free(datasel);
                exit(EXIT_FAILURE);
        }
    }

    if(inputflag[0]==0||inputflag[1]==0){//input flag 0 and 1 correspond to -i -o, if they are 0 no input/output file was declared
        fprintf(stderr,"No input/output file was declared. Please provide them with -i and -o options.\nFor more info type %s -h\n",argv[0]);
        if(inputflag[0]!=0)//input file was declared
            free(inputname);
        if(inputflag[1]!=0)//output file was declared
            free(outputname);
        exit(EXIT_FAILURE);
    }

    if(inputflag[2]==0){//Set default to read if no read type was declared
        read=calloc(1,5);
        strcpy(read,"all");
    }

    if(strstr(inputname,".dat")!=NULL){
        if((inputflag[2]!=0)||(inputflag[3]!=0)||(inputflag[4]!=0)||(inputflag[5]!=0)){
            fprintf(stderr,"When reading from a Binary file, no option other then -i and -o should be specified.\nFor more info type %s -h\n",argv[0]);
            free(inputname);
            free(outputname);
            free(read);
            if(inputflag[4]!=0)
                free(datasel);
            exit(EXIT_FAILURE);
        }
    }

    if(inputflag[3]==0){//Set default to sorting if no sort type was declared
        strcpy(sorting,"alfa,");
    }
    if((head=readtype(inputname,read))==NULL){//Reads data from the file accordingly
    //If the read type inserted by the user is incorrect or if the input file
    //has an invalid line then Head will be NULL
        free(inputname);
        free(outputname);
        free(read);
        if(inputflag[4]!=0)
            free(datasel);
        exit(EXIT_FAILURE);
    }

    if(inputflag[4]!=0)//If user asks for data selection
        dataselection(&head, datasel);
    if(head==NULL)//Head==NULL if no data is found with such criteria
        fprintf(stderr,"No data with such criteria\n");
    else if(inputflag[5]!=0)//If user asks for data restriction
        head=dataRestriction(head,datares);
    if(head!=NULL){//If there is a list with data we proceed to sort it, print it and clear it from memory
        head=quicksortThis(head,sorting);
        printselection(head,1,outputname);
        clearmemory(head);
    }
    free(inputname);
    free(outputname);
    free(read);
    if(inputflag[4!=0])//If user typed a data selection option
        free(datasel);
    return(EXIT_SUCCESS);
}

/******************************************************************************
* Function name: HelpMessage()
*
* Arguments: char program_name[] - string with the program name
*
* Return: void
*
* Side-effects: none to report
*
* Description: This function will print to the terminal a help message
*              for the user with a brief of the program
*
*******************************************************************************/
void HelpMessage (char program_name[]){
printf("You asked for help. Here is a brief of the program.\n\n\
    \
    Usage: %s -i [file name] -o [file name] [-more arguments]\n\n\
    Arguments:\n\
    -h --- Show help message\n\
    -i --- Input file\n\
    -i --- Output file\n\
    -L --- Data read\n\
    -S --- Data sorting\n\
    -D --- Data selection\n\
    -P --- Data restriction\n\
    \
    \n\
    Usage:\n\
    -i [i] --- i = input file name\n\
    -o [o] --- o = output file name\n\
    -L [l] --- l = data read options (\"all\" \"continent_name\")\n\
    -S [s] --- s = data sorting options (\"alfa\" \"pop\" \"inf yyyy-ww\" \"dea yyyy-ww\")\n\
    -D [d] --- d = data selection options (\"inf\" \"dea\" \"racioinf\" \"raciodea\")\n\
    -P [p] --- p = data restriction options (\"min n\" \"max n\" \"date yyyy-ww\" \"dates yyyy-ww yyyy-ww\")\
    \n\
    \n",program_name);
}
