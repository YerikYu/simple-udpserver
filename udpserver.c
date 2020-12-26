/*

udpserver.c

Copyright 2012, Isaakidis Marios, Daniel Aguado

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdbool.h>

#include "constants.h"

#define FILE_DIR "/mnt/hgfs/log/vm_log/"

int main(int argc, char *argv[])
{

    if(argc != 2)
    {//Check if program is correclty called
        fprintf(stderr, "Usage: udpserver [server_port]\n");
        return USAGE_ERR;
    }

    int serverPort = atoi(argv[1]);
    if(serverPort<=0 || serverPort>65535)
    {//Checking if the port given is valid and withing the accepted values
        fprintf(stderr, "The port number given is wrong.\n");
        return BAD_PORT_NUM_ERR;
    }

    printf("Starting UDP server...\n");
    //Initializing the socket variables
    int sockfd, bytesReceived;
    struct sockaddr_in serverAddr,clientAddr;
    socklen_t len;
    char buffer[NETWORK_BUFFER];
    //Creating a socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd < 0)
    {
        perror("Cannot open socket ");
        return SOCK_OPEN_ERR;
    }

    //Preparing serverAddr
    bzero(&serverAddr,sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(serverPort);
    memset(serverAddr.sin_zero, '\0', sizeof(serverAddr.sin_zero));

    //Binding on the socket
    if(bind(sockfd, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0)
    {
        perror("Cannot bind on UDP port");
        close(sockfd);
        return SOCK_BIND_ERR;
    }
    fprintf(stderr, "Bind on UDP port %d.\n", serverPort);

    
    char* strDir = FILE_DIR;
    if(access(strDir, 0 ) == -1){
        mkdir(strDir,0777);
    }

    struct timeval tv;
    gettimeofday(&tv,NULL);

    time_t ts = tv.tv_sec;
    struct tm* pTimeInfo = localtime(&ts);
    char mytime[20] = {0};
    strftime(mytime,sizeof(mytime),"%Y%m%d-%H%M%S", pTimeInfo);
    printf("Time:%s\n", mytime);

    char file[128] = {0};
    snprintf(file, sizeof(file) - 1, "%s%s-port-%d.txt", strDir, mytime, serverPort);
    printf("%s\n", file);

	if(daemon(1, 1) < 0)
	{
		perror("error daemon.../n");
		exit(1);
	}


    FILE *fp = fopen(file,"w"); 
    while(true)
    {

        len = sizeof(clientAddr);
        bytesReceived = recvfrom(sockfd, buffer, NETWORK_BUFFER - 1, 0, (struct sockaddr *)&clientAddr, &len);
        buffer[bytesReceived] = '\0';
        // printf("%s", buffer);
        fwrite(buffer, sizeof(char), strlen(buffer), fp);
        
    }

    //close the socket
    fclose(fp);
    close(sockfd);
    return 0;
}

