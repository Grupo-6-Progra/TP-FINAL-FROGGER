#include "juego.h"
#include "colisiones.h"
#include "menu.h"

#include <math.h>
#include <tgmath.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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
 * VELOCIDADES Y TIEMPOS POR DEFAULT
***************************************************/
#define UN_SEGUNDO                (1*REFRESCO)

#define TIEMPO_UNITARIO_ANCHO     (5.0/6.0) //en segundos
#define TIEMPO_UNITARIO_ALTO      (5.0/6.0)

#define VELOCIDAD_UNITARIA_ANCHO        (CASILLA_ANCHO / (TIEMPO_UNITARIO_ANCHO * REFRESCO))
#define VELOCIDAD_UNITARIA_ALTO        (CASILLA_ALTO / (TIEMPO_UNITARIO_ALTO * REFRESCO))


/***************************************************
 * TIEMPOS DE LOS AUTOS
***************************************************/
#define TIEMPO_CAMBIAR_CARRIL       5.0 //en segundos
#define FRAMES_CAMBIAR_CARRIL       (APROX(TIEMPO_CAMBIAR_CARRIL * REFRESCO))

#define TIEMPO_CAMBIAR_VELOCIDAD      5.0 //en segundos
#define FRAMES_CAMBIAR_VELOCIDAD      (APROX(TIEMPO_CAMBIAR_VELOCIDAD * REFRESCO))

/***************************************************
 * TIEMPOS DE LAS TORTUGAS
***************************************************/

#define TIEMPO_HASTA_HUNDIRSE   2.0 //en segundos
#define FRAMES_HASTA_HUNDIRSE   (APROX(REFRESCO * TIEMPO_HASTA_HUNDIRSE))

/***************************************************
 * TIEMPOS DEL COCODRILO
***************************************************/
#define TIEMPO_SIN_COCODRILO      5.0 //en segundos
#define FRAMES_SIN_COCODRILO     (APROX(TIEMPO_SIN_COCODRILO * REFRESCO))

#define TIEMPO_CON_COCODRILO     1.0 //en segundos
#define FRAMES_CON_COCODRILO     (APROX(TIEMPO_CON_COCODRILO  * REFRESCO))

/***************************************************
 * TIEMPO DE CHOQUE
***************************************************/
#define TIEMPO_CHOQUE     1.0 //en segundos
#define FRAMES_CHOQUE     (APROX(TIEMPO_CHOQUE * REFRESCO))

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

static void move_enemies(int nivel);
static void move_autos(int nivel);
static void move_troncos(int nivel);
static void move_tortugas(int nivel);
static void move_cocodrilo(int nivel);

static void sumar_puntaje_salto(void);

static int game_over(void);

/***************************************************
 *  DEFINICIÓN DE VARIABLES GLOBALES
***************************************************/

int estado_juego = MENU;

double tiempo_restante;

char vidas_restantes = 3;

char nivel = 1;

unsigned long int puntaje_juego=0;

int selector_menu = PLAY;

int timer_pasar_nivel = 0;

int timer_perder = 0;


bool key_pressed[TECLAS_MAX] = {false,false,false,false,false};

RANA rene;

AUTOS autos[FILAS_DE_AUTOS][AUTOS_POR_FILA];

TRONCO troncos [FILAS_DE_TRONCOS][TRONCOS_POR_FILA];

TORTUGAS tortugas [FILAS_DE_TORTUGAS][TORTUGAS_POR_FILA];

LLEGADA llegadas [CANT_CASILLAS_LLEGADA];


/******************************************************************************************
 ******************************************************************************************
 * 
 *     DEFINICIÓN DE FUNCIONES GLOBALES
 * 
 ******************************************************************************************
*******************************************************************************************/

 /******************************************************************************************
 * 
 *  FROGGER: Función principal
 * 
 ******************************************************************************************/

