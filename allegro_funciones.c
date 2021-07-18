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

#define FPS             REFRESCO                            //Cantidad de imágenes que se mostrarán por segundo en pantalla

#define BUFFER_W        MUNDO_ANCHO                         //Cantidad de pixeles de Ancho del mundo
#define BUFFER_H        MUNDO_ALTO                          //Cantidad de pixeles de Alto del mundo

#define SCREEN_SCALE    1                                   //Escala de ampliación en la que se desea mostrar el mundo en pantalla

#define SCREEN_W        (BUFFER_W * SCREEN_SCALE)           //Definición del alto y ancho de pantalla totales
#define SCREEN_H        (BUFFER_H * SCREEN_SCALE * 17/15)

#define CANT_MAX_CHAR 100                                   //Constante para la cantidad de caracteres que podrá albergar el arreglo
                                                            //auxiliar para impresión de variables en el display

/***************************************************
 *  DEFINICIÓN DE VARIABLES GLOBALES
***************************************************/

ALLEGRO_TIMER       * timer;                                //Creación de timers y cola de eventos usados en la versión de pc.
ALLEGRO_EVENT_QUEUE * event_queue;

/***************************************************
 *  DEFINICIÓN DE VARIABLES LOCALES
***************************************************/

/*
 *  Creación de los punteros a los objetos que usará 
 *  la versión de PC para mostrar el juego en pantalla
 */

static ALLEGRO_DISPLAY     * display;                       
static ALLEGRO_BITMAP      * mundo_buffer;                 
static ALLEGRO_FONT        * font;                          
static ALLEGRO_BITMAP      * al_cocodrilo;
static ALLEGRO_SAMPLE      * sample_rana_salto;
static ALLEGRO_SAMPLE      * sample_rana_ahogada;
static ALLEGRO_SAMPLE      * sample_rana_chocada;
static ALLEGRO_SAMPLE      * sample_tiempo;
static ALLEGRO_SAMPLE      * sample_musica_fondo;
static ALLEGRO_SAMPLE      * sample_coin;



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
 * DEFINICIÓN DE FUNCIONES GLOBALES
 * 
 **********************************************************************************/



/***********************************************************************
 *  Función que inicializa tanto la librería Allegro                   *
 *  todos los objetos usados por ella en el programa                   *
 *      También es la encargada de destruir los objetos creados en caso*
 *      de que haya un error.                                          *
 **********************************************************************/

