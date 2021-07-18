#define RASPI5
#ifdef RASPI5

#ifndef DISPLAY_H
#define DISPLAY_H
#include <stdbool.h>


    void redraw_disp(void);
    bool timer (void);
    void display_teclas(void);
    void init_display(void);
    
    
#define ANCHO_RENE 3.0
#define ALTO_RENE 3.0
    
#define TAM_PIXEL (RANA_ALTO/ALTO_RENE)




#endif /* DISPLAY_H */

#endif