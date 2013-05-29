COMPILE = $(CROSS_COMPILE)
EXEC = server
OBJS = ../mdns_core/mdns_core.o ../mdns_core/dns-sd.o ../mdns_core/mdns_txt.o ../mdns_socket/mdns_socket.o  ../mdns_responser/mdns_responser.o  server.o
CC = $(COMPILE)gcc
CFLAGS = 
RM = rm -f
$(EXEC) : $(OBJS)
	$(CC) -o $(EXEC)  $(OBJS)  $(CFLAGS)
#$(OBJS): %.o  :  %.c 
#	$(CC) -c $(CFLAGS) $< -o $@

clean :
	-$(RM) $(EXEC)  $(OBJS) name.conf
