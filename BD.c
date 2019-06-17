/*
///////////////////////CRIAÇÃO DE TABELAS//////////////////////
CREATE TABLE tab_dominio (id_dominio SERIAL NOT NULL PRIMARY KEY, dominio varchar (50)); 
 
CREATE TABLE tab_questoes (id_questao SERIAL NOT NULL PRIMARY KEY, id_dominio INTEGER REFERENCES  tab_dominio(id_dominio), dificuldade INTEGER, pergunta varchar(250) UNIQUE, resposta varchar(250));
 
CREATE TABLE tab_temas (id_tema SERIAL PRIMARY KEY NOT NULL, id_dominio INTEGER references tab_dominio(id_dominio), tema varchar(50));
 
CREATE TABLE questaotema (pergunta varchar(255) references tab_questoes(pergunta), id_tema int references tab_temas(id_tema));
*/

/*
////////////////Alterações feitas para funcionamento do programa cadastro de questões////////////////
 
	alterar o encoding do banco de dados de UTF-8 para LATIN1:
 
update pg_database set encoding = pg_char_to_encoding('LATIN1')
where datname = 'postgres';
 
*/
/*
    Command line to PostgreSQL database
 */
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>
#include <ctype.h>
#include "libpq-fe.h"
 
 
/* IMPORTANT change this line to setup connection */
#define DBDATA "host=localhost port=5432 dbname=postgres user=postgres password=postgres";
    char query[500];
    ////////////////////////
    ////////VARIÁVEIS//////
    //////////////////////
    int dificuldade, temas[3], id_dominio, id_questao, id_tema, max=0;
    char dominio[50], questao[255], resposta[255], tema[50];
 
const char *conninfo;
    PGconn     *conn;
    PGresult   *res;
    int         nFields;
    int         i, j;    
   
void print_tuplas(PGresult* res) {
    int nFields = PQnfields(res);
    int i, j;
    for (i = 0; i < nFields; i++)
        printf("%-15s", PQfname(res, i));
    printf("\n");
 
    /* next, print out the rows */
    for (i = 0; i < PQntuples(res); i++)
    {
        for (j = 0; j < nFields; j++)
            printf("%-15s", PQgetvalue(res, i, j));
        printf("\n");
    }
}  
 