bool allegro_startup (void)
{
    /***************************************************************************
     *  Zona de inicialización de las herramientas necesarias para el correcto *
     *  funcionamiento del programa                                            *
     **************************************************************************/
    
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

    if (!al_reserve_samples(5)) {
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
    
    font = al_load_ttf_font("Karumbi-Regular.ttf", 40 * SCREEN_SCALE, 0); //Asigna la fuente que se va a usar
    if (!font)
    {
        fprintf(stderr, "Could not load 'Karumbi-Regular.ttf'.\n");
        al_shutdown_font_addon();
        al_shutdown_ttf_addon();
        return false;
    }
    
    if (!al_init_primitives_addon()) // Inicializa el complemento que permite reconocer fuentes ttf
    {
        fprintf(stderr, "failed to initialize the primitives addon!\n");
        al_shutdown_font_addon();
        al_shutdown_ttf_addon();
        al_destroy_font(font);
        return false;
        
    }
    
    if(!al_init_image_addon())
    {
        fprintf(stderr, "failed to initialize allegro images!\n");
        al_shutdown_font_addon();
        al_shutdown_ttf_addon();
        al_destroy_font(font);
        al_shutdown_primitives_addon();
        return false;
    }
    
    if (!al_install_keyboard())
    {
        fprintf(stderr, "failed to initialize the keyboard!\n");
        al_shutdown_font_addon();
        al_shutdown_ttf_addon();
        al_destroy_font(font);
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
        al_destroy_font(font);
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
        al_destroy_font(font);
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
        al_destroy_font(font);
        al_shutdown_primitives_addon();
        al_shutdown_image_addon();
        al_uninstall_keyboard();
        al_destroy_timer(timer);
        al_destroy_event_queue(event_queue);
        return false;
    }
    
    /*******************************************************************
     *                                                                 *
     *  Definición de las imágenes usadas para representar el juego a  *
     *  partir de una sprite sheet                                     *
     *                                                                 *
     *******************************************************************/
    sprites._sheet = al_load_bitmap("spritesheet.png");
    if (!sprites._sheet) 
    {
        fprintf(stderr, "failed to create !sprites._sheet!\n");
        al_shutdown_font_addon();
        al_shutdown_ttf_addon();
        al_destroy_font(font);
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
        al_destroy_font(font);
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
        al_destroy_font(font);
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
        al_destroy_font(font);
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
        al_destroy_font(font);
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
        al_destroy_font(font);
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
        al_destroy_font(font);
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
        al_destroy_font(font);
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
        al_destroy_font(font);
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
        al_destroy_font(font);
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
        al_destroy_font(font);
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
        al_destroy_font(font);
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
        al_destroy_font(font);
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
        al_destroy_font(font);
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
        al_destroy_font(font);
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
        al_destroy_font(font);
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
        al_destroy_font(font);
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
        al_destroy_font(font);
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
        al_destroy_font(font);
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
        al_destroy_font(font);
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

/****************************************************************
 *  Creación del display que mostrará todo lo relativo al juego *
 ***************************************************************/
    
    display = al_create_display(SCREEN_W, SCREEN_H);
    if (!display)
    {
        fprintf(stderr, "failed to create display!\n");
        al_shutdown_font_addon();
        al_shutdown_ttf_addon();
        al_destroy_font(font);
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
    
    
/******************************************************************
 *  Definición de sonidos                                         *
 *****************************************************************/
 
    
    sample_rana_salto = al_load_sample("sound-frogger-hop.wav");
    if (!sample_rana_salto)
    {
        printf("Audio clip sample_rana_salto not loaded!\n");
        al_shutdown_font_addon();
        al_shutdown_ttf_addon();
        al_destroy_font(font);
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
        printf("Audio clip sample_rana_ahogada not loaded!\n");
        al_shutdown_font_addon();
        al_shutdown_ttf_addon();
        al_destroy_font(font);
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
        printf("Audio clip sample_rana_chocada not loaded!\n");
        al_shutdown_font_addon();
        al_shutdown_ttf_addon();
        al_destroy_font(font);
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
        printf("Audio clip sample_tiempo not loaded!\n");
        al_shutdown_font_addon();
        al_shutdown_ttf_addon();
        al_destroy_font(font);
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
    
    sample_musica_fondo = al_load_sample("1052515_Peglin---Forest.wav");
    if (!sample_musica_fondo)
    {
        printf("Audio clip sample_musica_fondo not loaded!\n");
        al_shutdown_font_addon();
        al_shutdown_ttf_addon();
        al_destroy_font(font);
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
        al_destroy_sample(sample_tiempo);
        return false;
    }    
    
    sample_coin = al_load_sample("sound-frogger-coin-in.wav");
    if (!sample_coin)
    {
        printf("Audio clip sample_coin not loaded!\n");
        al_shutdown_font_addon();
        al_shutdown_ttf_addon();
        al_destroy_font(font);
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
        al_destroy_sample(sample_tiempo);
        return false;
    }
    
    return true;
}

//Función encargada de declarar las fuentes de eventos 

void allegro_event_register(void)
{
    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_register_event_source(event_queue, al_get_keyboard_event_source());
}


/*******************************************************************************
 * 
 * Función que modifica el arreglo de teclas pulsadas según el evento recibido
 *  También cierra el programa en caso que el evento provenga del display 
 *  (Cerrar la ventana con el mouse)
 * 
 ******************************************************************************/

bool allegro_teclas (ALLEGRO_EVENT * ev)
{
    bool do_exit = false;
    
    if (ev->type == ALLEGRO_EVENT_DISPLAY_CLOSE) // Caso en el que se cierra la ventana manualmente
    {
        do_exit = true;
    }

    if (ev->type == ALLEGRO_EVENT_KEY_DOWN)     // Caso en el que el evento es el de pulsar alguna tecla
    {                                           // En este caso se pone en "true" la tecla pulsada
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
    
    else if (ev->type == ALLEGRO_EVENT_KEY_UP)  // Caso en el que el evento es el de soltar alguna tecla
    {                                           // En este caso se pone en "false" la tecla pulsada
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

            case ALLEGRO_KEY_ESCAPE:            // Se agrega el caso donde se suelta el escape para
                do_exit = true;                 // terminar el programa
                break;
        }
    }
    
    return do_exit;
}


void allegro_initialize_display(void)           //Función que inicializa el display en negro
{ 
    al_set_target_bitmap(al_get_backbuffer(display));
    al_clear_to_color(al_map_rgb(0, 0, 0));
    al_flip_display();
}

/*********************************************************************************************
 * 
 * Función encargada de redibujar todo el mundo una vez por frame dependiendo
 * de la parte del juego en la que se esté.
 *      En la sección de Menús, imprime el fondo y una interfaz de menú, no imprime enemigos
 * 
 *      En la sección de juego, imprime el fondo, todos los enemigos que se encuentren en el mundo,
 *      la rana, los troncos y tortugas.
 * 
 *********************************************************************************************/


void allegro_redraw(void)
{
    char print_string[CANT_MAX_CHAR]; //Creo un arreglo que "simularía" un string (el último elemento debe ser 0 por ser el terminador).
    const char *p_to_string = print_string; //creo un puntero constante para usar en "al_draw_text"
    
    
    
    /*
     *  Caso en el que se está jugando, no se está en ningún menú, no se pasó de nivel ni se perdió
     *      Se imprime el fondo, todos los enemigos y obstáculos, la rana, las casillas de llegada y toda
     *      la información relativa al juego (puntaje actual, tiempo restante, vidas restantes y nivel)
     */
    if(estado_juego != MENU && estado_juego != PAUSA && estado_juego != PASAR_NIVEL && estado_juego != PERDER)
    {
        
        al_set_target_bitmap(mundo_buffer); //Todo lo relativo al mundo se imprime en un buffer para poder
                                            //escalarlo y para poder tener zonas vacías donde imprimir la información de la partida

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
        
        //IMPRESION DEL TIEMPO RESTANTE (se realiza fuera del espacio destinado al mundo)
        redraw_tiempo();
        
        
    /***********************************************************************
    * Impresión de información de partida
    *   (variables de nivel, puntaje y vidas)
    * 
    ***********************************************************************/
        
        snprintf(print_string, 100, "%u", vidas_restantes); //Se completa el arreglo auxiliar con caracteres que representan el valor de una variable a imprimir
                       
        al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W - 115, 0, ALLEGRO_ALIGN_LEFT, "Vidas:");        
        al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W - 40, 0, ALLEGRO_ALIGN_LEFT, p_to_string); //Imprimo el "string" que contiene las vidas restantes
        
        
        //Se repite el proceso anterior con la variable nivel y puntaje, pero en posiciones distintas de la pantalla
        
        snprintf(print_string, 100, "%u", nivel);   
        
        al_draw_text(font, al_map_rgb(255, 255, 255), 5, 0, ALLEGRO_ALIGN_LEFT, "Nivel: "); 
        al_draw_text(font, al_map_rgb(255, 255, 255), 80, 0, ALLEGRO_ALIGN_LEFT, p_to_string);

        
        snprintf(print_string, 100, "%lu", puntaje_juego);
        
        al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W/2, 0, ALLEGRO_ALIGN_CENTER, "Puntaje: ");
        al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W*2/3-30, 0, ALLEGRO_ALIGN_CENTER, p_to_string);    
    }
    
    /*******************************************************************************
     * Caso en el que no se está jugando, sino que está en la transición de nivel
     *  Se imprime el fondo con información para el jugador:
     *      - El nivel al que se pasó
     *      - El puntaje actual del jugador
     *******************************************************************************/
    
    else if(estado_juego == PASAR_NIVEL)
    {
        al_set_target_backbuffer(display);
        al_clear_to_color(al_map_rgb(0,0,0));
        redraw_fondo();
        
        
         /* La transición se divide en dos partes, cada una de 5 segundos donde solamente se 
            muestra la información al usuario */
         
        
        if(timer_pasar_nivel > UN_SEGUNDO*5) //Los primeros 5 segundos se imprime el puntaje actual
        {
            snprintf(print_string, 100, "%lu", puntaje_juego);
        
            al_draw_filled_rectangle(SCREEN_W/3,(SCREEN_H/3)-SCREEN_H/40, SCREEN_W*2/3,(SCREEN_H/2)+SCREEN_H/20, al_map_rgb(28,40,51));
            
            al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W/2, SCREEN_H/3.0, ALLEGRO_ALIGN_CENTER, "Puntaje ");
            al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W/2, SCREEN_H/2.2, ALLEGRO_ALIGN_CENTER, p_to_string);
        }
        else //Los siguientes 5 segundos se imprime el nivel que está por comenzar
        {
            snprintf(print_string, 100, "%u", nivel+1);
  
            al_draw_filled_rectangle(SCREEN_W/3,(SCREEN_H/3)-SCREEN_H/40, SCREEN_W*2/3,(SCREEN_H/2)+SCREEN_H/20, al_map_rgb(28,40,51));
            
            al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W/2.0, SCREEN_H * 1 / 3.0, ALLEGRO_ALIGN_CENTER, "Nivel ");
            al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W/2.0, SCREEN_H/2.2, ALLEGRO_ALIGN_CENTER, p_to_string);
        }
    }
    
    /********************************************************************
     *  Caso en el que se terminan las 3 vidas del jugador
     *  Se imprime el nivel y el puntaje con el que se terminó el juego,
     *  Así como el puntaje máximo histórico anterior 
     ********************************************************************/
    
    else if(estado_juego == PERDER)     //en el caso de que se pierde
    {
        al_set_target_backbuffer(display);
        al_clear_to_color(al_map_rgb(0,0,0));
        redraw_fondo();
        
        //La transición se divide en dos partes de 5 segundos, la primera que muestra el nivel alcanzado,
        //y la segunda que muestra el puntaje alcanzado y el puntaje máximo histórico
        
        
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
   
    /************************************************************************************************************************
     * Casos donde se imprimen interfaces de menús:
     * Dependiendo del estado de la variable "Selector_menú" (modificada solamente cuando el estado de juego sea algún menú)
     * se imprimirá el menú/submenú correspondiente y se mostrará información necesaria
     ************************************************************************************************************************/
 
    else if (estado_juego == MENU)  
    {
        al_set_target_backbuffer(display);      //Se pone en negro el fondo de la pantalla, para borrar las informaciones anteriores
        al_clear_to_color(al_map_rgb(0,0,0));
        
        redraw_fondo (); //se imprime el fondo del mundo con fines decorativos.
        
        /*
         * CASO MENÚ PRINCIPAL:
         *      Se muestran todas las posibilidades que posee el menú principal:
         *          La que se esté queriendo seleccionar tiene un recuadro con color distinto a los recuadros
         *          que poseen todas las opciones.
         */
        
        if (selector_menu!=MENU_LEVELS)    
        {  
        al_draw_filled_rectangle(SCREEN_W/3,(SCREEN_H/16)-SCREEN_H/40, SCREEN_W*2/3,(SCREEN_H/16)+SCREEN_H/10, al_map_rgb(28,40,51)); //RECTANGULO MENU
        al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W/2.0, SCREEN_H*1.0/16, ALLEGRO_ALIGN_CENTER, "MENU PRINCIPAL");
        
        
        al_draw_filled_rectangle(SCREEN_W/3,(SCREEN_H/3), SCREEN_W*2/3,(SCREEN_H/3)+SCREEN_H/15, al_map_rgb(25,25,112));// Rectángulo jugar
        al_draw_text(font, al_map_rgb(124,252, 0), SCREEN_W/2.0, SCREEN_H/3.0, ALLEGRO_ALIGN_CENTER, "JUGAR");
        
        al_draw_filled_rectangle(SCREEN_W*2/9,(SCREEN_H/2.0), SCREEN_W*7/9,(SCREEN_H/2.0)+SCREEN_H/15, al_map_rgb(25,25,112));      //recuadro de selección para los niveles
        al_draw_text(font, al_map_rgb(124,252, 0), SCREEN_W/2.0, SCREEN_H/2.0, ALLEGRO_ALIGN_CENTER, "SELECCIONAR NIVEL");
        
        al_draw_filled_rectangle(SCREEN_W/3,(SCREEN_H*2.0/3), SCREEN_W*2/3,(SCREEN_H*2.0/3)+SCREEN_H/15, al_map_rgb(25,25,112));            
        al_draw_text(font, al_map_rgb(124,252, 0), SCREEN_W/2.0, SCREEN_H*2.0/3, ALLEGRO_ALIGN_CENTER, "SALIR");
                     
                
        
        
        
        /*
         * Sección donde se imprime el recuadro en la opción preseleccionada
         */
        
        switch(selector_menu)
        {
           
            case PLAY:      //caso en donde se elige la opción de iniciar el juego en el nivel 1. Se muestra "jugar" en pantalla con un recuadro distinto
            {   
                al_draw_filled_rectangle(SCREEN_W/3,(SCREEN_H/3), SCREEN_W*2/3,(SCREEN_H/3)+SCREEN_H/15, al_map_rgb(247,220,110));
                
                al_draw_text(font, al_map_rgb(124,36,28), SCREEN_W/2.0, SCREEN_H/3.0, ALLEGRO_ALIGN_CENTER, "JUGAR"); 
          
                break;
            case LEVEL: //opción de elegir niveles en el menú (Se muestra la frase "seleccionar nivel" con un recuadro distinto)
            {   
                al_draw_filled_rectangle(SCREEN_W*2.0/9,(SCREEN_H/2.0), SCREEN_W*7.0/9,(SCREEN_H/2.0)+SCREEN_H/15, al_map_rgb(247,220,110));      //recuadro de selección para los niveles
                
                al_draw_text(font, al_map_rgb(124,36,28), SCREEN_W/2.0, SCREEN_H/2.0, ALLEGRO_ALIGN_CENTER, "SELECCIONAR NIVEL");                 //imprime el texto de la opción        
                
                break;
            }
            case QUIT:      //opción de terminar el nivel, se muestra "quit" resaltado
            {   
                al_draw_filled_rectangle(SCREEN_W/3,(SCREEN_H*2.0/3), SCREEN_W*2/3,(SCREEN_H*2.0/3)+SCREEN_H/15, al_map_rgb(247,220,110));    
                
                al_draw_text(font, al_map_rgb(124,36,28), SCREEN_W/2.0, SCREEN_H*2.0/3, ALLEGRO_ALIGN_CENTER, "SALIR"); //imprime el texto de la opción
                //           fuente         color               ancho          alto             flag            texto
                break;
            } 
          } 
        }  
      }
        /*
         * CASO SUBMENÚ SELECCIÓN DE NIVELES:
         *      Se imprime el valor preliminar del nivel a elegir junto con la leyenda "seleccionar nivel"
         */ 
            else
            {
                al_draw_filled_rectangle(SCREEN_W/3,(SCREEN_H/16)-SCREEN_H/40, SCREEN_W*2/3+20,(SCREEN_H/16)+SCREEN_H/10, al_map_rgb(28,40,51));
                al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W/2.0, SCREEN_H*1.0/16, ALLEGRO_ALIGN_CENTER, "SELECCIONAR NIVEL");
                al_draw_filled_rectangle((SCREEN_W/3),(SCREEN_H/2), SCREEN_W*2/3-50,(SCREEN_H/3)+SCREEN_H/5, al_map_rgb(25,25,112));
        
                snprintf(print_string, 100, "%u", nivel);
                
                al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W/2.0, SCREEN_H/2.0-20, ALLEGRO_ALIGN_CENTER, "NIVEL:");
                //           fuente         color               ancho          alto    flag            texto
                al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W/2.0, SCREEN_H/2.0, ALLEGRO_ALIGN_LEFT, p_to_string); //Imprimo el "string" creado que contiene el nivel preseleccionado
                //           fuente         color               ancho          alto    flag      
        
        
            }
       }
    
    
    else if (estado_juego == PAUSA)     //muestra cuando el juego está en estado de pausa
    {
      al_draw_filled_rectangle(SCREEN_W/3,(SCREEN_H/12)-SCREEN_H/40, SCREEN_W*2/3,(SCREEN_H/12)+SCREEN_H/10, al_map_rgb(28,40,51));     //Muestra el título y caja de texto de pausa
      al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W/2.0, SCREEN_H*1.0/12, ALLEGRO_ALIGN_CENTER, "PAUSA");                      
      
      al_draw_filled_rectangle(SCREEN_W/3,(SCREEN_H/3), SCREEN_W*2/3,(SCREEN_H/3)+SCREEN_H/15, al_map_rgb(25,25,112));// Rectángulo jugar
      al_draw_filled_rectangle(SCREEN_W*2/9,(SCREEN_H/2.0), SCREEN_W*7/9,(SCREEN_H/2.0)+SCREEN_H/15, al_map_rgb(25,25,112));
      al_draw_filled_rectangle(SCREEN_W/3,(SCREEN_H*2.0/3), SCREEN_W*2/3,(SCREEN_H*2.0/3)+SCREEN_H/15, al_map_rgb(25,25,112));
      
      al_draw_text(font, al_map_rgb(255,255,255), SCREEN_W/2.0, SCREEN_H/3.0, ALLEGRO_ALIGN_CENTER, "CONTINUAR");      //se imprimen los 3 textos de las opciones del menú
      al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W/2.0, SCREEN_H/2.0, ALLEGRO_ALIGN_CENTER, "MENU PRINCIPAL");
      al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W/2.0, SCREEN_H*2.0/3, ALLEGRO_ALIGN_CENTER, "SALIR");
      
      switch(selector_menu)
      {  
      
       case RESUME:         //caso en donde se selecciona la opción de continuar
       {        
                al_draw_text(font, al_map_rgb(33,153, 84), SCREEN_W/2.0, SCREEN_H/3.0, ALLEGRO_ALIGN_CENTER, "CONTINUAR");    //imprime el texto en color como seleccionado
                break;
       }
       case MAIN_MENU:  //caso en el que se selecciona la opción de menú principal
                
                al_draw_text(font, al_map_rgb(33,153, 84), SCREEN_W/2.0, SCREEN_H/2.0, ALLEGRO_ALIGN_CENTER, "MENU PRINCIPAL"); //imprime el texto en color como seleccionado 
                break;
    
        case QUIT:     //caso en el que se selecciona la opción de salir
               {   
                  al_draw_text(font, al_map_rgb(33,153, 84), SCREEN_W/2.0, SCREEN_H*2.0/3, ALLEGRO_ALIGN_CENTER, "SALIR");  //imprime el texto en color como seleccionado
                   //           fuente         color               ancho          alto    flag            texto
                   break;
               } 
         }
    }
    al_flip_display(); //Luego de redibujar todo, se muestra el display
}

