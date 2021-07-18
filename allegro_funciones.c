//#define RASPI_ALLEGRO
#ifndef RASPI_ALLEGRO
#include "allegro_funciones.h"
#include "juego.h"
#include "menu.h"

#include <stdbool.h>

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_color.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_ttf.h>

/*****************************************************
 * CONSTANTES PARA ALLEGRO
 *****************************************************/

#define FPS             REFRESCO

#define BUFFER_W        MUNDO_ANCHO
#define BUFFER_H        MUNDO_ALTO

#define SCREEN_SCALE    1
#define SCREEN_W        (BUFFER_W * SCREEN_SCALE)
#define SCREEN_H        (BUFFER_H * SCREEN_SCALE * 17/15)


/***************************************************
 *  DEFINICIÓN DE VARIABLES GLOBALES
***************************************************/
ALLEGRO_TIMER       * timer;
ALLEGRO_EVENT_QUEUE * event_queue;

/***************************************************
 *  DEFINICIÓN DE VARIABLES GLOBALES
***************************************************/
static ALLEGRO_DISPLAY     * display;
static ALLEGRO_BITMAP      * mundo_buffer;
static ALLEGRO_FONT        * font;
static ALLEGRO_BITMAP      * al_cocodrilo;
static ALLEGRO_SAMPLE      * sample_rana_salto;
static ALLEGRO_SAMPLE      * sample_rana_ahogada;
static ALLEGRO_SAMPLE      * sample_rana_chocada;
static ALLEGRO_SAMPLE      * sample_tiempo;


/*********************************************
 * 
 * SPRITES
 * 
 *********************************************/
#define RANA_FRAMES 2
#define TORTUGAS_FRAMES 4

typedef struct SPRITES
{
    ALLEGRO_BITMAP* _sheet;

    ALLEGRO_BITMAP* al_rene[RANA_FRAMES];
    ALLEGRO_BITMAP* al_rene_perdio;
    
    ALLEGRO_BITMAP* al_auto1;
    ALLEGRO_BITMAP* al_auto2;
    ALLEGRO_BITMAP* al_camion;

    ALLEGRO_BITMAP* al_tronco1;
    ALLEGRO_BITMAP* al_tronco2;
    ALLEGRO_BITMAP* al_tronco3;

    ALLEGRO_BITMAP* al_tortugas[TORTUGAS_FRAMES];
    
    ALLEGRO_BITMAP* al_llegada;
    ALLEGRO_BITMAP* al_fila_superior;
    ALLEGRO_BITMAP* al_agua;
    ALLEGRO_BITMAP* al_calle;
    ALLEGRO_BITMAP* al_fila_segura;

}SPRITES;

static SPRITES sprites;