bool frogger (void)
{
    static bool salir = false; //variable que indica si el programa debe terminar
    static int choque_en_proceso = 0;
    static int esperar_menu = 0;
    
    static bool enter_prev = false;
    
    bool choque = false;
    switch(estado_juego)
    {
        
        case MENU: //menu de inicio
        {
            salir = menu_start();
            
            break;
        }
        
        case INICIO: //inicio de un nivel
        {
            vidas_restantes = 3;
            initialize_enemies (nivel);
            initialize_frog();
            initialize_llegada();
            estado_juego = JUEGO;
            tiempo_restante = TIEMPO_TOTAL; //Inicializo el tiempo restante en 100 segundos
            break;
        }
        
        case PASAR_NIVEL:
        {
            if(timer_pasar_nivel == 0)
            {
                timer_pasar_nivel = UN_SEGUNDO*10;
            }
            else
            {
                timer_pasar_nivel--;
                if(timer_pasar_nivel == 0)
                {
                    nivel++;
                    move_frog(true);
                    estado_juego = INICIO;
                }
            }
            
            break;
        }
        
        case JUEGO: //caso en el que la rana está viva
        {

            if((key_pressed[KEY_ENTER] == true) || (enter_prev == true))
            {
                if (enter_prev == false)
                {
                    enter_prev = true;
                }
                else if (enter_prev == true && key_pressed[KEY_ENTER] == false)
                {
                    enter_prev = false;
                    estado_juego = PAUSA;
                }
            }
            else if (tiempo_restante-- > 0) //Decremento una vez por FPS el tiempo restante
            {
                move_enemies(nivel);
                
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
                        puntaje_juego += APROX(100 * (((tiempo_restante*1.0 / TIEMPO_TOTAL)) + 1));
                        estado_juego = REINICIO;
                    }

                    else
                    {
                        vidas_restantes--;
                        rene.chocada = true;
                        estado_juego = REINICIO;
                    }
                }
                
                sumar_puntaje_salto();
                
            }
            else
            {
                vidas_restantes--;
                rene.chocada = true;
                estado_juego = REINICIO;
            }
            
            break;
        }
        
        case REINICIO: //caso al que se entra cuando se pierde una vida
        {
            if(rene.llego == true)
            {                              
                if (choque_en_proceso == 0) //significa que acabo de ganar
                {
                    choque_en_proceso = FRAMES_CHOQUE;
                }
                else if (choque_en_proceso > 0)
                {
                    choque_en_proceso--;
                    move_enemies(nivel);
                    if (choque_en_proceso == 0)
                    {
                        rene.llego = false;
                        int casillas_llegadas = 0;
                        int i;

                        for (i = 0; i < 5; i++)
                        {
                            if(llegadas[i].ocupado == true)
                            {
                                casillas_llegadas++;
                            }
                        }

                        if(casillas_llegadas == 5) //se completaron todas las casillas y se pasó de nivel
                        {
                            puntaje_juego += 500*nivel;
                            estado_juego = PASAR_NIVEL;
                        }

                        else //se llegó al otro lado, pero no se completaron todas las casillas
                        {
                            initialize_frog();
                            move_frog(true);
                            tiempo_restante = TIEMPO_TOTAL; 
                            estado_juego = JUEGO;
                        }
                    }
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
                    choque_en_proceso = FRAMES_CHOQUE;
                    rene.frame_chocada = 1;
                }

                else if (choque_en_proceso > 0)
                {
                    choque_en_proceso--; //pierdo la cantidad de frames que quiera (por ahora) (la intención es hacer alguna animación después)
                    
                    move_enemies(nivel);
                    
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
        
        case PAUSA:
        {
            salir = menu_pausa();
            break;
        }   
        
        case PERDER:
        {
            if(timer_perder == 0)
            {
                timer_perder = UN_SEGUNDO*10;
            }
            else
            {
                timer_perder--;
                if(timer_perder == 0)
                {
                    puntaje_juego = 0;
                    nivel = 1;
                    selector_menu = PLAY;
                    estado_juego = MENU;
                }
            }
            
           /* if(game_over() == 0)
            {
                
                selector_menu = PLAY;
                estado_juego = MENU;
            }*/
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


 /******************************************************************************************
 * 
 *  FUNCIONES RELACIONADAS A LA INICIALIZACIÓN Y MOVIMIENTO DE LA RANA
 * 
 ******************************************************************************************/

 /******************************************************************************************
 *  INITIALIZE_FROG: inicializa la RANA
 ******************************************************************************************/
static void initialize_frog(void)
{
    rene.x = 8 * CASILLA_ANCHO - CASILLA_ANCHO/2.0;
    rene.y = MUNDO_ALTO - CASILLA_ALTO/2.0;
    rene.dy = VELOCIDAD_RANA_ALTO;
    rene.dx = VELOCIDAD_RANA_ANCHO;
    rene.dx_extra = 0;
    rene.chocada = false;
    rene.llego = false;
    rene.direccion = ARRIBA;
    rene.saltando = false;
    rene.frame_chocada = 0;
    rene.fila_max = 0;
}

 /******************************************************************************************
 * 
 *  MOVE_FROG: mueve la rana en función de las teclas presionadas y de las velocidades adquiridas
 * 
 ******************************************************************************************/

static void move_frog (bool choque)
{
    static unsigned int timer_up = 0, timer_down = 0, timer_right = 0, timer_left = 0;
    
    if((rene.x <= MUNDO_ANCHO - RANA_ANCHO/2 - rene.dx_extra) && (rene.x >= RANA_ANCHO/2 + rene.dx_extra))
    {
        rene.x += rene.dx_extra; //se mueve la rana la velocidad extra adquirida por un tronco o tortuga
    }
    else if ((rene.x > MUNDO_ANCHO - RANA_ANCHO/2 - rene.dx_extra) && (rene.dx_extra < 0)) //si llego al borde derecho y su velocidad extra es menor a 0 (hacia la izquierda)
    {
        rene.x += rene.dx_extra;
    }
    else if ((rene.x < RANA_ANCHO/2 + rene.dx_extra) && (rene.dx_extra > 0)) //si llego al borde izquierdo y su velocidad extra es mayor a 0 (hacia la derecha)
    {
        rene.x += rene.dx_extra;
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
                rene.direccion = ARRIBA;
            }

            if(timer_up > TARDA_SALTO && rene.y >=  CASILLA_ALTO/2 + rene.dy)
            {
                 rene.y -= rene.dy;
                 rene.saltando = true;
            }
            else
            {
                rene.saltando = false;
            }
            timer_up--;

        }

        if ( (key_pressed[KEY_DOWN] == true || timer_down != 0 ) && timer_up == 0 && timer_right == 0 && timer_left == 0)
        {
            if(timer_down == 0)
            {
                timer_down = (int) (FRAMES_POR_SALTO_ALTO + TARDA_SALTO);
                rene.direccion = ABAJO;
            
            }

            if (timer_down > TARDA_SALTO && rene.y <= MUNDO_ALTO - CASILLA_ALTO/2 -rene.dy)
            {
                rene.y += rene.dy;
                rene.saltando = true;
            }
            else
            {
                rene.saltando = false;
            }
            timer_down--;
        }

        if ((key_pressed[KEY_RIGHT] == true || timer_right != 0 ) && timer_down == 0 && timer_up== 0 && timer_left == 0)
        {
            if(timer_right == 0)
            {
                timer_right = (int) (FRAMES_POR_SALTO_ANCHO + TARDA_SALTO);
                rene.direccion = DERECHA;
                rene.saltando = true;
                
            }

            if (timer_right > TARDA_SALTO && rene.x <= MUNDO_ANCHO - CASILLA_ANCHO/2 - rene.dx)
            {
                rene.x += rene.dx;
                rene.saltando = true;
            }

            else
            {
                rene.saltando = false;
            }
            timer_right--;
        }

        if ((key_pressed[KEY_LEFT] == true || timer_left != 0 ) && timer_down == 0 && timer_right == 0 && timer_up == 0)
        {
            if(timer_left == 0)
            {
                timer_left = (int) (FRAMES_POR_SALTO_ANCHO + TARDA_SALTO);
                rene.direccion = IZQUIERDA;
            }

            if(timer_left > TARDA_SALTO && rene.x >= CASILLA_ANCHO/2 + rene.dx)
            {
                rene.x -= rene.dx;
                rene.saltando = true;
            }

            else
            {
                rene.saltando = false;
            }
            timer_left--;
        }
    }
}

/*******************************************************************************************
 * 
 * INITIALIZE_LLEGADA: inicializa las casillas de llegada, junto con su posible cocodrilo
 *  
 ********************************************************************************************/

static void initialize_llegada(void)
{
    int i;
    
    for(i = 0; i < CANT_CASILLAS_LLEGADA; i++)
    {
        llegadas[i].x = 0.5*CASILLA_ANCHO + i * (1.2+1.75) * CASILLA_ANCHO + 1.2*CASILLA_ANCHO/2.0;
        llegadas[i].y = CASILLA_ALTO / 2.0;
        llegadas[i].alto = LLEGADA_ALTO;
        llegadas[i].ancho = LLEGADA_ANCHO;
        llegadas[i].ocupado = false;
        llegadas[i].cocodrilo = false;
    }
    
}

/********************************************************************************
 * 
 * INITIALIZE_ENEMIES: inicializa los enemigos (autos, troncos y torutugas)
 *  
 *******************************************************************************/

 /*************************************************************************************************************
  * 
  * Initialize_enemies:
  *     Funcion que dependiendo del nivel que se esté jugando, inicializará a los enemigos con velocidades distintas
  *     Se encarga de inicializar todos los campos de todos los enemigos.
 ****************************************************************************************************************/

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

 /*************************************************************************************************************
  * 
  * Initialize_autos:
  *     Funcion que dependiendo del nivel que se esté jugando, inicializará a los autos con velocidades y posiciones distintas
  *  
 ****************************************************************************************************************/
static void initialize_autos(unsigned int nivel)
{
    int j;
    int k;
    for(j=0; j < FILAS_DE_AUTOS; j++)                                   //Cada ciclo de este loop trabaja sobre una fila distinta
    {
        if(j < 4) //inicializamos los autos (pequeños)
        {
            for(k=0; k < AUTOS_POR_FILA; k++)                            //recorremos los 5 autos por fila
            {
                autos[j][k].fila = j + 1; //completamos con la fila en la que se encuentra el auto                                   
                autos[j][k].y = (CANT_CASILLAS_COLUMNA - autos[j][k].fila) * CASILLA_ALTO - CASILLA_ALTO / 2.0; //ubicamos el auto en su fila
                autos[j][k].dy = 0; //inicializamos su velocidad en y en 0
                autos[j][k].largo = AUTO1_ANCHO;                         
                autos[j][k].alto = AUTO1_ALTO; //inicializamos sus dimensiones
                
                if(j == 0) //inicializamos la primera fila de autos
                {
                    if(nivel < 5) //
                    {
                        autos[j][k].dx = - (( 1 + (((double)nivel) / 2.0) ) * VELOCIDAD_UNITARIA_ANCHO) ;
                        autos[j][k].x = (k - k % 3) * MUNDO_ANCHO/3.0 + MUNDO_ANCHO/4.0 * (k%3);                      //Hago que aparezcan como máximo 3 enemigos por fila a la vez
                        autos[j][k].direccion = IZQUIERDA;
                    }
                    else //del nivel 4 en adelante, la configuración será la misma
                    {
                        autos[j][k].dx = -3 * VELOCIDAD_UNITARIA_ANCHO;
                        autos[j][k].x = (k - k % 3) * MUNDO_ANCHO/3.0 + MUNDO_ANCHO/4.0 * (k%3);                     //Hago que aparezcan como máximo 3 enemigos por fila a la vez
                        autos[j][k].direccion = IZQUIERDA;
                    }
                }
                
                else if (j == 1)
                {
                    if(nivel < 5)
                    {
                        autos[j][k].dx = ((0.5 + (((double)nivel) / 2.0)) * VELOCIDAD_UNITARIA_ANCHO) ;
                        autos[j][k].x =  k * MUNDO_ANCHO/3.0;                  //Hago que aparezcan como máximo 3 enemigos por fila a la vez
                        autos[j][k].direccion = DERECHA;
                    }
                    else
                    {
                        autos[j][k].dx = 2.5 * VELOCIDAD_UNITARIA_ANCHO;
                        autos[j][k].x = k * MUNDO_ANCHO/3.0;                       //Hago que aparezcan como máximo 3 enemigos por fila a la vez
                        autos[j][k].direccion = DERECHA;
                    }
                }
                
                else if (j == 2)
                {
                    if(nivel < 5)
                    {
                        autos[j][k].dx = - ( (1 + (((double)nivel) / 2.0)) * VELOCIDAD_UNITARIA_ANCHO) ;
                        autos[j][k].x = (k - k % 4) * MUNDO_ANCHO/3.0 + MUNDO_ANCHO/4.0 * (k%4);                     //Hago que aparezcan como máximo 3 enemigos por fila a la vez
                        autos[j][k].direccion = IZQUIERDA;
                    }
                    else
                    {
                        autos[j][k].dx = -3 * VELOCIDAD_UNITARIA_ANCHO;
                        autos[j][k].x = (k - k % 4) * MUNDO_ANCHO/3.0 + MUNDO_ANCHO/4.0 * (k%4);                    //Hago que aparezcan como máximo 3 enemigos por fila a la vez
                        autos[j][k].direccion = IZQUIERDA;
                    }
                }
                
                else if (j == 3) //inicializamos la fila de autos veloces
                {
                    if(nivel < 5)
                    {
                        autos[j][k].dx = (2 + ((double)nivel)) * VELOCIDAD_UNITARIA_ANCHO;
                        autos[j][k].x = (k - k % 2) * MUNDO_ANCHO/3.0 + MUNDO_ANCHO/5.0 * (k%2);                     //Hago que aparezcan como máximo 3 enemigos por fila a la vez
                        autos[j][k].direccion = DERECHA;
                    }
                    else
                    {
                        autos[j][k].dx = 6 * VELOCIDAD_UNITARIA_ANCHO;
                        autos[j][k].x = (k - k % 2) * MUNDO_ANCHO/3.0 + MUNDO_ANCHO/5.0 * (k%2);                    //Hago que aparezcan como máximo 3 enemigos por fila a la vez
                        autos[j][k].direccion = DERECHA;
                    }
                }
            }
        }
        
        if(j == 4) //inicializamos los camiones
        {    
            for(k=0; k < AUTOS_POR_FILA; k++)                              
            {         
                autos[j][k].fila = j + 1;                                   
                autos[j][k].y = (CANT_CASILLAS_COLUMNA - autos[j][k].fila) * CASILLA_ALTO - CASILLA_ALTO / 2.0;
                autos[j][k].largo = CAMION_ANCHO;                          
                autos[j][k].alto = CAMION_ALTO;
                
                if(nivel < 5)
                {
                    autos[j][k].dx = - ((1.5 + (((double)nivel) / 4.0)) * VELOCIDAD_UNITARIA_ANCHO) ;
                    autos[j][k].direccion = IZQUIERDA;
                    autos[j][k].x = k * MUNDO_ANCHO / (nivel);                  //Hago que aparezcan como máximo 3 enemigos por fila a la vez
                }
                else
                {
                    autos[j][k].dx = -2.5 * VELOCIDAD_UNITARIA_ANCHO;
                    autos[j][k].direccion = IZQUIERDA;
                    autos[j][k].x = k * MUNDO_ANCHO / 4.0;                      //Hago que aparezcan como máximo 3 enemigos por fila a la vez
                }
            }
        }
    }
}

/*************************************************************************************************************
  * 
  * Initialize_troncos:
  *     Funcion que dependiendo del nivel que se esté jugando, inicializará a los troncos con velocidades distintas
  *  
 ****************************************************************************************************************/
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
                if(nivel < 5)
                {
                    troncos[j][k].dx = (0.5 + (((double)nivel) / 4.0)) * VELOCIDAD_UNITARIA_ANCHO;
                    troncos[j][k].x = (k - k % 3) * MUNDO_ANCHO/2.5 + MUNDO_ANCHO/3.0 * (k%3) - MUNDO_ANCHO/2;
                }
                else
                {
                    troncos[j][k].dx = (0.5 + 1) * VELOCIDAD_UNITARIA_ANCHO;
                    troncos[j][k].x = (k - k % 3) * MUNDO_ANCHO/2.5 + MUNDO_ANCHO/3.0 * (k%3) - MUNDO_ANCHO/2;
                }
                
                troncos[j][k].fila = 8;                                                                                 //necesito que los autos empiecen en la fila 1
                troncos[j][k].y = (CANT_CASILLAS_COLUMNA - troncos[j][k].fila) * CASILLA_ALTO - CASILLA_ALTO / 2.0;
                troncos[j][k].largo = TRONCO1_ANCHO ;                          //Cada enemigo será tan ancho como una casilla
                troncos[j][k].alto = TRONCO1_ALTO;
            }
        }
        
        if(j == 1)
        {
            for(k = 0; k < TRONCOS_POR_FILA; k++) 
            {
                if(nivel < 5)
                {
                    troncos[j][k].dx = (2 + (((double)nivel) / 4.0)) * VELOCIDAD_UNITARIA_ANCHO;
                    troncos[j][k].x = k * MUNDO_ANCHO / 2.0;                      //Hago que aparezcan como máximo 3 enemigos por fila a la vez
                }
                else
                {
                    troncos[j][k].dx = (2 + 1) * VELOCIDAD_UNITARIA_ANCHO;
                    troncos[j][k].x = k * MUNDO_ANCHO / 2.0;                      //Hago que aparezcan como máximo 3 enemigos por fila a la vez
                }

                
                troncos[j][k].fila = 9;//necesito que los autos empiecen en la fila 1               
                troncos[j][k].y = (CANT_CASILLAS_COLUMNA - troncos[j][k].fila) * CASILLA_ALTO - CASILLA_ALTO / 2.0;
                troncos[j][k].largo = TRONCO2_ANCHO ;                          //Cada enemigo será tan ancho como una casilla
                troncos[j][k].alto = TRONCO3_ALTO;
            }
        }
        
        if(j == 2)
        {
            for(k = 0; k < TRONCOS_POR_FILA; k++) 
            {
                if(nivel < 5)
                {
                    troncos[j][k].dx = (1.5 + (((double)nivel) / 4.0)) * VELOCIDAD_UNITARIA_ANCHO;
                    troncos[j][k].x = (k - k % 2) * MUNDO_ANCHO/2.5 + MUNDO_ANCHO/3.0 * (k%2);                     //Hago que aparezcan como máximo 3 enemigos por fila a la vez
                }
                else
                {
                    troncos[j][k].dx = (1.5 + 1) * VELOCIDAD_UNITARIA_ANCHO;
                    troncos[j][k].x = (k - k % 2) * MUNDO_ANCHO/2.5 + MUNDO_ANCHO/3.0 * (k%2);                     //Hago que aparezcan como máximo 3 enemigos por fila a la vez
                }
                
                troncos[j][k].fila = 11;                                                                                      //necesito que los autos empiecen en la fila 1
                troncos[j][k].y = (CANT_CASILLAS_COLUMNA - troncos[j][k].fila) * CASILLA_ALTO - CASILLA_ALTO / 2.0;
                troncos[j][k].largo = TRONCO3_ANCHO ;                          //Cada enemigo será tan ancho como una casilla
                troncos[j][k].alto = TRONCO3_ALTO;
            }
        }
        
    }
}

