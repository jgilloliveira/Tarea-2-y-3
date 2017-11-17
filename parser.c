/*********** Inclusión de cabecera **************/
#include "anlex.h"

/************* Variables globales **************/
token t; // token global para recibir componentes del Analizador Lexico
FILE *archivo; // Fuente JSON
FILE *output; // Traducción en XML
char msg[41]; // Mensaje de error.
char trad[41]; // Texto de la traducción.
short error_flag=0; // Bandera para mensajes.
short nivel=0; // Nivel de tabulacion.
/**************** Funciones **********************/
// Funciones para cada no terminal.
void json(void);
void element(void);
void array(void);
void ar(void);
void element_list(void);
void el(void);
void object(void);
void o(void);
void attribute_list(void);
void attribute(void);
void al(void);
void attribute_name(void);
void attribute_value(void);

// Funciones de traducción.
char* tabulador(char*);
//void destabulador(char*);

// Función de mensaje.
void error_msg(char* mensaje){
    error_flag=1;
    printf("Lin %d: Error Sintáctico. %s.\n",linea(),mensaje);	
}

void getToken(void) {
    sigLex();
}

void match(char* c){
    if (strcmp(t.compLex, c)==0) getToken(); 
    else error_msg("Error en el match.");
}

void json(){
    printf("json: %s\n",t.compLex);
    element();
    match("EOF");
}

void element(){  
    printf("element: %s\n",t.compLex);
    if(strcmp(t.compLex,"L_LLAVE")==0){
        object();
    }
    else if (strcmp(t.compLex,"L_CORCHETE")==0){
        array();
    }
    else if(strcmp(t.compLex,"R_CORCHETE")==0||strcmp(t.compLex,"R_LLAVE")==0||strcmp(t.compLex,"COMA")==0){
        sprintf(msg,"Se esperaba un \"{\" o \"[\" no \"%s\"", t.pe->lexema);
        error_msg(msg);
    }
    else getToken();
}

void array(){
    printf("array: %s\n",t.compLex);
    if(strcmp(t.compLex,"L_CORCHETE")==0){
        match("L_CORCHETE");
        ar();  
        nivel--;
    }
    else if(strcmp(t.compLex,"R_CORCHETE")==0||strcmp(t.compLex,"R_LLAVE")==0||strcmp(t.compLex,"COMA")==0){
        sprintf(msg,"Se esperaba un \"[\" no \"%s\"", t.pe->lexema);
        error_msg(msg);
    }
    else getToken();
}

void ar(){
    printf("ar: %s\n",t.compLex);
    if(strcmp(t.compLex,"R_CORCHETE")==0){
        match("R_CORCHETE"); 
    }
    else if(strcmp(t.compLex,"L_CORCHETE")==0||strcmp(t.compLex,"L_LLAVE")==0){
        element_list();
        match("R_CORCHETE");
    }
    else if(strcmp(t.compLex,"R_LLAVE")==0){
        sprintf(msg,"Se esperaba un \"[\" o \"]\" o \"{\" no \"%s\"", t.pe->lexema);
        error_msg(msg);
    }   
    else getToken();
}

void element_list(){
    printf("element-list: %s +++\n",t.compLex);
    if(strcmp(t.compLex,"L_CORCHETE")==0||strcmp(t.compLex,"L_LLAVE")==0){
        nivel++;
        element();
        //nivel--;
        el();       
    }
    else if(strcmp(t.compLex,"R_CORCHETE")==0){
        sprintf(msg,"Se esperaba un \"[\" o \"{\" no \"%s\"", t.pe->lexema);
        error_msg(msg);
    }
    else getToken();
    
}

void el(){
    printf("el: %s +++\n",t.compLex);
    if(strcmp(t.compLex,"COMA")==0){
        match("COMA");
        nivel++;
        element();
        //nivel--;
        el();
    }
    else if(strcmp(t.compLex,"R_CORCHETE")!=0){       
        getToken(); 
    }
}

void object(){
    printf("object: %s ---\n",t.compLex);
    if(strcmp(t.compLex,"L_LLAVE")==0){
        match("L_LLAVE");
        o(); 
        nivel--;
    }
    else if(strcmp(t.compLex,"R_CORCHETE")==0||strcmp(t.compLex,"R_LLAVE")==0||strcmp(t.compLex,"COMA")==0){
        sprintf(msg,"Se esperaba un \"{\" no \"%s\"", t.pe->lexema);
        error_msg(msg);
    }
    else getToken();
}

void o(){
    printf("o: %s\n",t.compLex);
    if(strcmp(t.compLex,"R_LLAVE")==0){
        match("R_LLAVE");   
    }
    else if(strcmp(t.compLex,"LITERAL_CADENA")==0){
        attribute_list();
        match("R_LLAVE");
    }
    else if(strcmp(t.compLex,"R_CORCHETE")==0||strcmp(t.compLex,"COMA")==0){
        sprintf(msg,"Se esperaba un \"}\" o \"string\" no \"%s\"", t.pe->lexema);
        error_msg(msg);
    }
    else getToken();     
}

