#ifndef JUEGO_H
#define JUEGO_H

////////////////////////////////////////////////////////////////////////////////////////////

#include <stdbool.h>  
#include <math.h>
#include <tgmath.h>



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

#define RANA_ANCHO              CASILLA_ANCHO           
#define RANA_ALTO               CASILLA_ALTO 
                
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

typedef struct //Defino una estructura nueva para los enemigos
{
    double x, dx, largo;      //Posición, velocidad y largo del enemigo
    unsigned int fila;        //Fila en la que se moverá el enemigo
}enemigo_t;


/***************************************************
 * DECLARACIÓN DE FUNCIONES GLOBALES
***************************************************/

void frogger (void);



//////////////////////////////////////////////////////////////////////////////////////

#endif /* JUEGO_H */