/*************************************************************************************************************
  * 
  * Initialize_tortugas:
  *     Funcion que dependiendo del nivel que se esté jugando, inicializará a las tortugas con velocidades distintas
  *  
 ****************************************************************************************************************/
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
                if(nivel < 5)
                {
                    tortugas[j][k].dx = (-2 - (((double)nivel) / 4.0)) * VELOCIDAD_UNITARIA_ANCHO;
                }
                else
                {
                    tortugas[j][k].dx = (-2 - 1) * VELOCIDAD_UNITARIA_ANCHO;
                }
                tortugas[j][k].x = (k - k % 3) * MUNDO_ANCHO/2.5 + MUNDO_ANCHO/3.0 * (k%3);                      //Hago que aparezcan como máximo 3 enemigos por fila a la vez
                tortugas[j][k].direccion = IZQUIERDA;
                
                tortugas[j][k].fila = 7;                                                                                 //necesito que los autos empiecen en la fila 1
                tortugas[j][k].y = (CANT_CASILLAS_COLUMNA - tortugas[j][k].fila) * CASILLA_ALTO - CASILLA_ALTO / 2.0;
                tortugas[j][k].largo = TORTUGAS_ANCHO*3;                          //Cada enemigo será tan ancho como una casilla
                tortugas[j][k].alto = TORTUGAS_ALTO;
                tortugas[j][k].hundirse = false;
                tortugas[j][k].frames = 0;
            }
        }
        
        if(j == 1)
        {
            for(k = 0; k < TORTUGAS_POR_FILA; k++) 
            {
                if(nivel < 5)
                {
                    tortugas[j][k].dx = (-1 - (((double)nivel) / 4.0)) * VELOCIDAD_UNITARIA_ANCHO;
                }
                else
                {
                    tortugas[j][k].dx = (-1 - 1) * VELOCIDAD_UNITARIA_ANCHO;
                }

                tortugas[j][k].x = k * MUNDO_ANCHO / 3.0;                      //Hago que aparezcan como máximo 3 enemigos por fila a la vez
                tortugas[j][k].direccion = IZQUIERDA;
                
                tortugas[j][k].fila = 10;    //necesito que los autos empiecen en la fila 1
                tortugas[j][k].y = (CANT_CASILLAS_COLUMNA - tortugas[j][k].fila) * CASILLA_ALTO - CASILLA_ALTO / 2.0;
                tortugas[j][k].x = k * MUNDO_ANCHO / 2.0;                      //Hago que aparezcan como máximo 3 enemigos por fila a la vez
                tortugas[j][k].largo = TORTUGAS_ANCHO*2;                          //Cada enemigo será tan ancho como una casilla
                tortugas[j][k].alto = TORTUGAS_ALTO;
                tortugas[j][k].hundirse = false;
                tortugas[j][k].frames = 0;
            }
        }        
    }
}

