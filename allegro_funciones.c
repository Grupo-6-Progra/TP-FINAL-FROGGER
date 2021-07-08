#include "allegro_funciones.h"
#include "juego.h"

#include <stdbool.h>

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
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

#define SCREEN_SCALE    1.5
#define SCREEN_W        (BUFFER_W * SCREEN_SCALE)
#define SCREEN_H        (BUFFER_H * SCREEN_SCALE)


/***************************************************
 *  DEFINICIÓN DE VARIABLES GLOBALES
***************************************************/
ALLEGRO_DISPLAY     * display;
ALLEGRO_EVENT_QUEUE * event_queue;
ALLEGRO_TIMER       * timer;
ALLEGRO_BITMAP      * mundo_buffer;
ALLEGRO_BITMAP      * al_rene;
ALLEGRO_BITMAP      * al_auto_fila1;
ALLEGRO_BITMAP      * al_tronco1;
ALLEGRO_BITMAP      * al_tronco2;
ALLEGRO_BITMAP      * al_tronco3;
ALLEGRO_BITMAP      * al_tortuga1;
ALLEGRO_BITMAP      * al_tortuga2;
ALLEGRO_BITMAP      * fondo;
ALLEGRO_FONT        * font;

/***************************************************
 *  DECLARACIÓN DE FUNCIONES LOCALES
***************************************************/
static void redraw_autos(void);
static void redraw_troncos(void);
static void redraw_tortugas(void);



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
    
    if (!al_init_font_addon()) // Inicializa el complemento de fuentes
    {
        fprintf(stderr, "failed to initialize the fonts!\n");
        return false;
    }
    
    if (!al_init_ttf_addon()) // Inicializa el complemento que permite reconocer fuentes ttf
    {
        fprintf(stderr, "failed to initialize the ttf addon!\n");
        return false;
        
    }
    
    if(!al_init_image_addon())
    {
        fprintf(stderr, "failed to initialize allegro images!\n");
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
        return false;
    }
    
    al_auto_fila1 = al_create_bitmap(CASILLA_ANCHO, CASILLA_ALTO);
    if(!al_auto_fila1)
    {
        fprintf(stderr, "failed to create al_auto_fila1!\n");
        al_destroy_timer(timer);
        al_destroy_event_queue(event_queue);
        al_destroy_bitmap(mundo_buffer);
        al_destroy_bitmap(al_rene);
        return false;
    }
    
    al_tronco1 = al_create_bitmap(CASILLA_ANCHO*3, CASILLA_ALTO);
    if(!al_tronco1)
    {
        fprintf(stderr, "failed to create al_tronco1!\n");
        al_destroy_timer(timer);
        al_destroy_event_queue(event_queue);
        al_destroy_bitmap(mundo_buffer);
        al_destroy_bitmap(al_rene);
        al_destroy_bitmap(al_auto_fila1);
        return false;
    }
    
    al_tronco2 = al_create_bitmap(CASILLA_ANCHO*5, CASILLA_ALTO);
    if(!al_tronco2)
    {
        fprintf(stderr, "failed to create al_tronco2!\n");
        al_destroy_timer(timer);
        al_destroy_event_queue(event_queue);
        al_destroy_bitmap(mundo_buffer);
        al_destroy_bitmap(al_rene);
        al_destroy_bitmap(al_auto_fila1);
        al_destroy_bitmap(al_tronco1);
        return false;
    }
    
    al_tronco3 = al_create_bitmap(CASILLA_ANCHO*4, CASILLA_ALTO);
    if(!al_tronco3)
    {
        fprintf(stderr, "failed to create al_tronco3!\n");
        al_destroy_timer(timer);
        al_destroy_event_queue(event_queue);
        al_destroy_bitmap(mundo_buffer);
        al_destroy_bitmap(al_rene);
        al_destroy_bitmap(al_auto_fila1);
        al_destroy_bitmap(al_tronco1);
        al_destroy_bitmap(al_tronco2);
        return false;
    }
    
    al_tortuga1 = al_create_bitmap(CASILLA_ANCHO*3, CASILLA_ALTO);
    if(!al_tortuga1)
    {
        fprintf(stderr, "failed to create al_tronco3!\n");
        al_destroy_timer(timer);
        al_destroy_event_queue(event_queue);
        al_destroy_bitmap(mundo_buffer);
        al_destroy_bitmap(al_rene);
        al_destroy_bitmap(al_auto_fila1);
        al_destroy_bitmap(al_tronco1);
        al_destroy_bitmap(al_tronco2);
        al_destroy_bitmap(al_tronco3);
        return false;
    }
    
    al_tortuga2 = al_create_bitmap(CASILLA_ANCHO*2, CASILLA_ALTO);
    if(!al_tortuga2)
    {
        fprintf(stderr, "failed to create al_tronco3!\n");
        al_destroy_timer(timer);
        al_destroy_event_queue(event_queue);
        al_destroy_bitmap(mundo_buffer);
        al_destroy_bitmap(al_rene);
        al_destroy_bitmap(al_auto_fila1);
        al_destroy_bitmap(al_tronco1);
        al_destroy_bitmap(al_tronco2);
        al_destroy_bitmap(al_tronco3);
        al_destroy_bitmap(al_tortuga1);
        return false;
    }
    
    fondo = al_load_bitmap("fondo2.png");
    if(!fondo)
    {
        fprintf(stderr, "failed to create fondo!\n");
        al_destroy_timer(timer);
        al_destroy_event_queue(event_queue);
        al_destroy_bitmap(mundo_buffer);
        al_destroy_bitmap(al_rene);
        al_destroy_bitmap(al_auto_fila1);
        al_destroy_bitmap(al_tronco1);
        al_destroy_bitmap(al_tronco2);
        al_destroy_bitmap(al_tronco3);
        al_destroy_bitmap(al_tortuga1);
        al_destroy_bitmap(al_tortuga2);
        return false;
    }
    
    display = al_create_display(SCREEN_W, SCREEN_H);
    if (!display)
    {
        fprintf(stderr, "failed to create display!\n");
        al_destroy_timer(timer);
        al_destroy_event_queue(event_queue);
        al_destroy_bitmap(mundo_buffer);
        al_destroy_bitmap(al_rene);
        al_destroy_bitmap(al_auto_fila1);
        al_destroy_bitmap(al_tronco1);
        al_destroy_bitmap(al_tronco2);
        al_destroy_bitmap(al_tronco3);
        al_destroy_bitmap(al_tortuga1);
        al_destroy_bitmap(al_tortuga2);
        al_destroy_bitmap(fondo);
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

    if (!font) {
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

            case ALLEGRO_KEY_ESCAPE:
                do_exit = true;
                break;
        }
    }
    
    return do_exit;
}


