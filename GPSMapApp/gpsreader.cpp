#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<termios.h>

#include "gpsreader.h"

GPSReader::GPSReader()
{
    m_latitude = 0.0f;
    m_longitude = 0.0f;
}
int GPSReader::startFD()
{
    int fd, nset;

    fd = open( "/dev/ttySAC1", O_RDWR);
    if (fd == -1)
    {
        printf("error open ttySAC1\n");
        return -1;
    }

     nset = set_opt(fd, 4800, 8, 'N', 1);
     if(nset == -1)
     {
        printf("set_opt fail!\n");
        return -1;
     }

     return fd;
}

int GPSReader::writeToFile()
{
    FILE *file;
    file = fopen("/data/map/start.txt","w");
    char buf[100] = "";
    sprintf(buf,"%lf %lf",m_longitude, m_latitude);
    fprintf(file, "%s", buf);
    fclose(file);
    return 0;
}

int GPSReader::set_opt( int fd, int nSpeed, int nBits, char nEvent, int nStop )
{
    struct termios newtio,oldtio;
    if  ( tcgetattr( fd,&oldtio)  !=  0)
    {
        perror("SetupSerial 1");
        return -1;
    }
    bzero( &newtio, sizeof( newtio ) );
    newtio.c_cflag  |=  CLOCAL | CREAD;
    newtio.c_cflag &= ~CSIZE;

    switch( nBits )
    {
    case 7:
        newtio.c_cflag |= CS7;
        break;
    case 8:
        newtio.c_cflag |= CS8;
        break;
    }

    switch( nEvent )
    {
     case 'O':
         newtio.c_cflag |= PARENB;
         newtio.c_cflag |= PARODD;
         newtio.c_iflag |= (INPCK | ISTRIP);
         break;
     case 'E':
         newtio.c_iflag |= (INPCK | ISTRIP);
         newtio.c_cflag |= PARENB;
         newtio.c_cflag &= ~PARODD;
         break;
     case 'N':
         newtio.c_cflag &= ~PARENB;
         break;
    }

    switch( nSpeed )
    {
     case 2400:
         cfsetispeed(&newtio, B2400);
         cfsetospeed(&newtio, B2400);
         break;
     case 4800:
         cfsetispeed(&newtio, B4800);
         cfsetospeed(&newtio, B4800);
         break;
     case 9600:
         cfsetispeed(&newtio, B9600);
         cfsetospeed(&newtio, B9600);
         break;
     case 115200:
         cfsetispeed(&newtio, B115200);
         cfsetospeed(&newtio, B115200);
         break;
     case 460800:
         cfsetispeed(&newtio, B460800);
         cfsetospeed(&newtio, B460800);
         break;
     default:
         cfsetispeed(&newtio, B9600);
         cfsetospeed(&newtio, B9600);
         break;
     }
     if( nStop == 1 )
         newtio.c_cflag &=  ~CSTOPB;
     else if ( nStop == 2 )
         newtio.c_cflag |=  CSTOPB;
     newtio.c_cc[VTIME]  = 0;
     newtio.c_cc[VMIN] = 0;
     tcflush(fd,TCIFLUSH);
     if((tcsetattr(fd,TCSANOW,&newtio))!=0)
     {
         perror("com set error");
         return -1;
     }
     printf("set done!\n\r");
     return 0;
}

int GPSReader::parseNE(char *buff)
{
    char *temp;
    char temp1[500] = "";
    char strN[20] = "";
    char strE[20] = "";

    temp = strstr(buff,"$GPRMC");
    if(temp == NULL) return -1;

    int ret = sscanf(temp,"%*[^A]%[^N]",temp1);
    if(ret == 0)
    {
        printf("can't find A~N");
        return -1;
    }
    char* p = temp1;
    p = p + 2;
    memcpy(strN,p,9);

    ret = sscanf(temp,"%*[^N]%[^E]",temp1);
    if(ret == 0)
    {
        printf("can't find N~E");
        return -1;
    }

    p = temp1;
    p = p + 2;
    memcpy(strE,p,9);

    m_latitude = atof(strN);
    m_longitude = atof(strE);
    return 0;
}
