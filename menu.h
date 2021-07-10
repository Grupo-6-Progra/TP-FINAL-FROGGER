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

enum SELECTOR
{
    PLAY, LEVEL, QUIT, MENU_LEVELS
};

static int level (void);
static int menu_levels(void);
static void play (void);
static bool quit(void);
bool menu_start(void);
    
    

#endif /* MENU_H */

