/***************************************************************************************************************
 *      FROGGER
 * 
 *      MENU.C:
 * 
 *              Autores:    Alegre, Marcos
 *                          Di Sanzo, Bruno
 *                          Hertter, José Iván
 *                          Ibáñez, Lucía
 * 
 *      Contiene las funciones referidas al acceso de los distintos menues y submenues. 
 *      Se puede ingresar a un menu principal que contiene la opcion de empezar a jugar, elegir nivel, o 
 *      cerrar el juego (y por ende dejar de ejecutarse el programa).
 * 
 *      Tambien se puede ingresar a un submenu, que es el menu de pausa, en el que se puede reanudar el juego, 
 *      volver al menu principal o salir del juego.
 *  
 **************************************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "menu.h"
#include "juego.h"

/***********************************
 * DECLARACION DE FUNCIONES LOCALES
 ***********************************/
static void menu_levels(void); //funcion que permite cambiar de nivel

/***********************************
 *DEFINICION DE FUNCIONES LOCALES 
 ***********************************/
static void menu_levels(void)
{
    
    static int esperar = 0; //variable para perder tiempo  para darle tiempo al usuario de soltar una tecla que esta apretando
    
    if(esperar != 0) 
    {
        esperar--;
    }
    
    else
    {
        if(key_pressed[KEY_LEFT]) //Si se presiona la tecla hacia la izquierda, se sale del submenú y se vuelve al menú principal
        {
            esperar = 10; //se espera 10 frames 
            selector_menu = LEVEL;
        }

        if(key_pressed[KEY_DOWN]) //Si el usuario se desplaza a abajo, se aumenta en 1 el nivel seleccionado
        {
            if(nivel < 6) //nivel maximo al que se puede ir es el 6
            {
                esperar = 10;
                nivel++;                
            }
        }

        if(key_pressed[KEY_UP]) //Si el usuario se desplaza arriba, se decrementa en 1 el nivel seleccionado
        {
            if(nivel > 1) //nivel minimo al que se puede ir es el 1
            {
                esperar = 10;
                nivel--;
            }
        }
    
    }

}
                                       
/**********************************
 * DEFINICION DE FUNCIONES GLOBALES
 **********************************/                                      

bool menu_start(void)  //funcion que inicializa todos los menues
{  
    static bool enter_prev = false; //estado previo de la tecla enter
    
    static int esperar = 0; //tiempo que se le da al usuario para soltar la tecla
    
    if(esperar != 0)
    {
        esperar--;
    }
    else
    {
        if(selector_menu != MENU_LEVELS) //si no estoy dentro del submenu de niveles
        {
            if (key_pressed[KEY_DOWN] == true)  //si estoy presionando la tecla para bajar
            {
                esperar = 10;
                if(selector_menu == QUIT) //y estoy posicionada en QUIT
                {
                    selector_menu = PLAY; //va a volver a PLAY que es el que esta mas arriba 
                }
                else
                {
                    selector_menu++; //si no estoy en QUIT (que es el ultimo), voy al que esta abajo
                }
            }
            else if (key_pressed[KEY_UP] == true) //si se presiona la tecla para subir
            {
                esperar = 10;
                if(selector_menu == PLAY)//y estoy en la opcion de mas arriba
                {
                    selector_menu = QUIT;// voy a la opcion de mas abajo
                }
                else
                {
                    selector_menu--;//si no estoy en la opcion de mas arriba, subo normal
                }
            }
            else if((key_pressed[KEY_ENTER] == true) || (enter_prev == true)) //si esta presionada y todavia no solte la tecla
            {
                if (enter_prev == false) //actualizo enter_prev
                {
                    enter_prev = true;
                }
                else if (enter_prev == true && key_pressed[KEY_ENTER] == false)//si solte la tecla
                {
                    enter_prev = false; //actualizo enter_prev
                    esperar = 10; 
                    switch(selector_menu) //dependiendo en que parte del menu hice enter, es como actua
                    {
                        case PLAY: //si le hice enter a PLAY
                            nivel = 1; //arranca siempre desde el nivel 1
                            selector_menu = RESUME; //inicializa que si se hace pausa, se va a RESUME 
                            estado_juego = INICIO; //si se da play, se inicia el juego
                            break;

                        case LEVEL: //si le hice enter a LEVEL
                            selector_menu = MENU_LEVELS; //hago que empiece a entrar siempre a este menú, hasta que se seleccione un nivel.
                            break;

                        case QUIT: //si hago enter en QUIT, se deja de ejecutar el programa y se cierra el juego
                            return true;
                            break;
                    }
                }

            }
        }
        else //si estoy dentro de MENU_LEVELS
        {
            menu_levels();//llamo a la funcion local que me permite cambiar de nivel
            
            if ((key_pressed[KEY_ENTER] == true) || (enter_prev == true)) //si no solte la tecla
            {
                if (enter_prev == false) //actualizo enter_prev
                {
                    enter_prev = true;
                }
                else if (enter_prev == true && key_pressed[KEY_ENTER] == false) //si solte la tecla
                {
                    enter_prev = false; 
                    estado_juego = INICIO;//se va a inicio para arrancar a jugar en el nivel pedido
                    selector_menu = RESUME;//en caso de ir a pausa, lo primero que aparece es RESUME
                }
            }
            
        }
        
    }
    return false;
    

}




bool menu_pausa(void)       //función para el menú de pausa
{  
    static bool enter_prev = false;
    
    static int esperar = 0; //variable que espera a que se suelte la tecla presionada
    
    if(esperar != 0)
    {
        esperar--;
    }
    else
    {
        
        if (key_pressed[KEY_DOWN] == true)  //ve si se presionó la tecla de abajo
        {
            esperar = 10;   //hace una espera
            if(selector_menu == QUIT)   //ve si el selector está en QUIT(el de mas abajo) y quiero bajar
            {
                selector_menu = RESUME; //cambia a estado resume(el de mas arriba)
            }
            else
            {
                selector_menu--; //si no estoy abajo de todo y quiero bajar, paso a la opcion de abajo
            }
        }
        else if (key_pressed[KEY_UP] == true)   //se ve si se presionó la tecla hacia arriba 
        {
            esperar = 10;       //hace una espera
            if(selector_menu == RESUME) //si estoy arriba y quiero subir 
            {
                selector_menu = QUIT;   //se mueve a la opción de Quit que es la de abajo de todo
            }
            else
            {
                selector_menu++; //si no estoy en el caso de arriba de todo, puedo bajar sin problemas
            }
        }
        else if((key_pressed[KEY_ENTER] == true) || (enter_prev == true)) //si se presiona enter
        {
            if (enter_prev == false)//se actualiza la variable
            {
                enter_prev = true;
            }
            else if (enter_prev == true && key_pressed[KEY_ENTER] == false)//si se solto la tecla
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
                        selector_menu = PLAY;//en el menu principal voy a la opcion play
                        estado_juego = MENU;    //se vuelve al estado de menú
                        break;

                    case QUIT: //en caso de presionar enter en QUIT, se termina de ejecutar el programa
                        return true;        //devuelve un true así termina el juego
                        break;
                }
            }

        }
     
    }
    return false;       //de lo contrario devuelve falso
    

}
