/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   menu.h
 * Author: ivan
 *
 * Created on 9 de julio de 2021, 19:27
 */

#ifndef MENU_H
#define MENU_H


#include "juego.h"



/***************************************************
 * DECLARACIÓN DE FUNCIONES GLOBALES
***************************************************/
bool menu_pausa(void);
bool menu_start(void);
    
/***************************************************
 * DEFINICION DE CONSTANTES PARA EL MENU
***************************************************/
enum SELECTOR
{
    MENU_LEVELS, PLAY, LEVEL, QUIT, MAIN_MENU, RESUME
};
    

#endif /* MENU_H */

