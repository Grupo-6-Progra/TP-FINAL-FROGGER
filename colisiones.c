#include <stdbool.h>
#include "colisiones.h"
/**********************************************************/

static bool choque (double x1, double y1, double ancho1, double alto1, double x2, double y2, double ancho2, double alto2);

/***************************************************************************/

bool colision (void)
{
    int j, k;
    for(j= 0; j < FILAS_DE_AUTOS ; j++)
    {
        for(k=0; k < AUTOS_POR_FILA; k++)
        {
            if (choque (autos[j][k].x, autos[j][k].y, autos[j][k].largo, autos[j][k].alto, rene.x, rene.y, RANA_ANCHO, RANA_ALTO) == true)
            {
                return true;
            }
        }
    }//Si no se cumplió nunca dentro de ambos "for" (todos los autos), no choqué
    return false; 
}

bool rana_sobre_tronco(void)
{
    int j, k;
    for(j= 0; j < FILAS_DE_TRONCOS; j++)
    {
        for(k=0; k < TRONCOS_POR_FILA; k++)
        {
            if (choque (troncos[j][k].x, troncos[j][k].y, troncos[j][k].largo, troncos[j][k].alto, rene.x, rene.y, RANA_ANCHO, RANA_ALTO) == true)
            {
                rene.dx_extra = troncos[j][k].dx;
                
                return true;
            }
        }
    } // si choca un tronco, adquiere su velocidad
    rene.dx_extra = 0;
    return false;
}

bool rana_sobre_tortuga(void)
{
    int j, k;
    for(j= 0; j < FILAS_DE_TORTUGAS; j++)
    {
        for(k=0; k < TORTUGAS_POR_FILA; k++)
        {
            if (tortugas[j][k].hundirse == false)
            {
                if (choque (tortugas[j][k].x, tortugas[j][k].y, tortugas[j][k].largo, tortugas[j][k].alto, rene.x, rene.y, RANA_ANCHO, RANA_ALTO) == true)
                {
                    rene.dx_extra = tortugas[j][k].dx;

                    return true;
                }
            }
        }
    } // si choca un tronco, adquiere su velocidad
    rene.dx_extra = 0;
    return false;
}

static bool choque (double x1, double y1, double ancho1, double alto1, double x2, double y2, double ancho2, double alto2)
{
    if ((x1 + ancho1/2.0) < (x2 - ancho2/2.0))
    {
        return false;
    }
    else if ((x2 + ancho2/2.0) < (x1 - ancho1/2.0))
    {
        return false;
    }
    else if ((y1 + alto1/2.0) < (y2 - alto2/2.0))
    {
        return false;
    }
    else if ((y2 + alto2/2.0) < (y1 - alto1/2.0))
    {
        return false;
    }
    else
    {
        return true;
    }
}