/*************************************************************************************
 * 
 * MOVE_ENEMIES:
 *      Se encarga de mover los enemigos en función del nivel en el que se encuentren
 *  
 *************************************************************************************/

static void move_enemies(int nivel)
{
    move_autos(nivel);
    move_troncos(nivel);
    move_tortugas(nivel);
    move_cocodrilo(nivel);
}

/*************************************************************************************
 * 
 * MOVE_AUTOS:
 *      Se encarga de mover los autos en función del nivel en el que se encuentren
 *  
 *************************************************************************************/

static void move_autos(int nivel)
{
    static int contador_cambiar_carril = 0;
    static int contador_cambiar_velocidad = FRAMES_CAMBIAR_VELOCIDAD;
    
    unsigned int i, j, k;
    for(j=0; j < FILAS_DE_AUTOS; j++)
    {
        
        for(k=0; k < AUTOS_POR_FILA; k++)
        {
            /* Analizo si los enemigos están dentro del campo de movimiento
             * , sino, los teletransporto al lado contrario de donde desaparecieron*/
            
            if(autos[j][k].x < -MUNDO_ANCHO/2 || autos[j][k].x > MUNDO_ANCHO + MUNDO_ANCHO/2)
            {
                if(autos[j][k].x < -MUNDO_ANCHO/2)
                {
                    autos[j][k].x = MUNDO_ANCHO + MUNDO_ANCHO/2;
                }
                else
                {
                    autos[j][k].x = -MUNDO_ANCHO/2;
                }
            }
            
            autos[j][k].x += autos[j][k].dx;
        }
    }

    
    if (nivel > 2) //autos que cambien de carril
    {

        if(contador_cambiar_carril == 0)
        {
            if(autos[0][3].fila == 1)
            {
                if(autos[0][3].y > CASILLA_ALTO * 10 - CASILLA_ALTO/2)
                {
                    autos[0][3].dy = -4 * VELOCIDAD_UNITARIA_ANCHO;
                    autos[0][3].y += autos[0][3].dy;

                }
                else
                {
                    autos[0][3].fila = 3;
                    autos[0][3].dx = autos[2][0].dx;
                    autos[0][3].direccion = IZQUIERDA;
                    contador_cambiar_carril = FRAMES_CAMBIAR_CARRIL ;
                }
            }
            else if (autos[0][3].fila == 3)
            {
                if(autos[0][3].y < CASILLA_ALTO * 12 - CASILLA_ALTO/2)
                {
                    autos[0][3].dy = 4 * VELOCIDAD_UNITARIA_ANCHO;
                    autos[0][3].y += autos[0][3].dy;
                }
                else
                {
                    autos[0][3].fila = 1;
                    autos[0][3].dx = autos[0][1].dx;
                    autos[0][3].direccion = IZQUIERDA;
                    contador_cambiar_carril = FRAMES_CAMBIAR_CARRIL;
                }
            }
        }
        else
        {
            contador_cambiar_carril--;
        }
    }
    
    enum ACELERACION
    {
        ACELERA, DESACELERA
    };
    
    static int aceleracion = ACELERA;
    
    if (nivel > 3) //autos que cambien de carril
    {
        switch (aceleracion)
        {
            case ACELERA:
            {
                if(autos[1][0].dx < 4 * VELOCIDAD_UNITARIA_ANCHO)
                {
                    for(i=0; i < AUTOS_POR_FILA; i++)
                    {
                        autos[1][i].dx += 0.01 * VELOCIDAD_UNITARIA_ANCHO;
                    }
                }
                else if(contador_cambiar_velocidad != 0)
                {
                    contador_cambiar_velocidad--;
                }
                else
                {
                    contador_cambiar_velocidad = FRAMES_CAMBIAR_VELOCIDAD;
                    aceleracion = DESACELERA;
                }
                break;
            }
            case DESACELERA:
            {
                if(autos[1][0].dx > -4 * VELOCIDAD_UNITARIA_ANCHO)
                {
                    for(i=0; i < AUTOS_POR_FILA; i++)
                    {
                        autos[1][i].dx -= 0.01 * VELOCIDAD_UNITARIA_ANCHO;
                    }
                }
                else if(contador_cambiar_velocidad != 0)
                {
                    contador_cambiar_velocidad--;
                }
                else
                {
                    contador_cambiar_velocidad = FRAMES_CAMBIAR_VELOCIDAD;
                    aceleracion = ACELERA;
                }
                break;
            }
        }
    }
}

