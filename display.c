
//#define RASPI_DISPLAY
#ifdef RASPI_DISPLAY 

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "juego.h"
#include "colisiones.h"
#include "display.h"
#include "menu.h"

#include "/home/pi/libs/joydisp/disdrv.h"
#include "/home/pi/libs/joydisp/joydrv.h"
#include "/home/pi/libs/joydisp/termlib.h"
/********************************************
*  CONSTATNTES PARA RPI                     *
*********************************************/
#define THRESHOLD 40	//Límite a partir del cual se mueve el LED encendido

#define APROX(x)                ((x>0) ? ((int)((x)+0.5)) : ((int)((x)-0.5)))
#define SEGUNDO  (REFRESCO)

/*********************************************
*  DECLARACION DE FUNCIONES LOCALES         *
*********************************************/

static void redraw_rana_d (void);
static void print_rana (int mat[][3]);

static void redraw_autos_d(void);       //funciones para la impresion de autos
static void print_auto1 (int,int, int);
static void print_auto2 (int,int, int);

static void redraw_troncos_d(void);     //funciones para la impresion de troncos
static void print_tronco1(int, int);
static void print_tronco2(int, int);
static void print_tronco3(int, int);

static void redraw_tortugas_d(void);    //funciones para la impresion de tortugas
static void print_tortuga1(int,int,int);
static void print_tortuga2(int,int,int);
static void print_tortuga3(int,int,int);
static void print_tortugas_d(int mat[][5], int,int,int);


static void redraw_tiempo_d(void);  //imprime el tiempo restante en el display

static void redraw_llegada_d(void); //funciones para la impresion de la llegada
static void print_llegada_vacia(int, int);
static void print_llegada_cocodrilo(int, int);
static void print_llegada_llena(int, int);
static void print_casilla(int mat[][6], int, int);

static void redraw_fondo_d(void);   //funcion que delimita los bordes e instancias del juego

static void vidas(void);    //funcion que imprime vidas restantes

static void delete_disp(void);  //funcion que limpia la pantalla del display

static void print_pixels(int mat[][16]); //dibuja toda la matriz de 16x16
static void print_num (char arr[], int largo, int altura , int ret);    //desplazamiento de numeros en pantalla
static void print_decimal (char ,int, int); //impresion de numeros en pantalla
static void print_numero (int mat[][5], int,int);

/*************************************
* DEFINICION DE FUNCIONES GLOBALES  *
*************************************/

void init_display()
{
    joy_init();     //inicializa el joystick
    disp_init();    //inicializa el display
    disp_clear();   //limpia el displau
}