void allegro_initialize_bitmaps(void)
{
    al_set_target_bitmap(al_rene);
    al_clear_to_color(al_map_rgb(0,255,0));
    
    al_set_target_bitmap(al_auto_fila1);
    al_clear_to_color(al_map_rgb(150,152,154));
    
    
    
    al_set_target_bitmap(al_tronco1);
    al_clear_to_color(al_map_rgb(80,40,0));
    
    al_set_target_bitmap(al_tronco2);
    al_clear_to_color(al_map_rgb(80,40,0));
    
    al_set_target_bitmap(al_tronco3);
    al_clear_to_color(al_map_rgb(80,40,0));
    
    
    al_set_target_bitmap(al_tortuga1);
    al_clear_to_color(al_map_rgb(255,0,0));
    
    al_set_target_bitmap(al_tortuga2);
    al_clear_to_color(al_map_rgb(255,0,0));
    
    
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
    al_set_target_bitmap(mundo_buffer);
    //al_clear_to_color(al_map_rgb(0,0,0));
    al_draw_scaled_bitmap(fondo,0,0,448,422, 0, 0, BUFFER_W, BUFFER_H, 0);
    
    
    //IMPRESION DE LOS AUTOS
    redraw_autos();
    
    
    //impresión de troncos
    redraw_troncos();
    
    //IMPRESIÓN DE TORTUGAS
    redraw_tortugas();
    
    //IMPRESIÓN DE LA RANA
    al_draw_bitmap(al_rene,rene.x - RANA_ANCHO/2, rene.y - RANA_ALTO/2, 0);
    
    switch(tortugas[1][0].hundirse)
    {
        case true:
        {
            al_draw_text(font, al_map_rgb(255, 255, 255), MUNDO_ANCHO - 15, 15, ALLEGRO_ALIGN_RIGHT, "true");
            //           fuente         color               ancho          alto    flag            texto
            break;
        }
        case false:
        {
            al_draw_text(font, al_map_rgb(255, 255, 255), MUNDO_ANCHO - 15, 15, ALLEGRO_ALIGN_RIGHT, "false");
            //           fuente         color               ancho          alto    flag            texto
            break;
        }
        case 3:
        {
            al_draw_text(font, al_map_rgb(255, 255, 255), MUNDO_ANCHO - 15, 15, ALLEGRO_ALIGN_RIGHT, "3");
            //           fuente         color               ancho          alto    flag            texto
            break;
        }
        default:
        {
            al_draw_text(font, al_map_rgb(255, 255, 255), MUNDO_ANCHO - 15, 15, ALLEGRO_ALIGN_RIGHT, "Error, algo salió mal");
            //           fuente         color               ancho          alto    flag            texto
            break;
        }
    }
    
    al_set_target_backbuffer(display);
    al_draw_scaled_bitmap(mundo_buffer, 0, 0, BUFFER_W, BUFFER_H, 0, 0, SCREEN_W, SCREEN_H, 0);

    al_flip_display();
}