/***************************************************
 *  DECLARACIÓN DE FUNCIONES LOCALES
***************************************************/
static void redraw_rana(void);
static void redraw_autos(void);
static void redraw_troncos(void);
static void redraw_tortugas(void);
static void redraw_tiempo(void);
static void redraw_llegada(void);
static void redraw_fondo(void);




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
    
    if (!al_install_audio()) {
        fprintf(stderr, "failed to initialize audio!\n");
        return -1;
    }

    if (!al_init_acodec_addon()) {
        fprintf(stderr, "failed to initialize audio codecs!\n");
        return -1;
    }

    if (!al_reserve_samples(4)) {
        fprintf(stderr, "failed to reserve samples!\n");
        return -1;
    }
    
    if (!al_init_font_addon()) // Inicializa el complemento de fuentes
    {
        fprintf(stderr, "failed to initialize the fonts!\n");
        return false;
    }
    
    if (!al_init_ttf_addon()) // Inicializa el complemento que permite reconocer fuentes ttf
    {
        fprintf(stderr, "failed to initialize the ttf addon!\n");
        al_shutdown_font_addon();
        return false;
        
    }
    
    if (!al_init_primitives_addon()) // Inicializa el complemento que permite reconocer fuentes ttf
    {
        fprintf(stderr, "failed to initialize the primitives addon!\n");
        al_shutdown_font_addon();
        al_shutdown_ttf_addon();
        return false;
        
    }
    
    if(!al_init_image_addon())
    {
        fprintf(stderr, "failed to initialize allegro images!\n");
        al_shutdown_font_addon();
        al_shutdown_ttf_addon();
        al_shutdown_primitives_addon();
        return false;
    }
    
    if (!al_install_keyboard())
    {
        fprintf(stderr, "failed to initialize the keyboard!\n");
        al_shutdown_font_addon();
        al_shutdown_ttf_addon();
        al_shutdown_primitives_addon();
        al_shutdown_image_addon();
        return false;
    }
    
    timer = al_create_timer(1.0 / FPS);
    if (!timer)
    {
        fprintf(stderr, "failed to create timer!\n");
        al_shutdown_font_addon();
        al_shutdown_ttf_addon();
        al_shutdown_primitives_addon();
        al_shutdown_image_addon();
        al_uninstall_keyboard();
        return false;
    }
    
    event_queue = al_create_event_queue();
    if (!event_queue)
    {
        fprintf(stderr, "failed to create event_queue!\n");
        al_shutdown_font_addon();
        al_shutdown_ttf_addon();
        al_shutdown_primitives_addon();
        al_shutdown_image_addon();
        al_uninstall_keyboard();
        al_destroy_timer(timer);
        return false;
    }

    mundo_buffer = al_create_bitmap(BUFFER_W, BUFFER_H);
    if (!mundo_buffer) 
    {
        fprintf(stderr, "failed to create buffer!\n");
        al_shutdown_font_addon();
        al_shutdown_ttf_addon();
        al_shutdown_primitives_addon();
        al_shutdown_image_addon();
        al_uninstall_keyboard();
        al_destroy_timer(timer);
        al_destroy_event_queue(event_queue);
        return false;
    }
    
    sprites._sheet = al_load_bitmap("spritesheet.png");
    if (!sprites._sheet) 
    {
        fprintf(stderr, "failed to create !sprites._sheet!\n");
        al_shutdown_font_addon();
        al_shutdown_ttf_addon();
        al_shutdown_primitives_addon();
        al_shutdown_image_addon();
        al_uninstall_keyboard();
        al_destroy_timer(timer);
        al_destroy_event_queue(event_queue);
        al_destroy_bitmap(mundo_buffer);
        return false;
    }
    
    sprites.al_rene[0]= al_create_sub_bitmap(sprites._sheet, 2, 30, 51, 36);
    if (!sprites.al_rene[0]) 
    {
        fprintf(stderr, "failed to create al_rene[0]!\n");
        al_shutdown_font_addon();
        al_shutdown_ttf_addon();
        al_shutdown_primitives_addon();
        al_shutdown_image_addon();
        al_uninstall_keyboard();
        al_destroy_timer(timer);
        al_destroy_event_queue(event_queue);
        al_destroy_bitmap(mundo_buffer);
        return false;
    }
    sprites.al_rene[1]= al_create_sub_bitmap(sprites._sheet, 174, 8, 53, 64);
    if (!sprites.al_rene[1]) 
    {
        fprintf(stderr, "failed to create al_rene[1]!\n");
        al_shutdown_font_addon();
        al_shutdown_ttf_addon();
        al_shutdown_primitives_addon();
        al_shutdown_image_addon();
        al_uninstall_keyboard();
        al_destroy_timer(timer);
        al_destroy_event_queue(event_queue);
        al_destroy_bitmap(mundo_buffer);
        al_destroy_bitmap(sprites.al_rene[0]);
        return false;
    }
    
    sprites.al_auto1 = al_create_sub_bitmap(sprites._sheet, 306, 482, 134, 69);
    if(!sprites.al_auto1)
    {
        fprintf(stderr, "failed to create al_auto_fila1!\n");
        al_shutdown_font_addon();
        al_shutdown_ttf_addon();
        al_shutdown_primitives_addon();
        al_shutdown_image_addon();
        al_uninstall_keyboard();
        al_destroy_timer(timer);
        al_destroy_event_queue(event_queue);
        al_destroy_bitmap(mundo_buffer);
        al_destroy_bitmap(sprites.al_rene[0]);
        al_destroy_bitmap(sprites.al_rene[1]);
        return false;
    }
    
    sprites.al_auto2 = al_create_sub_bitmap(sprites._sheet, 13, 483, 127, 67);
    if(!sprites.al_auto1)
    {
        fprintf(stderr, "failed to create al_auto_fila1!\n");
        al_shutdown_font_addon();
        al_shutdown_ttf_addon();
        al_shutdown_primitives_addon();
        al_shutdown_image_addon();
        al_uninstall_keyboard();
        al_destroy_timer(timer);
        al_destroy_event_queue(event_queue);
        al_destroy_bitmap(mundo_buffer);
        al_destroy_bitmap(sprites.al_rene[0]);
        al_destroy_bitmap(sprites.al_rene[1]);
        al_destroy_bitmap(sprites.al_auto1);
        return false;
    }
    
    sprites.al_camion = al_create_sub_bitmap(sprites._sheet, 11, 406, 177, 65);
    if(!sprites.al_camion)
    {
        fprintf(stderr, "failed to create al_camion!\n");
        al_shutdown_font_addon();
        al_shutdown_ttf_addon();
        al_shutdown_primitives_addon();
        al_shutdown_image_addon();
        al_uninstall_keyboard();
        al_destroy_timer(timer);
        al_destroy_event_queue(event_queue);
        al_destroy_bitmap(mundo_buffer);
        al_destroy_bitmap(sprites.al_rene[0]);
        al_destroy_bitmap(sprites.al_rene[1]);
        al_destroy_bitmap(sprites.al_auto1);
        al_destroy_bitmap(sprites.al_auto2);
        return false;
    }
    
    sprites.al_tronco1 = al_create_sub_bitmap(sprites._sheet, 388, 254, 184, 64);
    if(!sprites.al_tronco1)
    {
        fprintf(stderr, "failed to create al_tronco1!\n");
        al_shutdown_font_addon();
        al_shutdown_ttf_addon();
        al_shutdown_primitives_addon();
        al_shutdown_image_addon();
        al_uninstall_keyboard();
        al_destroy_timer(timer);
        al_destroy_event_queue(event_queue);
        al_destroy_bitmap(mundo_buffer);
        al_destroy_bitmap(sprites.al_rene[0]);
        al_destroy_bitmap(sprites.al_rene[1]);
        al_destroy_bitmap(sprites.al_auto1);
        al_destroy_bitmap(sprites.al_auto2);
        al_destroy_bitmap(sprites.al_camion);
        return false;
    }
    
    sprites.al_tronco2 = al_create_sub_bitmap(sprites._sheet, 14, 255, 353, 63);
    if(!sprites.al_tronco2)
    {
        fprintf(stderr, "failed to create al_tronco2!\n");
        al_shutdown_font_addon();
        al_shutdown_ttf_addon();
        al_shutdown_primitives_addon();
        al_shutdown_image_addon();
        al_uninstall_keyboard();
        al_destroy_timer(timer);
        al_destroy_event_queue(event_queue);
        al_destroy_bitmap(mundo_buffer);
        al_destroy_bitmap(sprites.al_rene[0]);
        al_destroy_bitmap(sprites.al_rene[1]);
        al_destroy_bitmap(sprites.al_auto1);
        al_destroy_bitmap(sprites.al_auto2);
        al_destroy_bitmap(sprites.al_camion);
        al_destroy_bitmap(sprites.al_tronco1);
        return false;
    }
    
    sprites.al_tronco3 = al_create_sub_bitmap(sprites._sheet, 14, 325, 272, 64);
    if(!sprites.al_tronco3)
    {
        fprintf(stderr, "failed to create al_tronco3!\n");
        al_shutdown_font_addon();
        al_shutdown_ttf_addon();
        al_shutdown_primitives_addon();
        al_shutdown_image_addon();
        al_uninstall_keyboard();
        al_destroy_timer(timer);
        al_destroy_event_queue(event_queue);
        al_destroy_bitmap(mundo_buffer);
        al_destroy_bitmap(sprites.al_rene[0]);
        al_destroy_bitmap(sprites.al_rene[1]);
        al_destroy_bitmap(sprites.al_auto1);
        al_destroy_bitmap(sprites.al_auto2);
        al_destroy_bitmap(sprites.al_camion);
        al_destroy_bitmap(sprites.al_tronco1);
        al_destroy_bitmap(sprites.al_tronco2);
        return false;
    }
    
    sprites.al_tortugas[0]= al_create_sub_bitmap(sprites._sheet, 405, 10, 70, 56);
    if(!sprites.al_tortugas[0])
    {
        fprintf(stderr, "failed to create al_tortugas[0]!\n");
        al_shutdown_font_addon();
        al_shutdown_ttf_addon();
        al_shutdown_primitives_addon();
        al_shutdown_image_addon();
        al_uninstall_keyboard();
        al_destroy_timer(timer);
        al_destroy_event_queue(event_queue);
        al_destroy_bitmap(mundo_buffer);
        al_destroy_bitmap(sprites.al_rene[0]);
        al_destroy_bitmap(sprites.al_rene[1]);
        al_destroy_bitmap(sprites.al_auto1);
        al_destroy_bitmap(sprites.al_auto2);
        al_destroy_bitmap(sprites.al_camion);
        al_destroy_bitmap(sprites.al_tronco1);
        al_destroy_bitmap(sprites.al_tronco2);
        al_destroy_bitmap(sprites.al_tronco3);
    }
    
    sprites.al_tortugas[1]= al_create_sub_bitmap(sprites._sheet, 81, 81, 71, 68);
    if(!sprites.al_tortugas[1])
    {
        fprintf(stderr, "failed to create al_tortugas[1]!\n");
        al_shutdown_font_addon();
        al_shutdown_ttf_addon();
        al_shutdown_primitives_addon();
        al_shutdown_image_addon();
        al_uninstall_keyboard();
        al_destroy_timer(timer);
        al_destroy_event_queue(event_queue);
        al_destroy_bitmap(mundo_buffer);
        al_destroy_bitmap(sprites.al_rene[0]);
        al_destroy_bitmap(sprites.al_rene[1]);
        al_destroy_bitmap(sprites.al_auto1);
        al_destroy_bitmap(sprites.al_auto2);
        al_destroy_bitmap(sprites.al_camion);
        al_destroy_bitmap(sprites.al_tronco1);
        al_destroy_bitmap(sprites.al_tronco2);
        al_destroy_bitmap(sprites.al_tronco3);
        al_destroy_bitmap(sprites.al_tortugas[0]);
    }
    sprites.al_tortugas[2]= al_create_sub_bitmap(sprites._sheet, 378, 91, 66, 50);
    if(!sprites.al_tortugas[2])
    {
        fprintf(stderr, "failed to create al_tortugas[2]!\n");
        al_shutdown_font_addon();
        al_shutdown_ttf_addon();
        al_shutdown_primitives_addon();
        al_shutdown_image_addon();
        al_uninstall_keyboard();
        al_destroy_timer(timer);
        al_destroy_event_queue(event_queue);
        al_destroy_bitmap(mundo_buffer);
        al_destroy_bitmap(sprites.al_rene[0]);
        al_destroy_bitmap(sprites.al_rene[1]);
        al_destroy_bitmap(sprites.al_auto1);
        al_destroy_bitmap(sprites.al_auto2);
        al_destroy_bitmap(sprites.al_camion);
        al_destroy_bitmap(sprites.al_tronco1);
        al_destroy_bitmap(sprites.al_tronco2);
        al_destroy_bitmap(sprites.al_tronco3);
        al_destroy_bitmap(sprites.al_tortugas[0]);
        al_destroy_bitmap(sprites.al_tortugas[1]);
    }
    
    sprites.al_tortugas[3]= al_create_sub_bitmap(sprites._sheet, 511, 92, 51, 46);
    if(!sprites.al_tortugas[3])
    {
        fprintf(stderr, "failed to create al_tortugas[3]!\n");
        al_shutdown_font_addon();
        al_shutdown_ttf_addon();
        al_shutdown_primitives_addon();
        al_shutdown_image_addon();
        al_uninstall_keyboard();
        al_destroy_timer(timer);
        al_destroy_event_queue(event_queue);
        al_destroy_bitmap(mundo_buffer);
        al_destroy_bitmap(sprites.al_rene[0]);
        al_destroy_bitmap(sprites.al_rene[1]);
        al_destroy_bitmap(sprites.al_auto1);
        al_destroy_bitmap(sprites.al_auto2);
        al_destroy_bitmap(sprites.al_camion);
        al_destroy_bitmap(sprites.al_tronco1);
        al_destroy_bitmap(sprites.al_tronco2);
        al_destroy_bitmap(sprites.al_tronco3);
        al_destroy_bitmap(sprites.al_tortugas[0]);
        al_destroy_bitmap(sprites.al_tortugas[1]);
        al_destroy_bitmap(sprites.al_tortugas[2]);
    }
    
    sprites.al_llegada = al_create_sub_bitmap(sprites._sheet, 511, 159, 55, 79);
    if(!sprites.al_llegada)
    {
        fprintf(stderr, "failed to create al_llegada!\n");
        al_shutdown_font_addon();
        al_shutdown_ttf_addon();
        al_shutdown_primitives_addon();
        al_shutdown_image_addon();
        al_uninstall_keyboard();
        al_destroy_timer(timer);
        al_destroy_event_queue(event_queue);
        al_destroy_bitmap(mundo_buffer);
        al_destroy_bitmap(sprites.al_rene[0]);
        al_destroy_bitmap(sprites.al_rene[1]);
        al_destroy_bitmap(sprites.al_auto1);
        al_destroy_bitmap(sprites.al_auto2);
        al_destroy_bitmap(sprites.al_camion);
        al_destroy_bitmap(sprites.al_tronco1);
        al_destroy_bitmap(sprites.al_tronco2);
        al_destroy_bitmap(sprites.al_tronco3);
        al_destroy_bitmap(sprites.al_tortugas[0]);
        al_destroy_bitmap(sprites.al_tortugas[1]);
        al_destroy_bitmap(sprites.al_tortugas[2]);
        al_destroy_bitmap(sprites.al_tortugas[3]);
    }
    
    sprites.al_fila_superior = al_create_sub_bitmap(sprites._sheet, 409, 159, 79, 79);
    if(!sprites.al_fila_superior)
    {
        fprintf(stderr, "failed to create al_fila_superior!\n");
        al_shutdown_font_addon();
        al_shutdown_ttf_addon();
        al_shutdown_primitives_addon();
        al_shutdown_image_addon();
        al_uninstall_keyboard();
        al_destroy_timer(timer);
        al_destroy_event_queue(event_queue);
        al_destroy_bitmap(mundo_buffer);
        al_destroy_bitmap(sprites.al_rene[0]);
        al_destroy_bitmap(sprites.al_rene[1]);
        al_destroy_bitmap(sprites.al_auto1);
        al_destroy_bitmap(sprites.al_auto2);
        al_destroy_bitmap(sprites.al_camion);
        al_destroy_bitmap(sprites.al_tronco1);
        al_destroy_bitmap(sprites.al_tronco2);
        al_destroy_bitmap(sprites.al_tronco3);
        al_destroy_bitmap(sprites.al_tortugas[0]);
        al_destroy_bitmap(sprites.al_tortugas[1]);
        al_destroy_bitmap(sprites.al_tortugas[2]);
        al_destroy_bitmap(sprites.al_tortugas[3]);
        al_destroy_bitmap(sprites.al_llegada);
    }
    
    sprites.al_agua = al_create_sub_bitmap(sprites._sheet, 230, 160, 76, 77);
    if(!sprites.al_agua)
    {
        fprintf(stderr, "failed to create al_agua!\n");
        al_shutdown_font_addon();
        al_shutdown_ttf_addon();
        al_shutdown_primitives_addon();
        al_shutdown_image_addon();
        al_uninstall_keyboard();
        al_destroy_timer(timer);
        al_destroy_event_queue(event_queue);
        al_destroy_bitmap(mundo_buffer);
        al_destroy_bitmap(sprites.al_rene[0]);
        al_destroy_bitmap(sprites.al_rene[1]);
        al_destroy_bitmap(sprites.al_auto1);
        al_destroy_bitmap(sprites.al_auto2);
        al_destroy_bitmap(sprites.al_camion);
        al_destroy_bitmap(sprites.al_tronco1);
        al_destroy_bitmap(sprites.al_tronco2);
        al_destroy_bitmap(sprites.al_tronco3);
        al_destroy_bitmap(sprites.al_tortugas[0]);
        al_destroy_bitmap(sprites.al_tortugas[1]);
        al_destroy_bitmap(sprites.al_tortugas[2]);
        al_destroy_bitmap(sprites.al_tortugas[3]);
        al_destroy_bitmap(sprites.al_llegada);
        al_destroy_bitmap(sprites.al_fila_superior);
    }
    
    sprites.al_calle = al_create_sub_bitmap(sprites._sheet, 319, 160, 79, 76);
    if(!sprites.al_calle)
    {
        fprintf(stderr, "failed to create al_calle!\n");
        al_shutdown_font_addon();
        al_shutdown_ttf_addon();
        al_shutdown_primitives_addon();
        al_shutdown_image_addon();
        al_uninstall_keyboard();
        al_destroy_timer(timer);
        al_destroy_event_queue(event_queue);
        al_destroy_bitmap(mundo_buffer);
        al_destroy_bitmap(sprites.al_rene[0]);
        al_destroy_bitmap(sprites.al_rene[1]);
        al_destroy_bitmap(sprites.al_auto1);
        al_destroy_bitmap(sprites.al_auto2);
        al_destroy_bitmap(sprites.al_camion);
        al_destroy_bitmap(sprites.al_tronco1);
        al_destroy_bitmap(sprites.al_tronco2);
        al_destroy_bitmap(sprites.al_tronco3);
        al_destroy_bitmap(sprites.al_tortugas[0]);
        al_destroy_bitmap(sprites.al_tortugas[1]);
        al_destroy_bitmap(sprites.al_tortugas[2]);
        al_destroy_bitmap(sprites.al_tortugas[3]);
        al_destroy_bitmap(sprites.al_llegada);
        al_destroy_bitmap(sprites.al_fila_superior);
        al_destroy_bitmap(sprites.al_agua);
    }
    
    sprites.al_fila_segura = al_create_sub_bitmap(sprites._sheet, 138, 159, 79, 79);
    if(!sprites.al_fila_segura)
    {
        fprintf(stderr, "failed to create al_fila_segura!\n");
        al_shutdown_font_addon();
        al_shutdown_ttf_addon();
        al_shutdown_primitives_addon();
        al_shutdown_image_addon();
        al_uninstall_keyboard();
        al_destroy_timer(timer);
        al_destroy_event_queue(event_queue);
        al_destroy_bitmap(mundo_buffer);
        al_destroy_bitmap(sprites.al_rene[0]);
        al_destroy_bitmap(sprites.al_rene[1]);
        al_destroy_bitmap(sprites.al_auto1);
        al_destroy_bitmap(sprites.al_auto2);
        al_destroy_bitmap(sprites.al_camion);
        al_destroy_bitmap(sprites.al_tronco1);
        al_destroy_bitmap(sprites.al_tronco2);
        al_destroy_bitmap(sprites.al_tronco3);
        al_destroy_bitmap(sprites.al_tortugas[0]);
        al_destroy_bitmap(sprites.al_tortugas[1]);
        al_destroy_bitmap(sprites.al_tortugas[2]);
        al_destroy_bitmap(sprites.al_tortugas[3]);
        al_destroy_bitmap(sprites.al_llegada);
        al_destroy_bitmap(sprites.al_fila_superior);
        al_destroy_bitmap(sprites.al_agua);
        al_destroy_bitmap(sprites.al_calle);
    }
    
    sprites.al_rene_perdio = al_create_sub_bitmap(sprites._sheet, 301, 330, 69, 53);
    if(!sprites.al_rene_perdio)
    {
        fprintf(stderr, "failed to create al_rene_perdio!\n");
        al_shutdown_font_addon();
        al_shutdown_ttf_addon();
        al_shutdown_primitives_addon();
        al_shutdown_image_addon();
        al_uninstall_keyboard();
        al_destroy_timer(timer);
        al_destroy_event_queue(event_queue);
        al_destroy_bitmap(mundo_buffer);
        al_destroy_bitmap(sprites.al_rene[0]);
        al_destroy_bitmap(sprites.al_rene[1]);
        al_destroy_bitmap(sprites.al_auto1);
        al_destroy_bitmap(sprites.al_auto2);
        al_destroy_bitmap(sprites.al_camion);
        al_destroy_bitmap(sprites.al_tronco1);
        al_destroy_bitmap(sprites.al_tronco2);
        al_destroy_bitmap(sprites.al_tronco3);
        al_destroy_bitmap(sprites.al_tortugas[0]);
        al_destroy_bitmap(sprites.al_tortugas[1]);
        al_destroy_bitmap(sprites.al_tortugas[2]);
        al_destroy_bitmap(sprites.al_tortugas[3]);
        al_destroy_bitmap(sprites.al_llegada);
        al_destroy_bitmap(sprites.al_fila_superior);
        al_destroy_bitmap(sprites.al_agua);
        al_destroy_bitmap(sprites.al_calle);
        al_destroy_bitmap(sprites.al_fila_segura);
    }
    
    al_cocodrilo= al_load_bitmap("cocodrilo.png");
    if (!al_cocodrilo)
    {
        fprintf(stderr, "failed to create al_cocodrilo!\n");
        al_shutdown_font_addon();
        al_shutdown_ttf_addon();
        al_shutdown_primitives_addon();
        al_shutdown_image_addon();
        al_uninstall_keyboard();
        al_destroy_timer(timer);
        al_destroy_event_queue(event_queue);
        al_destroy_bitmap(mundo_buffer);
        al_destroy_bitmap(sprites.al_rene[0]);
        al_destroy_bitmap(sprites.al_rene[1]);
        al_destroy_bitmap(sprites.al_auto1);
        al_destroy_bitmap(sprites.al_auto2);
        al_destroy_bitmap(sprites.al_camion);
        al_destroy_bitmap(sprites.al_tronco1);
        al_destroy_bitmap(sprites.al_tronco2);
        al_destroy_bitmap(sprites.al_tronco3);
        al_destroy_bitmap(sprites.al_tortugas[0]);
        al_destroy_bitmap(sprites.al_tortugas[1]);
        al_destroy_bitmap(sprites.al_tortugas[2]);
        al_destroy_bitmap(sprites.al_tortugas[3]);
        al_destroy_bitmap(sprites.al_llegada);
        al_destroy_bitmap(sprites.al_fila_superior);
        al_destroy_bitmap(sprites.al_agua);
        al_destroy_bitmap(sprites.al_calle);
        al_destroy_bitmap(sprites.al_fila_segura);
        al_destroy_bitmap(sprites.al_rene_perdio);
        return false;
    }

    
    display = al_create_display(SCREEN_W, SCREEN_H);
    if (!display)
    {
        fprintf(stderr, "failed to create display!\n");
        al_shutdown_font_addon();
        al_shutdown_ttf_addon();
        al_shutdown_primitives_addon();
        al_shutdown_image_addon();
        al_uninstall_keyboard();
        al_destroy_timer(timer);
        al_destroy_event_queue(event_queue);
        al_destroy_bitmap(mundo_buffer);
        al_destroy_bitmap(sprites.al_rene[0]);
        al_destroy_bitmap(sprites.al_rene[1]);
        al_destroy_bitmap(sprites.al_auto1);
        al_destroy_bitmap(sprites.al_auto2);
        al_destroy_bitmap(sprites.al_camion);
        al_destroy_bitmap(sprites.al_tronco1);
        al_destroy_bitmap(sprites.al_tronco2);
        al_destroy_bitmap(sprites.al_tronco3);
        al_destroy_bitmap(sprites.al_tortugas[0]);
        al_destroy_bitmap(sprites.al_tortugas[1]);
        al_destroy_bitmap(sprites.al_tortugas[2]);
        al_destroy_bitmap(sprites.al_tortugas[3]);
        al_destroy_bitmap(sprites.al_llegada);
        al_destroy_bitmap(sprites.al_fila_superior);
        al_destroy_bitmap(sprites.al_agua);
        al_destroy_bitmap(sprites.al_calle);
        al_destroy_bitmap(sprites.al_fila_segura);
        al_destroy_bitmap(sprites.al_rene_perdio);
        al_destroy_bitmap(al_cocodrilo);
        al_destroy_display(display);
        return false;
    }
    
    
    
    sample_rana_salto = al_load_sample("sound-frogger-hop.wav");
    if (!sample_rana_salto)
    {
        printf("Audio clip sample_rana_salto not loaded!\n");
        al_shutdown_font_addon();
        al_shutdown_ttf_addon();
        al_shutdown_primitives_addon();
        al_shutdown_image_addon();
        al_uninstall_keyboard();
        al_destroy_timer(timer);
        al_destroy_event_queue(event_queue);
        al_destroy_bitmap(mundo_buffer);
        al_destroy_bitmap(sprites.al_rene[0]);
        al_destroy_bitmap(sprites.al_rene[1]);
        al_destroy_bitmap(sprites.al_auto1);
        al_destroy_bitmap(sprites.al_auto2);
        al_destroy_bitmap(sprites.al_camion);
        al_destroy_bitmap(sprites.al_tronco1);
        al_destroy_bitmap(sprites.al_tronco2);
        al_destroy_bitmap(sprites.al_tronco3);
        al_destroy_bitmap(sprites.al_tortugas[0]);
        al_destroy_bitmap(sprites.al_tortugas[1]);
        al_destroy_bitmap(sprites.al_tortugas[2]);
        al_destroy_bitmap(sprites.al_tortugas[3]);
        al_destroy_bitmap(sprites.al_llegada);
        al_destroy_bitmap(sprites.al_fila_superior);
        al_destroy_bitmap(sprites.al_agua);
        al_destroy_bitmap(sprites.al_calle);
        al_destroy_bitmap(sprites.al_fila_segura);
        al_destroy_bitmap(sprites.al_rene_perdio);
        al_destroy_bitmap(al_cocodrilo);
        al_destroy_display(display);
        return false;
    }
    
    sample_rana_ahogada = al_load_sample("sound-frogger-squash.wav");
    if (!sample_rana_ahogada)
    {
        printf("Audio clip sample_rana_salto not loaded!\n");
        al_shutdown_font_addon();
        al_shutdown_ttf_addon();
        al_shutdown_primitives_addon();
        al_shutdown_image_addon();
        al_uninstall_keyboard();
        al_destroy_timer(timer);
        al_destroy_event_queue(event_queue);
        al_destroy_bitmap(mundo_buffer);
        al_destroy_bitmap(sprites.al_rene[0]);
        al_destroy_bitmap(sprites.al_rene[1]);
        al_destroy_bitmap(sprites.al_auto1);
        al_destroy_bitmap(sprites.al_auto2);
        al_destroy_bitmap(sprites.al_camion);
        al_destroy_bitmap(sprites.al_tronco1);
        al_destroy_bitmap(sprites.al_tronco2);
        al_destroy_bitmap(sprites.al_tronco3);
        al_destroy_bitmap(sprites.al_tortugas[0]);
        al_destroy_bitmap(sprites.al_tortugas[1]);
        al_destroy_bitmap(sprites.al_tortugas[2]);
        al_destroy_bitmap(sprites.al_tortugas[3]);
        al_destroy_bitmap(sprites.al_llegada);
        al_destroy_bitmap(sprites.al_fila_superior);
        al_destroy_bitmap(sprites.al_agua);
        al_destroy_bitmap(sprites.al_calle);
        al_destroy_bitmap(sprites.al_fila_segura);
        al_destroy_bitmap(sprites.al_rene_perdio);
        al_destroy_bitmap(al_cocodrilo);
        al_destroy_display(display);
        al_destroy_sample(sample_rana_salto);
        return false;
    }
    
    sample_rana_chocada = al_load_sample("sound-frogger-plunk.wav");
    if (!sample_rana_chocada)
    {
        printf("Audio clip sample_rana_salto not loaded!\n");
        al_shutdown_font_addon();
        al_shutdown_ttf_addon();
        al_shutdown_primitives_addon();
        al_shutdown_image_addon();
        al_uninstall_keyboard();
        al_destroy_timer(timer);
        al_destroy_event_queue(event_queue);
        al_destroy_bitmap(mundo_buffer);
        al_destroy_bitmap(sprites.al_rene[0]);
        al_destroy_bitmap(sprites.al_rene[1]);
        al_destroy_bitmap(sprites.al_auto1);
        al_destroy_bitmap(sprites.al_auto2);
        al_destroy_bitmap(sprites.al_camion);
        al_destroy_bitmap(sprites.al_tronco1);
        al_destroy_bitmap(sprites.al_tronco2);
        al_destroy_bitmap(sprites.al_tronco3);
        al_destroy_bitmap(sprites.al_tortugas[0]);
        al_destroy_bitmap(sprites.al_tortugas[1]);
        al_destroy_bitmap(sprites.al_tortugas[2]);
        al_destroy_bitmap(sprites.al_tortugas[3]);
        al_destroy_bitmap(sprites.al_llegada);
        al_destroy_bitmap(sprites.al_fila_superior);
        al_destroy_bitmap(sprites.al_agua);
        al_destroy_bitmap(sprites.al_calle);
        al_destroy_bitmap(sprites.al_fila_segura);
        al_destroy_bitmap(sprites.al_rene_perdio);
        al_destroy_bitmap(al_cocodrilo);
        al_destroy_display(display);
        al_destroy_sample(sample_rana_salto);
        al_destroy_sample(sample_rana_ahogada);
        return false;
    }
    
    sample_tiempo = al_load_sample("sound-frogger-time.wav");
    if (!sample_tiempo)
    {
        printf("Audio clip sample_rana_salto not loaded!\n");
        al_shutdown_font_addon();
        al_shutdown_ttf_addon();
        al_shutdown_primitives_addon();
        al_shutdown_image_addon();
        al_uninstall_keyboard();
        al_destroy_timer(timer);
        al_destroy_event_queue(event_queue);
        al_destroy_bitmap(mundo_buffer);
        al_destroy_bitmap(sprites.al_rene[0]);
        al_destroy_bitmap(sprites.al_rene[1]);
        al_destroy_bitmap(sprites.al_auto1);
        al_destroy_bitmap(sprites.al_auto2);
        al_destroy_bitmap(sprites.al_camion);
        al_destroy_bitmap(sprites.al_tronco1);
        al_destroy_bitmap(sprites.al_tronco2);
        al_destroy_bitmap(sprites.al_tronco3);
        al_destroy_bitmap(sprites.al_tortugas[0]);
        al_destroy_bitmap(sprites.al_tortugas[1]);
        al_destroy_bitmap(sprites.al_tortugas[2]);
        al_destroy_bitmap(sprites.al_tortugas[3]);
        al_destroy_bitmap(sprites.al_llegada);
        al_destroy_bitmap(sprites.al_fila_superior);
        al_destroy_bitmap(sprites.al_agua);
        al_destroy_bitmap(sprites.al_calle);
        al_destroy_bitmap(sprites.al_fila_segura);
        al_destroy_bitmap(sprites.al_rene_perdio);
        al_destroy_bitmap(al_cocodrilo);
        al_destroy_display(display);
        al_destroy_sample(sample_rana_salto);
        al_destroy_sample(sample_rana_ahogada);
        al_destroy_sample(sample_rana_chocada);
        return false;
    }
    
    return true;
}