void redraw_disp (void) 
{
    
    char numeros[32];
    delete_disp();
    if(estado_juego != MENU && estado_juego != PAUSA && estado_juego != PASAR_NIVEL && estado_juego != PERDER)
    {

        //IMPRESION DEL FONDO
        redraw_fondo_d();

        //IMPRESION DE LOS AUTOS
        redraw_autos_d();

        //impresión de troncos
        redraw_troncos_d();

        //IMPRESIÓN DE TORTUGAS
        redraw_tortugas_d();
        
        //IMPRESION DE LAS CASILLAS DE LLEGADA
        redraw_llegada_d();

        //IMPRESIÓN DE LA RANA
        redraw_rana_d();
        
        //IMPRESION DE TIEMPO
        redraw_tiempo_d();
        
        //IMPRESION DE VIDASS
        vidas();     
    }
    
    else if (estado_juego == PASAR_NIVEL)   //caso en el que se gana el nivel y se muestra el siguiente
    {
        
        if(timer_pasar_nivel > 5*SEGUNDO)
        {
            
            static int ret = 0;
            if(timer_pasar_nivel > SEGUNDO*9.5)
            {
                ret = 0;
            }
            int mat[16][16] =  {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                {0,1,1,1,0,1,1,0,1,1,1,0,1,1,1,0},
                                {0,1,0,0,0,1,0,0,1,0,1,0,1,0,0,0},
                                {0,1,1,1,0,1,0,0,1,1,1,0,1,1,1,0},
                                {0,0,0,1,0,1,0,0,1,1,0,0,1,0,0,0},
                                {0,1,1,1,0,1,1,0,1,0,1,0,1,1,1,0},
                                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}}; //primero se muestra el SCORE
           
            print_pixels(mat);//impresion de la matriz

            
            snprintf(numeros,32*(sizeof(char)),"%lu",puntaje_juego);    //convierte a un arreglo en un string
           
            if (ret == 0) //impresion del string en el display
            {
                print_num(numeros,32,10,ret);  //tiene como parametros el arreglo, el tamano, la altura en la que se posicionan los numeros,
                ret = 1;                       //y la variable que indica la impresion
            }
            else
            {
                print_num(numeros,32,10,1);     
            }
        }
        
        else 
        {
            static int ret = 0;
            if(timer_pasar_nivel > SEGUNDO*4.5)
            {
                ret = 0;
            }
            int mat[16][16] =  {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                {1,0,0,1,1,0,1,0,1,0,1,1,0,1,0,0},
                                {1,0,0,1,0,0,1,0,1,0,1,0,0,1,0,0},
                                {1,0,0,1,1,0,1,0,1,0,1,1,0,1,0,0},
                                {1,0,0,1,0,0,1,0,1,0,1,0,0,1,0,0},
                                {1,1,0,1,1,0,0,1,0,0,1,1,0,1,1,0},
                                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},  //luego de un tiempo dado por el timer se imprime el nivel
                                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},  //al que se pasa
                                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}};   
            
            print_pixels(mat); //impresion de la matriz
            snprintf(numeros,32*(sizeof(char)),"%u",nivel+1); //conversion de arreglo a string
            if (ret == 0) //impresion del string
            {
                print_num(numeros,32,10,ret);
                ret = 1;
            }
            else
            {
                print_num(numeros,32,10,1);
            }
        }

    }
    else if (estado_juego == PERDER) //caso en el que se pierden las tres vidas
    {
        if(timer_perder > SEGUNDO*7) 
        {
            static int ret = 0;
            if(timer_perder > SEGUNDO*9.5)
            {
                ret = 0;
            }
            int mat[16][16] =  {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                {1,0,0,1,1,0,1,0,1,0,1,1,0,1,0,0},
                                {1,0,0,1,0,0,1,0,1,0,1,0,0,1,0,0},
                                {1,0,0,1,1,0,1,0,1,0,1,1,0,1,0,0},
                                {1,0,0,1,0,0,1,0,1,0,1,0,0,1,0,0},
                                {1,1,0,1,1,0,0,1,0,0,1,1,0,1,1,0},
                                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}}; //matriz de la palabra LEVEL
            
            print_pixels(mat); //impresion de la matriz
            
            snprintf(numeros,32*(sizeof(char)),"%u",nivel); //conversion a string
            if (ret == 0) //impresion del string
            {
                print_num(numeros,32,10,ret);
                ret = 1;
            }
            else
            {
                print_num(numeros,32,10,1);
            }

        }
        else if(timer_perder > 4*SEGUNDO)
        {
            static int ret = 0;
            if(timer_perder > SEGUNDO*6.5)
            {
                ret = 0;
            }
            int mat[16][16] =  {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                {0,1,0,1,0,1,0,1,1,1,0,1,0,1,0,0},
                                {0,1,0,1,0,1,0,1,0,0,0,1,0,1,0,0},
                                {0,1,1,1,0,1,0,1,1,1,0,1,1,1,0,0},
                                {0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,0},
                                {0,1,0,1,0,1,0,1,1,1,0,1,0,1,0,0},
                                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}}; //matriz del HIGH SCORE 
            
            print_pixels(mat); //impresion de la matriz 

            snprintf(numeros,32*(sizeof(char)),"%lu",high_score);   //conversion a string 
           
            if (ret == 0) //impresion del string
            {
                print_num(numeros,32,10,ret);
                ret = 1;
            }
            else
            {
                print_num(numeros,32,10,1);
            }
        }
        
        else
        {
            static int ret = 0;
            if(timer_perder > SEGUNDO*3.5)
            {
                ret = 0;
            }
            int mat[16][16] =  {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                {0,1,1,1,0,1,1,0,1,1,1,0,1,1,1,0},
                                {0,1,0,0,0,1,0,0,1,0,1,0,1,0,0,0},
                                {0,1,1,1,0,1,0,0,1,1,1,0,1,1,1,0},
                                {0,0,0,1,0,1,0,0,1,1,0,0,1,0,0,0},
                                {0,1,1,1,0,1,1,0,1,0,1,0,1,1,1,0},
                                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}}; //matriz que muestra el SCORE
            
            print_pixels(mat); //impresion de la matriz

            snprintf(numeros,32*(sizeof(char)),"%lu",puntaje_juego);    //conversion a string
           
            if (ret == 0)   //impresion del string
            {
                print_num(numeros,32,10,ret);
                ret = 1;
            }
            else
            {
                print_num(numeros,32,10,1);
            }
        }

    }
    else if (estado_juego == MENU || estado_juego == PAUSA) //caso en el que se ingresa al menu o al menu de pausa
    {
        switch(selector_menu)
        {
            case PLAY:  //ingreso al juego desde el menu de inicio
            {    
                int mat[16][16] = { {0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0},
                                    {0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0},
                                    {0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0},
                                    {0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0},
                                    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                    {0,1,1,1,0,1,0,0,1,1,1,0,1,0,1,0},
                                    {0,1,0,1,0,1,0,0,1,0,1,0,1,0,1,0},
                                    {0,1,1,1,0,1,0,0,1,1,1,0,1,1,1,0},
                                    {0,1,0,0,0,1,0,0,1,0,1,0,0,1,0,0},
                                    {0,1,0,0,0,1,1,0,1,0,1,0,0,1,0,0},
                                    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                    {0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0},
                                    {0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0},
                                    {0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0},
                                    {0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0}}; //matriz de PLAY
                
                print_pixels(mat);  //impresion de PLAY
                break;
            }
            case LEVEL: //ingreso al menu de niveles desde el menu principal
            {
                int mat[16][16] = { {0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0},
                                    {0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0},
                                    {0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0},
                                    {0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0},
                                    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                    {1,0,0,1,1,0,1,0,1,0,1,1,0,1,0,0},
                                    {1,0,0,1,0,0,1,0,1,0,1,0,0,1,0,0},
                                    {1,0,0,1,1,0,1,0,1,0,1,1,0,1,0,0},
                                    {1,0,0,1,0,0,1,0,1,0,1,0,0,1,0,0},
                                    {1,1,0,1,1,0,0,1,0,0,1,1,0,1,1,0},
                                    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                    {0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0},
                                    {0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0},
                                    {0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0},
                                    {0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0}}; //matriz de LEVEL
                
                print_pixels(mat);  //impresion de LEVEL
                break;
            }
            case QUIT:  //caso en el que se cierra el juego y se deja de ejecutar el programa
            {
                int mat[16][16] = { {0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0},
                                    {0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0},
                                    {0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0},
                                    {0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0},
                                    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                    {0,0,1,1,1,0,1,0,1,0,1,0,1,1,1,0},
                                    {0,0,1,0,1,0,1,0,1,0,1,0,0,1,0,0},
                                    {0,0,1,0,1,0,1,0,1,0,1,0,0,1,0,0},
                                    {0,0,1,1,1,0,1,0,1,0,1,0,0,1,0,0},
                                    {0,0,1,1,1,0,1,1,1,0,1,0,0,1,0,0},
                                    {0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0},
                                    {0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0},
                                    {0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0},
                                    {0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0},
                                    {0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0}}; //matriz de QUIT
                
                print_pixels(mat);  //impresion de QUIT
                break;
            }
            case MENU_LEVELS:   //una vez ingresado a LEVEL, se ingresa al menu de niveles en el que se puede seleccionar nivel
            {
                int mat[16][16] = { {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                    {1,0,0,1,1,0,1,0,1,0,1,1,0,1,0,0},
                                    {1,0,0,1,0,0,1,0,1,0,1,0,0,1,0,0},
                                    {1,0,0,1,1,0,1,0,1,0,1,1,0,1,0,0},
                                    {1,0,0,1,0,0,1,0,1,0,1,0,0,1,0,0},
                                    {1,1,0,1,1,0,0,1,0,0,1,1,0,1,1,0},
                                    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}}; //matriz de LEVEL
                print_pixels(mat);  //impresion de LEVEL en el menu level
                
                char decimal;
                decimal = nivel+'0';
                print_decimal(decimal,6,9); //impresion de los niveles
                break;
            }
            
            case MAIN_MENU: //caso del menu de PAUSA, en el que se puede ingresar al menu principal
            {
                 int mat[16][16] = {{0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0},
                                    {0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0},
                                    {0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0},
                                    {0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0},
                                    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                    {1,0,1,0,1,1,1,0,1,0,0,1,0,1,0,1},
                                    {1,1,1,0,1,0,0,0,1,1,0,1,0,1,0,1},
                                    {1,1,1,0,1,1,1,0,1,1,1,1,0,1,0,1},
                                    {1,0,1,0,1,0,0,0,1,0,1,1,0,1,0,1},
                                    {1,0,1,0,1,1,1,0,1,0,0,1,0,1,1,1},
                                    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                    {0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0},
                                    {0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0},
                                    {0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0},
                                    {0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0}}; //matriz de MENU
                print_pixels(mat);  //impresion de MENU
 
                break;
            }
            case RESUME:    //caso del menu de PAUSA, en el que se puede reanudar el juego
            {
                int mat[16][16] = { {0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0},
                                    {0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0},
                                    {0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0},
                                    {0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0},
                                    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                    {1,1,1,0,1,1,1,0,1,0,1,0,1,1,1,0},
                                    {1,0,1,0,1,0,0,0,1,1,1,0,1,0,0,0},
                                    {1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,0},
                                    {1,1,0,0,0,0,1,0,1,0,1,0,1,0,0,0},
                                    {1,0,1,0,1,1,1,0,1,0,1,0,1,1,1,0},
                                    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                    {0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0},
                                    {0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0},
                                    {0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0},
                                    {0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0}}; //matriz de RESUME
                
                print_pixels(mat);  //impresion de RESUME
                break;
            }
        }
    }
    
    disp_update();  //actualizacion del display

}
bool timer(void) //funcion de tiempo para analizar en el main cuando paso un frame
{
    static bool flag = true;
    static clock_t t;       
    if (flag == true)
    {
        t = clock();
    }

    if (clock() -t < CLOCKS_PER_SEC/20/REFRESCO)
    {
        flag = false; //si el tiempo es menor al requerido, devuelve un 0 y se sigue esperando en el main
        return false;
    }
    else
    {
        flag = true;    //el flag se pone en 1 cuando paso el tiempo requerido
        return true; 
        
    }
}