/*
 *  Función que imprime el fondo del mundo a partir de una sprite sheet
 */

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


/*
 * Función que imprime la rana en el mundo
 */

static void redraw_rana(void)
{
    if (rene.chocada == false && rene.frame_chocada == 0) //Si la rana está efectuando un movimiento, se imprime la rana estirada en el proceso, orientada con la cabeza hacia la dirección en la que se mueve
    {
        if(rene.saltando == true)
        {
            al_draw_scaled_rotated_bitmap(sprites.al_rene[1],
            al_get_bitmap_width(sprites.al_rene[0])/2, al_get_bitmap_height(sprites.al_rene[0])/2, rene.x, rene.y, RANA_ANCHO/al_get_bitmap_width(sprites.al_rene[1]), RANA_ALTO/al_get_bitmap_height(sprites.al_rene[1]),
            rene.direccion*ALLEGRO_PI/2, 0);
        }
        
        else //Si la rana está quieta, se imprime la rana "sentada" con la cabeza orientada en la dirección del último salto
        {
            al_draw_scaled_rotated_bitmap(sprites.al_rene[0],
            al_get_bitmap_width(sprites.al_rene[0])/2, al_get_bitmap_height(sprites.al_rene[0])/2, rene.x, rene.y, RANA_ANCHO/al_get_bitmap_width(sprites.al_rene[0]), RANA_ALTO/al_get_bitmap_height(sprites.al_rene[0]),
            rene.direccion*ALLEGRO_PI/2, 0);
        }
    }
    
    else if (rene.chocada == true) //Si la rana chocó o se hundió, se muestra una imagen que lo represente en la posición donde lo hizo.
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


/*
 * Función que imprime todos los autos en el mundo
 */

static void redraw_autos(void)
{
    unsigned int j,k;
    
    //IMPRESION DE LOS AUTOS
    for(j=0; j < FILAS_DE_AUTOS; j++)
    {
        for(k=0; k<AUTOS_POR_FILA; k++)
        {
            //Se separa en 3 casos porque hay dos pares de filas con autos iguales, y una en la que están los camiones
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

/*
 *  Función que imprime los troncos en el mundo:
 *      Diferencia entre tamaños de troncos para las distintas filas
 */

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

/*
 *  Función que imprime las tortugas en el mundo:
 */

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
                if(j == 0) //En la primer fila se imprimen 3 tortugas consecutivas
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

                if(j == 1) //En la primer fila se imprimen 2 tortugas consecutivas
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

/*
 * Función que imprime las casillas de llegada en las posiciones correspondientes.
 *      También es la encargada de mostrar los cocodrilos que aparecen en las llegadas
 */

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


/*
 *  Función que imprime la barra de tiempo abajo del todo en el mundo, junto con la palabra "tiempo" a la derecha
 */
static void redraw_tiempo(void)
{
    
    static double xf;
    xf = (SCREEN_W*17.0/20) * ((TIEMPO_TOTAL - tiempo_restante)/TIEMPO_TOTAL); //le resto 100 para poder escribir la palabra "time" a la derecha
    al_draw_line(SCREEN_W * 17.0/20, SCREEN_H, xf, SCREEN_H, al_color_name("darkgreen"), SCREEN_W / 18);
    //           x0            y0         xf        yf               color          thickness
    al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W*35/40, SCREEN_H*34/36, ALLEGRO_ALIGN_LEFT, "Tiempo");
    //           fuente         color               ancho          alto    flag            texto
    
    
}

enum PRIMERA_VEZ
{
    CHOQUE = 1, AHOGADA, SALTO, TIEMPO, MUSICA, LLEGO
};

void allegro_audio (void)
{
    static int primera_vez = 0;

    static bool primera_vez_musica = true;
    
    static ALLEGRO_SAMPLE_ID id_salto; //variable que guarda el ID correspondiente al sonido del salto mientras está sonando. Permite detener la reproducción
    
    if (estado_juego == MENU && primera_vez_musica == true) //Esto se ejecuta una sola vez y dura todo el programa, COMIENZA LA MUSICA DE FONDO AL MOSTRAR EL MENU
    {
        primera_vez_musica = false;
        al_play_sample(sample_musica_fondo, 0.05, 0.0, 1.0, ALLEGRO_PLAYMODE_LOOP, NULL);
    }
    if(rene.saltando == true && rene.chocada == false && (primera_vez == 0 || primera_vez == TIEMPO)) //si la rana salta reproduce el sonido del salto
    {
        primera_vez = SALTO;
        al_play_sample(sample_rana_salto, 0.3, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, &id_salto);
    }
    if (rene.chocada == true && (rene.y <= CASILLA_ALTO * 14 && rene.y >= CASILLA_ALTO*6 + CASILLA_ALTO/2.0) && (primera_vez == 0 || primera_vez == TIEMPO || primera_vez == SALTO))
    {
        //Si la rana estaba saltando y el sonido se estaba reproduciendo, lo detiene y reproduce el sonido del choque
        primera_vez = CHOQUE;
        al_stop_sample(&id_salto);
        al_play_sample(sample_rana_chocada, 0.4, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
    }
    if (rene.chocada == true && (rene.y <= CASILLA_ALTO * 6 + CASILLA_ALTO/2.0 && rene.y >= CASILLA_ALTO) && (primera_vez == 0 || primera_vez == TIEMPO || primera_vez == SALTO))
    {
        //En el caso de que se caiga al agua, se reproduce el sonido de ahogada
        primera_vez = AHOGADA;
        al_play_sample(sample_rana_ahogada, 0.4, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
    }
    if (tiempo_restante < ((TIEMPO_TOTAL) / 4.0) && primera_vez == 0)
    {
        //si queda menos de 1/4 del tiempo total, se reproduce una alerta
        primera_vez = TIEMPO;
        al_play_sample(sample_tiempo, 0.4, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
    }
    if (rene.llego == true && (primera_vez == 0 || primera_vez == TIEMPO || primera_vez == SALTO))
    {
        //Si la rana llega a una casilla de llegada, se reproduce un sonido.
        primera_vez = LLEGO;
        al_play_sample(sample_coin, 0.4, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
    }
    
    
    /*
     *  RESET DE VARIABLES DE ESTADO 
     *      Consiste en devolver la variable de estado a 0 cuando se terminó la acción que dispara
     *      algún sonido, de esta forma no se reproducen más de una vez, superponiéndose.
     */
    
    if(primera_vez == SALTO && rene.saltando == false)
    {
        primera_vez = 0;
    }
    else if(primera_vez == TIEMPO && tiempo_restante > ((TIEMPO_TOTAL)/4.0))
    {
        primera_vez = TIEMPO;
    }
    else if(primera_vez == LLEGO && rene.llego == false)
    {
        primera_vez = 0;
    }
    else if(primera_vez == AHOGADA && rene.chocada == false)
    {
        primera_vez = 0;
    }
    else if(primera_vez == CHOQUE && rene.chocada == false)
    {
        primera_vez = 0;
    }

}



void allegro_destroy(void)
{
    al_destroy_timer(timer);
    al_destroy_event_queue(event_queue);
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
    al_destroy_sample(sample_rana_salto);
    al_destroy_sample(sample_rana_ahogada);
    al_destroy_sample(sample_rana_chocada);
    al_destroy_sample(sample_tiempo);
    al_destroy_sample(sample_musica_fondo);
    al_destroy_sample(sample_coin);
    al_destroy_font(font);
    al_destroy_bitmap(mundo_buffer);
    al_destroy_display(display);
    
}



#endif