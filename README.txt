1.how to make?
	gcc udpserver.c -o udpserver

2.how to use?
	1)change the file dir micro in file udpserver.c to your own dir
		#define FILE_DIR "/mnt/hgfs/log/vm_log/"
	2)run udpserver with your port to listen.
		./udpserver [port]