/*************************************
* DEFINICION DE FUNCIONES LOCALES    *
**************************************/
static void vidas(void) //funcion que imprime las vidas restantes abajo a la derercha en el display
{
    dcoord_t tresVidas = {13,15};
    dcoord_t dosVidas = {14,15};
    dcoord_t unaVida = {15,15};
    int i;
    int j;
    
    switch (vidas_restantes)
    {
    	case 0: 
                disp_write(unaVida, D_OFF);
    		disp_write(dosVidas, D_OFF);
    		disp_write(tresVidas, D_OFF);
    		break;
    	
    	
    	case 1: 
    	
    		disp_write(unaVida, D_ON);
    		disp_write(dosVidas, D_OFF);
    		disp_write(tresVidas, D_OFF);
    		break;
    	
    	
    	case 2: 
    	
    		disp_write(unaVida, D_ON);
    		disp_write(dosVidas, D_ON);
    		disp_write(tresVidas, D_OFF);
    		break;
    	
    	case 3: 
    	
    		disp_write(unaVida, D_ON);
    		disp_write(dosVidas, D_ON);
    		disp_write(tresVidas, D_ON);
    		break;
    	
    	    	    
    }

}

void display_teclas (void) //funcion que analiza los movimientos del joystick y los relaciona con el de la rana
{
    jcoord_t coord;
    
    joy_update();
    
    coord = joy_get_coord();
    
    if(coord.x > THRESHOLD) //si se mueve a la derecha el joystick
    {
        key_pressed[KEY_RIGHT] = true;  //gira a la derecha la rana
    }
    else
    {
        key_pressed[KEY_RIGHT] = false;
    }
    
    if(coord.x < -THRESHOLD)    //si se mueve a la izquierda el joystick
    {
        key_pressed[KEY_LEFT] = true;   //gira a la izquierda la rana
    }
    else
    {
        key_pressed[KEY_LEFT] = false;
    }
    
    if(coord.y > THRESHOLD) //si se mueve para arriba el joystick
    {
        key_pressed[KEY_UP] = true; //sube la rana

    }
    else
    {
        key_pressed[KEY_UP] = false;
    }
    
    if(coord.y < -THRESHOLD)    //si se mueve para abajo el joystick
    {
        key_pressed[KEY_DOWN] = true;   //baja la rana

    }
    else
    {
        key_pressed[KEY_DOWN] = false;  
    }
    
    if(joy_get_switch() == J_NOPRESS)   //si no se aprieta el pulsador del joystick
    {
        key_pressed[KEY_ENTER] = false; //no pasa nada
    }
    else
    {
        key_pressed[KEY_ENTER] = true;  //si se aprieta, se va al menu de pausa
    }
}

 
static void redraw_rana_d (void)    //funcion que imprime a la rana en el display
{
    int i;
    int j;
    dcoord_t coord;
    if (rene.chocada == false)  //caso rana si no es chocada
    {
        if(rene.direccion == ARRIBA)    //si la rana esta subiendo
        {
            int mat[3][3] = {{0,1,0},
                             {1,1,1},
                             {1,0,1}}; //matriz de la rana subiendo
            print_rana(mat);    //impresion de la rana subiendo

        }
        else if (rene.direccion == ABAJO)   //si la rana esta bajando
        {
            int mat[3][3] =  {{1,0,1},
                              {1,1,1},
                              {0,1,0}}; //matriz de la rana bajamdo
            print_rana(mat); //impresion de la rana bajando
        }

        else if (rene.direccion == DERECHA) //si la rana se esta desplazando a la derecha 
        {
            int mat[3][3] = {{1,1,0},
                             {0,1,1},
                             {1,1,0}}; //matriz de la rana girando a la derecha
            print_rana(mat);  //impresion de la rana girando a la derecha      
        }
        else if (rene.direccion == IZQUIERDA) //si la rana se esta desplazando a la izquierda
        {
            int mat[3][3] = {{0,1,1},
                             {1,1,0},
                             {0,1,1}};  //matriz de la rana girando a la izquierda  
            print_rana(mat);    //impresion de la rana girando a la izquierda
        }
    }
    
    else if (rene.chocada != false) //si la rana es chocada
    {
       
        int mat[3][3] = {{1,0,1},
                         {0,1,0},
                         {1,0,1}}; //matriz de la rana hundiendose
        print_rana(mat);  //impresion de la rana hundiendose

    }

}
/* FORMA DE LA RANA "NORMAL"
 .  
...
. .  
 
 */

