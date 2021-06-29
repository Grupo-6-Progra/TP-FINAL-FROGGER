#include "juego.h"

#include <math.h>
#include <tgmath.h>

/***************************************************
 * VELOCIDADES Y TIEMPOS DE LA RANA
***************************************************/

#define SALTO_RANA_ANCHO        CASILLA_ANCHO
#define SALTO_RANA_ALTO         CASILLA_ALTO 

#define APROX(x)                ((int)((x)+0.5))

#define TIEMPO_SALTO_ANCHO      (1.0/12) // tiempo que tarda en saltar a lo ancho
#define FRAMES_POR_SALTO_ANCHO  (APROX(TIEMPO_SALTO_ANCHO * REFRESCO))
#define VELOCIDAD_RANA_ANCHO    (SALTO_RANA_ANCHO / FRAMES_POR_SALTO_ANCHO)
    
#define TIEMPO_SALTO_ALTO       (1.0/12)// tiempo que tarda en saltar a lo alto
#define FRAMES_POR_SALTO_ALTO   (APROX(TIEMPO_SALTO_ALTO * REFRESCO))
#define VELOCIDAD_RANA_ALTO     (SALTO_RANA_ALTO / FRAMES_POR_SALTO_ALTO)

#define TIEMPO_TARDA_SALTO      (1.0/12)
#define TARDA_SALTO             (TIEMPO_TARDA_SALTO * REFRESCO) //frames que espera entre salto y salto


/***************************************************
 *  DECLARACIÓN DE FUNCIONES LOCALES
***************************************************/
static void move_frog (void);

static void initialize_enemies (unsigned int nivel);

static void enemigos (unsigned int);

static void move_enemies(void);
/***************************************************
 *  DEFINICIÓN DE VARIABLES GLOBALES
***************************************************/

int estado_juego = INICIO;

bool key_pressed[TECLAS_MAX];

RANA rene = {.x = 8 * CASILLA_ANCHO - RANA_ALTO/2, .y = MUNDO_ALTO - RANA_ALTO/2, .dx = VELOCIDAD_RANA_ANCHO, .dy = VELOCIDAD_RANA_ALTO};

AUTOS autos_fila1[AUTOS_POR_FILA];
AUTOS autos_fila2[AUTOS_POR_FILA];
AUTOS autos_fila3[AUTOS_POR_FILA];
AUTOS autos_fila4[AUTOS_POR_FILA];
AUTOS autos_fila5[AUTOS_POR_FILA];


AUTOS acuaticos[5];


/******************************************************************************************
 ******************************************************************************************
 * 
 *     DEFINICIÓN DE FUNCIONES GLOBALES
 * 
 ******************************************************************************************
*******************************************************************************************/

void frogger (void)
{
    if(estado_juego == INICIO)
    {
        initialize_enemies (2);
    }
    
    estado_juego = JUEGO;
    
    move_enemies();
    
    move_frog();
    
}


/******************************************************************************************
 ******************************************************************************************
 * 
 *     DEFINICIÓN DE FUNCIONES LOCALES
 * 
 ******************************************************************************************
*******************************************************************************************/

static void move_frog (void)
{
    static unsigned int timer_up = 0, timer_down = 0, timer_right = 0, timer_left = 0;
    
    if ((key_pressed[KEY_UP] == true || timer_up != 0 ) && timer_down == 0 && timer_right == 0 && timer_left == 0)
    {
        if(timer_up == 0)
        {
            timer_up = (int) (FRAMES_POR_SALTO_ALTO + TARDA_SALTO);
        }
        
        if(timer_up > TARDA_SALTO && rene.y >= RANA_ALTO/2 + rene.dy)
        {
             rene.y -= rene.dy;
        }
        
        timer_up--;
        
    }
    
    if ( (key_pressed[KEY_DOWN] == true || timer_down != 0 ) && timer_up == 0 && timer_right == 0 && timer_left == 0)
    {
        if(timer_down == 0)
        {
            timer_down = (int) (FRAMES_POR_SALTO_ALTO + TARDA_SALTO);
        }
        
        if (timer_down > TARDA_SALTO && rene.y <= MUNDO_ALTO - RANA_ALTO/2 -rene.dy)
        {
            rene.y += rene.dy;
        }
        
        timer_down--;
    }
    
    if ((key_pressed[KEY_RIGHT] == true || timer_right != 0 ) && timer_down == 0 && timer_up== 0 && timer_left == 0)
    {
        if(timer_right == 0)
        {
            timer_right = (int) (FRAMES_POR_SALTO_ANCHO + TARDA_SALTO);
        }
        
        if (timer_right > TARDA_SALTO && rene.x <= MUNDO_ANCHO - RANA_ANCHO/2 - rene.dx)
        {
            rene.x += rene.dx;
        }
        
        timer_right--;
    }
    
    if ((key_pressed[KEY_LEFT] == true || timer_left != 0 ) && timer_down == 0 && timer_right == 0 && timer_up == 0)
    {
        if(timer_left == 0)
        {
            timer_left = (int) (FRAMES_POR_SALTO_ANCHO + TARDA_SALTO);
        }
        
        if(timer_left > TARDA_SALTO && rene.x >= RANA_ANCHO/2 + rene.dx)
        {
            rene.x -= rene.dx;
        }
        
        timer_left--;
    }
}


//LO PONGO ACA PERO HAY QUE SACARLO
enum const_autos {AUTO1,AUTO2,AUTO3,AUTO4,AUTO5};
enum const_acuaticos {TORTUGA1,TORTUGA2,TRONCO1,TRONCO2,TRONCO3};

static void initialize_enemies (unsigned int nivel)
/* 
 * Funcion que dependiendo del nivel que se esté jugando, inicializará a los enemigos con velocidades distintas
 * Se encarga de inicializar todos los campos de todos los enemigos.
 */
{
    int i;
    for(i=0; i < AUTOS_POR_FILA ; i++)  //inicialización de enemigos según nivel
    {
        autos_fila1[i].dx = (2 * nivel + i*0.5) * pow(-1, i); //cuenta arbitraria para aumentar la velocidad de los enemigos
        autos_fila1[i].fila = i + 1;
        autos_fila1[i].y = (CANT_CASILLAS_COLUMNA - autos_fila1[i].fila) * CASILLA_ALTO - CASILLA_ALTO / 2.0;
        autos_fila1[i].x = i * 100;
        autos_fila1[i].largo = CASILLA_ANCHO;
          
    }  
}

static void move_enemies(void)
{
    unsigned int i;
    for (i = 0; i < AUTOS_POR_FILA; i++)
    {
        autos_fila1[i].x += autos_fila1[i].dx;
    }
}