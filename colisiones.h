#ifndef COLISIONES_H
#define COLISIONES_H

#include "juego.h"

extern AUTOS autos[FILAS_DE_AUTOS][AUTOS_POR_FILA];
extern RANA rene;


bool colision(void);
bool rana_sobre_tronco(void);

#endif /* COLISIONES_H */

