#include "allegro_funciones.h"
#include "mundo.h"
#include "juego.h"

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_image.h>

/*****************************************************
 * CONSTANTES PARA ALLEGRO
 *****************************************************/

#define FPS             REFRESCO

#define BUFFER_W        MUNDO_ANCHO
#define BUFFER_H        MUNDO_ALTO

#define SCREEN_SCALE    1.5
#define SCREEN_W        (BUFFER_W * SCREEN_SCALE)
#define SCREEN_H        (BUFFER_H * SCREEN_SCALE)


/***************************************************
 *  DEFINICIÓN DE VARIABLES GLOBALES
***************************************************/
ALLEGRO_DISPLAY * display;
ALLEGRO_EVENT_QUEUE * event_queue;
ALLEGRO_TIMER * timer;
ALLEGRO_BITMAP * mundo_buffer;
ALLEGRO_BITMAP * al_rene;

/**********************************************************************************
 * 
 * DEFINICION DE FUNCIONES GLOBALES
 * 
 *******************************************************************************+**/

bool allegro_startup (void)
{
    if (!al_init())
    {
        fprintf(stderr, "failed to initialize allegro!\n");
        return false;
    }
    
    if (!al_install_keyboard())
    {
        fprintf(stderr, "failed to initialize the keyboard!\n");
        return false;
    }
    
    timer = al_create_timer(1.0 / FPS);
    if (!timer)
    {
        fprintf(stderr, "failed to create timer!\n");
        return false;
    }
    
    event_queue = al_create_event_queue();
    if (!event_queue)
    {
        fprintf(stderr, "failed to create event_queue!\n");
        al_destroy_timer(timer);
        return false;
    }

    mundo_buffer = al_create_bitmap(BUFFER_W, BUFFER_H);
    if (!mundo_buffer) 
    {
        fprintf(stderr, "failed to create buffer!\n");
        al_destroy_timer(timer);
        al_destroy_event_queue(event_queue);
        return false;
    }
    
    al_rene = al_create_bitmap(RANA_ANCHO, RANA_ALTO);
    if (!al_rene) 
    {
        fprintf(stderr, "failed to create al_rene!\n");
        al_destroy_timer(timer);
        al_destroy_event_queue(event_queue);
        al_destroy_bitmap(mundo_buffer);
        return -1;
    }
    
    display = al_create_display(SCREEN_W, SCREEN_H);
    if (!display)
    {
        fprintf(stderr, "failed to create display!\n");
        al_destroy_timer(timer);
        al_destroy_event_queue(event_queue);
        al_destroy_bitmap(mundo_buffer);
        al_destroy_bitmap(al_rene);
        return false;
    }
    
    return true;
}


void allegro_destroy(void)
{
    al_destroy_bitmap(al_rene);
    al_destroy_bitmap(mundo_buffer);
    al_destroy_timer(timer);
    al_destroy_event_queue(event_queue);
    al_destroy_display(display);
}