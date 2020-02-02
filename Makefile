IDIR = include
LDIR = lib
SDIR = src/
ODIR = obj/


LIB = -lpthread -lprotobuf-c -lprotobuf -lprotoc
CFLAGS = -g -Wall -I$(IDIR)


all: monitord monitorc

monitord: $(SDIR)server/monitord.c csapp.o monitor_d_helper.o monitor.pb-c.o
	gcc $(CFLAGS) $(ODIR)csapp.o $(ODIR)monitor_d_helper.o $(ODIR)monitor.pb-c.o $(SDIR)server/monitord.c $(LIB) -o $@

monitorc: $(SDIR)client/monitorc.c csapp.o monitor_c_helper.o monitor.pb-c.o
	gcc $(CFLAGS) $(ODIR)csapp.o $(ODIR)monitor_c_helper.o $(ODIR)monitor.pb-c.o $(SDIR)client/monitorc.c $(LIB) -o $@



csapp.o: $(SDIR)csapp.c
	gcc $(CFLAGS) -c $(SDIR)csapp.c -o $(ODIR)csapp.o


monitor.pb-c.o: $(SDIR)monitor.pb-c.c
	gcc $(CFLAGS) -c $(SDIR)monitor.pb-c.c -o $(ODIR)monitor.pb-c.o

monitor_c_helper.o: $(SDIR)client/monitor_c_helper.c
	gcc -c $(SDIR)client/monitor_c_helper.c $(CFLAGS) -o $(ODIR)monitor_c_helper.o

monitor_d_helper.o: $(SDIR)server/monitor_d_helper.c
	gcc -c $(SDIR)server/monitor_d_helper.c $(CFLAGS) -lm -o $(ODIR)monitor_d_helper.o



.PHONY: clean
clean:
	rm  monitorc monitord  $(ODIR)*.o
