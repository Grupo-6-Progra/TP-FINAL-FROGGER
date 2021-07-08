#include <stdio.h>
#include <stdlib.h>

#include <stdbool.h>

#include "juego.h"
#include "allegro_funciones.h"

#include <allegro5/allegro.h>



int main (void)
{    
    bool redraw = false;
    bool do_exit = false;
    
    
    if(allegro_startup() == false)
    {
        return -1;
    };
    
    allegro_event_register();

    allegro_initialize_bitmaps();
    
    allegro_fuentes();
    
    al_start_timer(timer);
    ALLEGRO_EVENT ev;
    
    while (!do_exit)
    {
        if (al_get_next_event(event_queue, &ev)) //Toma un evento de la cola, VER RETURN EN DOCUMENT.
        {
            if (ev.type == ALLEGRO_EVENT_TIMER)
            {
                /*******************************************
                 * LÓGICA DEL JUEGO
                 ********************************************/
               
                frogger();
         
                redraw = true;
            }
            
            /*******************************************
            * DETECCIÓN INPUTS
            ********************************************/
            
            else
            {
                do_exit = allegro_teclas(&ev);
            }
            
        }

        if (redraw && al_is_event_queue_empty(event_queue))
        {
            redraw = false;
            
            allegro_redraw();
            
            //dibujar en display
        }
    }

    allegro_destroy();
    
    return 0;
}