void allegro_event_register(void)
{
    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_register_event_source(event_queue, al_get_keyboard_event_source());
}

void allegro_fuentes(void)
{
    font = al_load_ttf_font("Karumbi-Regular.ttf", 40, 0); //asigno la fuente que voy a usar

    if (!font)
    {

        fprintf(stderr, "Could not load 'Karumbi-Regular.ttf'.\n");

        return;
    }
}

bool allegro_teclas (ALLEGRO_EVENT * ev)
{
    bool do_exit = false;
    
    if (ev->type == ALLEGRO_EVENT_DISPLAY_CLOSE)
    {
        do_exit = true;
    }

    if (ev->type == ALLEGRO_EVENT_KEY_DOWN)
    {
        switch (ev->keyboard.keycode)
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
                
            case ALLEGRO_KEY_ENTER:
                key_pressed[KEY_ENTER] = true;
                break;
        }
    }
    
    else if (ev->type == ALLEGRO_EVENT_KEY_UP)
    {
        switch (ev->keyboard.keycode) {
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
            
            case ALLEGRO_KEY_ENTER:
                key_pressed[KEY_ENTER] = false;
                break;

            case ALLEGRO_KEY_ESCAPE:
                do_exit = true;
                break;
        }
    }
    
    return do_exit;
}


