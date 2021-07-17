#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "menu.h"
#include "juego.h"


static void menu_levels(void);
static void play (void);


bool menu_start(void)
{  
    static bool enter_prev = false;
    
    static int esperar = 0;
    static int esperar_play = 0;
    
    if(esperar != 0)
    {
        esperar--;
    }
    else
    {
        if(selector_menu != MENU_LEVELS)
        {
            if (key_pressed[KEY_DOWN] == true) 
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
            else if (key_pressed[KEY_UP] == true)
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
            else if((key_pressed[KEY_ENTER] == true) || (enter_prev == true))
            {
                if (enter_prev == false)
                {
                    enter_prev = true;
                }
                else if (enter_prev == true && key_pressed[KEY_ENTER] == false)
                {
                    enter_prev = false;
                    esperar = 10;
                    switch(selector_menu)
                    {
                        case PLAY:
                            nivel = 1;
                            selector_menu = RESUME;
                            estado_juego = INICIO;
                            break;

                        case LEVEL:
                            selector_menu = MENU_LEVELS; //hago que empiece a entrar siempre a este menú, hasta que se seleccione un nivel.
                            break;

                        case QUIT:
                            return true;
                            break;
                    }
                }

            }
        }
        else
        {
            menu_levels();
            
            if ((key_pressed[KEY_ENTER] == true) || (enter_prev == true))
            {
                if (enter_prev == false)
                {
                    enter_prev = true;
                }
                else if (enter_prev == true && key_pressed[KEY_ENTER] == false)
                {
                    enter_prev = false;
                    estado_juego = INICIO;
                    selector_menu = RESUME;
                }
            }
            
        }
        
    }
    return false;
    

}

static void menu_levels(void)
{
    
    static int esperar = 0;
    
    if(esperar != 0)
    {
        esperar--;
    }
    
    else
    {
        if(key_pressed[KEY_LEFT]) //Si se presiona la tecla hacia la izquierda, se sale del submenú y se vuelve al menú principal
        {
            esperar = 10;
            selector_menu = LEVEL;
        }

        if(key_pressed[KEY_DOWN]) //Si el usuario se desplaza a abajo, se aumenta en 1 el nivel seleccionado
        {
            if(nivel < 6)
            {
                esperar = 10;
                nivel++;                
            }
        }

        if(key_pressed[KEY_UP]) //Si el usuario se desplaza arriba, se decrementa en 1 el nivel seleccionado
        {
            if(nivel > 1)
            {
                esperar = 10;
                nivel--;
            }
        }
    
    }

}



/*bool menu_pausa(void)                           //función del menú en pausa, tiene 3 opciones regidas por variables globales
{    
    /*if(key_pressed[KEY_DOWN]== true)                       //ve si toco hacia abajo el stick //key_pressed[KEY_DOWN]== true  // se elige ir hacia abajo para cambiar de opción
    {
        if (key_pressed[KEY_DOWN]== true)            //sige hasta la opción de Quit
        {
            printf("Quit\n");       //MOSTRAR EN PANTALLA LO INTERNO DEL PRINTF

            if(key_pressed[KEY_DOWN]== true)    //si se ingresó la tecla hacia abajo para moverse abajo en las opciones
            {
                printf("NO HACE NADA"); //acá no debería hacer nada y solo volver a mostrar la opción de REANUDAR, ya que se le acabaron las opciones
            }

            else if (key_pressed[KEY_ENTER]== true) //si se tocó la tecla de enter entonces es porque seleccionó
            {
                estado_juego=MENU;     //se vuelve al menú principal ya que cambió el estado de la variable global

            }

        }  
        else if (key_pressed[KEY_ENTER]== true)     // ve si se seleccionó algo
        {
            menu_levels();       //va a la función de elegir niveles (externa)
        }                   
    }				
    else if(key_pressed[KEY_ENTER]== true)   //si fue tocado enter entonces vuelve al juego
    {
        estado_juego =JUEGO;	//vuelve al estado de juego      
    }/
    static int esperar = 0;
    if(esperar != 0)
    {
        esperar--;
    }
    else
    {
        //lo que voy a usar va a ser selector_menu = {MAIN_MENU, RESUME, QUIT}
        if (key_pressed[KEY_DOWN] == true) 
        {
            esperar = 10;
            if(selector_menu == RESUME) //en el momento donde se me terminan los estados, vuelvo al primero de los que uso
            {
                selector_menu = QUIT;
            }
            else
            {
                selector_menu++;
            }
        }
        else if (key_pressed[KEY_UP] == true)
        {
            esperar = 10;
            if(selector_menu == QUIT)
            {
                selector_menu = RESUME;
            }
            else
            {
                selector_menu--;
            }
        }
        else if(key_pressed[KEY_ENTER] == true)
        {
            esperar = 10;
            
                switch(selector_menu)
                {
                    case RESUME:

                        break;

                    case MAIN_MENU:
                        estado_juego = MENU;
                        break;

                    case QUIT:
                        return true;
                        break;
                }
            
        }
        
    }
    return false;
    

}*/

bool menu_pausa(void)       //función para el menú de pausa
{  
    static bool enter_prev = false;
    
    static int esperar = 0;
    static int esperar_play = 0;
    
    if(esperar != 0)
    {
        esperar--;
    }
    else
    {
        
        if (key_pressed[KEY_DOWN] == true)  //ve si se presionó la tecla de abajo
        {
            esperar = 10;   //hace una espera
            if(selector_menu == QUIT)   //ve si el selector está en QUIT
            {
                selector_menu = RESUME; //si es así cambia a estado resume
            }
            else
            {
                selector_menu--;
            }
        }
        else if (key_pressed[KEY_UP] == true)   //se ve si se presionó la tecla hacia arriba
        {
            esperar = 10;       //hace una espera
            if(selector_menu == RESUME)
            {
                selector_menu = QUIT;   //se mueve a la opción de Quit
            }
            else
            {
                selector_menu++;
            }
        }
        else if((key_pressed[KEY_ENTER] == true) || (enter_prev == true))
        {
            if (enter_prev == false)
            {
                enter_prev = true;
            }
            else if (enter_prev == true && key_pressed[KEY_ENTER] == false)
            {
                enter_prev = false;
                esperar = 10;       //espera para la tecla 
                switch(selector_menu)
                {
                    case RESUME:        //en el caso de elegir la opción resume el juego vuelve
                        estado_juego = JUEGO;
                        break;

                    case MAIN_MENU:     //en el caso de la opcion de menu
                        puntaje_juego = 0; //el puntaje se reinica
                        nivel = 1;          //se vuelve al nivel 1
                        selector_menu = PLAY;
                        estado_juego = MENU;    //se vuelve al estado de menú
                        break;

                    case QUIT:
                        return true;        //devuelve un true así termina el juego
                        break;
                }
            }

        }
     
    }
    return false;       //de lo contrario devuelve falso
    

}
