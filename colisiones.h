/***************************************************************************************************************
 *      FROGGER
 * 
 *      COLISIONES.C:
 * 
 *              Autores:    Alegre, Marcos
 *                          Di Sanzo, Bruno
 *                          Hertter, José Iván
 *                          Ibáñez, Lucía
 * 
 *      Contiene las funciones referidas a la colisión de la rana con objetos.
 *      
 * 
 * 
 *  
 **************************************************************************************************************/

#ifndef COLISIONES_H
#define COLISIONES_H

#include "juego.h"

extern AUTOS autos[FILAS_DE_AUTOS][AUTOS_POR_FILA];
extern RANA rene;


bool colision(void); //detecta si la rana chocó contra algún auto
bool rana_sobre_tronco(void); //detecta si la rana está sobre un tronco y, en tal caso, se modifica su velocidad extra
bool rana_sobre_tortuga(void); //detecta si la rana está sobre una tortuga y, en tal caso, se modifica su velocidad extra
bool rana_llego (void);  //detecta si la rana llegó a la casilla de llegada, sin chocar contra un cocodrilo

#endif /* COLISIONES_H */