/*************************************************************************************
 * 
 * MOVE_TRONCOS:
 *      Se encarga de mover los troncos en función del nivel en el que se encuentren
 *  
 *************************************************************************************/
static void move_troncos(int nivel)
{
    unsigned int j, k;
    for(j=0; j < FILAS_DE_TRONCOS; j++)
    {
        
        for(k=0; k < TRONCOS_POR_FILA; k++)
        {
            /* Analizo si los enemigos están dentro del campo de movimiento
             * , sino, los teletransporto al lado contrario de donde desaparecieron*/
            
            if(troncos[j][k].x < -MUNDO_ANCHO/2 || troncos[j][k].x > MUNDO_ANCHO + MUNDO_ANCHO/2)
            {
                if(troncos[j][k].x < -MUNDO_ANCHO/2)
                {
                    troncos[j][k].x = MUNDO_ANCHO + MUNDO_ANCHO/2;
                }
                else
                {
                    troncos[j][k].x = -MUNDO_ANCHO/2;
                }
            }
                        
            troncos[j][k].x += troncos[j][k].dx;
        }
    }
}

/*************************************************************************************
 * 
 * MOVE_TORUGAS:
 *      Se encarga de mover las tortugas en función del nivel en el que se encuentren
 *  
 *************************************************************************************/

