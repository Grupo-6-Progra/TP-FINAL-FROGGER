#include <stdio.h>
#include <stdlib.h>


#include <stdbool.h>

#include "juego.h"
#include "allegro_funciones.h"

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_image.h>


int main (void)
{    
    bool redraw = false;
    bool do_exit = false;
    
    if(allegro_startup() == false)
    {
        return -1;
    };
    
    allegro_event_register();
    

    al_set_target_bitmap(al_rene);
    al_clear_to_color(al_map_rgb(0,255,0));
    
    al_set_target_bitmap(al_auto_fila1);
    al_clear_to_color(al_map_rgb(150,152,154));
    
    al_set_target_bitmap(al_get_backbuffer(display));
    al_clear_to_color(al_map_rgb(0, 0, 0));
    al_flip_display();
    
    
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
        }
    }

    allegro_destroy();
    
    return 0;
}
