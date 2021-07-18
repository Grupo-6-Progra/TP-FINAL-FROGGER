/***************************************************************************************************************
 *      FROGGER
 * 
 *      JUEGO.C:
 * 
 *              Autores:    Alegre, Marcos
 *                          Di Sanzo, Bruno
 *                          Hertter, José Iván
 *                          Ibáñez, Lucía
 * 
 *      
 *          
 * 
 * 
 *  
 **************************************************************************************************************/



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

static unsigned long int get_high_score(int *);

static int save_new_highscore(unsigned long int);

/***************************************************
 *  DEFINICIÓN DE VARIABLES GLOBALES
***************************************************/

int estado_juego = MENU;       //es una variable que definirá en qué estado se encuentra el juego (VER "juego.h")

double tiempo_restante;        //almacena el tiempo restante para perder

unsigned int vidas_restantes = 3;   //variable que se inicializa con la cantidad de vidas de la rana

unsigned int nivel = 1;         //variable que indica el nivel en el que se encuentra

unsigned long int puntaje_juego = 0;  //es la variable que indicará el puntaje a medida que avance la rana

unsigned long int high_score;      //variable que almacena el puntaje máximo (al ser leida desde el archivo correspondiente)

int selector_menu = PLAY;       //varibale que cambia el estado del menú

int timer_pasar_nivel = 0;      //es un timer para esperar a pasar entre niveles

int timer_perder = 0;       //timer para esperar entre perder y volver a empezar.


bool key_pressed[TECLAS_MAX] = {false,false,false,false,false}; //arreglo donde se almacenan las teclas ingresadas (ver "juego.h")

RANA rene;  //estructura de la rana

AUTOS autos[FILAS_DE_AUTOS][AUTOS_POR_FILA];    //matriz con estructuras de autos 

TRONCO troncos [FILAS_DE_TRONCOS][TRONCOS_POR_FILA]; //matriz con estructuras de troncos

TORTUGAS tortugas [FILAS_DE_TORTUGAS][TORTUGAS_POR_FILA]; //matriz con estructuras de tortugas