void allegro_initialize_bitmaps(void)
{ 
    al_set_target_bitmap(al_get_backbuffer(display));
    al_clear_to_color(al_map_rgb(0, 0, 0));
    al_flip_display();
}

/*************************************************************************
 * 
 * REDRAW
 * 
 ***************************************************************************/


void allegro_redraw(void)
{
    char print_string[100]; //Creo un arreglo que "simularía" un string (el último elemento es 0 por ser el terminador)
    const char *p_to_string = print_string; //creo un puntero constante para usar en "al_draw_text"
    
    if(estado_juego != MENU && estado_juego != PAUSA && estado_juego != PASAR_NIVEL && estado_juego != PERDER)
    {
        
        al_set_target_bitmap(mundo_buffer);

        //IMPRESION DEL FONDO
        redraw_fondo();

        //IMPRESION DE LOS AUTOS
        redraw_autos();

        //impresión de troncos
        redraw_troncos();

        //IMPRESIÓN DE TORTUGAS
        redraw_tortugas();
        
        //IMPRESION DE LAS CASILLAS DE LLEGADA
        redraw_llegada();

        //IMPRESIÓN DE LA RANA
        redraw_rana();
        
        al_set_target_backbuffer(display);
        al_clear_to_color(al_map_rgb(0,0,0));
        
        al_draw_scaled_bitmap(mundo_buffer, 0, 0, BUFFER_W, BUFFER_H, 0, SCREEN_W/15, SCREEN_W, SCREEN_H-2*SCREEN_W/15, 0);
        
        //IMPRESION DEL TIEMPO RESTANTE
        redraw_tiempo();
        
        
        //***********************************************************************
        //ZONA DE PRUEBAS DE tratar de imprimir variables con allegro
        //***********************************************************************
        
        snprintf(print_string, 100, "%lu", vidas_restantes);
                       
        al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W - 115, 0, ALLEGRO_ALIGN_LEFT, "Vidas:"); //Imprimo el "string" creado que contiene las vidas restantes       
        al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W - 40, 0, ALLEGRO_ALIGN_LEFT, p_to_string); //Imprimo el "string" creado que contiene las vidas restantes
        
        /* El mismo concepto lo puedo usar para imprimir el nivel actual, ya que está previsto que haya menos de 10 niveles*/
        snprintf(print_string, 100, "%lu", nivel);
        
        al_draw_text(font, al_map_rgb(255, 255, 255), 5, 0, ALLEGRO_ALIGN_LEFT, "Nivel: "); //Imprimo el "string" creado que contiene las vidas restantes
        al_draw_text(font, al_map_rgb(255, 255, 255), 80, 0, ALLEGRO_ALIGN_LEFT, p_to_string); //Imprimo el "string" creado que contiene las vidas restantes

        
        snprintf(print_string, 100, "%lu", puntaje_juego);
        
        al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W/2, 0, ALLEGRO_ALIGN_CENTER, "Puntaje: ");
        al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W*2/3-30, 0, ALLEGRO_ALIGN_CENTER, p_to_string);
        
        
        
        
    }
    
    else if(estado_juego == PASAR_NIVEL)
    {
        al_set_target_backbuffer(display);
        redraw_fondo();
        
        if(timer_pasar_nivel > UN_SEGUNDO*5)
        {
            snprintf(print_string, 100, "%lu", puntaje_juego);
        
            al_draw_filled_rectangle(SCREEN_W/3,(SCREEN_H/3)-SCREEN_H/40, SCREEN_W*2/3,(SCREEN_H/2)+SCREEN_H/20, al_map_rgb(28,40,51));
            
            al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W/2, SCREEN_H/3.0, ALLEGRO_ALIGN_CENTER, "Puntaje ");
            al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W/2, SCREEN_H/2.2, ALLEGRO_ALIGN_CENTER, p_to_string);
        }
        else
        {
            snprintf(print_string, 100, "%u", nivel+1);
  
            al_draw_filled_rectangle(SCREEN_W/3,(SCREEN_H/3)-SCREEN_H/40, SCREEN_W*2/3,(SCREEN_H/2)+SCREEN_H/20, al_map_rgb(28,40,51));
            
            al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W/2.0, SCREEN_H * 1 / 3.0, ALLEGRO_ALIGN_CENTER, "Nivel ");
            al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W/2.0, SCREEN_H/2.2, ALLEGRO_ALIGN_CENTER, p_to_string);
        }
    }
    
    else if(estado_juego == PERDER)     //en el caso de que se pierde
    {
        al_set_target_backbuffer(display);
        redraw_fondo();
        
        if(timer_perder > UN_SEGUNDO*5)
        {
            snprintf(print_string, 100, "%u", nivel);
            al_draw_filled_rectangle(SCREEN_W/3-15,(SCREEN_H/3.0), SCREEN_W*2/3 + 15 + 10,(SCREEN_H/2.0)-15, al_map_rgb(25,25,112));
            al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W/2.0, SCREEN_H * 1 / 3.0, ALLEGRO_ALIGN_CENTER, "NIVEL ALCANZADO");
            al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W/2.0, SCREEN_H/2.5, ALLEGRO_ALIGN_CENTER, p_to_string);
                       
        }
        
        else
        {
            snprintf(print_string, 100, "%lu", puntaje_juego);
            
            al_draw_filled_rectangle(SCREEN_W/6,(SCREEN_H/3.0), SCREEN_W*5.0/6 ,(SCREEN_H/(8.0/5)), al_map_rgb(25,25,112));
            
            al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W/3, SCREEN_H * 1 / 3.0, ALLEGRO_ALIGN_CENTER, "SU PUNTAJE: ");
            al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W/3, SCREEN_H/2, ALLEGRO_ALIGN_CENTER, p_to_string);
            
            snprintf(print_string, 100, "%lu", high_score);
            al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W*2/3, SCREEN_H * 1 / 3.0, ALLEGRO_ALIGN_CENTER, "HIGHSCORE: ");
            al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W*2/3, SCREEN_H/2, ALLEGRO_ALIGN_CENTER, p_to_string);
        }
    }
       else if (estado_juego == MENU)      //casos del menú visualmente
    {
        al_set_target_backbuffer(display);
        redraw_fondo ();
         al_draw_filled_rectangle(SCREEN_W/3,(SCREEN_H/16)-SCREEN_H/40, SCREEN_W*2/3,(SCREEN_H/16)+SCREEN_H/10, al_map_rgb(28,40,51));
        al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W/2.0, SCREEN_H*1.0/16, ALLEGRO_ALIGN_CENTER, "MENU PRINCIPAL");
        al_draw_filled_rectangle(SCREEN_W/3+70,(SCREEN_H/3), SCREEN_W*2/3-70,(SCREEN_H/3)+SCREEN_H/15, al_map_rgb(25,25,112));
        al_draw_filled_rectangle(SCREEN_W/3-20,(SCREEN_H/2.0), SCREEN_W*2/3+20,(SCREEN_H/2.0)+SCREEN_H/15, al_map_rgb(25,25,112));      //recuardo de sleeción para los niveles
                
                al_draw_text(font, al_map_rgb(124,252, 0), SCREEN_W/2.0, SCREEN_H/2.0, ALLEGRO_ALIGN_CENTER, "SELECCIONAR NIVEL");
                al_draw_filled_rectangle(SCREEN_W/3+70,(SCREEN_H*2.0/3), SCREEN_W*2/3-70,(SCREEN_H*2.0/3)+SCREEN_H/15, al_map_rgb(25,25,112));    
                
                al_draw_text(font, al_map_rgb(124,252, 0), SCREEN_W/2.0, SCREEN_H*2.0/3, ALLEGRO_ALIGN_CENTER, "SALIR");
                     
                
                al_draw_text(font, al_map_rgb(124,252, 0), SCREEN_W/2.0, SCREEN_H/3.0, ALLEGRO_ALIGN_CENTER, "PLAY");
        
        switch(selector_menu)
        {
           
            case PLAY:      //caso en donde se elige la opción de jugar directamente al juego e inicia en el nivel número 1
            {   
                 
              
             
                
                al_draw_filled_rectangle(SCREEN_W/3+70,(SCREEN_H/3), SCREEN_W*2/3-70,(SCREEN_H/3)+SCREEN_H/15, al_map_rgb(247,220,110));
                
                al_draw_text(font, al_map_rgb(124,36,28), SCREEN_W/2.0, SCREEN_H/3.0, ALLEGRO_ALIGN_CENTER, "PLAY"); //imrpime el texto de la opción
                
                
                
                //al_draw_filled_circle(rand() % 640, rand() % 480, rand() % 64, al_map_rgb(rand() % 255, 127, 80));
                break;
            }
            case LEVEL: //opción de niveles para elegirlos en el menú
            {   
                
              
                al_draw_filled_rectangle(SCREEN_W/3-20,(SCREEN_H/2.0), SCREEN_W*2/3+20,(SCREEN_H/2.0)+SCREEN_H/15, al_map_rgb(247,220,110));      //recuardo de selección para los niveles
                
                al_draw_text(font, al_map_rgb(124,36,28), SCREEN_W/2.0, SCREEN_H/2.0, ALLEGRO_ALIGN_CENTER, "SELECCIONAR NIVEL");                 //imrpime el texto de la opción
                
                
                //al_draw_filled_circle(rand() % 640, rand() % 480, rand() % 64, al_map_rgb(rand() % 255, 127, 80));
                
               break;
            }
            case QUIT:      //opción de terminado
            {   
             
    
                al_draw_filled_rectangle(SCREEN_W/3+70,(SCREEN_H*2.0/3), SCREEN_W*2/3-70,(SCREEN_H*2.0/3)+SCREEN_H/15, al_map_rgb(247,220,110));    
                
                al_draw_text(font, al_map_rgb(124,36,28), SCREEN_W/2.0, SCREEN_H*2.0/3, ALLEGRO_ALIGN_CENTER, "SALIR"); //imrpime el texto de la opción
                
                
             
                //al_draw_filled_circle(rand() % 640, rand() % 480, rand() % 64, al_map_rgb(rand() % 255, 127, 80));
           
                //           fuente         color               ancho          alto    flag            texto
                break;
            }
            case MENU_LEVELS:
            { 
                
                snprintf(print_string, 100, "%u", nivel);
                
                al_draw_filled_rectangle(SCREEN_W/3,(SCREEN_H/2.0), SCREEN_W*2/3 + 10,(SCREEN_H/2.0)+SCREEN_H/15, al_map_rgb(25,25,112));
                al_draw_text(font, al_map_rgb(124,252, 0), SCREEN_W/2.0, SCREEN_H/2.0, ALLEGRO_ALIGN_CENTER, "NIVEL");
                //           fuente         color               ancho          alto    flag            texto
                al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W/2.0 + 100, SCREEN_H/2.0, ALLEGRO_ALIGN_LEFT, p_to_string); //Imprimo el "string" creado que contiene las vidas restantes
                //           fuente         color               ancho          alto    flag            texto
                break;
            }
            
         
        }
    }
    
       else if (estado_juego == PAUSA)     //muestra cuando el juego está en estado de pausa
    {
      al_draw_filled_rectangle(SCREEN_W/3,(SCREEN_H/12)-SCREEN_H/40, SCREEN_W*2/3,(SCREEN_H/12)+SCREEN_H/10, al_map_rgb(28,40,51));   //ésta es la secuencia de título y caja de texto de pausa
      al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W/2.0, SCREEN_H*1.0/12, ALLEGRO_ALIGN_CENTER, "PAUSA");                      //se colocan fuera del switch ya que se repiten en cada una de las secuencias
      switch(selector_menu)
        {  
      
       case RESUME:         //caso en donde se selecciona la opción de resume
       {        
                al_draw_filled_rectangle(SCREEN_W/3+20,(SCREEN_H/3)-SCREEN_H/40, SCREEN_W*2/3-20,(SCREEN_H/3)+SCREEN_H/10, al_map_rgb(26,54,96));
                al_draw_text(font, al_map_rgb(33,153, 84), SCREEN_W/2.0, SCREEN_H/3.0, ALLEGRO_ALIGN_CENTER, "RESUME");
                al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W/2.0, SCREEN_H/2.0, ALLEGRO_ALIGN_CENTER, "MAIN MENU");
                al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W/2.0, SCREEN_H*2.0/3, ALLEGRO_ALIGN_CENTER, "QUIT");
                 
                break;
       }
       case MAIN_MENU:
                
                
                al_draw_filled_rectangle((SCREEN_W/3),(SCREEN_H/2)-SCREEN_H/40, (SCREEN_W*2/3),(SCREEN_H/2)+SCREEN_H/10, al_map_rgb(26,54,96));
                 
                al_draw_text(font, al_map_rgb(255,255,255), SCREEN_W/2.0, SCREEN_H/3.0, ALLEGRO_ALIGN_CENTER, "RESUME");
                al_draw_text(font, al_map_rgb(33,153, 84), SCREEN_W/2.0, SCREEN_H/2.0, ALLEGRO_ALIGN_CENTER, "MAIN MENU");
                al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W/2.0, SCREEN_H*2.0/3, ALLEGRO_ALIGN_CENTER, "QUIT");
                break;
    
     case QUIT:
            {   
                al_draw_filled_rectangle(SCREEN_W/3,(SCREEN_H*2/3)-SCREEN_H/40, SCREEN_W*2/3,(SCREEN_H*2/3)+SCREEN_H/10, al_map_rgb(26,54,96));
                al_draw_text(font, al_map_rgb(255,255,255), SCREEN_W/2.0, SCREEN_H/3.0, ALLEGRO_ALIGN_CENTER, "RESUME");
                al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W/2.0, SCREEN_H/2.0, ALLEGRO_ALIGN_CENTER, "MAIN MENU");
                al_draw_text(font, al_map_rgb(33,153, 84), SCREEN_W/2.0, SCREEN_H*2.0/3, ALLEGRO_ALIGN_CENTER, "QUIT");
                //           fuente         color               ancho          alto    flag            texto
                break;
            } 
      }
    }
    al_flip_display();
}


