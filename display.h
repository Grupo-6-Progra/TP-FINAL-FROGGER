//#define RASPI4
#ifdef RASPI4

#ifndef DISPLAY_H
#define DISPLAY_H



    void redraw_disp(void);
    bool timer (void);
    void print_object_display (double , double, int);
    void display_teclas(void);
    void vidas(int);
    void init_display(void);
    void print_rene (void);
    
#define ANCHO_RENE 3
#define ALTO_RENE 3
    
#define TAM_PIXEL RANA_ALTO/ALTO_RENE


#endif /* DISPLAY_H */

#endif