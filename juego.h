/***************************************************************************************************************
 *      FROGGER
 * 
 *      JUEGO.H:
 * 
 *              Autores:    Alegre, Marcos
 *                          Di Sanzo, Bruno
 *                          Hertter, José Iván
 *                          Ibáñez, Lucía
 * 
 *      Dependiendo del estado del juego, se ejecutan las funciones referidas al juego, o a los menués de
 *      inicio o pausa
 * 
 * 
 *  
 **************************************************************************************************************/


#ifndef JUEGO_H
#define JUEGO_H

////////////////////////////////////////////////////////////////////////////////////////////

#include <stdbool.h>  

/************************+
 * ESTADOS DEL JUEGO
 ************************/

enum ESTADOS
{
    INICIO, REINICIO, PAUSA, JUEGO, PERDER, MENU, PASAR_NIVEL
};

extern int estado_juego; //variable con el estado del juego


extern double tiempo_restante; //variable con el tiempo restante hasta perder

extern unsigned int nivel; //variable con el nivel actual
extern unsigned int vidas_restantes; //variable con la cantidad de vidas restantes

extern unsigned long int puntaje_juego; //variable con el puntaje del juego
extern unsigned long int high_score; //variable que almacena el high_score

extern int selector_menu; //variable que indica sobre qué opción del menú nos encontramos

extern int timer_pasar_nivel; //timer con el tiempo que queda para pasar de nivel (para mostrar puntaje actual y nivel)
extern int timer_perder; //timer con el tiempo que queda para reiniciar el juego desde que se pierde (para mostrar nivel llegado, high_score y puntaje actual)


/***************************************************
 * MUNDO Y VELOCIDAD DE JUEGO
***************************************************/
    
#define REFRESCO            60.0        //define la tasa de refresco
#define UN_SEGUNDO          (1*REFRESCO) //frames por segundo

#define CASILLA_ANCHO       50.0        //ancho casillas en x
#define CASILLA_ALTO        50.0        //alto casillas en y

#define CANT_CASILLAS_FILA          14  //define la cantidad de filas que va a tener
#define CANT_CASILLAS_COLUMNA       13  //define la cantidad de columnas que va a tener

#define MUNDO_ALTO          (CANT_CASILLAS_COLUMNA * CASILLA_ALTO) //alto mundo total
#define MUNDO_ANCHO         (CANT_CASILLAS_FILA * CASILLA_ANCHO) //ancho mundo totla

#define TIEMPO_TOTAL        (40 * REFRESCO) //tiempo total hasta perder

#define TIEMPO_PERDER       UN_SEGUNDO*10
#define TIEMPO_PASAR_NIVEL  UN_SEGUNDO*10

/***************************************************
 * MANEJO DE TECLAS DETECTADAS
***************************************************/

#define TECLAS_MAX          5

enum MYKEYS
{
    KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT, KEY_ENTER //arrow keys
};
    
extern bool key_pressed[TECLAS_MAX]; //arreglo con las 5 teclas que necesita el juego para moverse y seleccionar el menu y pausa

    

/***************************************************
 * RANA
***************************************************/

#define RANA_ANCHO              (CASILLA_ANCHO - 20.0)         
#define RANA_ALTO               (CASILLA_ALTO - 20.0)
              
enum direcciones        //las posibles direcciones de la rana
{
    IZQUIERDA = 1, ARRIBA, DERECHA, ABAJO
};

typedef struct
{
    
    double x, y; //coordenadas
    double dx, dy; //velocidad
    double dx_extra; //velocidad extraadquirida por tronco o torutgas
    int direccion; //dirección de la rana
    bool saltando; //true si está saltando
    bool chocada; //true si está chocada
    int frame_chocada; // 0 = no chocada, 1 = mostrar perder
    bool llego; //true si llegó al otro lado
    int fila_max; //máxima fila a la cual llegó
    
}RANA;

extern RANA rene; //variable con la rana del jugador


/***************************************************
 * ENEMIGOS
 **************************************************/