void executarsql(){
    res = PQexec(conn, query);
 
        /* handle the response */
        switch (PQresultStatus(res)) {
            case PGRES_EMPTY_QUERY:
            case PGRES_COMMAND_OK: printf("Done.\n"); break;
            case PGRES_TUPLES_OK: print_tuplas(res); break;
            case PGRES_BAD_RESPONSE: printf("[ERROR] Bad Response"); break;
            case PGRES_NONFATAL_ERROR:
            case PGRES_FATAL_ERROR: printf(PQresultErrorMessage(res)); break;
            default:
                printf("Unexpected Response");
        }
        /* clear the response pointer */
        PQclear(res);
}
static void
exit_nicely(PGconn *conn)
{
    PQfinish(conn);
    exit(1);
}
void cadastrarquestao(){
    do{
        printf("\n\t\tCADASTRAR QUESTÃO\n");
        printf("\n\tInforme qual o domínio da questão\n\tPara cadastrar um dominio digite 0\n\n");
        sprintf(query,"SELECT * FROM tab_dominio;");
        executarsql();
        printf("\nDomínio da questão: ");
        scanf("%d", &id_dominio);
        setbuf(stdin, NULL);
        if(id_dominio==0){
            printf("\n\tCadastre um novo dominio\n");
            printf("\tDigite o Dominio: ");
            fgets(dominio, 50, stdin);
            sprintf(query,"INSERT INTO tab_dominio (dominio) VALUES ('%s');", dominio);
            executarsql();
        }
        system("cls");
        setbuf(stdin, NULL);               
    }while(id_dominio==0);
    char loop='n';
    do{
        system("cls");
        printf("\n\n\tCADASTRAR TEMAS\n");
        sprintf(query,"SELECT id_tema, tema FROM tab_temas WHERE id_dominio = '%d';", id_dominio);
        executarsql();
        printf("\nDigite o numero do tema para a questão(MÁX de 4 temas).\nPara cadastrar um novo tema digite 0.\n\tOpção desejada: ");
        scanf("%d", &id_tema);
        setbuf(stdin, NULL);
        if(id_tema == 0){
            loop='s';
            printf("\n\tDigite o Tema: ");
            fgets(tema, 50, stdin);
            sprintf(query,"INSERT INTO tab_temas (tema, id_dominio) VALUES ('%s','%d');", tema, id_dominio);
            executarsql();
        }else{
            temas[max] = id_tema;
            max++;
            system("cls");
        }
        if(max==4){
            loop='n';
            printf("\n\tVocê já cadastrou o limite de temas para esta questão!");
            system("pause");
            break;
        }
        printf("Cadastrar outro tema?(s/n): ");
        scanf("%c", &loop);
        setbuf(stdin, NULL);
               
    }while(loop=='s');
    printf("Questão: ");
    fgets(questao, 255, stdin);
    setbuf(stdin, NULL);
    printf("Nivel de dificuldade 1 a 5(1-Muito fácil, 5-Muito difícil): ");
    scanf("%d", &dificuldade);
    setbuf(stdin, NULL);
    printf("Resposta da questão: ");
    fgets(resposta, 255, stdin);
    setbuf(stdin, NULL);  
    sprintf(query, "INSERT INTO tab_questoes(pergunta, resposta, dificuldade, id_dominio) VALUES ('%s', '%s', %d, %d);", questao, resposta, dificuldade, id_dominio);
    executarsql();        
    printf("\n\tQuestão cadastrada com sucesso!\n");
    for(int a=0;a<max;a++){
        sprintf(query,"INSERT INTO questaotema VALUES ('%s', %d);", questao, temas[a]);
        executarsql();
    }  
    system("pause");    
}
void buscar(){
    int busca;
    char procura[100], sair='n';
    do{
        system("cls");
        printf("\n\t\tBUSCAR QUESTÃO\n");
        printf("\n\t1-Texto da questao\n\t2-Dominio\n\t3-Tema\n\t4-Dificuldade\n\t5-Sair\nOpção: ");
        scanf("%d", &busca);
        setbuf(stdin, NULL);
        switch(busca){
            case 1: printf("\nBuscar por texto na questão: \n");
                    fgets(procura, 100, stdin);
                    setbuf(stdin, NULL);
                    //system("cls");
                    sprintf(query,"SELECT pergunta, resposta FROM tab_questoes WHERE pergunta LIKE '%s';", procura);
                    executarsql(); printf("\n\n"); system("pause"); break;
            case 2: printf("\nBuscar por Dominio\n\n");
                    sprintf(query,"SELECT * FROM tab_dominio;");
                    executarsql();
                    printf("Digite o id do dominio: ");
                    scanf("%d", &id_dominio);
                    setbuf(stdin, NULL);
                    sprintf(query, "SELECT pergunta, resposta FROM tab_questoes WHERE id_dominio = %d;", id_dominio);
                    executarsql();
                    printf("\n\n"); system("pause"); break;
            case 3: sprintf(query,"SELECT * FROM tab_temas;");
                    executarsql();
                    printf("\nInforme o número do tema: ");
                    scanf("%d", &id_tema);
                    setbuf(stdin, NULL);
                    //system("cls");
                    sprintf(query,"SELECT tab_questoes.pergunta, tab_questoes.resposta FROM questaotema NATURAL JOIN tab_questoes NATURAL JOIN tab_temas WHERE id_tema = %d;", id_tema);
                    executarsql();
                    printf("\n\n"); system("pause"); break;
            case 4: printf("\nInforme a Dificuldade 1 à 5: ");
                    scanf("%d", &dificuldade);
                    //system("cls");
                    sprintf(query,"SELECT * FROM tab_questoes WHERE dificuldade = %d;", dificuldade);
                    executarsql();
                    printf("\n\n"); system("pause"); break;
            case 5: sair='s' ; break;
        }
    }while(sair=='n');
}
 
int main(int argc, char **argv){    
   
    setlocale(LC_ALL, "portuguese");
     
    /*
     * If the user supplies a parameter on the command line, use it as the
     * conninfo string; otherwise default to setting dbname=postgres and using
     * environment variables or defaults for all other connection parameters.
     */
     
    if (argc > 1)
        conninfo = argv[1];
    else
        conninfo = DBDATA;
       
    /* Make a connection to the database */
    conn = PQconnectdb(conninfo);
 
    /* Check to see that the backend connection was successfully made */
    if (PQstatus(conn) != CONNECTION_OK)
    {
        fprintf(stderr, "Connection to database failed: %s",
                PQerrorMessage(conn));
        exit_nicely(conn);
    }
  //////////////////////////////////MEU CÓDIGO ABAIXO/////////////////////////////////////////////
    int opcao;
    do{
        system("cls");
        printf("\n\t\tBANCO DE QUESTÕES\n");
        printf("\t1 - Cadastrar questão\n\t2 - Listar questões\n\t3 - Buscar questões\n\t4 - Sair");
        printf("\n\tOpção: ");
        scanf("%d", &opcao);
        switch(opcao){
            case 1: cadastrarquestao(); break;
            case 2: sprintf(query,"SELECT tab_dominio.dominio, tab_questoes.pergunta, tab_questoes.resposta FROM tab_questoes JOIN tab_dominio ON tab_dominio.id_dominio = tab_questoes.id_dominio;");
                    executarsql();
                    printf("\n\t");
                    system("pause");
                    printf("\n\n");
                    break;
            case 3: buscar(); break;
            case 4: printf("saindo..."); exit_nicely(conn);
            default: printf("\n\n\tOpção inválida\n\n\t");
                     system("pause");
                     system("cls");
                     break;
        }
    }while(1);      
}
