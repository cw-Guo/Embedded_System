CC = arm-linux-gcc 
PROGRAM = ts_main
OBJ = font_8x8.o font_8x16.o testutils.o fbutils.o 

default : $(PROGRAM) 
ts_main : ts_main.o $(OBJ) 
	$(CC) -o $@ $^  -lts -L /home/student/xxx/lib -lm
%.o : %.c 
	 $(CC) -c $< 
cp :
	cp $(PROGRAM) /srv/nfs4/touchscreen/bin