LLEGADA llegadas [CANT_CASILLAS_LLEGADA]; //matriz con estructuras de casillas de llegada


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
    
    static int choque_en_proceso = 0; //contador de frames si hubo choque
    
    static bool enter_prev = false; //variable necesaria para detectar cuando se soltó el enter
    
    bool choque = false; //variable que almacena si hubo un choque reciente
    
    /***************************************************
     *
     * Se analizan los posibles estados del juego
     *       
     ****************************************************/
    
    switch(estado_juego)        //se analizan los posibles estados del juego y dependiendo de cada uno se realizan operaciones
    {
        
        /**************************************
         * MENU DE INICIO
         **************************************/ 
        case MENU: //menu de inicio
        {
            salir = menu_start();   //se llama al menú del inicio
            
            break;
        }
        
        /**************************************
         * INICIO DE NIVEL
         **************************************/
        case INICIO: //inicio de un nivel
        {
            vidas_restantes = 3;    //se inicializan las vidas en 3 
            initialize_enemies (nivel); //se inicializa a los enemigos en función del nivel que sea
            initialize_frog();  //se inicializa a la rana
            initialize_llegada();   //se inicializa la llegada
            estado_juego = JUEGO;   //el estado del juego es juego ahora porque comienza a jugarse
            tiempo_restante = TIEMPO_TOTAL; //Inicializo el tiempo restante en 100 segundos
            break;
        }
        
        /******************************************
         * PASAR DE NIVEL (mostrar puntaje y nivel)
         *****************************************/
        case PASAR_NIVEL:       //caso en el que se pasa de nivel
        {
            
            if(timer_pasar_nivel == 0)  
            {
                timer_pasar_nivel = TIEMPO_PASAR_NIVEL; //se pierde timepo para mostrar puntaje y nivel
                
            }
            else
            {
                timer_pasar_nivel--;       //se decrementa y se vuelve a preguntar si no llegó a 0
                if(timer_pasar_nivel == 0)
                {
                    nivel++;            //se pasa de nivel
                    move_frog(true);    //se inicializan los contadores que mueven la rana
                    estado_juego = INICIO;  //se vuelve a iniciar el estado del juego ya que es un nuevo nivel
                }
                
            }
            
            break;
        }
        
        
        /**************************************
         * JUEGO (RANA VIVA)
         **************************************/
        case JUEGO: //caso en el que la rana está viva
        {

            if((key_pressed[KEY_ENTER] == true) || (enter_prev == true)) //se detecta enter presionado y espera que se suelte, para entrar en la pausa
            {
                if (enter_prev == false)
                {
                    enter_prev = true; //si recién se presionó, se indica
                }
                else if (enter_prev == true && key_pressed[KEY_ENTER] == false) //se analiza si está presionado y se soltó
                {
                    enter_prev = false; //se indica que se soltó
                    estado_juego = PAUSA;   //el juego se pone en estado de pausa
                }
            }
            
            else if (tiempo_restante-- > 0) //Decremento una vez por FPS el tiempo restante. Si no es 0, no se perdió
            {
                move_enemies(nivel); //movemos los enemigos según el nivel
                
                move_frog(false); //movemos la rana

                choque = colision(); //detectamos si hubo colisión con autos

                if(choque == true)      //se ve si la rana efectivamente chocó
                {
                    vidas_restantes--;  //se resta una vida
                    rene.chocada = true;    //se coloca el estado de la rana chocada
                    estado_juego = REINICIO; //el juego se reinicia
                }

                if((rana_sobre_tronco() == false) && (rana_sobre_tortuga() == false) && (rene.y <= CASILLA_ALTO * 6 && rene.y >= CASILLA_ALTO))
                {
                    /*
                     * se detecta si la rana se subió a una tortuga o tronco cuando está sobre el agua, en caso contario, se hundió
                    */
                    vidas_restantes--;      //se resta una vida
                    rene.chocada = true;
                    estado_juego = REINICIO;    //se reinicia el juego
                }

                if((rene.y <= CASILLA_ALTO))        //se analiza si la rana llegó o chocó con el pasto de la fila superior
                {
                    if(rana_llego() == true)        //se ve el caso en el que la rana llega (sin chocar contra el pasto o cocodrilo)
                    {
                        rene.llego = true;          //cambia el estado de la rana
                        if(nivel <= 5)              //se ve que el nivel esté dentro de la cantidad posible de niveles
                        {
                            puntaje_juego += APROX(100 * nivel * (((tiempo_restante*1.0 / TIEMPO_TOTAL)) + 1));     //se realiza una suma al puntaje por haber llegado y además dependiendo del tiempo que le quedaba
                        }
                        else        //caso en el que superó el último nivel (a partir de acá, la dificultad se mantiene)
                        {
                            puntaje_juego += APROX(100 * 6 * (((tiempo_restante*1.0 / TIEMPO_TOTAL)) + 1)); //se suma el puntaje correspondiente
                        }
                        estado_juego = REINICIO;    // se va a analizar si se ganó o hay que seguir jugando
                    }

                    else        //en caso contrario es porque no llegó (perdió)
                    {
                        vidas_restantes--; //pierde una vida
                        rene.chocada = true; //se inidica que chocó
                        estado_juego = REINICIO; //se analiza si se perdió todas las vidas
                    }
                }
                
                sumar_puntaje_salto(); // se suma puntaje dependiendo de la fila a la que haya legado
                
            }
            
            else //si se acabó el tiempo, se pierde
            {
                vidas_restantes--;
                rene.chocada = true;
                estado_juego = REINICIO;
            }
            
            break;
        }
        
        /**************************************************************
         * REINICIO DE RANA (al llegar a casilla de llegada o perder)
         *************************************************************/
        case REINICIO: //caso al que se entra cuando se pierde una vida
        {
            if(rene.llego == true)
            {                              
                if (choque_en_proceso == 0) //significa que acabo de ganar
                {
                    choque_en_proceso = FRAMES_CHOQUE; // inicializo el timer para mostrar animación al llegar
                }
                
                else if (choque_en_proceso > 0) 
                {
                    choque_en_proceso--;
                    
                    move_enemies(nivel); //seguimos moviendo enemigos
                    
                    if (choque_en_proceso == 0)
                    {
                        rene.llego = false; //se reinicia el indicador de llegar
                        int casillas_llegadas = 0;
                        int i;

                        for (i = 0; i < 5; i++)
                        {
                            if(llegadas[i].ocupado == true)
                            {
                                casillas_llegadas++; //se analiza la cantidad de casillas llegadas
                            }
                        }

                        if(casillas_llegadas == 5) //se completaron todas las casillas y se pasó de nivel
                        {
                            //se suman los puntos al llegar
                            if(nivel <= 5)
                            {
                                puntaje_juego += 500*nivel;
                            }
                            else
                            {
                                puntaje_juego += 500*6;
                            }
                            
                            estado_juego = PASAR_NIVEL; //pasamos de nivel
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
                if(vidas_restantes == 0)        //se ve que la rana se quedó sin vidas
                {
                    estado_juego = PERDER;      //el estado del juego cambia a perder
                    move_frog(true);
                }

                else if (rene.chocada == true && choque_en_proceso == 0) //significa que acabo de chocar
                {
                    choque_en_proceso = FRAMES_CHOQUE;
                    rene.frame_chocada = 1;
                }

                else if (choque_en_proceso > 0)
                {
                    choque_en_proceso--; //pierdo frames para dar tiempo a mostrar imagen
                    
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
                    estado_juego = JUEGO; //vuelve a abajo
                }
            }
           
            
            break;
        }
        
        /**************************************
         * MENU DE PAUSA
         **************************************/
        case PAUSA:     //el juego entra en el estado de pausa
        {
            salir = menu_pausa();   //entra a la función del menú de pausa
            break;
        }   
        
        /**********************************************************
         * PERDER (se reinicia el juego + mostrar nivel + puntajes)
         **********************************************************/
        case PERDER:    //caso en el que el jugador pierde
        {
            int error; //variable por si no se puede leer el high_score
            
            if(timer_perder == 0)       //caso en el que el tiempo se termine
            {
                timer_perder = TIEMPO_PERDER; //tardamos 10 segundos para mostrar nivel y puntajes maximo y propio
                
                high_score = get_high_score(&error); //busco el puntaje máximo hasta el momento
                if(error != 0)
                {
                    salir = true;
                }
            }
            else
            {
                timer_perder--;
                if(timer_perder == 0)
                {
                    if(puntaje_juego > high_score)      //se analiza si el puntaje obtenido es mayor o menor al high score
                    {
                        error = save_new_highscore(puntaje_juego);
                        if(error != 0)
                        {
                            salir = true;
                        }
                    }
                    
                    puntaje_juego = 0;      //se reinicia el puntaje del juego 
                    nivel = 1;               //se inicia el nivel 1
                    selector_menu = PLAY;
                    estado_juego = MENU;       //vuelve al estado de menú comun
                }
            }
            
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
    rene.x = 8 * CASILLA_ANCHO - CASILLA_ANCHO/2.0;     //se inicializa la posición de la rana en x
    rene.y = MUNDO_ALTO - CASILLA_ALTO/2.0;             //se inicializa la posición de la rana en y
    rene.dy = VELOCIDAD_RANA_ALTO;
    rene.dx = VELOCIDAD_RANA_ANCHO;
    rene.dx_extra = 0;
    rene.chocada = false;                   //el estado de la rana se coloca por defecto en no chocada
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

        /*
         * Dependiendo de la tecla presionada y de los timers, se mueve la rana y se da tiempo entre salto y salto
         * para dar la sensación de salto y tener una buena lectura de las teclas presionadas
         */
        
        
        /*
         * Arriba
         */
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

        /*
         * Abajo
         */
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

        
        /*
         * Derecha
         */
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

        /*
         * Izquierda
         */
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
    
    for(i = 0; i < CANT_CASILLAS_LLEGADA; i++)      //se hace una secuencia para llenar las casillas e inicializar las de llegada
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
    initialize_autos(nivel);        //casa una recibe nivel, ya que a medida que es mayor el nivel
    initialize_troncos(nivel);      //mayor será la dificultad
    initialize_tortugas(nivel);
}

 /*************************************************************************************************************
  * 
  * Initialize_autos:
  *     Funcion que dependiendo del nivel que se esté jugando, inicializará a los autos con velocidades y posiciones distintas
  *  
 ****************************************************************************************************************/
static void initialize_autos(unsigned int nivel)    //inicializa a los autos
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
                    if(nivel <= 4)
                    {
                        autos[j][k].dx = - (( 1 + (((double)nivel) / 2.0) ) * VELOCIDAD_UNITARIA_ANCHO) ; // la velocidad aumenta segun el nivel
                        autos[j][k].x = (k - k % 3) * MUNDO_ANCHO/3.0 + MUNDO_ANCHO/4.0 * (k%3);          //ubicamos en forma 3 y 2
                        autos[j][k].direccion = IZQUIERDA;
                    }
                    else //del nivel 5 en adelante, la configuración será la misma
                    {
                        autos[j][k].dx = -3 * VELOCIDAD_UNITARIA_ANCHO;
                        autos[j][k].x = (k - k % 3) * MUNDO_ANCHO/3.0 + MUNDO_ANCHO/4.0 * (k%3);                    
                        autos[j][k].direccion = IZQUIERDA;
                    }
                }
                
                else if (j == 1) //inicializamos la segunda fila de autos
                {
                    if(nivel <= 4)
                    {
                        autos[j][k].dx = ((0.5 + (((double)nivel) / 2.0)) * VELOCIDAD_UNITARIA_ANCHO) ; // la velocidad aumenta segun el nivel
                        autos[j][k].x =  k * MUNDO_ANCHO/3.0;                  //Hago que aparezcan igual de espaciados
                        autos[j][k].direccion = DERECHA;
                    }
                    else //del nivel 5 en adelante, la configuración será la misma
                    {
                        autos[j][k].dx = 2.5 * VELOCIDAD_UNITARIA_ANCHO;
                        autos[j][k].x = k * MUNDO_ANCHO/3.0;                       
                        autos[j][k].direccion = DERECHA;
                    }
                }
                
                else if (j == 2)
                {
                    if(nivel <= 4)
                    {
                        autos[j][k].dx = - ( (1 + (((double)nivel) / 2.0)) * VELOCIDAD_UNITARIA_ANCHO) ; // la velocidad aumenta segun el nivel
                        autos[j][k].x = (k - k % 4) * MUNDO_ANCHO/3.0 + MUNDO_ANCHO/4.0 * (k%4);                     //ubicamos en forma 4 y 1 
                        autos[j][k].direccion = IZQUIERDA;
                    }
                    else //del nivel 5 en adelante, la configuración será la misma
                    {
                        autos[j][k].dx = -3 * VELOCIDAD_UNITARIA_ANCHO;
                        autos[j][k].x = (k - k % 4) * MUNDO_ANCHO/3.0 + MUNDO_ANCHO/4.0 * (k%4);
                        autos[j][k].direccion = IZQUIERDA;
                    }
                }
                
                else if (j == 3) //inicializamos la fila de autos veloces
                {
                    if(nivel <= 4)
                    {
                        autos[j][k].dx = (2 + ((double)nivel)) * VELOCIDAD_UNITARIA_ANCHO;                      // la velocidad aumenta segun el nivel
                        autos[j][k].x = (k - k % 2) * MUNDO_ANCHO/3.0 + MUNDO_ANCHO/5.0 * (k%2);                    //ubicamos en forma 2 y 2
                        autos[j][k].direccion = DERECHA;
                    }
                    else //del nivel 5 en adelante, la configuración será la misma
                    {
                        autos[j][k].dx = 6 * VELOCIDAD_UNITARIA_ANCHO;
                        autos[j][k].x = (k - k % 2) * MUNDO_ANCHO/3.0 + MUNDO_ANCHO/5.0 * (k%2);                    
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
                
                if(nivel <= 4)
                {
                    autos[j][k].dx = - ((1.5 + (((double)nivel) / 4.0)) * VELOCIDAD_UNITARIA_ANCHO) ; // la velocidad aumenta segun el nivel
                    autos[j][k].direccion = IZQUIERDA;
                    autos[j][k].x = k * MUNDO_ANCHO / (nivel);                  //ubicamos igual de espaciados, pero con el paso de los niveles se acercan
                }
                else //del nivel 5 en adelante, la configuración será la misma
                {
                    autos[j][k].dx = -2.5 * VELOCIDAD_UNITARIA_ANCHO;
                    autos[j][k].direccion = IZQUIERDA;
                    autos[j][k].x = k * MUNDO_ANCHO / 4.0;                      
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
        if(j == 0) //fila 1
        {       
            for(k = 0; k < TRONCOS_POR_FILA; k++)
            {
                if(nivel <= 4)
                {
                    troncos[j][k].dx = (0.5 + (((double)nivel) / 4.0)) * VELOCIDAD_UNITARIA_ANCHO; // la velocidad aumenta segun el nivel
                    troncos[j][k].x = (k - k % 3) * MUNDO_ANCHO/2.5 + MUNDO_ANCHO/3.0 * (k%3) - MUNDO_ANCHO/2; //ubicamos en forma 3 y 2
                }
                else //del nivel 5 en adelante, la configuración será la misma
                {
                    troncos[j][k].dx = (0.5 + 1) * VELOCIDAD_UNITARIA_ANCHO;
                    troncos[j][k].x = (k - k % 3) * MUNDO_ANCHO/2.5 + MUNDO_ANCHO/3.0 * (k%3) - MUNDO_ANCHO/2;
                }
                
                troncos[j][k].fila = 8;                                                                                 
                troncos[j][k].y = (CANT_CASILLAS_COLUMNA - troncos[j][k].fila) * CASILLA_ALTO - CASILLA_ALTO / 2.0;
                troncos[j][k].largo = TRONCO1_ANCHO ;                         
                troncos[j][k].alto = TRONCO1_ALTO;
            }
        }
        
        if(j == 1) //fila 2
        {
            for(k = 0; k < TRONCOS_POR_FILA; k++) 
            {
                if(nivel <= 4)
                {
                    troncos[j][k].dx = (2 + (((double)nivel) / 4.0)) * VELOCIDAD_UNITARIA_ANCHO; // la velocidad aumenta segun el nivel
                    troncos[j][k].x = k * MUNDO_ANCHO / 2.0;                      
                }
                else //del nivel 5 en adelante, la configuración será la misma
                {
                    troncos[j][k].dx = (2 + 1) * VELOCIDAD_UNITARIA_ANCHO;
                    troncos[j][k].x = k * MUNDO_ANCHO / 2.0;                      //igual de espaciados
                }

                
                troncos[j][k].fila = 9;//necesito que los autos empiecen en la fila 1               
                troncos[j][k].y = (CANT_CASILLAS_COLUMNA - troncos[j][k].fila) * CASILLA_ALTO - CASILLA_ALTO / 2.0;
                troncos[j][k].largo = TRONCO2_ANCHO ;                          //Cada enemigo será tan ancho como una casilla
                troncos[j][k].alto = TRONCO3_ALTO;
            }
        }
        
        if(j == 2) //fila 3
        {
            for(k = 0; k < TRONCOS_POR_FILA; k++) 
            {
                if(nivel <= 4)
                {
                    troncos[j][k].dx = (1 + (((double)nivel) / 4.0)) * VELOCIDAD_UNITARIA_ANCHO;        // la velocidad aumenta segun el nivel
                    troncos[j][k].x = (k - k % 2) * MUNDO_ANCHO/2.5 + MUNDO_ANCHO/3.0 * (k%2);                     //ubicamos 2 y 2 y 1
                }
                else //del nivel 5 en adelante, la configuración será la misma
                {
                    troncos[j][k].dx = (1 + 1) * VELOCIDAD_UNITARIA_ANCHO;
                    troncos[j][k].x = (k - k % 2) * MUNDO_ANCHO/2.5 + MUNDO_ANCHO/3.0 * (k%2);                     
                }
                
                troncos[j][k].fila = 11;                                                                                      
                troncos[j][k].y = (CANT_CASILLAS_COLUMNA - troncos[j][k].fila) * CASILLA_ALTO - CASILLA_ALTO / 2.0;
                troncos[j][k].largo = TRONCO3_ANCHO ;
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
        if(j == 0) //fila 1
        {       
            for(k = 0; k < TORTUGAS_POR_FILA; k++)                               
            {
                if(nivel <= 4)
                {
                    tortugas[j][k].dx = (-2 - (((double)nivel) / 4.0)) * VELOCIDAD_UNITARIA_ANCHO; //velocidad auenta según el nivel
                }
                else //del nivel 5 en adelante, la configuración será la misma
                {
                    tortugas[j][k].dx = (-2 - 1) * VELOCIDAD_UNITARIA_ANCHO;
                }
                tortugas[j][k].x = (k - k % 3) * MUNDO_ANCHO/2.5 + MUNDO_ANCHO/3.0 * (k%3);                      //ubicación 3 y 2
                tortugas[j][k].direccion = IZQUIERDA;
                
                tortugas[j][k].fila = 7;                                                                                 
                tortugas[j][k].y = (CANT_CASILLAS_COLUMNA - tortugas[j][k].fila) * CASILLA_ALTO - CASILLA_ALTO / 2.0;
                tortugas[j][k].largo = TORTUGAS_ANCHO*3;                          
                tortugas[j][k].alto = TORTUGAS_ALTO;
                tortugas[j][k].hundirse = false;
                tortugas[j][k].frames = 0;
            }
        }
        
        if(j == 1)//fila 2
        {
            for(k = 0; k < TORTUGAS_POR_FILA; k++) 
            {
                if(nivel <= 4)
                {
                    tortugas[j][k].dx = (-1 - (((double)nivel) / 4.0)) * VELOCIDAD_UNITARIA_ANCHO; //velocidad auenta según el nivel
                }
                else //del nivel 5 en adelante, la configuración será la misma
                {
                    tortugas[j][k].dx = (-1 - 1) * VELOCIDAD_UNITARIA_ANCHO;
                }

                tortugas[j][k].x = k * MUNDO_ANCHO / 3.0;                      //misma distancia entre tortugas
                tortugas[j][k].direccion = IZQUIERDA;
                
                tortugas[j][k].fila = 10;    //necesito que los autos empiecen en la fila 1
                tortugas[j][k].y = (CANT_CASILLAS_COLUMNA - tortugas[j][k].fila) * CASILLA_ALTO - CASILLA_ALTO / 2.0;
                tortugas[j][k].x = k * MUNDO_ANCHO / 2.0;                     
                tortugas[j][k].largo = TORTUGAS_ANCHO*2;                         
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
    move_autos(nivel);      //mueve a los autos
    move_troncos(nivel);    //mueve a los troncos
    move_tortugas(nivel);   //mueve a las tortugas
    move_cocodrilo(nivel);  //mueve a los cocodrilos
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

    
    if (nivel >= 3) //auto que cambia de carril
    {

        if(contador_cambiar_carril == 0) //si hay que cambiar de carril
        {
            if(autos[0][3].fila == 1) //si está en la fila 1, se lo sube a la 3
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
            else if (autos[0][3].fila == 3) //si está en la fila 3, se lo baja a la 1
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
            contador_cambiar_carril--; //se decrementa tiempo para cambiar de carril
        }
    }
    
    enum ACELERACION
    {
        ACELERA, DESACELERA
    };
    
    static int aceleracion = ACELERA;
    
    if (nivel >= 4) //autos que cambian de aceleración
    {
        switch (aceleracion)
        {
            case ACELERA:
            {
                int random;
                random = rand()%2; //generamos random si se acelera o desacelera cuando la velocidad es 0 (a partir del nivel 6)
                
                if(autos[1][0].dx < 4 * VELOCIDAD_UNITARIA_ANCHO)
                {
                    for(i=0; i < AUTOS_POR_FILA; i++)
                    {
                        if (autos[1][0].dx > -0.01 * VELOCIDAD_UNITARIA_ANCHO && autos[1][0].dx < 0.01 * VELOCIDAD_UNITARIA_ANCHO) //si la velocidad es prácticamente 0
                        {
                            if(nivel <= 5) //se aumenta la velocidad
                            {
                               autos[1][i].dx += 0.01 * VELOCIDAD_UNITARIA_ANCHO;    
                            }
                            else 
                            {
                                if(random == 0) //dependiendo del valor de radom, acelera o desacelera
                                {
                                    autos[1][i].dx += 0.01 * VELOCIDAD_UNITARIA_ANCHO;
                                }
                                else
                                {
                                    autos[1][i].dx -= 0.01 * VELOCIDAD_UNITARIA_ANCHO;
                                    contador_cambiar_velocidad = FRAMES_CAMBIAR_VELOCIDAD;
                                    aceleracion = DESACELERA;
                                }
                            }
                        }
                        else
                        {
                            autos[1][i].dx += 0.01 * VELOCIDAD_UNITARIA_ANCHO; //se aumenta la velocidad
                        }
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
                    int random;
                    random = rand()%2;
                    
                    for(i=0; i < AUTOS_POR_FILA; i++)
                    {
                        if (autos[1][0].dx > -0.01 * VELOCIDAD_UNITARIA_ANCHO && autos[1][0].dx < 0.01 * VELOCIDAD_UNITARIA_ANCHO) //si la velocidad es prácticamente 0
                        {
                            if(nivel <= 5) 
                            {
                               autos[1][i].dx -= 0.01 * VELOCIDAD_UNITARIA_ANCHO;    
                            }
                            else
                            {
                                if(random == 0) //dependiendo del valor de radom, acelera o desacelera
                                {
                                    autos[1][i].dx -= 0.01 * VELOCIDAD_UNITARIA_ANCHO;
                                }
                                else
                                {
                                    autos[1][i].dx += 0.01 * VELOCIDAD_UNITARIA_ANCHO;
                                    contador_cambiar_velocidad = FRAMES_CAMBIAR_VELOCIDAD;
                                    aceleracion = ACELERA;
                                }
                            }
                        }
                        else
                        {
                            autos[1][i].dx -= 0.01 * VELOCIDAD_UNITARIA_ANCHO;
                        }
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
    
    if(timer_moverse == 0) //indicamos de cambiar de frame de animación de nadar
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
    
    /*
     * Sólo se hunde una tanda de tortugas por fila
     */
    
    if(timer_hundirse == (int)(FRAMES_HASTA_HUNDIRSE * (3.0/4.0)) ) //indicamos que tiene que nadar
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
    if(timer_hundirse == FRAMES_HASTA_HUNDIRSE / 2) //indicamos que tiene que hundirse 
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
            if (tortugas[i][0].hundirse == false) //indicamos que tiene que a está prácticamente hundida
            {
                tortugas[i][0].frames = 3;
            }
        }
    }
    if (timer_hundirse == 0) //indicamos que tiene que está hundida
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
    
    if (nivel <= 4)
    {
        tiempo = FRAMES_SIN_COCODRILO/ nivel; //el tiempo que tarda el cocodrilo en aparecer dismuye hasta el nivel 4
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
            i = rand() % CANT_CASILLAS_LLEGADA; //de manera random, vemos qué casilla poner el cocodrilo
            
            if(llegadas[i].ocupado == false) //si está vacía, se la ocupa
            {
                llegadas[i].cocodrilo = true;
                timer_con_cocodrilo--;
            }
            else //si está llena, simplemente vuelve a empezar el timer
            {
                timer_con_cocodrilo = FRAMES_CON_COCODRILO;
                timer_sin_cocodrilo = tiempo;
            }
        }
        else
        {
            timer_con_cocodrilo--;
            if(timer_con_cocodrilo == 0) //si se acabó el tiempo, se cambia el estado de la casilla con cocodrilo a sin cocodrilo
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

/**********************************************************************
 * 
 * SUMAR_PUNTAJE_SALTO:
 *      suma puntaje según la posición máxima alcanzada por la rana
 * 
 *********************************************************************/

static void sumar_puntaje_salto(void)
{
    int i;
    int fila_actual;
    
    /*
     * Dependiendo de la fila actual, se suma puntos a la rana
     */
    
    
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


/**************************************************************************************
 * 
 * get_high_score:
 *      - recibe un puntero a una variable sobre la cual devolverá si hubo un error
 *      - devuelve el high_score encontrado
 * 
 *************************************************************************************/

static unsigned long int get_high_score(int * error)        //toma el puntaje máximo
{
    FILE * p_highscore;
    int c;                                          //se crea una variable para uso
    unsigned long int high_score = 0;               //se usa una variable para el puntaje máximo
    
    p_highscore = fopen("high_score.txt", "r");     //se abre el archivo donde se aloja el puntaje máximo para escritura también
    
    if(p_highscore == NULL)     //caso de error con el archivo
    {
        fprintf(stderr, "failed to open file p_highscore\n");   //mensaje de error
        *error = -1;
        return 0;
    }
    
    *error = 0;
      
    
    while ((c = fgetc(p_highscore)) != EOF && c != '\n') //mientras no se llegue al final del archivo o al final de una línea
    {
        if(c >= '0' && c <= '9')    //trata de tomar el valor escrito en el archivo de high score
        {
            c = c -'0';
            high_score = high_score * 10 + c;
        }
    }
    
    fclose(p_highscore);
    
    return high_score;      //devuelve el valor del puntaje máximo
}

/**************************************************************************************
 * 
 * save_new_score:
 *      - recibe el high_score a guardar
 *      - devuelve un int en -1 si hubo error o 0 si no lo hubo
 * 
 *************************************************************************************/

static int save_new_highscore(unsigned long int new_high_score) //se ve para guardar un nuevo puntaje
{
    FILE * p_highscore;
    
    p_highscore = fopen("high_score.txt", "w"); //abre y se usa le archivo en modo escritura
    
    if(p_highscore == NULL)     //caso error
    {
        fprintf(stderr, "failed to open file p_highscore\n");
        return -1;
    }
    
    fprintf(p_highscore, "%lu", new_high_score);    //imrpime el valor del puntaje y sobreescribe los datos
    
    fclose(p_highscore);
    
    return 0;
}