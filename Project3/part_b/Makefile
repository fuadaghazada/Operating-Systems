obj-m += test.o

all: app module;

module:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

app: app.c
	gcc -Wall -o app app.c

clean: clean_app clean_module;

clean_app:
	rm -fr app *~ *.o

clean_module:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
