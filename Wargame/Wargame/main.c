/*****************************************************************************
 * Projeto intermédio - Jogo de batalha Naval
 *
 * Autor 1: Eduardo Filipe Braz Barata, nº 99930
 * Autor 2: Nuno Filipe Trigo Fernandes, nº 100695
 * Unidade curricular: Programação, MEEC2006, 1º ano, 1º semestre
 * Data: 22/04/2021
 *
 * Exemplos de uso:
 * ./wargame -j 0 -p 2 -1 1 -2 1 -3 1 -4 1
 * ./wargame -h
 * ./wargame -t 15x24 -j 2 -d 1 -1 3 -2 3 -3 3 -4 3 -5 2 -6 2 -7 2 -8 2
 *
 *****************************************************************************/
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

/**
 * Macro
 * errormessage
 *
 * \brief Imprime para o stderr, uma mensagem de erro e indica como o utilizador poderá obter mais ajuda
 *
 */
#define errormessage (fprintf(stderr, "Formato inválido.\nPara mais ajuda digite: %s -h\n", argv[0]))

/**
 * Macro
 * optarghasvalue
 *
 * \param value ->corresponde ao 1º valor do optarg
 * Caso o value seja um '-', ou seja, não possui optarg imprime a mensagem de erro, caso contrário devolve 1
 *
 */
#define optarghasvalue(value) (value=='-')? errormessage:1
//Dimensão máxima do tabuleiro
#define tabuleirolinhamax 15
#define tabuleriocolunamax 24

void impressaotabuleiro(int linhas, int colunas);
void impressaoinformacoes(int linhas,int colunas);
int inicializatabuleiro(int linhas, int colunas);
void checksforinvalidspace(int linha, int coluna,int linhamax, int colunamax);
int pecas(int linha, int coluna, int globalid, int linhamax, int colunamax);
int checksifhasvalue(int offsetlinha, int offsetcoluna);
int colocapecas(int posicaolivre[],int linha, int coluna, int linhamax, int colunamax, int num);
void modo_p1 (int linhas, int colunas);
int modo_p2();
int modo_p2_aux(int linhas, int colunas,int num_peca[],int randomseed);
void peca_para_globalid(int peca,int *inicio, int *fim);
int restricao3();
void modo_d1(int linhas, int colunas);
void modo_d2(int linhas, int colunas);
int sequencia_disparo(int l, int c, int *disparos, int linhas);
void modo_d3(int linhas, int colunas);
void helpmessage(char *name);
int modo_j0(int linhas, int colunas, int posicionamento);
int modo_j1(int linhas, int colunas, int posicionamento);
int modo_j2(int linhas, int colunas, int disparo);
char validinput (int linha, int coluna,int linhamax);

struct{
char tabuleiro[15][24];
int tabuleirolog[15][24];
int num_pecas[8];
}s;