static void redraw_autos_d (void) //impresion de los autos en el display
{
    int i;
    int j;
    int coordx;
    int coordy;
    for (i = 0 ; i< FILAS_DE_AUTOS ; i++)
    {
        for (j = 0; j< AUTOS_POR_FILA ; j++)
        { 
             coordx = APROX((autos[i][j].x - rene.x)/TAM_PIXEL)+7;//coordenadas del centro del auto
             coordy = APROX((autos[i][j].y - rene.y)/TAM_PIXEL)+7;
             
             if (i<4) //en las primeras cuatro filas se imprimen los autos normales
             {
                print_auto1(coordx, coordy, autos[i][j].direccion); //recibe diferencia de coordenadas auto-rana   
             }
             else //en la utlima fila se imprime el auto mas largo
             {
                 print_auto2(coordx, coordy, autos[i][j].direccion); //recibe diferencia de coordenadas auto-rana 
             }
        }
    
    }

}


static void print_auto1(int coordx, int coordy, int direc)//recibe la posicion relativa entre la rana y el enemigo
{
    dcoord_t coord;  
    int i;
    int j;   
        for (i=0 ; i<3 ; i++) 
        {
            for (j=0 ; j<3 ; j++)
            {
                if ((coordx - 1 + j < DISP_CANT_X_DOTS && coordx -1 +j >= 0)&&(coordy - 1 + i < DISP_CANT_Y_DOTS -1 && coordy -1 +i >= 0))
                {   //verificacion de que los enemigos no se salgan de los limites
                    if (direc == DERECHA)   //si los enemigos se desplazan a la derecha
                    {
                        if((j!=2 || i!=0) && (j!=2 || i!=2)) //no se imprimen ciertos bordes de la derecha para que se entienda la direccion
                        {
                            coord.x = coordx + j -1;
                            coord.y = coordy + i -1;
                
                         disp_write(coord, D_ON);  //impresion del enemigo                      
                        }                       
                    }
                    
                    else if (direc == IZQUIERDA) //si los enemigos se desplazan a la izquierda
                    {
                        if((j!=0 || i!=0) && (j!=0 || i!=2)) //no se imprimen ciertos bordes de la izq para que se entienda la direccion
                        {
                            coord.x = coordx + j -1;
                            coord.y = coordy + i -1;
                
                         disp_write(coord, D_ON); //impresion del enemigo                     
                        }      
                    }

                }
            }
        }
    

    }
    
static void print_auto2(int coordx, int coordy, int direc) //impresion del ultimo auto, mas largo de lo normal
{
    dcoord_t coord;
    
    int i;
    int j;   
        for (i=0 ; i<3 ; i++)
        {
            for (j=0 ; j<8 ; j++)
            {
                if ((coordx - 3 + j < DISP_CANT_X_DOTS && coordx -3 +j >= 0)&&(coordy - 1 + i < DISP_CANT_Y_DOTS -1 && coordy -1 +i >= 0))
                {
                    if (direc == DERECHA) //misma logica que los autos normales para darles direccion
                    {
                        if((j!=7 || i!=0) && (j!=7 || i!=2))
                        {
                            coord.x = coordx + j -3;
                            coord.y = coordy + i -1;
                
                         disp_write(coord, D_ON);                        
                        }                       
                    }
                    
                   else if (direc == IZQUIERDA)
                    {
                        if((j!=0 || i!=0) && (j!=0 || i!=2))
                        {
                            coord.x = coordx + j -3;
                            coord.y = coordy + i -1;
                
                         disp_write(coord, D_ON);                        
                        }      
                    }

                }
            }
        }
    
}


static void redraw_troncos_d(void) //impresion de los troncos en el display
{
    unsigned int j,k;
    int coordx;
    int coordy;

    for(j=0; j < FILAS_DE_TRONCOS; j++)
    {
        for(k=0; k < TRONCOS_POR_FILA; k++)
        {
            coordx = APROX((troncos[j][k].x - rene.x)/TAM_PIXEL)+7;//coordenadas del centro del tronco
            coordy = APROX((troncos[j][k].y - rene.y)/TAM_PIXEL)+7;
            
            if(j == 0)
            {
                print_tronco1(coordx, coordy);
            }
            if(j == 1)
            {
                print_tronco2(coordx, coordy);
            }
            if(j == 2)
            {
                print_tronco3(coordx,coordy);
            }
        }   //se imprimen tres filas de troncos
    }
}
    
static void print_tronco1(int coordx, int coordy) //impresion de la primera fila de troncos
{
    dcoord_t coord;
    
    int i;
    int j;   
        for (i=0 ; i<5 ; i++)
        {
            for (j=0 ; j<15 ; j++)
            {
                if ((coordx - 7 + j < DISP_CANT_X_DOTS && coordx -7 +j >= 0)&&(coordy - 2 + i < DISP_CANT_Y_DOTS -1 && coordy -2 +i >= 0))
                {   //se analiza que no se vaya de los limites el tronco
                    if (j==0 || j==14 || i==0 || i==4)  //se imprimen solo los bordes del tronco para que se distinga la rana
                    {
                        coord.x = coordx + j -7;
                        coord.y = coordy + i -2;

                        disp_write(coord, D_ON);                         
                    }

                }
            }
        }
}

