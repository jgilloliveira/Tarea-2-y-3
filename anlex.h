
/*********** Librerias utilizadas **************/
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>

/************* Definiciones ********************/
#define TAMBUFF 5
#define TAMCMLEX 50
#define TAMLEX 50
#define TAMHASH 101

/************* Estructuras ********************/

typedef struct entrada{
    //definir los campos de 1 entrada de la tabla de simbolos
    char compLex[TAMCMLEX];
    char lexema[TAMLEX];	
    struct entrada *tipoDato; // null puede representar variable no declarada	
    // aqui irian mas atributos para funciones y procedimientos...
} entrada;

typedef struct {
    char compLex[50];
    entrada *pe;
} token;

/************* Prototipos ********************/
void insertar(entrada e);
entrada* buscar(const char *clave);
void initTabla();
void initTablaSimbolos();
void sigLex();
int linea();
