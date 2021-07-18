

#ifndef ALLEGRO_FUNCIONES_H
#define ALLEGRO_FUNCIONES_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h> 
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_image.h>


/////////////////////////////////////////////////////////////////////////////////////////

/***********************************************************************
 * 
 * VARIABLES GLOBALES
 * 
 ***********************************************************************/

extern ALLEGRO_EVENT_QUEUE *event_queue;
extern ALLEGRO_TIMER * timer;


/***************************************************
 * DECLARACIÓN DE FUNCIONES GLOBALES
***************************************************/

bool allegro_startup (void);

void allegro_event_register(void);

void allegro_initialize_display(void);

void allegro_destroy(void);

bool allegro_teclas (ALLEGRO_EVENT *);

void allegro_redraw(void);

void allegro_audio (void);

/////////////////////////////////////////////////////////////////////////////////////////

#endif /* ALLEGRO_FUNCIONES_H */