static void redraw_autos(void)
{
    unsigned int j,k;
    
    //IMPRESION DE LOS AUTOS
    for(j=0; j < FILAS_DE_AUTOS; j++)
    {
        for(k=0; k<AUTOS_POR_FILA; k++)
        {
            al_draw_bitmap(al_auto_fila1, autos[j][k].x - autos[j][k].largo/2, autos[j][k].y - autos[j][k].alto/2, 0);
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
                al_draw_bitmap(al_tronco1, troncos[j][k].x - troncos[j][k].largo/2, troncos[j][k].y - troncos[j][k].alto/2, 0);
            }
            if(j == 1)
            {
                al_draw_bitmap(al_tronco2, troncos[j][k].x - troncos[j][k].largo/2, troncos[j][k].y - troncos[j][k].alto/2, 0);
            }
            if(j == 2)
            {
                al_draw_bitmap(al_tronco3, troncos[j][k].x - troncos[j][k].largo/2, troncos[j][k].y - troncos[j][k].alto/2, 0);
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
                    al_draw_bitmap(al_tortuga1, tortugas[j][k].x - tortugas[j][k].largo/2, tortugas[j][k].y - tortugas[j][k].alto/2, 0);
                }
                
                if(j == 1)
                {
                    al_draw_bitmap(al_tortuga2, tortugas[j][k].x - tortugas[j][k].largo/2, tortugas[j][k].y - tortugas[j][k].alto/2, 0);
                }
            }
        }
    }
}


void allegro_destroy(void)
{

    al_destroy_timer(timer);
    al_destroy_event_queue(event_queue);
    al_destroy_bitmap(mundo_buffer);
    al_destroy_bitmap(al_rene);
    al_destroy_bitmap(al_auto_fila1);
    al_destroy_bitmap(al_tronco1);
    al_destroy_bitmap(al_tronco2);
    al_destroy_bitmap(al_tronco3);
    al_destroy_bitmap(al_tortuga1);
    al_destroy_bitmap(al_tortuga2);
    al_destroy_bitmap(fondo);
    al_destroy_display(display);
}