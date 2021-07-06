/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   colisiones.h
 * Author: ivan
 *
 * Created on 29 de junio de 2021, 19:51
 */

#ifndef COLISIONES_H
#define COLISIONES_H

#include "juego.h"

extern AUTOS autos[FILAS_DE_AUTOS][AUTOS_POR_FILA];
extern RANA rene;


bool colision(void);
bool rana_sobre_tronco(void);

#endif /* COLISIONES_H */

