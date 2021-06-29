#include <stdio.h>
#include <stdlib.h>


#include <stdbool.h>

#include "juego.h"
#include "allegro_funciones.h"
#include "mundo.h"

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_image.h>


#define FPS             REFRESCO

#define BUFFER_W        MUNDO_ANCHO
#define BUFFER_H        MUNDO_ALTO

#define SCREEN_SCALE    1
#define SCREEN_W        (BUFFER_W * SCREEN_SCALE)
#define SCREEN_H        (BUFFER_H * SCREEN_SCALE)


int main (void)
{    
    bool redraw = false;
    bool do_exit = false;
    
    if(allegro_startup() == false)
    {
        return -1;
    };
    
    
    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    
    al_set_target_bitmap(mundo_buffer);
    al_clear_to_color(al_map_rgb(0, 0, 0));
    
    al_set_target_bitmap(al_rene);
    al_clear_to_color(al_map_rgb(0,255,0));
    
    al_set_target_bitmap(al_get_backbuffer(display));
    al_clear_to_color(al_map_rgb(0, 0, 0));
    al_flip_display();
    
    
    al_start_timer(timer);
    
    while (!do_exit)
    {
        ALLEGRO_EVENT ev;
        if (al_get_next_event(event_queue, &ev)) //Toma un evento de la cola, VER RETURN EN DOCUMENT.
        {
            if (ev.type == ALLEGRO_EVENT_TIMER) {
               
                ///////////////////////////////////////////
                /// LÓGICA DEL JUEGO
                ////////////////////////////////////////////

                frogger();
         
                redraw = true;
            }
            
            
            ////////////////////////////////////////
            /// DETECCIÓN INPUTS
            ///////////////////////////////////////
            
            else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
                do_exit = true;

            else if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
                switch (ev.keyboard.keycode)
                {
                    case ALLEGRO_KEY_UP:
                        key_pressed[KEY_UP] = true;
                        break;

                    case ALLEGRO_KEY_DOWN:
                        key_pressed[KEY_DOWN] = true;
                        break;

                    case ALLEGRO_KEY_LEFT:
                        key_pressed[KEY_LEFT] = true;
                        break;

                    case ALLEGRO_KEY_RIGHT:
                        key_pressed[KEY_RIGHT] = true;
                        break;
                }
            }
            else if (ev.type == ALLEGRO_EVENT_KEY_UP)
            {
                switch (ev.keyboard.keycode) {
                    case ALLEGRO_KEY_UP:
                        key_pressed[KEY_UP] = false;
                        break;

                    case ALLEGRO_KEY_DOWN:
                        key_pressed[KEY_DOWN] = false;
                        break;

                    case ALLEGRO_KEY_LEFT:
                        key_pressed[KEY_LEFT] = false;
                        break;

                    case ALLEGRO_KEY_RIGHT:
                        key_pressed[KEY_RIGHT] = false;
                        break;

                    case ALLEGRO_KEY_ESCAPE:
                        do_exit = true;
                        break;
                }
            }
        }

        if (redraw && al_is_event_queue_empty(event_queue))
        {
            redraw = false;
            
            al_set_target_bitmap(mundo_buffer);
            al_clear_to_color(al_map_rgb(0,0,0));
            
            al_draw_bitmap(al_rene,rene.x - RANA_ANCHO/2, rene.y - RANA_ALTO/2, 0);
            
            
            al_set_target_backbuffer(display);
            al_draw_scaled_bitmap(mundo_buffer, 0, 0, BUFFER_W, BUFFER_H, 0, 0, SCREEN_W, SCREEN_H, 0);

            al_flip_display();
        }
    }

    allegro_destroy();
    
    return 0;
}