static void print_tronco2(int coordx, int coordy) //impresion de la segunda fila de troncos (+grande que el anterior)
{
    dcoord_t coord;
    
    int i;
    int j;   
        for (i=0 ; i<5 ; i++)
        {
            for (j=0 ; j<25 ; j++)
            {
                if ((coordx - 12 + j < DISP_CANT_X_DOTS && coordx -12 +j >= 0)&&(coordy - 2 + i < DISP_CANT_Y_DOTS -1 && coordy -2 +i >= 0))
                {   //se analiza que no se vaya de los limites del display
                    if (j==0 || j==24 || i==0 || i==4)  //se imprimen los bordes del tronco
                    {
                        coord.x = coordx + j -12;
                        coord.y = coordy + i -2;

                        disp_write(coord, D_ON);                         
                    }

                }
            }
        }
}



static void print_tronco3(int coordx, int coordy) //impresion del tercer tipo de tronco(+chica que el anterior pero mas grande que el primero)
{
    dcoord_t coord;
    
    int i;
    int j;   
        for (i=0 ; i<5 ; i++)
        {
            for (j=0 ; j<20 ; j++)
            {
                if ((coordx - 9 + j < DISP_CANT_X_DOTS && coordx -9 +j >= 0)&&(coordy - 2 + i < DISP_CANT_Y_DOTS -1 && coordy -2 +i >= 0))
                {   //se analiza que este dentro de los limites del display
                    if (j==0 || j==19 || i==0 || i==4) //impresion de los bordes del tronco
                    {
                        coord.x = coordx + j -9;
                        coord.y = coordy + i -2;

                        disp_write(coord, D_ON);                         
                    }

                }
            }
        }
}
static void redraw_tortugas_d(void) //impresion de tortugas
{
    unsigned int j,k;
    int coordx;
    int coordy;
    for(j=0; j < FILAS_DE_TORTUGAS; j++)
    {
        for(k=0; k < TORTUGAS_POR_FILA; k++)
        {
            coordx = APROX((tortugas[j][k].x - rene.x)/TAM_PIXEL)+7;//coordenadas del centro de las tortugas
            coordy = APROX((tortugas[j][k].y - rene.y)/TAM_PIXEL)+7;
            
            if(tortugas[j][k].hundirse == false) //si no estan HUNDIDAS
            {
                if(j == 0) //primera fila de tortugas
                {
                    if (tortugas[j][k].frames == 0 || tortugas[j][k].frames == 1) //caso en el que no se estan hundiendo
                    {
                        print_tortuga1(coordx-5, coordy ,tortugas[j][k].direccion); //voy a la funcion que imprime la tortuga nadando
                        print_tortuga1(coordx, coordy ,tortugas[j][k].direccion);
                        print_tortuga1(coordx+5, coordy ,tortugas[j][k].direccion);
                    }
                    else if (tortugas[j][k].frames == 2) //caso en el que se empiezan a hundir
                    {
                        print_tortuga2(coordx-5, coordy ,tortugas[j][k].direccion); //imprimo tres tortugas empezando a hundirse
                        print_tortuga2(coordx, coordy ,tortugas[j][k].direccion);
                        print_tortuga2(coordx+5, coordy ,tortugas[j][k].direccion);                        
                    }
                    
                    else if (tortugas[j][k].frames == 3) //caso en el que estan casi del todo hundidas
                    {
                        print_tortuga3(coordx-5, coordy ,tortugas[j][k].direccion);//imprimo tres tortugas casi hundidas
                        print_tortuga3(coordx, coordy ,tortugas[j][k].direccion);
                        print_tortuga3(coordx+5, coordy ,tortugas[j][k].direccion);                        
                    }

                }

                if(j == 1)  //segunda fila de tortugas
                {                   
                    if (tortugas[j][k].frames == 0 || tortugas[j][k].frames == 1)//no se estan hundiendo
                    {
                        print_tortuga1(coordx-2, coordy ,tortugas[j][k].direccion);//imprimo dos tortugas completas
                        print_tortuga1(coordx+3, coordy ,tortugas[j][k].direccion);
                    }
                    else if (tortugas[j][k].frames == 2)//se estan empezando a hundir
                    {
                        print_tortuga2(coordx-2, coordy ,tortugas[j][k].direccion);//dos tortugas hundiendose
                        print_tortuga2(coordx+3, coordy ,tortugas[j][k].direccion);                        
                    }
                    
                    else if (tortugas[j][k].frames == 3)//se estan casi hundiendo
                    {
                        print_tortuga3(coordx-2, coordy ,tortugas[j][k].direccion);//dos tortugas casi hundidas
                        print_tortuga3(coordx+3, coordy ,tortugas[j][k].direccion);                    
                    }
                    
                }//si ya estan hundidas las tortugas, no hay nada para analizar porque no aparece nada.
            }
         
        }
    }
}

static void print_tortuga1(int coordx, int coordy, int direc) //impresion de tortugas sin hundirse
{
    dcoord_t coord;
    int i;
    int j;
    
    int mat_izq[5][5]={{1,0,0,0,1},   
                       {0,1,1,1,0},
                       {1,0,0,1,0},
                       {0,1,1,1,0},
                       {1,0,0,0,1}};  //matriz tortuga desplazandose a la izquierda
    
    int mat_der[5][5]={{1,0,0,0,1},   
                       {0,1,1,1,0},
                       {0,1,0,0,1},
                       {0,1,1,1,0},
                       {1,0,0,0,1}};  //matriz tortuga desplazandose a la derecha
  
  
 /*  
.   .
 ...
.  .
 ...      
.   .      
 
 */
    
    for (i=0 ; i<5 ; i++)
    {
        for(j=0 ; j<5 ; j++)
        {
            if ((coordx - 2 + j < DISP_CANT_X_DOTS && coordx -2 +j >= 0)&&(coordy - 2 + i < DISP_CANT_Y_DOTS -1 && coordy -2 +i >= 0))
            {
                if (direc == IZQUIERDA) //si la direccion correspondiente es la izquierda
                {
                    if (mat_izq[i][j]==1) //se imprimen las tortugas desplazandose en esa direccion
                    {
                        coord.x = coordx -2 + j;

                        coord.y = coordy -2 + i;

                        disp_write(coord, D_ON);
                    }
                }
                else if (direc == DERECHA) //si la direccion es derecha
                {
                    if (mat_der[i][j]==1) //se imprimen las tortugas desplazandose en esa direccion
                    {
                        coord.x = coordx -2 + j;

                        coord.y = coordy -2 + i;

                        disp_write(coord, D_ON);
                    }
                }
                
            }
                    
        }
    }
/*  
.   .
 ...
.  .
 ...      
.   .      
 
 */
}