void attribute_list(){
    printf("attribute-list: %s\n",t.compLex);
    if(strcmp(t.compLex,"LITERAL_CADENA")==0){
        attribute();
        al();
    }
    else if(strcmp(t.compLex,"R_LLAVE")==0){
        sprintf(msg,"Se esperaba un \"string\" no \"%s\"", t.pe->lexema);
        error_msg(msg);
    }   
    else getToken(); 
}

void attribute(){
    printf("attribute: %s %s\n",t.compLex, t.pe->lexema);
    if(strcmp(t.compLex,"LITERAL_CADENA")==0){
        char* aux=t.pe->lexema;
        aux++;
        aux[strlen(aux)-1] = '\0';
        sprintf(trad,"<%s>",aux);
        strcpy(trad,tabulador(trad));
        fputs(trad,output);
        attribute_name();
        match("DOS_PUNTOS");
        attribute_value(); 
        sprintf(trad,"</%s>\n", aux);
        strcpy(trad,tabulador(trad));
        fputs(trad,output);
    }
    else if(strcmp(t.compLex,"R_LLAVE")==0||strcmp(t.compLex,"COMA")==0){
        sprintf(msg,"Se esperaba un \"string\" no \"%s\"", t.pe->lexema);
        error_msg(msg);
    }
    else getToken();    
}

void al(){
    printf("al: %s\n",t.compLex);
    if(strcmp(t.compLex,"COMA")==0){
        match("COMA");
        attribute();
        al();
    }
    else if(strcmp(t.compLex,"R_LLAVE")!=0){ 
        getToken(); 
    }       
}

void attribute_name(){
    printf("attribute-name: %s \n",t.compLex);
    if(strcmp(t.compLex,"LITERAL_CADENA")==0){
        match("LITERAL_CADENA");
    }
    else getToken();
}

void attribute_value(){
    printf("attribute-value: %s +++\n",t.compLex);
    if(strcmp(t.compLex,"L_LLAVE")==0||strcmp(t.compLex,"L_CORCHETE")==0){
        fputs("\n",output);
        nivel++;
        element();
        //nivel--;
    }
    else if(strcmp(t.compLex,"LITERAL_CADENA")==0){
        fputs(t.pe->lexema,output);
        match("LITERAL_CADENA");
    }
    else if(strcmp(t.compLex,"LITERAL_NUM")==0){
        fputs(t.pe->lexema,output);
        match("LITERAL_NUM");
    }
    else if(strcmp(t.compLex,"PR_TRUE")==0){
        fputs(t.pe->lexema,output);
        match("PR_TRUE");
    }
    else if(strcmp(t.compLex,"PR_FALSE")==0){
        fputs(t.pe->lexema,output);
        match("PR_FALSE");
    }
    else if(strcmp(t.compLex,"PR_NULL")==0){
        fputs(t.pe->lexema,output);
        match("PR_NULL");
    }
    else if(strcmp(t.compLex,"R_LLAVE")==0||strcmp(t.compLex,"COMA")==0||strcmp(t.compLex,"DOS_PUNTOS")==0){
        sprintf(msg,"Se esperaba un \"{\" o \"[\" o \"string\" o \"{number\" o \"true\" o \"false\" o \"null\" no \"%s\"", t.pe->lexema);
        error_msg(msg);
    }
    else getToken(); 
}

char* tabulador(char* texto){
    char *tabs= malloc(sizeof(texto));
    for(short i=0;i<nivel;i++){
        strcat(tabs,"\t");
    }
    strcat(tabs,texto);
    return tabs;
}

void destabulador(char* texto){
    for(short i=0;i<nivel+3;i++){
        printf("%s\n",texto);
        (*texto)++;
    }
}

void inicio(void){
    json(); 
    if(strcmp(t.compLex,"EOF")!=0) error_msg("No se esperaba fin del archivo.");
}

void parser(){
    while (strcmp(t.compLex,"EOF")!=0){
        getToken();
        printf("parser: %s\n",t.compLex);
        inicio();
    }   
}

int main(int argc,char* args[]){
    initTabla();
    initTablaSimbolos();
     
    if(argc > 1){
	if (!(archivo=fopen(args[1],"rt"))){
            printf("Archivo no encontrado.\n");
            exit(1);
	}
	while (strcmp(t.compLex,"EOF")!=0){
            output = fopen( "output.txt", "w" );
            parser();
            if(error_flag==0)
                printf("Sintácticamente correcto.\n");
            fclose(output);
	}
	fclose(archivo);
    }else{
	printf("Debe pasar como parametro el path al archivo fuente.\n");
	exit(1);
    }
    return 0;
}
