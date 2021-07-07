#include "juego.h"
#include "colisiones.h"

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

static void initialize_frog(void);

static void initialize_enemies (unsigned int nivel);
static void initialize_autos(unsigned int);
static void initialize_troncos(unsigned int);

static void enemigos (unsigned int);

static void move_enemies(void);
static void move_autos(void);
static void move_troncos(void);

/***************************************************
 *  DEFINICIÓN DE VARIABLES GLOBALES
***************************************************/

int estado_juego = INICIO;

bool key_pressed[TECLAS_MAX] = {false,false,false,false,false};

RANA rene;

AUTOS autos[FILAS_DE_AUTOS][AUTOS_POR_FILA];

TRONCO troncos [FILAS_DE_TRONCOS][TRONCOS_POR_FILA];


/******************************************************************************************
 ******************************************************************************************
 * 
 *     DEFINICIÓN DE FUNCIONES GLOBALES
 * 
 ******************************************************************************************
*******************************************************************************************/

void frogger (void)
{
    switch(estado_juego)
    {
        case INICIO:
            initialize_enemies (2);
            initialize_frog();
            estado_juego = JUEGO;
            
            break;
        
        case JUEGO:
            move_enemies();
            move_frog();
            
            if(colision())
            {
               //estado_juego = INICIO;
            }
            
            if((rana_sobre_tronco() == false) && (rene.y <= CASILLA_ALTO * 6 && rene.y >= CASILLA_ALTO))
            {
                  //estado_juego = INICIO;
            }
            
            break;
            
        case PAUSA:
            
            break;
            
     
    }
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
    
    rene.x += rene.dx_extra; //se mueve la rana la velocidad extra adquirida por un tronco o tortuga
    
    if ((key_pressed[KEY_UP] == true || timer_up != 0 ) && timer_down == 0 && timer_right == 0 && timer_left == 0)
    {
        if(timer_up == 0)
        {
            timer_up = (int) (FRAMES_POR_SALTO_ALTO + TARDA_SALTO);
        }
        
        if(timer_up > TARDA_SALTO && rene.y >= CASILLA_ALTO/2 + rene.dy)
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
        
        if (timer_down > TARDA_SALTO && rene.y <= MUNDO_ALTO - CASILLA_ALTO/2 -rene.dy)
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
        
        if (timer_right > TARDA_SALTO && rene.x <= MUNDO_ANCHO - CASILLA_ANCHO/2 - rene.dx)
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
        
        if(timer_left > TARDA_SALTO && rene.x >= CASILLA_ANCHO/2 + rene.dx)
        {
            rene.x -= rene.dx;
        }
        
        timer_left--;
    }
}

static void initialize_frog(void)
{
    rene.x = 8 * CASILLA_ANCHO - CASILLA_ANCHO/2.0;
    rene.y = MUNDO_ALTO - CASILLA_ALTO/2.0;
    rene.dy = VELOCIDAD_RANA_ALTO;
    rene.dx = VELOCIDAD_RANA_ANCHO;
    rene.dx_extra = 0;
}


/**************************************
 * 
 * INITIALIZE_ENEMIES:
 *  
 ***************************************/

static void initialize_enemies (unsigned int nivel)
/* 
 * Funcion que dependiendo del nivel que se esté jugando, inicializará a los enemigos con velocidades distintas
 * Se encarga de inicializar todos los campos de todos los enemigos.
 */
{
    initialize_autos(nivel);
    initialize_troncos(nivel);
}

static void initialize_autos(unsigned int nivel)
{
    int j;
    int k;
    for(j=0; j < FILAS_DE_AUTOS; j++)                                   //Cada ciclo de este loop trabaja sobre una fila distinta
    {
        for(k=0; k < AUTOS_POR_FILA; k++)                               //Acá se inicializan los autos DE CADA FILA
        {
            autos[j][k].dx = (nivel/2.0 + 0.2*j) * pow(-1,j);
            autos[j][k].fila = j + 1;                                   //necesito que los autos empiecen en la fila 1
            autos[j][k].y = (CANT_CASILLAS_COLUMNA - autos[j][k].fila) * CASILLA_ALTO - CASILLA_ALTO / 2.0;
            autos[j][k].x = k * MUNDO_ANCHO / 2.0;                      //Hago que aparezcan como máximo 3 enemigos por fila a la vez
            autos[j][k].largo = CASILLA_ANCHO;                          //Cada enemigo será tan ancho como una casilla
            autos[j][k].alto = CASILLA_ALTO;
        }
    }
}