static void redraw_fondo(void)
{
    int i,j;
    
    //IMPRESIÓN DE LA CALLE
    for(i = 0; i < 5; i++)
    {
        for(j = 0; j < CANT_CASILLAS_FILA; j++)
        {
            al_draw_scaled_bitmap(sprites.al_calle, 0,0,
                al_get_bitmap_width(sprites.al_calle), al_get_bitmap_height(sprites.al_calle),
                CASILLA_ANCHO * j, CASILLA_ALTO * (i+7), CASILLA_ANCHO, CASILLA_ALTO, 0);
        }
    }
    
    //IMPRESIÓN DEL AGUA
    for(i = 0; i < 5; i++)
    {
        for(j = 0; j < CANT_CASILLAS_FILA; j++)
        {
            if (i % 2 == 0) //imprimimos el agua
            {
                al_draw_scaled_bitmap(sprites.al_agua, 0,0,
                al_get_bitmap_width(sprites.al_agua), al_get_bitmap_height(sprites.al_agua),
                CASILLA_ANCHO * j, CASILLA_ALTO * (i+1), CASILLA_ANCHO, CASILLA_ALTO, 0);
            }
            
            else // en las filas impares, imprimimos el agua desplazada a la derecha, para que quede mejor visualmente
            {
                if(j == 0)
                {
                    al_draw_scaled_bitmap(sprites.al_agua, 0,0,
                    al_get_bitmap_width(sprites.al_agua), al_get_bitmap_height(sprites.al_agua),
                    CASILLA_ANCHO * (j-1) + CASILLA_ANCHO/2, CASILLA_ALTO * (i+1), CASILLA_ANCHO, CASILLA_ALTO, 0);
                }
                
                al_draw_scaled_bitmap(sprites.al_agua, 0,0,
                al_get_bitmap_width(sprites.al_agua), al_get_bitmap_height(sprites.al_agua),
                CASILLA_ANCHO * j + CASILLA_ANCHO/2, CASILLA_ALTO * (i+1), CASILLA_ANCHO, CASILLA_ALTO, 0);
            }
            
        }
    }
    
    //IMPRESIÓN DE LAS FILAS SEGURAS (FILA INICIAL Y FILA INTERMEDIA)
    for (i = 0; i < CANT_CASILLAS_FILA; i++)
    {
        al_draw_scaled_bitmap(sprites.al_fila_segura, 0,0,
                al_get_bitmap_width(sprites.al_fila_segura), al_get_bitmap_height(sprites.al_fila_segura),
                CASILLA_ANCHO * i, CASILLA_ALTO * 6, CASILLA_ANCHO, CASILLA_ALTO+2, 0);
    }
    
    for (i = 0; i < CANT_CASILLAS_FILA; i++)
    {
        al_draw_scaled_bitmap(sprites.al_fila_segura, 0,0,
                al_get_bitmap_width(sprites.al_fila_segura), al_get_bitmap_height(sprites.al_fila_segura),
                CASILLA_ANCHO * i, CASILLA_ALTO * 12 -2, CASILLA_ANCHO, CASILLA_ALTO+2, 0);
    }
    
    //IMPRESIÓN FILA DE LLEGADA
    for(i = 0; i < CANT_CASILLAS_FILA; i++)
    {
        al_draw_scaled_bitmap(sprites.al_fila_superior, 0,0,
                        al_get_bitmap_width(sprites.al_fila_superior), al_get_bitmap_height(sprites.al_fila_superior),
                        i * CASILLA_ANCHO, 0, CASILLA_ANCHO, CASILLA_ALTO, 0);
    }
    
    //IMPRESIÓN DE CASILLAS DE LLEGADA
    for(i = 0; i < CANT_CASILLAS_LLEGADA; i++)
    {
        al_draw_scaled_bitmap(sprites.al_llegada, 0,0,
                        al_get_bitmap_width(sprites.al_llegada), al_get_bitmap_height(sprites.al_llegada),
                        llegadas[i].x-llegadas[i].ancho/2, llegadas[i].y - llegadas[i].alto/2, llegadas[i].ancho, llegadas[i].alto, 0);  
    }
    
   
}



