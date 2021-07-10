#include "juego.h"
#include "colisiones.h"
#include "menu.h"

#include <math.h>
#include <tgmath.h>
#include <stdbool.h>
#include <stdio.h>

/***************************************************
 * VELOCIDADES Y TIEMPOS DE LA RANA
***************************************************/

#define SALTO_RANA_ANCHO        CASILLA_ANCHO
#define SALTO_RANA_ALTO         CASILLA_ALTO 

#define APROX(x)                ((x>0) ? (int)((x)+0.5) : ((int)((x)-0.5)))

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
static void move_frog (bool);

static void initialize_frog(void);

static void initialize_enemies (unsigned int nivel);
static void initialize_autos(unsigned int);
static void initialize_troncos(unsigned int);
static void initialize_tortugas(unsigned int nivel);
static void initialize_llegada(void);

static void move_enemies(void);
static void move_autos(void);
static void move_troncos(void);
static void move_tortugas(void);

static int game_over(void);
/***************************************************
 *  DEFINICIÓN DE VARIABLES GLOBALES
***************************************************/

int estado_juego = MENU;

double tiempo_restante;

char vidas_restantes = 3;

int nivel = 1;

int selector_menu = PLAY;

bool key_pressed[TECLAS_MAX] = {false,false,false,false,false};

RANA rene;

AUTOS autos[FILAS_DE_AUTOS][AUTOS_POR_FILA];

TRONCO troncos [FILAS_DE_TRONCOS][TRONCOS_POR_FILA];

TORTUGAS tortugas[FILAS_DE_TORTUGAS][TORTUGAS_POR_FILA];

LLEGADA llegadas[CANT_CASILLAS_LLEGADA];


/******************************************************************************************
 ******************************************************************************************
 * 
 *     DEFINICIÓN DE FUNCIONES GLOBALES
 * 
 ******************************************************************************************
*******************************************************************************************/

bool frogger (void)
{
    static bool salir = false; //variable que indica si el programa debe terminar
    static int choque_en_proceso = 0;
    static int esperar_menu = 0;
    
    bool choque = false;
    switch(estado_juego)
    {
        
        case INICIO:
        {
            vidas_restantes = 3;
            initialize_enemies (nivel);
            initialize_frog();
            initialize_llegada();
            estado_juego = JUEGO;
            tiempo_restante = TIEMPO_TOTAL; //Inicializo el tiempo restante en 100 segundos
            break;
        }
        case REINICIO: //caso al que se entra cuando se pierde una vida
        {
            if(rene.llego == true)
            {
                if (choque_en_proceso == 0) //significa que acabo de ganar
                {
                    choque_en_proceso = 20;
                } 
                else if (choque_en_proceso > 0)
                {
                    choque_en_proceso--; //pierdo la cantidad de frames que quiera (por ahora) (la intención es hacer alguna animación después)
                    move_enemies();
                    if (choque_en_proceso == 0)
                    {
                        rene.llego = false;
                    }
                }
                else
                {
                    initialize_frog();
                    move_frog(true);
                    tiempo_restante = TIEMPO_TOTAL; 
                    estado_juego = JUEGO;
                }
            }
            else
            {
                if(vidas_restantes == 0)
                {
                    estado_juego = PERDER;
                    move_frog(true);
                }

                else if (rene.chocada == true && choque_en_proceso == 0) //significa que acabo de chocar
                {
                    choque_en_proceso = 20;
                }

                else if (choque_en_proceso > 0)
                {
                    choque_en_proceso--; //pierdo la cantidad de frames que quiera (por ahora) (la intención es hacer alguna animación después)
                    move_enemies();
                    if (choque_en_proceso == 0)
                    {
                        rene.chocada = false;
                    }
                }
                else
                {
                    initialize_frog();
                    move_frog(true);
                    tiempo_restante = TIEMPO_TOTAL;
                    estado_juego = JUEGO;
                }
            }
           
            
            break;
        }
        
        case JUEGO: //caso en el que la rana está viva
        {

            if (tiempo_restante-- > 0) //Decremento una vez por FPS el tiempo restante
            {
                move_enemies();
                
                move_frog(false);

                choque = colision();

                if(choque == true)
                {
                    vidas_restantes--;
                    rene.chocada = true;
                    estado_juego = REINICIO;
                }

                if((rana_sobre_tronco() == false) && (rana_sobre_tortuga() == false) && (rene.y <= CASILLA_ALTO * 6 && rene.y >= CASILLA_ALTO))
                {
                        vidas_restantes--;
                        rene.chocada = true;
                        estado_juego = REINICIO;
                }

                if((rene.y <= CASILLA_ALTO))
                {
                    if(rana_llego() == true)
                    {
                        rene.llego = true;
                        estado_juego = REINICIO;
                    }

                    else
                    {
                        vidas_restantes--;
                        rene.chocada = true;
                        estado_juego = REINICIO;
                    }
                }
            }
            else
            {
                vidas_restantes--;
                rene.chocada = true;
                estado_juego = REINICIO;
            }
            
            break;
        }
        case PAUSA:
        {
            break;
        }   
        case PERDER:
        {
            if(!game_over())
            {
                estado_juego = MENU;
            }
            break;
        }
        case MENU:
        {
            salir = menu_start();
            
            break;
        }
     
    }
    return salir;
}



