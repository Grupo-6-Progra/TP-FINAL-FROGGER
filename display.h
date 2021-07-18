#ifndef DISPLAY_H
#define DISPLAY_H
#include <stdbool.h>


/***************************************************
 * DECLARACIÓN DE FUNCIONES GLOBALES
***************************************************/

    void redraw_disp(void);
    bool timer (void);
    void display_teclas(void);
    void init_display(void);
    
    
 /*********************************************
 * DEFINICION DE CONSTANTES PARA LA RPI
 **********************************************/   
    
#define ANCHO_RENE 3.0
#define ALTO_RENE 3.0
    
#define TAM_PIXEL (RANA_ALTO/ALTO_RENE)




#endif /* DISPLAY_H */

