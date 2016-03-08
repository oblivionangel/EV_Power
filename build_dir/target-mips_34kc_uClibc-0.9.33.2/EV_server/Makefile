#OBJS=main.o net_info.o protocal_conn.o  CRC.o AES.o
#SRCS=main.c net_info.c protocal_conn.c CRC.c AES.c  #$(OBJS:o=c)
#DST=a.out
#CFLAGS=-Wall -l/usr/include
#LDFLAGS=-L/usr/lib/openssl -lcrypto

#$(OBJS):$(SRCS)
#	gcc  -c  $(CFLAGS)  $(SRCS)  -o $@

#$(DST):$(OBJS)
#	gcc  $(CFLAGS) $(OBJS) -o $@ $(LDFLAGS)  

DST=EV_server

$(DST): main.o net_info.o CRC.o AES.o protocal_conn.o
	$(CC) $^ -lcrypto -L/usr/lib/x86_64-linux-guc/   -o $@   
main.o: main.c 
	$(CC) -c $< main.c
net_info.o:net_info.c
	$(CC) -c net_info.c
protocal_conn.o:protocal_conn.c
	$(CC) -c protocal_conn.c
CRC.o:CRC.c
	$(CC) -c CRC.c
AES.o:AES.c
	$(CC) -c  AES.c
clean:
	rm -rf *.o  *.o  $(DST)