static void print_tortuga2(int coordx, int coordy, int direc) //impresion de tortuga empezando a hundirse
{
    int mat[5][5]={{0,0,0,0,0},   
                   {0,1,1,1,0},
                   {0,1,0,1,0},
                   {0,1,1,1,0},
                   {0,0,0,0,0}};  
  
  
 /*  
   
 ...
 . .
 ...      
        
 
 */
    print_tortugas_d(mat,coordx,coordy,direc);//funcion que imprime a esa tortuga
    
}


static void print_tortuga3(int coordx, int coordy, int direc) //impresion de tortuga casi hundida
{
    dcoord_t coord;
    int i;
    int j;
    
    int mat[5][5]={{0,0,0,0,0},   
                   {0,0,0,0,0},
                   {0,0,1,0,0},
                   {0,0,0,0,0},
                   {0,0,0,0,0}};  
  
  

    print_tortugas_d(mat,coordx,coordy,direc); //funcion que imprime esa tortuga
}

static void redraw_fondo_d(void)    //funcion que imprime los bordes del juego y las lineas divisoras 
{
    int derecha;
    int izquierda;
    int abajo;
    int arriba;

    dcoord_t coord;
    
    derecha = APROX((MUNDO_ANCHO - rene.x)/TAM_PIXEL)+6;//coordenadas del borde derecho del juego
    abajo = APROX((MUNDO_ALTO - rene.y)/TAM_PIXEL)+6;//coordenadas del borde inferior del juego
    izquierda = APROX((0- rene.x)/TAM_PIXEL)+8;//coordenadas del borde izquierdo del juego
    arriba = APROX((0- rene.y)/TAM_PIXEL)+8;//coordenadas del borde superior del juego
    
    int i;
    int j;
    if(derecha < DISP_CANT_X_DOTS)
    {
        if(abajo < DISP_CANT_Y_DOTS-1) 
        {
            for (i=0; i<=derecha ; i++)//for de abajo
            {                
                coord.x = i;
                coord.y = abajo;
                disp_write(coord, D_ON);
            }
            for (j=0 ; j<=abajo;j++)//for de derecha
            {
                coord.x = derecha;
                coord.y=j;
                disp_write(coord,D_ON);
            }
        }
        else if(arriba >= 0)
        {
            for (i=0; i<=derecha ; i++)//for de arriba
            {                
                coord.x = i;
                coord.y = arriba;
                disp_write(coord, D_ON);
            }
            for (j=arriba ; j<DISP_CANT_Y_DOTS-1;j++) //for de derecha
            {
                coord.x = derecha;
                coord.y=j;
                disp_write(coord,D_ON);
            }
        }
        else
        {
            for(i=0 ; i<DISP_CANT_X_DOTS ; i++) //for de derecha
            {
                coord.x = derecha;
                coord.y = i;
                disp_write(coord,D_ON);
                        
            }
                 
        }

    }
    
    else if(izquierda >= 0)
    {
        if(abajo < DISP_CANT_Y_DOTS-1)
        {
            for (i=izquierda; i<DISP_CANT_X_DOTS ; i++)//for de abajo
            {                
                coord.x = i;
                coord.y = abajo;
                disp_write(coord, D_ON);
            }
            for (j=0 ; j<=abajo;j++)//for de izquierda
            {
                coord.x = izquierda;
                coord.y=j;
                disp_write(coord,D_ON);
            }
        }
        else if(arriba >= 0)
        {
            for (i=izquierda; i<DISP_CANT_X_DOTS ; i++)//for de arriba
            {                
                coord.x = i;
                coord.y = arriba;
                disp_write(coord, D_ON);
            }
            for (j=arriba ; j<DISP_CANT_Y_DOTS-1;j++) //for de izquierda
            {
                coord.x = izquierda;
                coord.y=j;
                disp_write(coord,D_ON);
            }
        }
        else
        {
            for(i=0 ; i<DISP_CANT_X_DOTS ; i++) //for de izquierda
            {
                coord.x = izquierda;
                coord.y = i;
                disp_write(coord,D_ON);
                        
            }
                 
        }
     

    }
   
    else if (arriba>=0)
    {
        for(i=0 ; i<DISP_CANT_X_DOTS ; i++) //for de arriba
            {
                coord.x = i;
                coord.y = arriba;
                disp_write(coord,D_ON);
                        
            }
    }
  
    else if (abajo<DISP_CANT_Y_DOTS-1)
    {
        for(i=0 ; i<DISP_CANT_X_DOTS ; i++) // for de abajo
            {
                coord.x = i;
                coord.y = abajo;
                disp_write(coord,D_ON);
                        
            }
    }
    
    /*
     * 
     * Ahora, imprimimos las líneas divisiorias de las filas "seguras"
     * 
     */
   
    /*
     * fila inferior    
     */
    
    if(abajo - 4 < DISP_CANT_Y_DOTS - 1 && abajo-4>=0)
    {
        if(derecha < DISP_CANT_X_DOTS)
        {
            for (i=0; i<=derecha ; i++)//for de abajo
            {                
                coord.x = i;
                coord.y = abajo - 4;
                disp_write(coord, D_ON);
            }
        }
        else if(izquierda >= 0)
        {
           for (i=izquierda; i < DISP_CANT_X_DOTS ; i++)//for de abajo
            {                
                coord.x = i;
                coord.y = abajo - 4;
                disp_write(coord, D_ON);
            } 
        }
        else
        {
            for (i=0; i < DISP_CANT_X_DOTS ; i++)//for de abajo
            {                
                coord.x = i;
                coord.y = abajo - 4;
                disp_write(coord, D_ON);
            } 
        }
    }
    
    /*
     * 
     * Fila del medio
     * 
     */
  
    if(abajo - 30 < DISP_CANT_Y_DOTS - 1 && abajo-30>=0)
    {
        if(derecha < DISP_CANT_X_DOTS)
        {
            for (i=0; i<=derecha ; i++)//for de abajo
            {                
                coord.x = i;
                coord.y = abajo - 30;
                disp_write(coord, D_ON);
            }
        }
        else if(izquierda >= 0)
        {
           for (i=izquierda; i < DISP_CANT_X_DOTS ; i++)//for de abajo
            {                
                coord.x = i;
                coord.y = abajo - 30;
                disp_write(coord, D_ON);
            } 
        }
        else
        {
            for (i=0; i < DISP_CANT_X_DOTS ; i++)//for de abajo
            {                
                coord.x = i;
                coord.y = abajo - 30;
                disp_write(coord, D_ON);
            } 
        }
    }
    
    if(abajo - 34 < DISP_CANT_Y_DOTS - 1 && abajo-34>=0)
    {
        if(derecha < DISP_CANT_X_DOTS)
        {
            for (i=0; i<=derecha ; i++)//for de abajo
            {                
                coord.x = i;
                coord.y = abajo - 34;
                disp_write(coord, D_ON);
            }
        }
        else if(izquierda >= 0)
        {
           for (i=izquierda; i < DISP_CANT_X_DOTS ; i++)//for de abajo
            {                
                coord.x = i;
                coord.y = abajo - 34;
                disp_write(coord, D_ON);
            } 
        }
        else
        {
            for (i=0; i < DISP_CANT_X_DOTS ; i++)//for de abajo
            {                
                coord.x = i;
                coord.y = abajo - 34;
                disp_write(coord, D_ON);
            } 
        }
    }
    
    /*
     * 
     * Fila superior
     * 
     */
  
    if(arriba + 4 >= 0 && arriba + 4 < DISP_CANT_Y_DOTS -1)
    {
        if(derecha < DISP_CANT_X_DOTS)
        {
            for (i=0; i<=derecha ; i++)//for de arriba
            {                
                coord.x = i;
                coord.y = arriba + 4;
                disp_write(coord, D_ON);
            }
        }
        else if(izquierda >= 0)
        {
           for (i=izquierda; i < DISP_CANT_X_DOTS ; i++)//for de arriba
            {                
                coord.x = i;
                coord.y = arriba + 4;
                disp_write(coord, D_ON);
            } 
        }
        else
        {
            for (i=0; i < DISP_CANT_X_DOTS ; i++)//for de arriba
            {                
                coord.x = i;
                coord.y = arriba + 4;
                disp_write(coord, D_ON);
            } 
        }
    }
   
    
}