static void redraw_rana(void)
{
    if (rene.chocada == false && rene.frame_chocada == 0)
    {
        if(rene.saltando == true)
        {
            al_draw_scaled_rotated_bitmap(sprites.al_rene[1],
            al_get_bitmap_width(sprites.al_rene[0])/2, al_get_bitmap_height(sprites.al_rene[0])/2, rene.x, rene.y, RANA_ANCHO/al_get_bitmap_width(sprites.al_rene[1]), RANA_ALTO/al_get_bitmap_height(sprites.al_rene[1]),
            rene.direccion*ALLEGRO_PI/2, 0);
            
        }
        else
        {
            al_draw_scaled_rotated_bitmap(sprites.al_rene[0],
            al_get_bitmap_width(sprites.al_rene[0])/2, al_get_bitmap_height(sprites.al_rene[0])/2, rene.x, rene.y, RANA_ANCHO/al_get_bitmap_width(sprites.al_rene[0]), RANA_ALTO/al_get_bitmap_height(sprites.al_rene[0]),
            rene.direccion*ALLEGRO_PI/2, 0);
        }
    }
    
    else if (rene.chocada == true)
    {
        if(estado_juego == PERDER)
        {
            al_draw_scaled_rotated_bitmap(sprites.al_rene[1],
            al_get_bitmap_width(sprites.al_rene[0])/2, al_get_bitmap_height(sprites.al_rene[0])/2, rene.x, rene.y, RANA_ANCHO/al_get_bitmap_width(sprites.al_rene[1]), RANA_ALTO/al_get_bitmap_height(sprites.al_rene[1]),
            rene.direccion*ALLEGRO_PI/2, 0);
        }
        if(rene.frame_chocada == 1)
        {
            al_draw_scaled_bitmap(sprites.al_rene_perdio,
                        0,0, al_get_bitmap_width(sprites.al_rene_perdio), al_get_bitmap_height(sprites.al_rene_perdio),
                        rene.x - RANA_ANCHO/2, rene.y - RANA_ALTO/2, RANA_ANCHO , RANA_ALTO, 0);
        }
    }
}