#define CANT_ENEMIGOS 5 //Cantidad de filas que contienen enemigos "del mismo tipo" (autos o los que van sobre el agua)

#define AUTOS_POR_FILA      5      
#define FILAS_DE_AUTOS      5

#define AUTO1_ANCHO              (CASILLA_ANCHO - 20)         
#define AUTO1_ALTO               (CASILLA_ALTO - 20)

#define AUTO2_ANCHO              AUTO1_ANCHO        
#define AUTO2_ALTO               AUTO1_ALTO

#define CAMION_ANCHO              (2*CASILLA_ANCHO - 20)         
#define CAMION_ALTO               (CASILLA_ALTO - 20)

typedef struct //Defino una estructura nueva para los enemigos
{
    double x, y;
    double dx, dy;
    double largo;               //Posición, velocidad y largo del enemigo
    double alto;
    unsigned int fila;          //Fila en la que se moverá el enemigo
    int direccion;             //dirección del auto
}AUTOS;

extern AUTOS autos[FILAS_DE_AUTOS][AUTOS_POR_FILA]; //Defino una sola matriz con todos los enemigos de un mismo tipo

/***************************************************
 * TRONCOS
 **************************************************/

#define TRONCOS_POR_FILA        5
#define FILAS_DE_TRONCOS        3

#define TRONCO1_ANCHO              (3*CASILLA_ANCHO)         
#define TRONCO1_ALTO               (CASILLA_ALTO)

#define TRONCO2_ANCHO              (5*CASILLA_ANCHO)         
#define TRONCO2_ALTO               (CASILLA_ALTO)

#define TRONCO3_ANCHO              (4*CASILLA_ANCHO)         
#define TRONCO3_ALTO               (CASILLA_ALTO)

typedef struct //Defino una estructura nueva para los troncos
{
    double x, y;
    double dx, dy;
    double largo;               //Posición, velocidad y largo del TRONCO
    double alto;
    unsigned int fila;          //Fila en la que se moverá el TRONCO
}TRONCO;

extern TRONCO troncos[FILAS_DE_TRONCOS][TRONCOS_POR_FILA]; //Defino una sola matriz con todos los enemigos de un mismo tipo

/***************************************************
 * TORTUGAS
 **************************************************/

#define TORTUGAS_POR_FILA       5
#define FILAS_DE_TORTUGAS       2

#define TORTUGAS_ANCHO              (CASILLA_ANCHO)         
#define TORTUGAS_ALTO               (CASILLA_ALTO)


typedef struct //Defino una estructura nueva para las tortugas
{
    double x, y;
    double dx, dy;
    int direccion;
    double largo;               //Posición, velocidad y largo de LAS TORUGAS
    double alto;
    unsigned int fila;          //Fila en la que se moverá LAS TORTUGAS
    bool hundirse;              //true si las torugas están hundidads
    int frames;                 //0 si están nadando, 1 otro frame nadando, 2 hundiéndose, 3 casi hundida
    
}TORTUGAS;

extern TORTUGAS tortugas[FILAS_DE_TORTUGAS][TORTUGAS_POR_FILA]; //Defino una sola matriz con todas las tortugas


/***************************************************
 * CASILLAS DE LLEGADA
 **************************************************/
#define CANT_CASILLAS_LLEGADA       5

#define LLEGADA_ALTO                CASILLA_ALTO
#define LLEGADA_ANCHO               (1.2 * CASILLA_ANCHO)

typedef struct              //una estructura para la llegada
{
    double x,y;
    double ancho;
    double alto;            //Posición, velocidad y largo de las casillas de llegada
    bool ocupado;           //true si hay una rana
    bool cocodrilo;         //true si hay un cocodrilo

}LLEGADA;

extern LLEGADA llegadas[CANT_CASILLAS_LLEGADA]; //arreglo con las casillas de llegada


/***************************************************
 * DECLARACIÓN DE FUNCIONES GLOBALES
***************************************************/

bool frogger (void); //función principal del juego


//////////////////////////////////////////////////////////////////////////////////////

#endif /* JUEGO_H */
