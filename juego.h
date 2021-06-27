/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   juego.h
 * Author: bruno
 *
 * Created on 26 de junio de 2021, 19:11
 */

#ifndef JUEGO_H
#define JUEGO_H

////////////////////////////////////////////////////////////////////////////////////////////

#include <stdbool.h>    

    
#define TECLAS_MAX 5
    
#define REFRESCO 60.0    
    
#define MUNDO_ALTO 600.0
#define MUNDO_ANCHO 600.0
#define CANT_SALTOS_ANCHO 14.0
#define CANT_SALTOS_ALTO 13.0


//////////////////////////////////
// RANA
//////////////////////////////////


#define SALTO_RANA_ANCHO        MUNDO_ANCHO / CANT_SALTOS_ANCHO
#define SALTO_RANA_ALTO         MUNDO_ALTO / CANT_SALTOS_ALTO                   

#define TIEMPO_SALTO_ANCHO      5.0 // VA A TARDAR 10 FRAMES EN COMPLETAR 1 SALTO HORIZONTAL
#define VELOCIDAD_RANA_ANCHO    SALTO_RANA_ANCHO / TIEMPO_SALTO_ANCHO
    
#define TIEMPO_SALTO_ALTO       5.0 // VA A TARDAR 10 FRAMES EN COMPLETAR 1 SALTO VERTICAL
#define VELOCIDAD_RANA_ALTO     SALTO_RANA_ALTO / TIEMPO_SALTO_ALTO

#define TARDA_SALTO             5.0 //frames que espera entre salto y salto

#define RANA_ANCHO              SALTO_RANA_ANCHO           
#define RANA_ALTO               SALTO_RANA_ALTO

typedef struct
{
    double x, y;
    double dx, dy;
    
}RANA;

//////////////////////////////////////////////////////////////////////////////////

enum MYKEYS
{
    KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT, KEY_ENTER //arrow keys
};
    
extern bool key_pressed[TECLAS_MAX];

    
/////////////////////////////////////////////////////////////////////////////////////


extern RANA rene;


void frogger (void);

#endif /* JUEGO_H */