static void initialize_troncos(unsigned int nivel)
{
    int j;
    int k;
    for(j=0; j < FILAS_DE_TRONCOS; j++)                                   //Cada ciclo de este loop trabaja sobre una fila distinta
    {
        for(k=0; k < TRONCOS_POR_FILA; k++)                               //Acá se inicializan los autos DE CADA FILA
        {
            troncos[j][k].dx = (nivel/2.0 + 0.2*j) * pow(-1,j);
            troncos[j][k].fila = j + 1;                                   //necesito que los autos empiecen en la fila 1
            troncos[j][k].y = (CANT_CASILLAS_COLUMNA - 6 - troncos[j][k].fila) * CASILLA_ALTO - CASILLA_ALTO / 2.0;
            troncos[j][k].x = k * MUNDO_ANCHO / 2.0;                      //Hago que aparezcan como máximo 3 enemigos por fila a la vez
            troncos[j][k].largo = CASILLA_ANCHO * 2;                          //Cada enemigo será tan ancho como una casilla
            troncos[j][k].alto = CASILLA_ALTO;
        }
    }
}

/**************************************
 * 
 * MOVE_ENEMIES:
 *  
 ***************************************/

static void move_enemies(void)
{
    move_autos();
    move_troncos();
}

static void move_autos(void)
{
    unsigned int j, k;
    for(j=0; j < FILAS_DE_AUTOS; j++)
    {
        
        for(k=0; k < AUTOS_POR_FILA; k++)
        {
            /* Analizo si los enemigos están dentro del campo de movimiento
             * , sino, los teletransporto al lado contrario de donde desaparecieron*/
            
            if(autos[j][k].x < -MUNDO_ANCHO/2.0 || autos[j][k].x > MUNDO_ANCHO*2.0)
            {
                autos[j][k].x = MUNDO_ANCHO/2.0 - ( (autos[j][k].dx / fabs(autos[j][k].dx)) * MUNDO_ANCHO);
            
            /*El sentido de esta cuenta es hacer que si el enemigo se mueve a la izquierda, su posición al reaparecer sea la derecha
             *el primer paréntesis devuelve el signo y lo que sigue (Según sea el signo obtenido) suma o resta "un mundo" desde el centro
             * del mundo (significado del primer MUNDO_ANCHO/2.0)
             * Si se mueve a la izquierda "dx" es negativo, entonces la cuenta es MUNDO_ANCHO/2.0 + (MUNDO_ANCHO)
             * El significado de sumar un mundo desde el centro es que tarden en volver a desaparecer/ aparecer lo que tardan en recorren un mundo
             */
            }
            autos[j][k].x += autos[j][k].dx;
        }
    }
}

static void move_troncos(void)
{
    unsigned int j, k;
    for(j=0; j < FILAS_DE_TRONCOS; j++)
    {
        
        for(k=0; k < TRONCOS_POR_FILA; k++)
        {
            /* Analizo si los enemigos están dentro del campo de movimiento
             * , sino, los teletransporto al lado contrario de donde desaparecieron*/
            
            if(troncos[j][k].x < -MUNDO_ANCHO/2.0 || troncos[j][k].x > MUNDO_ANCHO*2.0)
            {
                troncos[j][k].x = MUNDO_ANCHO/2.0 - ( (troncos[j][k].dx / fabs(troncos[j][k].dx)) * MUNDO_ANCHO);
            
            /*El sentido de esta cuenta es hacer que si el enemigo se mueve a la izquierda, su posición al reaparecer sea la derecha
             *el primer paréntesis devuelve el signo y lo que sigue (Según sea el signo obtenido) suma o resta "un mundo" desde el centro
             * del mundo (significado del primer MUNDO_ANCHO/2.0)
             * Si se mueve a la izquierda "dx" es negativo, entonces la cuenta es MUNDO_ANCHO/2.0 + (MUNDO_ANCHO)
             * El significado de sumar un mundo desde el centro es que tarden en volver a desaparecer/ aparecer lo que tardan en recorren un mundo
             */
            }
            troncos[j][k].x += troncos[j][k].dx;
        }
    }
}