/******************************************************************************************
 ******************************************************************************************
 * 
 *     DEFINICIÓN DE FUNCIONES LOCALES
 * 
 ******************************************************************************************
*******************************************************************************************/

static void move_frog (bool choque)
{
    static unsigned int timer_up = 0, timer_down = 0, timer_right = 0, timer_left = 0;
    
    if((rene.x <= MUNDO_ANCHO - RANA_ANCHO/2 - rene.dx_extra) && (rene.x >= RANA_ANCHO/2 + rene.dx_extra))
    {
        rene.x += rene.dx_extra; //se mueve la rana la velocidad extra adquirida por un tronco o tortuga
    }
    
    if (choque == true)
    {
        timer_up = 0;
        timer_down = 0;
        timer_left = 0;
        timer_right = 0;
    }
    
    else
    {

        if ((key_pressed[KEY_UP] == true || timer_up != 0 ) && timer_down == 0 && timer_right == 0 && timer_left == 0)
        {
            if(timer_up == 0)
            {
                timer_up = (int) (FRAMES_POR_SALTO_ALTO + TARDA_SALTO);
            }

            if(timer_up > TARDA_SALTO && rene.y >=  CASILLA_ALTO/2 + rene.dy)
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
}

static void initialize_frog(void)
{
    rene.x = 8 * CASILLA_ANCHO - CASILLA_ANCHO/2.0;
    rene.y = MUNDO_ALTO - CASILLA_ALTO/2.0;
    rene.dy = VELOCIDAD_RANA_ALTO;
    rene.dx = VELOCIDAD_RANA_ANCHO;
}

/**************************************
 * 
 * INITIALIZE_LLEGADA:
 *  
 ***************************************/

static void initialize_llegada(void)
{
    int i;
    
    for(i = 0; i < CANT_CASILLAS_LLEGADA; i++)
    {
        llegadas[i].x = 0.5*CASILLA_ANCHO + i * (1.2+1.75) * CASILLA_ANCHO + 1.2*CASILLA_ANCHO/2.0;
        llegadas[i].y = CASILLA_ALTO / 2.0;
        llegadas[i].alto = CASILLA_ALTO;
        llegadas[i].ancho = 1.2 * CASILLA_ANCHO;
        llegadas[i].ocupado = false;
    }
    
}

/**************************************
 * 
 * INITIALIZE_ENEMIES:
 *  
 ***************************************/

static void initialize_enemies (unsigned int nivel)
{
    /* 
 * Funcion que dependiendo del nivel que se esté jugando, inicializará a los enemigos con velocidades distintas
 * Se encarga de inicializar todos los campos de todos los enemigos.
 */
    initialize_autos(nivel);
    initialize_troncos(nivel);
    initialize_tortugas(nivel);
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
        if(j == 0)
        {       
            for(k = 0; k < TRONCOS_POR_FILA; k++)                               //Acá se inicializan los autos DE CADA FILA
            {
                troncos[j][k].dx = (nivel/2.0 + 0.2*j) * pow(-1,j);
                troncos[j][k].fila = 8;                                                                                 //necesito que los autos empiecen en la fila 1
                troncos[j][k].y = (CANT_CASILLAS_COLUMNA - troncos[j][k].fila) * CASILLA_ALTO - CASILLA_ALTO / 2.0;
                troncos[j][k].x = k * MUNDO_ANCHO / 2.0;                      //Hago que aparezcan como máximo 3 enemigos por fila a la vez
                troncos[j][k].largo = CASILLA_ANCHO * 3;                          //Cada enemigo será tan ancho como una casilla
                troncos[j][k].alto = CASILLA_ALTO;
            }
        }
        
        if(j == 1)
        {
            for(k = 0; k < TRONCOS_POR_FILA; k++) 
            {
                troncos[j][k].dx = (nivel/2.0 + 0.2*j) * pow(-1,j);
                troncos[j][k].fila = 9;                                                                                      //necesito que los autos empiecen en la fila 1
                troncos[j][k].y = (CANT_CASILLAS_COLUMNA - troncos[j][k].fila) * CASILLA_ALTO - CASILLA_ALTO / 2.0;
                troncos[j][k].x = k * MUNDO_ANCHO / 2.0;                      //Hago que aparezcan como máximo 3 enemigos por fila a la vez
                troncos[j][k].largo = CASILLA_ANCHO * 5;                          //Cada enemigo será tan ancho como una casilla
                troncos[j][k].alto = CASILLA_ALTO;
            }
        }
        
        if(j == 2)
        {
            for(k = 0; k < TRONCOS_POR_FILA; k++) 
            {
                troncos[j][k].dx = (nivel/2.0 + 0.2*j) * pow(-1,j);
                troncos[j][k].fila = 11;                                                                                      //necesito que los autos empiecen en la fila 1
                troncos[j][k].y = (CANT_CASILLAS_COLUMNA - troncos[j][k].fila) * CASILLA_ALTO - CASILLA_ALTO / 2.0;
                troncos[j][k].x = k * MUNDO_ANCHO / 2.0;                      //Hago que aparezcan como máximo 3 enemigos por fila a la vez
                troncos[j][k].largo = CASILLA_ANCHO * 4;                          //Cada enemigo será tan ancho como una casilla
                troncos[j][k].alto = CASILLA_ALTO;
            }
        }
        
    }
}

static void initialize_tortugas(unsigned int nivel)
{
    int j;
    int k;
    for(j=0; j < FILAS_DE_TORTUGAS; j++)                                   //Cada ciclo de este loop trabaja sobre una fila distinta
    {
        if(j == 0)
        {       
            for(k = 0; k < TORTUGAS_POR_FILA; k++)                               //Acá se inicializan los autos DE CADA FILA
            {
                tortugas[j][k].dx = (nivel/2.0 + 0.2*j) * pow(-1,j);
                tortugas[j][k].fila = 7;                                                                                 //necesito que los autos empiecen en la fila 1
                tortugas[j][k].y = (CANT_CASILLAS_COLUMNA - tortugas[j][k].fila) * CASILLA_ALTO - CASILLA_ALTO / 2.0;
                tortugas[j][k].x = k * MUNDO_ANCHO / 2.0;                      //Hago que aparezcan como máximo 3 enemigos por fila a la vez
                tortugas[j][k].largo = CASILLA_ANCHO * 3;                          //Cada enemigo será tan ancho como una casilla
                tortugas[j][k].alto = CASILLA_ALTO;
                tortugas[j][k].hundirse = false;
            }
        }
        
        if(j == 1)
        {
            for(k = 0; k < TORTUGAS_POR_FILA; k++) 
            {
                tortugas[j][k].dx = (nivel/2.0 + 0.2*j) * pow(-1,j);
                tortugas[j][k].fila = 10;                                                                                 //necesito que los autos empiecen en la fila 1
                tortugas[j][k].y = (CANT_CASILLAS_COLUMNA - tortugas[j][k].fila) * CASILLA_ALTO - CASILLA_ALTO / 2.0;
                tortugas[j][k].x = k * MUNDO_ANCHO / 2.0;                      //Hago que aparezcan como máximo 3 enemigos por fila a la vez
                tortugas[j][k].largo = CASILLA_ANCHO * 2;                          //Cada enemigo será tan ancho como una casilla
                tortugas[j][k].alto = CASILLA_ALTO;
                tortugas[j][k].hundirse = false;
            }
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
    move_tortugas();
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
            
            if(autos[j][k].x < -MUNDO_ANCHO || autos[j][k].x > MUNDO_ANCHO*2.0)
            {
                if(autos[j][k].x < -MUNDO_ANCHO)
                {
                    autos[j][k].x = MUNDO_ANCHO + MUNDO_ANCHO;
                }
                else
                {
                    autos[j][k].x = -MUNDO_ANCHO;
                }
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
            
            if(troncos[j][k].x < -MUNDO_ANCHO || troncos[j][k].x > MUNDO_ANCHO*2.0)
            {
                if(troncos[j][k].x < -MUNDO_ANCHO)
                {
                    troncos[j][k].x = MUNDO_ANCHO + MUNDO_ANCHO;
                }
                else
                {
                    troncos[j][k].x = -MUNDO_ANCHO;
                }
            }
            
            /*El sentido de esta cuenta es hacer que si el enemigo se mueve a la izquierda, su posición al reaparecer sea la derecha
             *el primer paréntesis devuelve el signo y lo que sigue (Según sea el signo obtenido) suma o resta "un mundo" desde el centro
             * del mundo (significado del primer MUNDO_ANCHO/2.0)
             * Si se mueve a la izquierda "dx" es negativo, entonces la cuenta es MUNDO_ANCHO/2.0 + (MUNDO_ANCHO)
             * El significado de sumar un mundo desde el centro es que tarden en volver a desaparecer/ aparecer lo que tardan en recorren un mundo
             */
            
            troncos[j][k].x += troncos[j][k].dx;
        }
    }
}

static void move_tortugas(void)
{
    unsigned int j, k;
    static unsigned int timer_hundirse = FRAMES_HASTA_HUNDIRSE;
    
    for(j=0; j < FILAS_DE_TORTUGAS; j++)
    {
        
        for(k=0; k < TORTUGAS_POR_FILA; k++)
        {
            /* Analizo si los enemigos están dentro del campo de movimiento
             * , sino, los teletransporto al lado contrario de donde desaparecieron*/
            
            if(tortugas[j][k].x < -MUNDO_ANCHO || tortugas[j][k].x > MUNDO_ANCHO*2.0)
            {
                if(tortugas[j][k].x < -MUNDO_ANCHO)
                {
                    tortugas[j][k].x = MUNDO_ANCHO + MUNDO_ANCHO;
                }
                else
                {
                    tortugas[j][k].x = -MUNDO_ANCHO;
                }
            }
            tortugas[j][k].x += tortugas[j][k].dx;
        }
    }
    
    if (timer_hundirse == 0)
    {
        int i;
        for (i=0; i<2; i++)
        {
            if (tortugas[i][0].hundirse == false)
            {
                tortugas[i][0].hundirse = true;
            }
            else
            {
                tortugas[i][0].hundirse = false;
            }
        }
        
        
        timer_hundirse = FRAMES_HASTA_HUNDIRSE;
    }
    
    timer_hundirse--;
}

/**************************************
 * 
 * GAMER_OVER():
 *  
 ***************************************/

static int game_over (void)
{
    static int contador_de_frames = 0;
    if (contador_de_frames == 0)
    {
        contador_de_frames = CANT_CASILLAS_FILA * (CANT_CASILLAS_COLUMNA + 1);
        rene.x = CASILLA_ANCHO / 2.0;
        rene.y = CASILLA_ALTO / 2.0;
    }
    contador_de_frames--;
    if(rene.x <= MUNDO_ANCHO - RANA_ANCHO / 2.0)
    {
        rene.x += rene.dx * TARDA_SALTO;
    }
    else
    {
        rene.x = CASILLA_ANCHO / 2.0;
        rene.y += CASILLA_ALTO;
    }
    return contador_de_frames;
}