static void move_tortugas(int nivel)
{
    unsigned int j, k;
    static unsigned int timer_hundirse = FRAMES_HASTA_HUNDIRSE;
    static unsigned int timer_moverse = FRAMES_HASTA_HUNDIRSE / 4;
    
    for(j=0; j < FILAS_DE_TORTUGAS; j++)
    {
        
        for(k=0; k < TORTUGAS_POR_FILA; k++)
        {
            /* Analizo si los enemigos están dentro del campo de movimiento
             * , sino, los teletransporto al lado contrario de donde desaparecieron*/
            
            if(tortugas[j][k].x < -MUNDO_ANCHO/2 || tortugas[j][k].x > MUNDO_ANCHO + MUNDO_ANCHO/2)
            {
                if(tortugas[j][k].x < -MUNDO_ANCHO/2)
                {
                    tortugas[j][k].x = MUNDO_ANCHO + MUNDO_ANCHO/2;
                }
                else
                {
                    tortugas[j][k].x = -MUNDO_ANCHO/2;
                }
            }
            tortugas[j][k].x += tortugas[j][k].dx;
        }
    }
    
    if(timer_moverse == 0)
    {
        int j,k;
        
        for(j=0; j < FILAS_DE_TORTUGAS; j++)
        {
            for(k=1; k < TORTUGAS_POR_FILA; k++)
            {
                if(tortugas[j][k].frames == 0)
                {
                    tortugas[j][k].frames = 1;
                }
                else if(tortugas[j][k].frames == 1)
                {
                    tortugas[j][k].frames = 0;
                }
            }
        }
        
        timer_moverse = FRAMES_HASTA_HUNDIRSE / 4;
    }
    
    if(timer_hundirse == (int)(FRAMES_HASTA_HUNDIRSE * (3.0/4.0)) )
    {
        int i;
        for (i=0; i<2; i++)
        {
            if (tortugas[i][0].hundirse == false)
            {
                tortugas[i][0].frames = 1;
            }
        }
    }    
    if(timer_hundirse == FRAMES_HASTA_HUNDIRSE / 2)
    {
        int i;
        for (i=0; i<2; i++)
        {
            if (tortugas[i][0].hundirse == false)
            {
                tortugas[i][0].frames = 2;
            }
        }
    }
    if(timer_hundirse == FRAMES_HASTA_HUNDIRSE / 4)
    {
        int i;
        for (i=0; i<2; i++)
        {
            if (tortugas[i][0].hundirse == false)
            {
                tortugas[i][0].frames = 3;
            }
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
                tortugas[i][0].frames = 0;
            }
            else
            {
                tortugas[i][0].hundirse = false;
                tortugas[i][0].frames = 0;
            }
        }
        
        
        timer_hundirse = FRAMES_HASTA_HUNDIRSE;
    }
    
    timer_hundirse--;
    timer_moverse--;
}