static void redraw_autos(void)
{
    unsigned int j,k;
    
    //IMPRESION DE LOS AUTOS
    for(j=0; j < FILAS_DE_AUTOS; j++)
    {
        for(k=0; k<AUTOS_POR_FILA; k++)
        {
            if(j == 0 || j == 2)
            {
                al_draw_scaled_rotated_bitmap(sprites.al_auto1,
                    al_get_bitmap_width(sprites.al_auto1)/2, al_get_bitmap_height(sprites.al_auto1)/2,
                    autos[j][k].x, autos[j][k].y, AUTO1_ANCHO/al_get_bitmap_width(sprites.al_auto1), AUTO1_ALTO/al_get_bitmap_height(sprites.al_auto1),
                    (autos[j][k].direccion +1)*ALLEGRO_PI/2, 0);
            }
            else if(j == 1 || j == 3)
            {
                al_draw_scaled_rotated_bitmap(sprites.al_auto2,
                    al_get_bitmap_width(sprites.al_auto2)/2, al_get_bitmap_height(sprites.al_auto2)/2,
                    autos[j][k].x, autos[j][k].y, AUTO1_ANCHO/al_get_bitmap_width(sprites.al_auto2), AUTO1_ALTO/al_get_bitmap_height(sprites.al_auto2),
                    (autos[j][k].direccion +1)*ALLEGRO_PI/2, 0);
            }
            else
            {
                al_draw_scaled_rotated_bitmap(sprites.al_camion,
                    al_get_bitmap_width(sprites.al_camion)/2, al_get_bitmap_height(sprites.al_camion)/2,
                    autos[j][k].x, autos[j][k].y, CAMION_ANCHO/al_get_bitmap_width(sprites.al_camion), CAMION_ALTO/al_get_bitmap_height(sprites.al_camion),
                    (autos[j][k].direccion + 1)*ALLEGRO_PI/2, 0);
            }
        }
    }
}


