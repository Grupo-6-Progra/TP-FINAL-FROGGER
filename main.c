/***************************************************************************************************************
 *      FROGGER MAIN:
 * 
 *              Autores:    Alegre, Marcos
 *                          Di Sanzo, Bruno
 *                          Hertter, José Iván
 *                          Ibáñez, Lucía
 * 
 *      
 *          El main del juego Frogger compila condicionalmente dependiendo de la plataforma en la que se quiera compilar.
 *      - Si la constante RASPI no está definida, compila el main correspondiente a la interfaz compatible con Allegro.
 *      - Si la constante RASPI está definida, compila el main correspondiente a la interfaz de la RASPBERRY PI.
 * 
 *          En ambos casos, el main principal incluye a "juego.h", puesto que llama a funciones correspondientes
 *      a la lógica del juego. Se inicializa la semilla del generador de números random, utilizada para generar
 *      aleatoriedad dentro de algunas funciones del juego. Luego, se inicializa lo necesario correspondiente a
 *      Allegro o la Raspberry, incluyendo la cola de eventos.
 * 
 *          Se entra al loop principal, donde, si no se detecta el evento del timer, que ocurrirá dependiendo
 *      de los FPS (definidos en juego.c como 60 por segundo), se llamará a las funciones que detectan las entradas     
 *      de cada plataforma. En caso contrario, al detectar el timer, se entra a las funciones del juego.
 *          - Esta función devuelve true si en algún momento se presionó QUIT, por lo que, en dicho caso, se sale
 *      del loop principal y termina el juego.
 *          - Si devuelve false, se indica que hay que redibujar el juego, llamando a las funciones que muestran
 *      las imágenes del juego y el audio del juego, correspondientes a cada plataforma. Así, se va actualizando
 *      el juego frame por frame. 
 * 
 * 
 *  
 **************************************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#include "juego.h"


//#define RASPI
#ifndef RASPI
/*********************************************************
 * 
 * VERSIÓN DEL MAIN PARA ALLEGRO
 *  
 *********************************************************/

#include "allegro_funciones.h"

#include <allegro5/allegro.h>



int main (void)
{    
    bool redraw = false;
    bool do_exit = false;

    srand(time(NULL)); //inicializamos la semilla de numeros random
    
    if(allegro_startup() == false) //inicializamos allegro
    {
        return -1; //si algo falló, se termina
    };
    
    allegro_event_register(); //registramos la cola de eventos

    allegro_initialize_bitmaps(); //inicializamos el display en negro
    
    allegro_fuentes();
    
    al_start_timer(timer); //inicializamos el timer
    
    ALLEGRO_EVENT ev;
    
    
    while (!do_exit)
    {
        if (al_get_next_event(event_queue, &ev)) //Toma un evento de la cola
        {
            if (ev.type == ALLEGRO_EVENT_TIMER) //si pasó 1 frame
            {
                /*******************************************
                 * LÓGICA DEL JUEGO
                 ********************************************/
               
                do_exit = frogger();        //se entra a la lógica del juego
         
                redraw = true; // se indica que hay que mostrar video y audio
            }
            
            /*******************************************
            * DETECCIÓN INPUTS
            ********************************************/
            
            else //entre frame y frame
            {
                do_exit = allegro_teclas(&ev); // se detectan las teclas presionadas (si se apreta ESC, sale)
            }
            
        }

        if (redraw && al_is_event_queue_empty(event_queue)) //si hay que redibujar
        {
            redraw = false; //indicamos que ya se dibujó
            
            allegro_redraw(); //mostramos las imágenes
            allegro_audio(); //mostramos el audio
            

        }
    }

    allegro_destroy(); //liberamos la memoria utilizada por Allegro
    
    return 0;       //termina el juego
}
    
#else

/*********************************************************
 * 
 * VERSIÓN DEL MAIN PARA  LA RASPEBRRY PI
 *  
 *********************************************************/

  #include "display.h"
  #include "/home/pi/libs/joydisp/disdrv.h"

  int main (void)
  {
      bool redraw = false;
      bool do_exit = false;
      
      srand(time(NULL)); //inicializamos la semilla de numeros random
      
      init_display(); //inicializamos la Raspeberry
      
      while (!do_exit)
      {
        if (timer()) //si pasó 1 frame
        {   
            redraw = true; // se indica que hay que mostrar video y audio
            
            do_exit = frogger(); //se entra a la lógica del juego
        }
        else 
        {
            display_teclas(); // se detectan las teclas presionadas
        }
        
        if (redraw == true) //si hay que redibujar
        {
            redraw_disp(); //indicamos que ya se dibujó
            redraw = false; //indicamos que ya se dibujó
        }
      }
      
      disp_clear(); //apagamos los leds del display
      
      return 0;
      
  }
    
    
    
#endif
    
    


