/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   mundo.h
 * Author: bruno
 *
 * Created on 28 de junio de 2021, 18:38
 */

#ifndef MUNDO_H
#define MUNDO_H

/////////////////////////////////////////////////////////////////////////////////

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

//////////////////////////////////////////////////////////////////////////////////

#endif /* MUNDO_H */

