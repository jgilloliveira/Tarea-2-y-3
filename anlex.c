/*********** Inclusión de cabecera **************/
#include "anlex.h"

/************* Variables globales **************/
int consumir; /* 1 indica al analizador lexico que debe devolver
		el sgte componente lexico, 0 debe devolver el actual */

char cad[5*TAMLEX]; // string utilizado para cargar mensajes de error
token t; // token global para recibir componentes del Analizador Lexico

// variables para el analizador lexico
FILE *archivo; // Fuente JSON
char buff[2*TAMBUFF]; // Buffer para lectura de archivo fuente
char id[TAMLEX]; // Utilizado por el analizador lexico
int delantero=-1; // Utilizado por el analizador lexico
int fin=0; // Utilizado por el analizador lexico
int numLinea=1; // Numero de Linea
char espacio[100];

/**************** Funciones **********************/
// Rutinas del analizador lexico
void error(const char* mensaje){
    printf("Lin %d: Error Lexico. %s.\n",numLinea,mensaje);	
}

void sigLex(){
    int i=0;
    char c=0;
    int acepto=0;
    int estado=0;
    char msg[41];
    entrada e;
    //t.pe=(entrada*)malloc(sizeof(entrada));
    while((c=fgetc(archivo))!=EOF){
		
	if (c==' ' || c=='\t'){
            strcat(espacio,&(c));
            continue; //eliminar espacios en blanco
        }
	else if(c=='\n'){
            //incrementar el numero de linea
            numLinea++;
            //strcat(espacio,&(c));
            continue;
	}
	else if (isalpha(c)){
            //es un boolean
            i=0;
            do{
                id[i]=tolower(c);
                i++;
		c=fgetc(archivo); // Avanza al siguiente caracter
		if (i>=TAMLEX)
                    error("Longitud de Identificador excede tamaño de buffer");
            }while(isalpha(c));
            id[i]='\0';
            if (c!=EOF)
		ungetc(c,archivo); // Vuelve al caracter anterior
            else
                c=0;
            // El puntero id tiene la palabra
            // Aqui basicamente se extrae de la TS la palabra y token
            t.pe=buscar(id); // devuelve puntero entrada que encontró en la TS.
            strcpy(t.compLex,t.pe->compLex);
            if (strcmp(t.pe->compLex,"-1")==0){ // como no encontró la palabra en la TS es una cadena
                strcpy(e.lexema,id); // guarda la palabra entera en entrada.
                strcpy(e.compLex,"LITERAL_CADENA");
		insertar(e);
		t.pe=buscar(id);
                strcpy(t.compLex,"LITERAL_CADENA");
            }
            break;
	}
	else if (isdigit(c)){
            //es un numero
            i=0;
            estado=0;
            acepto=0;
            id[i]=c;
            while(!acepto){
                switch(estado){
                    case 0: //una secuencia netamente de digitos, puede ocurrir . o e
                        c=fgetc(archivo);
                        if (isdigit(c)){
                            id[++i]=c;
                            estado=0;
                        }
                        else if(c=='.'){
                            id[++i]=c;
                            estado=1;
                        }
                        else if(tolower(c)=='e'){
                            id[++i]=c;
                            estado=3;
                        }
                        else{
                            estado=6;
                        }
                        break;
                    case 1://un punto, debe seguir un digito (caso especial de array, puede venir otro punto)
                        c=fgetc(archivo);						
                        if (isdigit(c)){
                            id[++i]=c;
                            estado=2;
			}
			else{
                            sprintf(msg,"No se esperaba '%c'",c);
                            estado=-1;
			}
			break;
                    case 2://la fraccion decimal, pueden seguir los digitos o e
                        c=fgetc(archivo);
			if (isdigit(c)){
                            id[++i]=c;
                            estado=2;
			}
                        else if(tolower(c)=='e'){
                            id[++i]=c;
                            estado=3;
			}
			else
                            estado=6;
			break;
                    case 3://una e, puede seguir +, - o una secuencia de digitos
                        c=fgetc(archivo);
                        if (c=='+' || c=='-'){
                            id[++i]=c;
                            estado=4;
			}
			else if(isdigit(c)){
                            id[++i]=c;
                            estado=5;
			}
			else{
                            sprintf(msg,"No se esperaba '%c'",c);
                            estado=-1;
			}
			break;
                    case 4://necesariamente debe venir por lo menos un digito
			c=fgetc(archivo);
			if (isdigit(c)){
                            id[++i]=c;
                            estado=5;
			}
			else{
                            sprintf(msg,"No se esperaba '%c'",c);
                            estado=-1;
			}
			break;
                    case 5://una secuencia de digitos correspondiente al exponente
			c=fgetc(archivo);
			if (isdigit(c)){
                            id[++i]=c;
                            estado=5;
			}
			else{
                            estado=6;
			}break;
                    case 6://estado de aceptacion, devolver el caracter correspondiente a otro componente lexico
			if (c!=EOF)
                            ungetc(c,archivo);
			else
                            c=0;
			id[++i]='\0';
			acepto=1;
			t.pe=buscar(id);
			if (strcmp(t.pe->compLex,"-1")==0){ // Si no encuentra el número lo agrega a la TS.
                            strcpy(e.lexema,id);
                            strcpy(e.compLex,"LITERAL_NUM");
                            insertar(e);
                            t.pe=buscar(id);
			}
                        strcpy(t.compLex,"LITERAL_NUM");
			break;
                    case -1:
			if (c==EOF)
                            error("No se esperaba el fin de archivo");
			else
                            error(msg);
                        exit(1);
                }
            }
            break;
	}
	else if (c==':'){
            strcpy(t.compLex,"DOS_PUNTOS");
            t.pe=buscar(":");
            break;
	}
        // BORRE COSAS
	else if (c==','){
            strcpy(t.compLex,"COMA");
            t.pe=buscar(",");
            break;
	}
	else if (c=='['){
            strcpy(t.compLex,"L_CORCHETE");
            t.pe=buscar("[");
            break;
	}
	else if (c==']'){
            strcpy(t.compLex,"R_CORCHETE");
            t.pe=buscar("]");
            break;
	}
        // AGREGE LLAVES
        else if (c=='{'){
            strcpy(t.compLex,"L_LLAVE");
            t.pe=buscar("{");
            break;
	}
	else if (c=='}'){
            strcpy(t.compLex,"R_LLAVE");
            t.pe=buscar("}");
            break;
	}
	else if (c=='\"'){//un caracter o una cadena de caracteres
            i=0;
            id[i]=c;
            i++;
            do{
                c=fgetc(archivo);
		if (c=='\"'){
                    c=fgetc(archivo);
                    if (c=='\"'){
                        id[i]=c;
                        i++;
                        id[i]=c;
                        i++;
                    }
                    else{
                        id[i]='\"';
                        i++;
                        break;
                    }
		}
		else if(c==EOF){
                    error("Se llego al fin de archivo sin finalizar un literal");
		}
		else{
                    id[i]=c;
                    i++;
		}
            }while(isascii(c));
            id[i]='\0';
            if (c!=EOF)
                ungetc(c,archivo);
            else
		c=0;
            t.pe=buscar(id);
            strcpy(t.compLex,t.pe->compLex);
            if (strcmp(t.pe->compLex,"-1")==0){
		strcpy(e.lexema,id);
                strcpy(e.compLex,"LITERAL_CADENA");
                insertar(e);
		t.pe=buscar(id);
                strcpy(t.compLex,e.compLex);
            }
            break;
	}
        else if (c!=EOF){
            sprintf(msg,"%c no esperado",c);
            error(msg);
        }
    }
    if (c==EOF){
        strcpy(t.compLex,"EOF");
        sprintf(e.lexema,"eof");
        t.pe=&e;
    }    
}

int linea(){
    return numLinea;
}
//int main(int argc,char* args[]){
//    // inicializar analizador lexico
//    initTabla();
//    initTablaSimbolos();
//
//    if(argc > 1){
//	if (!(archivo=fopen(args[1],"rt"))){
//            printf("Archivo no encontrado.\n");
//            exit(1);
//	}
//	while (strcmp(t.compLex,"EOF")!=0){
//            sigLex();
//            printf("%s%s ",espacio,t.compLex);
//            strcpy(espacio,"");
//	}
//	fclose(archivo);
//    }else{
//	printf("Debe pasar como parametro el path al archivo fuente.\n");
//	exit(1);
//    }
//
//    return 0;
//}