static void redraw_llegada_d(void) //funcion de casilla de llegada
{
    unsigned int i;
    int coordx;
    int coordy;
    for(i=0; i < CANT_CASILLAS_LLEGADA; i++)
    {
        coordx = APROX((llegadas[i].x - rene.x)/TAM_PIXEL)+7;//coordenadas del centro la casilla
        coordy = APROX((llegadas[i].y - rene.y)/TAM_PIXEL)+7;
        
        if (llegadas[i].ocupado == true) //si ya se ocupo la casilla, queda impresa una rana ahi
        {
            print_llegada_llena(coordx, coordy);
        }
        
        else if(llegadas[i].cocodrilo == true) //caso en el que aparece un cocodrilo en la casilla
        {
            print_llegada_cocodrilo(coordx, coordy);
        }
        
        else
        {
            print_llegada_vacia(coordx, coordy); //caso en el que la csasilla esta vacia
        }
    }
}

static void print_llegada_vacia(int coordx, int coordy) //funcion que analiza cuando la casilla esta vacia
{
    int mat[5][6]={{1,1,1,1,1,1},   
                   {1,0,0,0,0,1},
                   {1,0,0,0,0,1},
                   {1,0,0,0,0,1},
                   {1,0,0,0,0,1}};  //matriz de la casilla vacia
    
    print_casilla(mat,coordx, coordy);
    
}

static void print_llegada_cocodrilo(int coordx, int coordy)
{    
    int mat[5][6]={{1,1,1,1,1,1},   
                   {1,0,0,1,0,1},
                   {1,0,1,0,0,1},
                   {1,1,0,0,0,1},
                   {1,0,1,1,0,1}};  
  
    print_casilla(mat,coordx, coordy);
    
  
}

static void print_llegada_llena(int coordx, int coordy)
{
    int mat[5][6]={{1,1,1,1,1,1},   
                   {1,0,0,0,0,1},
                   {1,0,1,1,0,1},
                   {1,0,1,1,0,1},
                   {1,1,1,1,1,1}};  
    
    print_casilla(mat,coordx, coordy);
    
    
}
static void delete_disp(void)   //funcion que hace un clear en todo el display a mano
{
    int i;
    int j;

    dcoord_t coord;
    for (i=0 ; i<DISP_CANT_X_DOTS ; i++)
    {
        for (j=0 ; j<DISP_CANT_Y_DOTS ; j++)
        {
            coord.x = i;
            coord.y = j;
            disp_write(coord, D_OFF); //apaga todos los LEDS
        }
    }
}


static void redraw_tiempo_d(void) //funcion que imprime el tiempo restante de la jugada en en el display
{
    int xf;
    int i;
    dcoord_t coord;
    
    xf = APROX((12.0) * (tiempo_restante/TIEMPO_TOTAL)); 
    for (i=0 ; i< xf ; i++)
    {
       coord.x = i;
       coord.y = 15;
       disp_write(coord,D_ON);
    }
}

static void print_num (char numeros[], int largo, int altura, int momento) //funcion que imprime el desplazamiento de los numeros
{
    //momento es un indicador de que se quiere comenzar a escribir si esta en 0 (primera vez que se entra)
    int i;
    static int conta = 0; //se pone en 0 cuando se termina de escribir toda la palabra
    static int conta2 =0; //indica cuando hay que mover los numeros hacia la izquierda 1 pixel
    static int j=0;
    
    if(momento == 0 || conta == 0) 
    {
        conta2 = APROX(SEGUNDO/16.0);
        conta = SEGUNDO + largo*SEGUNDO;
        j=0;
    }
    else 
    {   
        if(conta2 == 0) //si conta2 esta en 0, se mueven los numeros 1 pixel a la izquierda
        {
            j++;
            conta2 = APROX(SEGUNDO/16.0);
        }
        for (i=0 ; i<largo && numeros[i] != 0 ; i++)
        {
           print_decimal (numeros[i],15+5*i-j,altura) ;
        }
    }
    conta--;
    conta2--;

    
}

