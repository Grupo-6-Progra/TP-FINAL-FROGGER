prueba: prueba.o 
	gcc prueba.o -o prueba /home/pi/libs/joydisp/disdrv.o /home/pi/libs/joydisp/joydrv.o /home/pi/libs/joydisp/termlib.o -Wall
	
prueba.o: pruebas_raspi.c /home/pi/libs/joydisp/juego.h /home/pi/libs/joydisp/colisiones.h
	gcc pruebas_raspi.c -c -o prueba.o  -Wall




