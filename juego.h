#ifndef JUEGO_H
#define JUEGO_H

////////////////////////////////////////////////////////////////////////////////////////////

#include <stdbool.h>  

/************************+
 * 
 ************************/

enum ESTADO_JUEGO
{
    INICIO, REINICIO, PAUSA, JUEGO, PERDER, MENU
};

extern int estado_juego;
extern double tiempo_restante;
extern int nivel;
extern int selector_menu;

/***************************************************
 * MUNDO Y VELOCIDAD DE JUEGO
***************************************************/
    
#define REFRESCO            60.0    

#define CASILLA_ANCHO       50.0
#define CASILLA_ALTO        50.0

#define CANT_CASILLAS_FILA          14
#define CANT_CASILLAS_COLUMNA       13

#define MUNDO_ALTO          (CANT_CASILLAS_COLUMNA * CASILLA_ALTO)
#define MUNDO_ANCHO         (CANT_CASILLAS_FILA * CASILLA_ANCHO)

#define TIEMPO_TOTAL (40 * REFRESCO)

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

#define RANA_ANCHO              (CASILLA_ANCHO - 20)         
#define RANA_ALTO               (CASILLA_ALTO - 20)
                
typedef struct
{
    
    double x, y;
    double dx, dy;
    double dx_extra;
    int direccion;
    bool chocada;
    bool llego;
    
    
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
 * TRONCOS
 **************************************************/

#define TRONCOS_POR_FILA        5
#define FILAS_DE_TRONCOS        3

typedef struct //Defino una estructura nueva para los troncos
{
    double x, y;
    double dx, dy;
    double largo;               //Posición, velocidad y largo del TRONCO
    double alto;
    unsigned int fila;          //Fila en la que se moverá el TRONCO
}TRONCO;

extern TRONCO troncos[FILAS_DE_TRONCOS][TRONCOS_POR_FILA]; //Defino una sola matriz con todos los enemigos de un mismo tipo

/***************************************************
 * TORTUGAS
 **************************************************/

#define TORTUGAS_POR_FILA       5
#define FILAS_DE_TORTUGAS       2

#define TIEMPO_HASTA_HUNDIRSE   2
#define FRAMES_HASTA_HUNDIRSE   ((int)(REFRESCO * TIEMPO_HASTA_HUNDIRSE))

typedef struct //Defino una estructura nueva para las tortugas
{
    double x, y;
    double dx, dy;
    double largo;               //Posición, velocidad y largo de LAS TORUGAS
    double alto;
    unsigned int fila;          //Fila en la que se moverá LAS TORTUGAS
    bool hundirse;
    
}TORTUGAS;

extern TORTUGAS tortugas[FILAS_DE_TORTUGAS][TORTUGAS_POR_FILA]; //Defino una sola matriz con todas las tortugas

/***************************************************
 * CASILLAS DE LLEGADA
 **************************************************/
#define CANT_CASILLAS_LLEGADA       5

typedef struct
{
    double x,y;
    double ancho;
    double alto;
    bool ocupado;

}LLEGADA;

extern LLEGADA llegadas[CANT_CASILLAS_LLEGADA];


/***************************************************
 * DECLARACIÓN DE FUNCIONES GLOBALES
***************************************************/

bool frogger (void);



//////////////////////////////////////////////////////////////////////////////////////

#endif /* JUEGO_H */
