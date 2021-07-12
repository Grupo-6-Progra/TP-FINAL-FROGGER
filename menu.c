#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include "menu.h"
#include "juego.h"



bool menu_start(void)
{  
    static int esperar = 0;
    if(esperar != 0)
    {
        esperar--;
    }
    else
    {
        if(selector_menu != MENU_LEVELS)
        {
            if (key_pressed[KEY_RIGHT] == true) 
            {
                esperar = 10;
                if(selector_menu == QUIT)
                {
                    selector_menu = PLAY;
                }
                else
                {
                    selector_menu++;
                }
            }
            else if (key_pressed[KEY_LEFT] == true)
            {
                esperar = 10;
                if(selector_menu == PLAY)
                {
                    selector_menu = QUIT;
                }
                else
                {
                    selector_menu--;
                }
            }
            else if(key_pressed[KEY_ENTER])
            {
                esperar = 10;
                switch(selector_menu)
                {
                    case PLAY:
                        play();
                        break;

                    case LEVEL:
                        level();
                        break;

                    case QUIT:
                        return quit();
                        break;
                    
                }
            }
        }
        else
        {
            esperar = menu_levels();
        }
        
    }
    return false;
    

}

static int level (void)
{
    if(key_pressed[KEY_ENTER])
    {
        return menu_levels();
    }
    return 0;
}

static int menu_levels(void)
{
    static int esperar;
    esperar = 0;
    selector_menu = MENU_LEVELS;
    
    if(key_pressed[KEY_RIGHT])
    {
        esperar = 10;
        nivel++;
    }
    if(key_pressed[KEY_LEFT])
    {
        esperar = 10;
        nivel--;
    }
    if(key_pressed[KEY_ENTER])
    {
        esperar = 10;
    }
    return esperar;
}

static void play (void)
{
    //mostrar la palabra play
    if(key_pressed[KEY_ENTER])
    {
        nivel = 1;
        estado_juego = INICIO;
    }
}

static bool quit(void)
{
    if(key_pressed[KEY_ENTER])
    {
        return true;
    }
}