static void print_decimal (char num, int x, int y) //funcion que imprime numeros individuales
{
    
    switch (num) //switch con los numeros del 0 al 9
    {
        case '0':
        {   
         int mat[5][5]={{0,1,1,1,0},   
                        {0,1,0,1,0},
                        {0,1,0,1,0},
                        {0,1,0,1,0},
                        {0,1,1,1,0}}; //numero 0
         
         print_numero(mat,x,y);
         
        }
            break;
            
        case '1':
        {
            int mat[5][5]={{0,0,0,1,0},   
                           {0,0,1,1,0},
                           {0,1,0,1,0},
                           {0,0,0,1,0},
                           {0,0,0,1,0}}; //numero 1
            print_numero(mat,x,y);
        }
            break;
            
        case '2':
        {
            int mat[5][5]={{0,1,1,1,0},   
                           {0,0,0,1,0},
                           {0,1,1,1,0},
                           {0,1,0,0,0},
                           {0,1,1,1,0}}; //numero 2
            print_numero(mat,x,y);
        }
            break;
        
        case '3':
        {
            int mat[5][5]={{0,1,1,1,0},   
                           {0,0,0,1,0},
                           {0,1,1,1,0},
                           {0,0,0,1,0},
                           {0,1,1,1,0}}; //numero 3
            print_numero(mat,x,y);
        }    
            break;
            
        case '4':
        {
            int mat[5][5]={{0,1,0,1,0},   
                           {0,1,0,1,0},
                           {0,1,1,1,0},
                           {0,0,0,1,0},
                           {0,0,0,1,0}}; //numero 4
            print_numero(mat,x,y);
        }
            break;
           
        case '5':
        {
            int mat[5][5]={{0,1,1,1,0},   
                           {0,1,0,0,0},
                           {0,1,1,1,0},
                           {0,0,0,1,0},
                           {0,1,1,1,0}}; //numero 5
            print_numero(mat,x,y);
        }
            break;
        
        case '6':
        {
            int mat[5][5]={{0,1,1,1,0},   
                           {0,1,0,0,0},
                           {0,1,1,1,0},
                           {0,1,0,1,0},
                           {0,1,1,1,0}}; //numero 6
            print_numero(mat,x,y);
        }
            break;
       
        case '7':
        {
            int mat[5][5]={{1,1,1,0,0},   
                           {0,0,1,0,0},
                           {0,1,1,1,0},
                           {0,0,1,0,0},
                           {0,0,1,0,0}}; //numero 7
            print_numero(mat,x,y);
        }
            break;
            
        case '8':
        {
            int mat[5][5]={{0,1,1,1,0},   
                           {0,1,0,1,0},
                           {0,1,1,1,0},
                           {0,1,0,1,0},
                           {0,1,1,1,0}}; //numero 8
            print_numero(mat,x,y);
        }
            break;
            
        case '9':
        {
            int mat[5][5]={{0,1,1,1,0},   
                           {0,1,0,1,0},
                           {0,1,1,1,0},
                           {0,0,0,1,0},
                           {0,1,1,1,0}}; //numero 9
            print_numero(mat,x,y);
        }
            break;
            
        default:
            break;
            
    }
    

}


static void print_pixels (int mat[][16]) //funcion que imprime palabras de los menues
{
    int i,j;
        dcoord_t coord;
        for(i=0;i<DISP_CANT_Y_DOTS;i++)    
        {
            for (j=0;j<DISP_CANT_X_DOTS;j++)
            {
                if(mat[i][j]==1)
                {
                    coord.x = j;
                    coord.y = i;
                    disp_write(coord,D_ON);
                }
            }
        }
}

static void print_rana (int mat[][3]) //funcion que imprime a la rana
{
    int i;
    int j;
    dcoord_t coord;
    for (i=0 ; i<ALTO_RENE ; i++)
    {
        for (j=0 ; j<ANCHO_RENE ; j++)
        {
            if (mat[i][j] ==1)
            {
                coord.x = j+6;
                coord.y = i+6;
                disp_write(coord, D_ON); 
            }
        }
    } 
}
static void print_tortugas_d(int mat[][5],int coordx, int coordy, int direc) //funcion que imprime a las tortugas
{ //no imprime a la tortuga que no se esta hundiendo porque esa necesita que se le analice la direccion
    dcoord_t coord;
    int i;
    int j;
    
    for (i=0 ; i<5 ; i++)
    {
        for(j=0 ; j<5 ; j++)
        {
            if ((coordx - 2 + j < DISP_CANT_X_DOTS && coordx -2 +j >= 0)&&(coordy - 2 + i < DISP_CANT_Y_DOTS -1 && coordy -2 +i >= 0))
            {

                    if (mat[i][j]==1)
                    {
                        coord.x = coordx -2 + j;

                        coord.y = coordy -2 + i;

                        disp_write(coord, D_ON);
                    }

               
            }
                
        }
                    
        
    }
    
    
}
static void print_casilla(int mat[][6], int coordx, int coordy) //funcion que imprime las casillas de llegada
{
    int i;
    int j;
    dcoord_t coord;
    for (i=0 ; i<5 ; i++)
    {
        for (j=0 ; j<6 ; j++)
        {
            if ((coordx - 2 + j < DISP_CANT_X_DOTS && coordx - 2 +j >= 0)&&(coordy - 2 + i < DISP_CANT_Y_DOTS -1 && coordy -2 +i >= 0))
            {
                if (mat[i][j] == 1)
                {
                    coord.x = coordx + j - 2;
                    coord.y = coordy + i - 2;

                    disp_write(coord, D_ON);                         
                }
                else
                {
                    coord.x = coordx + j - 2;
                    coord.y = coordy + i - 2;

                    disp_write(coord, D_OFF); 
                }

            }
        }
    }
}

static void print_numero (int mat[][5], int x, int y) //funcion que imprime los numeros en el display
{
    int i;
    int j;
    dcoord_t coord;
    for (i=0 ;i<5 ;i++)
         {
             for (j=0 ; j<5 ; j++)
             {
                 if (mat[i][j] == 1 && j+x < DISP_CANT_X_DOTS && j+x>= 0 && i+y< DISP_CANT_Y_DOTS && i+y >= 0)
                 {
                     coord.x = j + x;
                     coord.y = i + y;
                     
                     disp_write(coord,D_ON);
                 }
             }
         }
}
#endif




