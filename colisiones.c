
#include <stdbool.h>
#include "colisiones.h"

bool colision (void)
{
    int aux = ((CANT_CASILLAS_COLUMNA - rene.y / CASILLA_ALTO == 0)?(CANT_CASILLAS_COLUMNA - rene.y / CASILLA_ALTO) : (CANT_CASILLAS_COLUMNA - rene.y / CASILLA_ALTO - 1));
    int j, k;
    for(j= aux; j < FILAS_DE_AUTOS && j < aux + 2; j++)
    {
        for(k=0; k < AUTOS_POR_FILA; k++)
        {
            //Tengo que ver si alguna parte de la rana está adentro de algun enemigo
            if(rene.x + RANA_ANCHO/2.0 > autos[j][k].x - autos[j][k].largo / 2.0 && rene.x - RANA_ANCHO/2.0 < autos[j][k].x + autos[j][k].largo / 2.0)
            {
                //Hasta ahora estaría chocando, analizo la coordenada Y
                if(rene.y + RANA_ALTO/2.0 > autos[j][k].y - CASILLA_ALTO / 2.0 && rene.y - RANA_ALTO/2.0 < autos[j][k].y + CASILLA_ALTO / 2.0)
                {
                    //Si esto se cumple, choqué
                    return true;
                }
            }
        }
    }//Si no se cumplió nunca dentro de ambos "for" (todos los autos), no choqué
    return false; 
}