/************************************************************************************************************************
 * 
 * MOVE_COCODRILO:
 *      Se encarga de ubicar el cocodrilo en una casilla de llegada RANDOM en función del nivel en el que se encuentren
 *  
 **************************************************************+**********************************************************/

static void move_cocodrilo(int nivel)
{
    static int timer_sin_cocodrilo = 0;
    static int timer_con_cocodrilo = FRAMES_CON_COCODRILO;
    
    int tiempo;
    
    if (nivel < 5)
    {
        tiempo = FRAMES_SIN_COCODRILO/ nivel;
    }
    
    else
    {
        tiempo = FRAMES_SIN_COCODRILO / 4;
    }
    
    if (timer_sin_cocodrilo == 0)
    {
        if (timer_con_cocodrilo == FRAMES_CON_COCODRILO)
        {
            int i;
            i = rand() % CANT_CASILLAS_LLEGADA;
            
            if(llegadas[i].ocupado == false)
            {
                llegadas[i].cocodrilo = true;
                timer_con_cocodrilo--;
            }
            else
            {
                timer_con_cocodrilo = FRAMES_CON_COCODRILO;
                timer_sin_cocodrilo = tiempo;
            }
        }
        else
        {
            timer_con_cocodrilo--;
            if(timer_con_cocodrilo == 0)
            {
                int j;
                for (j = 0; j < CANT_CASILLAS_LLEGADA; j++)
                {
                    if(llegadas[j].cocodrilo == true)
                    {
                        llegadas[j].cocodrilo = false;
                        timer_con_cocodrilo = FRAMES_CON_COCODRILO;
                        timer_sin_cocodrilo = tiempo;
                    }
              
                }
            }
        }
    }
    else
    {
        timer_sin_cocodrilo--;
    }
}


static void sumar_puntaje_salto(void)
{
    int i;
    int fila_actual;
    
    for (i = 0; i < CANT_CASILLAS_COLUMNA; i++)
    {
        if(rene.y > CASILLA_ALTO * (CANT_CASILLAS_COLUMNA - 1 - i) && rene.y < CASILLA_ALTO * (CANT_CASILLAS_COLUMNA-i))
        {
            fila_actual = i;
            if(fila_actual > rene.fila_max)
            {
                rene.fila_max++;
                puntaje_juego += 10;
            }
        }
    }
    
    
}


/**************************************
 * 
 * GAMER_OVER():
 *      funcion al perder todas las vidas
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