static void redraw_troncos(void)
{
    unsigned int j,k;
    //impresión de troncos
    for(j=0; j < FILAS_DE_TRONCOS; j++)
    {
        for(k=0; k < TRONCOS_POR_FILA; k++)
        {
            if(j == 0)
            {
                al_draw_scaled_bitmap(sprites.al_tronco1,
                        0,0, al_get_bitmap_width(sprites.al_tronco1), al_get_bitmap_height(sprites.al_tronco1),
                        troncos[j][k].x - troncos[j][k].largo/2, troncos[j][k].y - troncos[j][k].alto/2, TRONCO1_ANCHO , TRONCO1_ALTO, 0);
            }
            if(j == 1)
            {
                al_draw_scaled_bitmap(sprites.al_tronco2, 0,0,
                        al_get_bitmap_width(sprites.al_tronco2), al_get_bitmap_height(sprites.al_tronco2),
                        troncos[j][k].x - troncos[j][k].largo/2, troncos[j][k].y - troncos[j][k].alto/2, TRONCO2_ANCHO , TRONCO2_ALTO, 0);
            }
            if(j == 2)
            {
                al_draw_scaled_bitmap(sprites.al_tronco3, 0,0,
                        al_get_bitmap_width(sprites.al_tronco3), al_get_bitmap_height(sprites.al_tronco3),
                        troncos[j][k].x - troncos[j][k].largo/2, troncos[j][k].y - troncos[j][k].alto/2, TRONCO3_ANCHO , TRONCO3_ALTO, 0);
            }
        }
    }
}

static void redraw_tortugas(void)
{
    unsigned int j,k;
    //impresión de troncos
    for(j=0; j < FILAS_DE_TORTUGAS; j++)
    {
        for(k=0; k < TORTUGAS_POR_FILA; k++)
        {
            if(tortugas[j][k].hundirse == false)
            {
                if(j == 0)
                {
                    al_draw_scaled_bitmap(sprites.al_tortugas[tortugas[j][k].frames], 0,0,
                        al_get_bitmap_width(sprites.al_tortugas[tortugas[j][k].frames]), al_get_bitmap_height(sprites.al_tortugas[tortugas[j][k].frames]),
                        tortugas[j][k].x - tortugas[j][k].largo/2, tortugas[j][k].y - tortugas[j][k].alto/2, TORTUGAS_ANCHO , TORTUGAS_ALTO, 1);

                    al_draw_scaled_bitmap(sprites.al_tortugas[tortugas[j][k].frames], 0,0,
                        al_get_bitmap_width(sprites.al_tortugas[tortugas[j][k].frames]), al_get_bitmap_height(sprites.al_tortugas[tortugas[j][k].frames]),
                        tortugas[j][k].x - tortugas[j][k].largo/2 + TORTUGAS_ANCHO, tortugas[j][k].y - tortugas[j][k].alto/2, TORTUGAS_ANCHO , TORTUGAS_ALTO, 1);

                    al_draw_scaled_bitmap(sprites.al_tortugas[tortugas[j][k].frames], 0,0,
                        al_get_bitmap_width(sprites.al_tortugas[tortugas[j][k].frames]), al_get_bitmap_height(sprites.al_tortugas[tortugas[j][k].frames]),
                        tortugas[j][k].x - tortugas[j][k].largo/2 + 2*TORTUGAS_ANCHO, tortugas[j][k].y - tortugas[j][k].alto/2, TORTUGAS_ANCHO , TORTUGAS_ALTO, 1);


                }

                if(j == 1)
                {
                     al_draw_scaled_bitmap(sprites.al_tortugas[tortugas[j][k].frames], 0,0,
                        al_get_bitmap_width(sprites.al_tortugas[tortugas[j][k].frames]), al_get_bitmap_height(sprites.al_tortugas[tortugas[j][k].frames]),
                        tortugas[j][k].x - tortugas[j][k].largo/2, tortugas[j][k].y - tortugas[j][k].alto/2, TORTUGAS_ANCHO , TORTUGAS_ALTO, 1);

                    al_draw_scaled_bitmap(sprites.al_tortugas[tortugas[j][k].frames], 0,0,
                        al_get_bitmap_width(sprites.al_tortugas[tortugas[j][k].frames]), al_get_bitmap_height(sprites.al_tortugas[tortugas[j][k].frames]),
                        tortugas[j][k].x - tortugas[j][k].largo/2 + TORTUGAS_ANCHO, tortugas[j][k].y - tortugas[j][k].alto/2, TORTUGAS_ANCHO , TORTUGAS_ALTO, 1);
                }
            }
        }
    }
}


static void redraw_llegada(void)
{
    int i;
    
    for (i = 0; i < CANT_CASILLAS_LLEGADA; i++)
    {
        
        if (llegadas[i].ocupado == true)
        {
            al_draw_scaled_rotated_bitmap(sprites.al_rene[0],
            al_get_bitmap_width(sprites.al_rene[0])/2, al_get_bitmap_height(sprites.al_rene[0])/2, llegadas[i].x, llegadas[i].y,
            RANA_ANCHO/al_get_bitmap_width(sprites.al_rene[0]), RANA_ALTO/al_get_bitmap_height(sprites.al_rene[0]),
            0, 0);
        }
        
        if(llegadas[i].cocodrilo == true)
        {
            al_draw_scaled_bitmap(al_cocodrilo, 0,0,
                        al_get_bitmap_width(al_cocodrilo), al_get_bitmap_height(al_cocodrilo),
                        llegadas[i].x - llegadas[i].ancho/2, llegadas[i].y - llegadas[i].alto/2, llegadas[i].ancho, llegadas[i].alto, 0);
        }
    }
}

static void redraw_tiempo(void)
{
    
    static double xf;
    xf = (SCREEN_W - 100) * ((TIEMPO_TOTAL - tiempo_restante)/TIEMPO_TOTAL); //le resto 100 para poder escribir la palabra "time" a la derecha
    al_draw_line(SCREEN_W  - 100, SCREEN_H, xf, SCREEN_H, al_color_name("darkgreen"), 20);
    //           x0            y0         xf        yf               color          thickness
    al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W - 100, SCREEN_H - 40, ALLEGRO_ALIGN_LEFT, "Tiempo");
    //           fuente         color               ancho          alto    flag            texto
    
    
}


void allegro_audio (void)
{
    if(rene.saltando == true && rene.chocada == false)
    {
        al_play_sample(sample_rana_salto, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
    }
}



void allegro_destroy(void)
{

    al_destroy_timer(timer);
    al_destroy_event_queue(event_queue);
    al_destroy_bitmap(mundo_buffer);
    al_destroy_bitmap(sprites.al_rene[0]);
    al_destroy_bitmap(sprites.al_rene[1]);
    al_destroy_bitmap(sprites.al_auto1);
    al_destroy_bitmap(sprites.al_auto2);
    al_destroy_bitmap(sprites.al_camion);
    al_destroy_bitmap(sprites.al_tronco1);
    al_destroy_bitmap(sprites.al_tronco2);
    al_destroy_bitmap(sprites.al_tronco3);
    al_destroy_bitmap(sprites.al_tortugas[0]);
    al_destroy_bitmap(sprites.al_tortugas[1]);
    al_destroy_bitmap(sprites.al_tortugas[2]);
    al_destroy_bitmap(sprites.al_tortugas[3]);
    al_destroy_bitmap(sprites.al_llegada);
    al_destroy_bitmap(sprites.al_fila_superior);
    al_destroy_bitmap(sprites.al_agua);
    al_destroy_bitmap(sprites.al_calle);
    al_destroy_bitmap(sprites.al_fila_segura);
    al_destroy_bitmap(sprites.al_rene_perdio);
    al_destroy_bitmap(al_cocodrilo);
    al_destroy_display(display);
    al_destroy_sample(sample_rana_salto);
    al_destroy_sample(sample_rana_ahogada);
    al_destroy_sample(sample_rana_chocada);
    al_destroy_sample(sample_tiempo);
    
}



#endif
