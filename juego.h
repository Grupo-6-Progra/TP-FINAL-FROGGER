#ifndef JUEGO_H
#define JUEGO_H

////////////////////////////////////////////////////////////////////////////////////////////

#include <stdbool.h>    



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
 * DECLARACIÓN DE FUNCIONES GLOBALES
***************************************************/

void frogger (void);



//////////////////////////////////////////////////////////////////////////////////////

#endif /* JUEGO_H */

