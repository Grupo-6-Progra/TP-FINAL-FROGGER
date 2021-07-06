#ifndef JUEGO_H
#define JUEGO_H

////////////////////////////////////////////////////////////////////////////////////////////

#include <stdbool.h>  

/************************+
 * 
 ************************/

enum ESTADO_JUEGO
{
    INICIO, PAUSA, JUEGO, MENU
};

extern int estado_juego;

/***************************************************
 * MUNDO Y VELOCIDAD DE JUEGO
***************************************************/
    
#define REFRESCO            60.0    

#define CASILLA_ANCHO       40.0
#define CASILLA_ALTO        40.0

#define CANT_CASILLAS_FILA          14
#define CANT_CASILLAS_COLUMNA       13

#define MUNDO_ALTO          (CANT_CASILLAS_COLUMNA * CASILLA_ALTO)
#define MUNDO_ANCHO         (CANT_CASILLAS_FILA * CASILLA_ANCHO)

/***************************************************
 * MANEJO DE TECLAS DETECTADAS
***************************************************/

#define TECLAS_MAX          5

enum MYKEYS
{
    KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT, KEY_ENTER //arrow keys
};
    
extern bool key_pressed[TECLAS_MAX];

    

/***************************************************
 * RANA
***************************************************/

#define RANA_ANCHO              (CASILLA_ANCHO - 10)         
#define RANA_ALTO               (CASILLA_ALTO - 10)
                
typedef struct
{
    
    double x, y;
    double dx, dy;
    
}RANA;

extern RANA rene;


/***************************************************
 * ENEMIGOS
 **************************************************/
#define CANT_ENEMIGOS 5 //Cantidad de filas que contienen enemigos "del mismo tipo" (autos o los que van sobre el agua)

#define AUTOS_POR_FILA      5
#define FILAS_DE_AUTOS      5

typedef struct //Defino una estructura nueva para los enemigos
{
    double x, y;
    double dx, dy;
    double largo;               //Posición, velocidad y largo del enemigo
    double alto;
    unsigned int fila;          //Fila en la que se moverá el enemigo
}AUTOS;


extern AUTOS autos[FILAS_DE_AUTOS][AUTOS_POR_FILA]; //Defino una sola matriz con todos los enemigos de un mismo tipo

/***************************************************
 * DECLARACIÓN DE FUNCIONES GLOBALES
***************************************************/

void frogger (void);



//////////////////////////////////////////////////////////////////////////////////////

#endif /* JUEGO_H */