int main(int argc, char *argv[])
{
    inicializatabuleiro(tabuleirolinhamax,tabuleriocolunamax);
    int opt;
    char *valor=NULL;
    int modojogo=-1,mododisparo=0,linhas=0,colunas=0,modoposicionamento=0;
    //Faz um loop enquanto opt é diferente de -1 (Sem opções ou uma opção inválida)
    while ((opt = getopt(argc, argv, "ht:j:p:d:1:2:3:4:5:6:7:8:")) != -1) {
        if((opt!='h')&&(opt!='?'))
            if(optarghasvalue(optarg[0])!=1) //confirma se foi inserido um valor asseguir à opção
               return -1;
        switch (opt) {
        case 't':
            valor=optarg;
            linhas=atoi(optarg);
            char delim[] = "x";
            strtok(valor, delim); //separa linhasxcolunas em duas strings para serem lidos valores
            valor= strtok(NULL, delim);
            if(valor==NULL){
                errormessage;
                return -1;
            }
            colunas=atoi(valor);
            if((colunas<9)||(linhas<9)||((colunas%3)!=0)||((linhas%3)!=0)||(linhas>15)||(colunas>24)){
                errormessage;
                return -1;
            }
            break;
        case 'h':
            helpmessage(argv[0]);
            return 1;
        case 'j':
            modojogo=atoi(optarg);
            if((modojogo<0)||(modojogo>2)){
                errormessage;
                return -1;
            }
            break;
        case 'p':
            modoposicionamento=atoi(optarg);
            if((modoposicionamento<1)||(modoposicionamento>2)){
                errormessage;
                return -1;
            }
            break;
        case 'd':
            mododisparo=atoi(optarg);
            if((mododisparo<1)||(mododisparo>3)){
                errormessage;
                return -1;
            }
            break;
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
            s.num_pecas[opt-'0'-1]=atoi(optarg);//converte a opt em inteiro para ir para a posição certa do vetor
            if(s.num_pecas[opt-'0'-1]==0){
                errormessage;
                return -1;
            }
            break;
        default:
            fprintf(stderr, "Para mais ajuda digite: %s -h\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }
    if(linhas==0)
        linhas=colunas=9;
    if(modojogo==-1)
        modojogo=0;
    if(modoposicionamento==0)
        modoposicionamento=1;
    switch(modojogo){//Faz-se o caminho para o modo de jogo pretendido
        case 0:
            if(mododisparo!=0){
                errormessage;
                return -1;
            }
            if(modo_j0(linhas, colunas, modoposicionamento)==0){//Caso falhe algo
                printf("-1\n");
                return -1;}
            impressaoinformacoes(linhas, colunas);
            impressaotabuleiro(linhas, colunas);
            break;
        case 1:
            if(mododisparo!=0){
                errormessage;
                return -1;}
            if(modo_j1(linhas,colunas,modoposicionamento)==0){//Caso falhe algo
                printf("-1\n");
                return -1;
            }
            break;
        case 2:
            if(mododisparo==0)
                mododisparo=1;
            if(modo_j2(linhas,colunas,mododisparo)==0){//Caso falhe algo
                printf("-1\n");
                return -1;
            }
            break;

    }
return 0;
}

/** \brief Função que realiza a impressão da mensagem de ajuda para o utilizador.
 *
 * \param name char* -> Nome do programa
 * \return void
 *
 */
void helpmessage(char *name){
    printf("\nUtilização: %s -[opção] [valor (se aplicável)]\n", name);
    printf("\nOpções:\n-h \t Mostra-lhe esta mensagem.\n"
           "-t \t Dimensão do tabuleiro. \n\t Deve ser escrito o valor da seguinte forma: linhaxcoluna.\n"
           "-j \t Modo de jogo. \n\t Deve ser escrito o valor [0 a 2] de seguida.\n"
           "-p \t Modo de posicionamento das peças pelo computador.\n\t Deve ser escrito o valor [1 ou 2] de seguida.\n"
           "-d \t Modo de disparo pelo computador.\n\t Deve ser escrito o valor [1 a 3] de seguida.\n"
           "-1 \t Quantidade de peças tipo 1. (mínimo de 1).\n"
           "-2 \t Quantidade de peças tipo 2.\n"
           "-3 \t Quantidade de peças tipo 3.\n"
           "-4 \t Quantidade de peças tipo 4.\n"
           "-5 \t Quantidade de peças tipo 5.\n"
           "-6 \t Quantidade de peças tipo 6.\n"
           "-7 \t Quantidade de peças tipo 7.\n"
           "-8 \t Quantidade de peças tipo 8.\n\t Nos casos de -1 a -8, escreva a quantidade de peças pretendida.\n"
           );
}

/** \brief Função que inicializa o tabuleiro e do tabuleiro lógico com "-" e -2, respetivamente
 *  \brief Inicializa, também, o número de peças de cada tipo a 0
 *
 * \param linhas int
 * \param colunas int -> Dimensões do tabuleiro do utilizador
 * \return int -> retoma 1 caso tudo tenha corrido conforme esperado
 *
 */
int inicializatabuleiro(int linhas, int colunas){
    int i,count;
    for(i=0; i<linhas; i++)
        for(count=0; count<colunas; count++){
            s.tabuleiro[i][count]='-';
            s.tabuleirolog[i][count]=-2;
        }
    for(i=0;i<8;i++)
        s.num_pecas[i]=0;
return 1;
}

/** \brief Função que é responsável pela impressão do tabuleiro
 *
 * \param linhas int
 * \param colunas int -> Dimensões do tabuleiro do utilizador
 * \return void
 *
 */
void impressaotabuleiro(int linhas, int colunas){
    int i,count, mask;
    char letra ='A';
    mask=linhas;
    //impressão por linha juntamente com o seu número (reserva 2 espaços de impressão)
    for(i=0; i<linhas; i++){
        printf("%2d",mask);
        for(count=0; count<colunas; count++)
            printf(" %c",s.tabuleiro[i][count]);
    printf("\n");
    mask--;
    }
    //impressão das letras de cada coluna
    printf("   ");
    for(i=0;i<colunas;i++){
        printf("%c ", letra);
        letra++;
    }
    printf("\n");
}

/** \brief impressão da informação da dimensão do tabuleiro e da quantidade de cada peça
 *
 * \param linhas int
 * \param colunas int -> Dimensões do tabuleiro do utilizador
 * \return void
 *
 */
void impressaoinformacoes(int linhas,int colunas){
    int i;
    printf("%dx%d ",linhas,colunas);
    for(i=0;i<8;i++)
        printf("%d ",s.num_pecas[i]);
    printf("\n");
}

/** \brief Função que contem a informação das peças do jogo
 *
 * \param linha int
 * \param coluna int -> Cada linha e coluna indica a matriz do tabuleiro em que estamos
 *        Ex: A 1ª matriz possui estes valores a 0 0. já a segunda a 0 3...
 * \param globalid int -> Id com o qual identificamos a peça
 * \param linhamax int
 * \param colunamax int -> Dimensões do tabuleiro do utilizador
 * \return int -> Retoma 1 caso não consiga colocar a peça e 0 caso consiga
 *
 */
int pecas(int linha, int coluna, int globalid, int linhamax, int colunamax){
    /*Com o encadeamento de if's pretende-se diminuir o tempo que demora a achar a informação da peça,
    ao achar primeiro o seu número e só depois a sua variante*/
    short orientacao=0;
    int masklinha= linha;
    int maskcoluna= coluna;
    int posicaolivre[5]={10,10,10,10,10};
    if(globalid<10){
        //Todas as peças do tipo 1 são iguais, apenas têm um desvio em linha(s) e/ou coluna(s) da peça globalid=1
        switch(globalid){
            case 2:
                coluna+=1;
                break;
            case 3:
                coluna+=2;
                break;
            case 4:
                linha+=1;
                break;
            case 5:
                linha+=1;
                coluna+=1;
                break;
            case 6:
                linha+=1;
                coluna+=2;
                break;
            case 7:
                linha+=2;
                break;
            case 8:
                linha+=2;
                coluna+=1;
                break;
            case 9:
                linha+=2;
                coluna+=2;
                break;
            }
            if(s.tabuleirolog[linha][coluna]==0)//Local inválido para colocar a peça
                return 1;
            else{//coloca a peça no tabuleiro
            s.tabuleiro[linha][coluna]='1';
            s.tabuleirolog[linha][coluna]=1;
            checksforinvalidspace(masklinha,maskcoluna,linhamax,colunamax);
            }
    }

    else if (globalid<22){
        //As peças do tipo 2 seguem a mesma lógica das do tipo 1 mas é necessário especificar se estão na horizontal ou vertical
        switch(globalid){
            case 10:
                orientacao=1;
                break;
            case 11:
                orientacao=1;
                coluna+=1;
                break;
            case 12:
                orientacao=1;
                linha+=1;
                break;
            case 13:
                orientacao=1;
                linha+=1;
                coluna+=1;
                break;
            case 14:
                orientacao=1;
                linha+=2;
                break;
            case 15:
                orientacao=1;
                linha+=2;
                coluna+=1;
                break;
            case 16:
                orientacao=2;
                break;
            case 17:
                orientacao=2;
                linha+=1;
                break;
            case 18:
                orientacao=2;
                coluna+=1;
                break;
            case 19:
                orientacao=2;
                linha+=1;
                coluna+=1;
                break;
            case 20:
                orientacao=2;
                coluna+=2;
                break;
            case 21:
                orientacao=2;
                linha+=1;
                coluna+=2;
                break;
        }
        if(orientacao==1){//horizontal
            if((s.tabuleirolog[linha][coluna]==0) || (s.tabuleirolog[linha][coluna+1]==0))
                return 1;//Local inválido para colocar a peça
            else{//coloca a peça no tabuleiro
            s.tabuleiro[linha][coluna]=s.tabuleiro[linha][coluna+1]='2';
            s.tabuleirolog[linha][coluna]=s.tabuleirolog[linha][coluna+1]=2;
            }
        }
        else {//vertical
            if((s.tabuleirolog[linha][coluna]==0) || (s.tabuleirolog[linha+1][coluna]==0))
                return 1;//Local inválido para colocar a peça
            else{//coloca a peça no tabuleiro
            s.tabuleiro[linha][coluna]=s.tabuleiro[linha+1][coluna]='2';
            s.tabuleirolog[linha][coluna]=s.tabuleirolog[linha+1][coluna]=2;
            }
        }
        checksforinvalidspace(masklinha,maskcoluna,linhamax,colunamax);
    }

    else if (globalid<28){
        //As peças do tipo 3 seguem a mesma lógica das peças tipo 2
        switch(globalid){
            case 22:
                orientacao=1;
                break;
            case 23:
                orientacao=1;
                linha+=1;
                break;
            case 24:
                orientacao=1;
                linha+=2;
                break;
            case 25:
                orientacao=2;
                break;
            case 26:
                orientacao=2;
                coluna+=1;
                break;
            case 27:
                orientacao=2;
                coluna+=2;
                break;
        }
        if(orientacao==1){//horizontal
            if((s.tabuleirolog[linha][coluna]==0) || (s.tabuleirolog[linha][coluna+1]==0) || (s.tabuleirolog[linha][coluna+2]==0))
                return 1;//Local inválido para colocar a peça
            else{//coloca a peça no tabuleiro
            s.tabuleiro[linha][coluna]=s.tabuleiro[linha][coluna+1]=s.tabuleiro[linha][coluna+2]='3';
            s.tabuleirolog[linha][coluna]=s.tabuleirolog[linha][coluna+1]=s.tabuleirolog[linha][coluna+2]=3;
            }
        }
        else {//vertical
            if((s.tabuleirolog[linha][coluna]==0) || (s.tabuleirolog[linha+1][coluna]==0) || (s.tabuleirolog[linha+2][coluna]==0))
                return 1;//Local inválido para colocar a peça
            else{//coloca a peça no tabuleiro
            s.tabuleiro[linha][coluna]=s.tabuleiro[linha+1][coluna]=s.tabuleiro[linha+2][coluna]='3';
            s.tabuleirolog[linha][coluna]=s.tabuleirolog[linha+1][coluna]=s.tabuleirolog[linha+2][coluna]=3;
            }
        }
        checksforinvalidspace(masklinha,maskcoluna,linhamax,colunamax);
    }
    //A partir daquí as peças são identificadas com as posições que têm livres e serão colocadas através
    //da função colocapecas
    else if (globalid<32){
        switch(globalid){
            case 28:
                posicaolivre[0]=2;
                posicaolivre[1]=5;
                posicaolivre[2]=6;
                posicaolivre[3]=7;
                posicaolivre[4]=8;
                break;
            case 29:
                posicaolivre[0]=0;
                posicaolivre[1]=3;
                posicaolivre[2]=6;
                posicaolivre[3]=7;
                posicaolivre[4]=8;
                break;
            case 30:
                posicaolivre[0]=0;
                posicaolivre[1]=1;
                posicaolivre[2]=2;
                posicaolivre[3]=5;
                posicaolivre[4]=8;
                break;
            case 31:
                posicaolivre[0]=0;
                posicaolivre[1]=1;
                posicaolivre[2]=2;
                posicaolivre[3]=3;
                posicaolivre[4]=6;
        }
        if(colocapecas(posicaolivre, linha, coluna, linhamax, colunamax, 4)==0)
            return 1;//Local inválido para colocar a peça
    }

    else if (globalid<36){
        switch(globalid){
            case 32:
                posicaolivre[0]= 3;
                posicaolivre[1]= 5;
                posicaolivre[2]= 6;
                posicaolivre[3]= 8;
                break;
            case 33:
                posicaolivre[0]= 1;
                posicaolivre[1]= 2;
                posicaolivre[2]= 7;
                posicaolivre[3]= 8;
                break;
            case 34:
                posicaolivre[0]= 0;
                posicaolivre[1]= 2;
                posicaolivre[2]= 3;
                posicaolivre[3]= 5;
                break;
            case 35:
                posicaolivre[0]= 0;
                posicaolivre[1]= 1;
                posicaolivre[2]= 6;
                posicaolivre[3]= 7;
                break;
        }
        if(colocapecas(posicaolivre, linha, coluna, linhamax, colunamax, 5)==0)
            return 1;//Local inválido para colocar a peça
    }

    else if (globalid<40){
        switch(globalid){
            case 36:
                posicaolivre[0]= 0;
                posicaolivre[1]= 2;
                posicaolivre[2]= 4;
                break;
            case 37:
                posicaolivre[0]= 0;
                posicaolivre[1]= 4;
                posicaolivre[2]= 6;
                break;
            case 38:
                posicaolivre[0]= 4;
                posicaolivre[1]= 6;
                posicaolivre[2]= 8;
                break;
            case 39:
                posicaolivre[0]= 2;
                posicaolivre[1]= 4;
                posicaolivre[2]= 8;
                break;
        }
        if(colocapecas(posicaolivre, linha, coluna, linhamax, colunamax, 6)==0)
            return 1;//Local inválido para colocar a peça
    }

    else if (globalid<42){
        switch(globalid){
            case 40:
                posicaolivre[0]= 1;
                posicaolivre[1]= 7;
                break;
            case 41:
                posicaolivre[0]= 3;
                posicaolivre[1]= 5;
                break;//Local inválido para colocar a peça
        }
        if(colocapecas(posicaolivre, linha, coluna, linhamax, colunamax, 7)==0)
            return 1;
    }

    else if (globalid==42){
        posicaolivre[0]=4;
        if(colocapecas(posicaolivre, linha, coluna, linhamax, colunamax, 8)==0)
            return 1;//Local inválido para colocar a peça
    }
return 0;
}


/** \brief Função que recebe informações da função "pecas" e atribui no tabuleiro
 *  \brief a peça correta ou, em caso de impossibilidade, devolve o valor 0
 *
 * \param posicaolivre[] int -> vetor com as posições livres da peça que pretendemos colocar
 * \param linha int
 * \param coluna int -> Informação da matriz do tabuleiro a que estamos a referir
 * \param linhamax int
 * \param colunamax int -> Dimensões do tabuleiro do utilizador
 * \param num int -> Indica o tipo de peça
 * \return int -> retoma 1 caso consiga colocar a peça ou 0 caso não consiga
 *
 */
int colocapecas(int posicaolivre[],int linha, int coluna, int linhamax, int colunamax, int num){
    int flag=0, temp=0, i, count;
    //Primeiro verifica se pode colocar as peças nas posições pretendidas
    for(i=0;i<3;i++){
        for(count=0;count<3;count++, temp++){
            if(posicaolivre[flag]==temp){
                flag++;
            }
            else{
                if(s.tabuleirolog[linha+i][coluna+count]==0)
                    return 0;
            }
        }
    }
    flag=0, temp=0;
    //Caso tenha analisado ser possivel colocar as peças, coloca-as
    for(i=0;i<3;i++){
        for(count=0;count<3;count++, temp++){
            if(posicaolivre[flag]==temp){
                flag++;
            }
            else{
                s.tabuleiro[linha+i][coluna+count]=num+48;
                s.tabuleirolog[linha+i][coluna+count]=num;
            }
        }
    }
    checksforinvalidspace(linha, coluna, linhamax, colunamax);
return 1;
}


/** \brief Função com um protocolo que procura à volta da matriz onde a peça foi colocada
 *  \brief se pode existir ou não outra peça nesse local (colocando zeros onde não podem ser colocadas)
 *
 * \param linha int
 * \param coluna int -> Informação da matriz a que estamos a referir
 * \param linhamax int
 * \param colunamax int -> Dimensões do tabuleiro do utilizador
 * \return void
 *
 */
void checksforinvalidspace(int linha, int coluna,int linhamax, int colunamax){
    //Deverá ter-se em considereção a analise de uma matriz "5x5" imaginária em volta da matriz do tabuleiro
    //1ª linha 1ª coluna
    if (checksifhasvalue(linha,coluna)==1){
        if (linha!=0)
            s.tabuleirolog[linha-1][coluna]=s.tabuleirolog[linha-1][coluna+1]=0;
        if  (coluna!=0){
            if (linha!=0)
                s.tabuleirolog[linha-1][coluna-1]=0;
            s.tabuleirolog[linha][coluna-1]=s.tabuleirolog[linha+1][coluna-1]=0;
        }
    }
    //1º linha 2º coluna
    if(checksifhasvalue(linha,coluna+1)==1)
        if(linha!=0)
            s.tabuleirolog[linha-1][coluna]=s.tabuleirolog[linha-1][coluna+1]=s.tabuleirolog[linha-1][coluna+2]=0;
    //1º linha 3º coluna
    if(checksifhasvalue(linha, coluna+2)==1){
        if (linha!=0)
            s.tabuleirolog[linha-1][coluna+2]=s.tabuleirolog[linha-1][coluna+1]=0;
        if  ((coluna+2)!=colunamax){
            if (linha!=0)
                s.tabuleirolog[linha-1][coluna+3]=0;
            s.tabuleirolog[linha][coluna+3]=s.tabuleirolog[linha+1][coluna+3]=0;
        }
    }
    //2º linha 1º coluna
    if(checksifhasvalue(linha+1,coluna)==1)
        if(coluna!=0)
            s.tabuleirolog[linha][coluna-1]=s.tabuleirolog[linha+1][coluna-1]=s.tabuleirolog[linha+2][coluna-1]=0;
    //2º linha 3º coluna
    if(checksifhasvalue(linha+1,coluna+2)==1)
        if((coluna+2)!=0)
            s.tabuleirolog[linha][coluna+3]=s.tabuleirolog[linha+1][coluna+3]=s.tabuleirolog[linha+2][coluna+3]=0;
    //3º linha 1º coluna
    if(checksifhasvalue(linha+2, coluna)==1){
        if ((linha+2)!=linhamax)
            s.tabuleirolog[linha+3][coluna]=s.tabuleirolog[linha+3][coluna+1]=0;
        if  (coluna!=0){
            if ((linha+2)!=linhamax)
                s.tabuleirolog[linha+3][coluna-1]=0;
            s.tabuleirolog[linha+2][coluna-1]=s.tabuleirolog[linha+1][coluna-1]=0;
        }
    }
    //3º linha 2º coluna
    if(checksifhasvalue(linha+2,coluna+1)==1)
        if((linha+2)!=linhamax)
            s.tabuleirolog[linha+3][coluna]=s.tabuleirolog[linha+3][coluna+1]=s.tabuleirolog[linha+3][coluna+2]=0;
    //3º linha 3º coluna
    if(checksifhasvalue(linha+2, coluna+2)==1){
        if ((linha+2)!=linhamax)
            s.tabuleirolog[linha+3][coluna+2]=s.tabuleirolog[linha+3][coluna+1]=0;
        if  ((coluna+2)!=colunamax){
            if ((linha+2)!=linhamax)
                s.tabuleirolog[linha+3][coluna+3]=0;
            s.tabuleirolog[linha+2][coluna+3]=s.tabuleirolog[linha+1][coluna+3]=0;
        }
    }
}

/** \brief Função que confirma se o local dado pelos argumentos tem alguma peça ou é inválido (pela restrição 1)
 *
 * \param offsetlinha int
 * \param offsetcoluna int
 * \return int -> retoma 1 caso tenha peça ou 0 caso não tenha
 *
 */
int checksifhasvalue(int offsetlinha, int offsetcoluna){
    if((s.tabuleirolog[offsetlinha][offsetcoluna]!=0)&& (s.tabuleirolog[offsetlinha][offsetcoluna]!=-2))
        return 1;
    else
        return 0;
}

/** \brief Função que coloca as peças segundo o modo 1
 *
 * \param linhas int
 * \param colunas int -> Dimensões do tabuleiro do utilizador
 * \return void
 *
 */
void modo_p1 (int linhas, int colunas){
    int i, count, randomnumber=0, flag=0;
    //gera uma seed aleatória
    srand((unsigned)time(NULL));
    for(i=0;i<linhas;i+=3)
        for(count=0;count<colunas;count+=3){
            //gera um número aleatório entre 0 e 8, se for 0 coloca uma matriz vazia, caso contrário vai escolher uma peça
            randomnumber=(rand()%9);
            if(randomnumber!=0){
                //gera um id de todas as peças disponiveis (1-42)
                randomnumber=(1+(rand()%42));
                //Caso não seja possivel colocar a peça, verifica se é a 3 vez que isso acontece, coloca a peça de globalid 5
                if(pecas(i,count,randomnumber,linhas,colunas)==1){
                    flag++;
                    if(flag==3){
                        pecas(i,count,5,linhas,colunas);
                    }
                    else
                        count-=3;
                }
                else
                    flag=0;
            }
        }
    //Associa ao vetor s.numpecas o número de peças de cada tipo colocadas
    for(i=0;i<linhas;i++)
        for(count=0;count<colunas;count++)
            if((s.tabuleirolog[i][count]!=0)&&(s.tabuleirolog[i][count]!=-2))
                s.num_pecas[(s.tabuleirolog[i][count]-1)]++;
    for(i=0;i<8;i++)
        s.num_pecas[i]/=(i+1);//Cada tipo de peça possui x peças dadas por: o número total de ocorrências no tabuleiro a dividir pelo seu número
}

/** \brief Função que coloca as peças segundo o modo 2
 *
 * \param linhas int
 * \param colunas int -> Dimensões do tabuleiro do utilizador
 * \return int -> 1 caso consiga gerar o tabuleiro, 0 caso contrário
 *
 */
int modo_p2 (int linhas, int colunas){
    int maskvector[8];
    int i=0, flag=0, temp=0, nummax=0, random=0;
    //Acha o número max de peças (restrição 4) - note-se que apenas nos interessa o arredondamento por defeito da divisão
    nummax=(((linhas*colunas)/9)/2);
    //Verifica-se se o número máximo de peças subtraindo a quantidade de cada peça é inferior a 0 (sendo, nesse caso, impossivel (restrição 4))
    for(i=0;i<8;i++)
        nummax=nummax-s.num_pecas[i];
    if(nummax<0){
        printf("*Número de peças impossivel\n");
        return 0;
    }
    //Verifica se a restrição 3 é cumprida
    if(restricao3()==0){
        printf("*Número de cada peça incompatível\n");
        return 0;
    }
    //Salvaguarda do vetor s.numpecas
    for(i=0;i<8;i++)
        maskvector[i]=s.num_pecas[i];
    srand((unsigned)time(NULL));
    do{
    random=rand();
    inicializatabuleiro(tabuleirolinhamax,tabuleriocolunamax);
    //retoma da salvaguarda
    for(i=0;i<8;i++)
        s.num_pecas[i]=maskvector[i];
    temp=(modo_p2_aux(linhas,colunas,s.num_pecas,random));
    if(temp==0) //caso não tenha conseguido gerar o tabuleiro
        flag++;
    if(flag==1000) //caso seja a 1000 vez que não consegue gerar o tabuleiro
        return 0;
    }
    while(temp==0);
    //retoma da salvaguarda
    for(i=0;i<8;i++)
        s.num_pecas[i]=maskvector[i];
return 1;
}

/** \brief Função auxiliar do modo p2
 *
 * \param linhas int
 * \param colunas int -> Dimensões do tabuleiro do utilizador
 * \param num_peca[] int -> nº de cada tipo de peça
 * \param randomseed int -> chave para gerar uma seed aleatória sempre que entra no programa
 * \return int -> retoma 0 sempre que falha
 *
 */
int modo_p2_aux(int linhas, int colunas,int num_peca[], int randomseed){
    int i, count,mask=0,temp=0, count1=0, flag=0,max=0, pecautilizada[8]={0};
    for(i=0;i<8;i++)
        mask+=num_peca[i];
    mask=(linhas*colunas/9-mask);
    num_peca[8]=mask; //nº de matrizes vazias
    srand(randomseed);
    fflush(stdin);
    for(i=0;i<linhas;i=i+3){
        for(count=0;count<colunas;count=count+3){
            do{
                temp=((rand()%9));
            }
            while((num_peca[temp]==0)||(pecautilizada[temp]==1)); //gera uma matriz vazia (temp=8) ou um tipo de peça das ainda disponiveis
            if(temp!=8){
                peca_para_globalid(temp,&count1,&max);
                mask=(rand()%(max-count1))+count1;//gera um número aleatório de entre os possiveis do intervalo do global id
            }
            if(temp!=8){
                if(pecas(i,count,mask,linhas,colunas)==1){
                    for( ;count1<max;count1++)//tenta colocar as peças do tipo pretendido seguindo a ordem do anexo
                        if(pecas(i,count,count1,linhas,colunas)==0)
                            break;
                    if(count1==max){//Se esgotou as tentativas da peça
                    pecautilizada[temp]++;
                    flag++;
                    count-=3;
                    }
                    else{//Consegue colocar a peça
                        num_peca[temp]--;//decrementa a possibilidade de colocar esse tipo de peça
                        for(count1=0;count1<8;count1++)
                            pecautilizada[count1]=0;//limpa o vetor auxilio de peças utilizadas das tentativas falhadas
                    }
                    if(flag==8)
                        return 0;
                }
                else{//Consegue colocar a peça
                    num_peca[temp]--;//decrementa a possibilidade de colocar esse tipo de peça
                    for(count1=0;count1<8;count1++)
                            pecautilizada[count1]=0;//limpa o vetor auxilio de peças utilizadas das tentativas falhadas
                }
            }
            else{//Consegue colocar a peça
                num_peca[8]--;//decrementa a possibilidade de colocar este tipo de peça
                for(count1=0;count1<8;count1++)
                    pecautilizada[count1]=0;//limpa o vetor auxilio de peças utilizadas das tentativas falhadas
            }
        }
    }
return 1;
}

/** \brief Função que indica o intervalo de valores em que o globalid de um tipo de peça se encontra
 *
 * \param peca int -> tipo de peça
 * \param inicio int* -> Começo do intervalo de valores
 * \param fim int* -> Fim do intervalo de valores
 * \return void
 *
 */
void peca_para_globalid(int peca,int *inicio, int *fim){
    switch(peca){
        case 0:
            *inicio=1;
            *fim=10;
            break;
        case 1:
            *inicio=10;
            *fim=22;
            break;
        case 2:
            *inicio=22;
            *fim=28;
            break;
        case 3:
            *inicio=28;
            *fim=32;
            break;
        case 4:
            *inicio=32;
            *fim=36;
            break;
        case 5:
            *inicio=36;
            *fim=40;
            break;
        case 6:
            *inicio=40;
            *fim=42;
            break;
        case 7:
            *inicio=42;
            *fim=43;
            break;
    }
}

/** \brief Função que verifica se a restrição 3 é cumprida
 *
 * \return int -> Se for cumprida devolve 1 se não devolve 0
 *
 */
int restricao3(){
    if((s.num_pecas[0]>=s.num_pecas[1]) && (s.num_pecas[1]>=s.num_pecas[2]) && (s.num_pecas[2]>=s.num_pecas[3]) && (s.num_pecas[3]>=s.num_pecas[4])
       && (s.num_pecas[4]>=s.num_pecas[5]) && (s.num_pecas[5]>=s.num_pecas[6]) && (s.num_pecas[6]>=s.num_pecas[7]))
        return 1;
    else
        return 0;
}

/** \brief Função que realiza os disparos segundo d1
 *
 * \param linhas int
 * \param colunas int -> Dimensões do tabuleiro do utilizador
 * \return void
 *
 */
void modo_d1(int linhas, int colunas){
    srand((unsigned)time(NULL));
    int maxdisparos=0,i,disparos=0,linha, coluna;
    for(i=0;i<8;i++)
        maxdisparos+=(s.num_pecas[i]*(i+1));//contabiliza os disparos totais (cada peça terá de ser disparada tantas vezes quanto o seu tipo)
    i=0;
    while(disparos<maxdisparos){
        do{
        linha=(rand()%linhas);
        coluna=(rand()%colunas);
        }
        while(s.tabuleirolog[linha][coluna]==-1);//gera uma linha e coluna aleatória enquando essa posição tem um disparo (=-1)
        if(validinput(linha,coluna,linhas)!='-')
            disparos++;
        i++;
        s.tabuleirolog[linha][coluna]=(-1);//marca no tabuleiro lógico -1
        if(i==(linhas*colunas)){
            printf("*Erro: Cheguei ao fim do tabuleiro.\n");
            break;
        }
    }
    printf("\nFim de Jogo: %d jogadas ", i);
}

/** \brief Função que realiza os disparos segundo d2
 *
 * \param linhas int
 * \param colunas int -> Dimensões do tabuleiro do utilizador
 * \return void
 *
 */
void modo_d2(int linhas, int colunas){
    int i, count,disparosmax=0, flag=0,disparos=0;
    for(i=0;i<linhas;i++)
        for(count=0;count<colunas;count++)
            if(s.tabuleirolog[i][count]==0)
                s.tabuleirolog[i][count]=-2;//remove os zeros gerados pela da restrição 1
    for(i=0;i<8;i++)
        disparosmax+=(s.num_pecas[i]*(i+1));//contabiliza os disparos totais (cada peça terá de ser disparada tantas vezes quanto o seu tipo)
    for(i=1;i<linhas;i+=3)
        for(count=1;count<colunas;count+=3){
            flag+=sequencia_disparo(i,count,&disparos,linhas);
            if(flag==disparosmax){
                    i=30;//garante que sai do loop externo
                    break;
                }
        }
    printf("\nFim de Jogo: %d jogadas ", disparos);
}

/** \brief Função que realiza a sequência de disparos do modo d2 e d3
 *
 * \param l int -> linha da matriz do tabuleiro a que nos referimos
 * \param c int -> Coluna da matriz do tabuleiro a que nos referimos
 * \param disparos int* -> disparos totais
 * \param linhas int -> dimensão do tabuleiro
 * \return int -> retoma quantos "pedaços" de peças acertou
 *
 */
int sequencia_disparo(int l, int c, int *disparos, int linhas){
    int flag=0, temp1=0, temp2=0, pecas_acertadas=0,i=0;
    char peca='\0';
    //Este vetor deve ser interpretado aos pares, ou seja, cada par contém a coordenada da matriz em que vai ser realizado o disparo
    int posicoes[18]={l,c,l-1,c,l+1,c,l,c-1,l,c+1,l-1,c-1,l+1,c+1,l-1,c+1,l+1,c-1};
    for(i=0;i<18;i+=2){
        temp1=posicoes[i];
        temp2=posicoes[i+1];
        if(s.tabuleirolog[temp1][temp2]==0)//Cada zero vem da restrição 1
            continue;
        *disparos+=1;
        peca=validinput(temp1,temp2,linhas);
        if(peca!='-'){
            s.tabuleirolog[temp1][temp2]=(peca-48);//peca-48 = conversão de ASCII para int
            flag++;
            if((peca-48)==flag){
                pecas_acertadas+=flag;
                break;
            }
        }
    }
return pecas_acertadas;
}

/** \brief Função que realiza os disparos segundo d3
 *
 * \param linhas int
 * \param colunas int -> Dimensões do tabuleiro do utilizador
 * \return void
 *
 */
void modo_d3(int linhas, int colunas){
    int i, count,disparosmax=0, flag=0, disparos=0;
    for(i=0;i<linhas;i++)
        for(count=0;count<colunas;count++)
            if(s.tabuleirolog[i][count]==0)
                s.tabuleirolog[i][count]=-2;//remove os zeros colocados pela restrição 1
    for(i=0;i<8;i++)
        disparosmax+=(s.num_pecas[i]*(i+1));//contabiliza os disparos totais (cada peça terá de ser disparada tantas vezes quanto o seu tipo)
    for(i=1;i<linhas;i+=3)
        for(count=1;count<colunas;count+=3){
            flag+=sequencia_disparo(i,count, &disparos,linhas);
            checksforinvalidspace(i-1,count-1,linhas,colunas);//volta a avaliar onde é inválido colocar peças e coloca zeros nessas  posições
        if(flag==disparosmax){
                i=30;
                break;
            }
        }
    printf("\nFim de Jogo: %d jogadas ", disparos);
}

/** \brief Verifica se o input digitado pelo utilizador é válido no contexto da informação do tabuleiro
 *
 * \param linha int
 * \param coluna int -> coordenadas de disparo correspondentes do tabuleiro
 * \param linhamax int -> dimensão do tabuleiro
 * \return char -> retoma o valor que o utilizador digitou
 *
 */
char validinput (int linha, int coluna,int linhamax){
    char uservalue='\0';
    int retscanf=0;
    printf("%c%d\n",coluna+65,(linhamax-linha));
    do{
        retscanf=scanf("%c",&uservalue);//retoma a quantidade de valores lidos
    }
    while((retscanf==0)||((uservalue!='-')&&((uservalue<'1')||(uservalue>'8'))));
    s.tabuleiro[linha][coluna]=uservalue;//associa ao tabuleiro o valor digitado
    return uservalue;
}

/** \brief Modo de jogo 0, pretende-se gerar tabuleiros
 *
 * \param linhas int
 * \param colunas int -> Dimensões do tabuleiro do utilizador
 * \param posicionamento int -> tipo de posicionamento
 * \return int -> retoma 0 caso algo esteja errado ou 1 caso gere o tabuleiro
 *
 */
int modo_j0(int linhas, int colunas, int posicionamento){
    if(posicionamento==1){
        //Se o posicionamento for o 1 não devem ter sido lidas peças da consola,
        //para isso se a restrição 3 for cumprida e não houverem peças do tipo 1 então sabemos que não foram lidas peças
        if(restricao3()==1)
            if(s.num_pecas[0]==0){
                modo_p1(linhas,colunas);
                return 1;
            }
    }
    else{//posicionamento 2
        if(s.num_pecas[0]==0)
            return 0;
        if(modo_p2(linhas,colunas)==0)
            return 0;
        else
            return 1;
    }
return 0;
}

/** \brief Modo de jogo 1, pretende-se gerar tabuleiros e fazer o jogador disparar contra eles
 *
 * \param linhas int
 * \param colunas int -> Dimensões do tabuleiro do utilizador
 * \param posicionamento int -> Tipo de posicionamento escolhido
 * \return int -> retoma 0 caso algo esteja errado ou 1 caso chegue ao fim do jogo com sucesso
 *
 */
int modo_j1(int linhas, int colunas, int posicionamento){
    time_t start,stop;
    int totalpecas=0,flag=0,i, totaldisparos=0,linha=0;
    char coluna='\0', disparo[5], rest=NULL;
    if(modo_j0(linhas,colunas,posicionamento)==0)//Aproveita-se o modo_jo para gerar o tabuleiro
        return 0;
    printf("* ================================\n"
           "* Modo de Jogo 1                  \n"
           "* Insira as Coordenadas de Disparo\n"
           "* ================================\n");
    impressaoinformacoes(linhas, colunas);
    for(i=0;i<8;i++)
        totalpecas+=(s.num_pecas[i]*(i+1));//contabiliza os disparos totais (cada peça terá de ser disparada tantas vezes quanto o seu tipo)
    time(&start);//Começa a contar o tempo
    while(flag<totalpecas){
    do{
        rest='\0';//O utilizador apenas deve escrever a coordenada de disparo
        fgets(disparo,5,stdin);
        sscanf(disparo, "%c%d%s",&coluna,&linha,&rest);
        linha=(linhas+1)-linha;
        }
    while((coluna<'A')||(coluna>('A'+colunas))|| (linha<1)||(linha>linhas)
          ||(s.tabuleirolog[linha-1][coluna-'A']==-1)||(rest!='\0'));//Pede uma peça enquanto não for válida
    printf("%c\n",s.tabuleiro[linha-1][coluna-'A']);
    if(s.tabuleiro[linha-1][coluna-'A']!='-')
        flag++;
    s.tabuleirolog[linha-1][coluna-'A']=-1;//regista o disparo
    totaldisparos++;
    }
    time(&stop);//regista o tempo final
    printf("Fim de Jogo: %d jogadas em %.0f segundos\n",totaldisparos,difftime(stop,start));
    impressaotabuleiro(linhas,colunas);
return 1;
}

/** \brief Modo de jogo 2, pretende-se que o computador dispare contra o tabuleiro do utilizador
 *
 * \param linhas int
 * \param colunas int -> Dimensões do tabuleiro do utilizador
 * \param disparo int -> Tipo de disparo escolhido
 * \return int -> retoma 0 caso algo esteja errado ou 1 caso chegue ao fim do jogo com sucesso
 *
 */
int modo_j2(int linhas, int colunas, int disparo){
    int i,flag=0;
    time_t start,stop;
    for(i=0;i<8;i++)
        if(s.num_pecas[i]==0)
            flag++;
    if(flag==8)//não tem nenhuma peça
        return 0;
    printf("* ================================                  \n"
           "* Modo de Jogo 2                                    \n"
           "* Crie um tabuleiro com as características indicadas\n"
           "* Responda aos disparos do programa                 \n"
           "* ================================                  \n");
    impressaoinformacoes(linhas,colunas);
    time(&start);//começa o timer
    switch(disparo){
    case 1:
        modo_d1(linhas,colunas);
        break;
    case 2:
        modo_d2(linhas,colunas);
        break;
    case 3:
        modo_d3(linhas,colunas);
        break;
    }
    time(&stop);//termina o timer
    printf("em %.0f segundos\n",difftime(stop,start));
    impressaotabuleiro(linhas,colunas);
return 1;
}
