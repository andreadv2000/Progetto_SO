#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <termios.h>
#include <gd.h>
#include <gdfonts.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>


int main(){

    //*****************Initialize integer variables*****************//
    int uart_fd, ret, msg_len;
    //*****************Inizialize float variables*****************//
    float sampling_time;
    //*****************Initialize char variables*****************//
    char user_msg[2048];

     /*
      * The termios functions describe a general terminal interface that
      * is provided to control asynchronous communications ports.(man)
    */
    struct termios settings;

    /*
     * Opens the serial device file "/dev/ttyACM0" for reading and writing.
     * The flags used in the open() function are:
     * O_RDWR: Open for reading and writing.
	  * O_NDELAY: Open in non-blocking mode. This means that any read or write operation on the file will not cause the calling process to wait until the device is ready. Instead, the function call will return immediately, indicating both success and failure.
	  * O_NOCTTY: Do not make this process the system's controlling terminal. This is useful when a program opens the port to communicate with an external device and does not want to become the system's controlling terminal.
    */
    uart_fd = open("/dev/ttyACM0", O_RDWR | O_NOCTTY | O_NDELAY);
    if(uart_fd < 0){
        perror("Client: Error opening UART device");
        return -1;
    }
    
    ret = tcgetattr(uart_fd, &settings);
    if(ret != 0){
        perror("Client: Error getting termios settings");
        return 1;
    }
    
   /* The termios structure(man)
      *      Many of the functions described here have a termios_p argument
      *     that is a pointer to a termios structure.  This structure
      *      contains at least the following members:

      *          tcflag_t c_iflag;        input modes 
      *          tcflag_t c_oflag;        output modes 
      *          tcflag_t c_cflag;        control modes 
      *          tcflag_t c_lflag;        local modes 
      *         cc_t     c_cc[NCCS];      special characters 
   */

  /* Set up serial port 
    * options.c_cflag sets the channel configuration flag, which contains the flags that define how the channel is used, 
    * including the number of bits per character, the number of stop bits, speed, parity, control flow, and data flow. 
    * In this case, we set options.c_cflag to B19200 | CS8, which means "19200 bits per second, 8 bits per character". 
  */
   
   /* Set the baud rate */
   settings.c_cflag |= CS8;
   cfsetispeed(&settings, B19200);
   cfsetospeed(&settings, B19200);

   ret = tcsetattr(uart_fd, TCSANOW, &settings);
   if(ret != 0){
       printf("Error settings termios settings %i %s\n", ret, strerror(errno));
       return 1;
   }

   /* Write to the serial port */
   do {
    printf("Client: Enter sampling time in ms (Max sampling time: 4000ms): \n");
    scanf("%f", &sampling_time);
   } while (sampling_time < 100 || sampling_time > 4000);

   /* Convert float to string for transmission to the serial port */
   sprintf(user_msg, "%.2f\n", sampling_time);
   
   /* Write to the serial port */
   msg_len = strlen(user_msg);
   ret = write(uart_fd, user_msg, msg_len);
   printf("Client:\n ->Samplig time: %s ->Size: %d bytes to serial port\n",user_msg,ret);
   if(ret < 0){
       perror("Client: Error writing to serial port");
       return 1;
   }
   
   
   
   FILE* data_stream, *channel1, *channel2, *channel3;
   FILE *wave12, *wave6, *wave2;
   char tokens[4][20];
   int i = 0;
   int n_values;

   /* Create images for each channel */ 
    gdImagePtr img1 = gdImageCreate(800, 600);
    gdImagePtr img2 = gdImageCreate(800, 600);
    gdImagePtr img3 = gdImageCreate(800, 600);

   /* Allocate some colors */
   int bg_color1 = gdImageColorAllocate(img1, 255, 255, 255);
   int bg_color2 = gdImageColorAllocate(img2, 255, 255, 255);
   int bg_color3 = gdImageColorAllocate(img3, 255, 255, 255);

   int axiscolor1 = gdImageColorAllocate(img1,0,0,0);
   int axiscolor2 = gdImageColorAllocate(img2,0,0,0);
   int axiscolor3 = gdImageColorAllocate(img3,0,0,0);

   int red = gdImageColorAllocate(img1, 255, 0, 0);
   int green = gdImageColorAllocate(img2, 0, 255, 0);
   int blue = gdImageColorAllocate(img3, 0, 0, 255);
   

   /* Open files */
   data_stream = fopen("data.txt", "w+");
   if(data_stream == NULL){
      perror("Client: Error opening file data.txt\n");
       return 0;
   }

   channel1 = fopen("channel1.txt", "w");
   if(channel1 == NULL){
      perror("Client: Error opening file channel1.txt\n");
      return 0;
   }

   channel2 = fopen("channel2.txt", "w");
   if(channel2 == NULL){
      perror("Client: Error opening file channel2.txt\n");
      return 0;
   }

   channel3 = fopen("channel3.txt", "w");
   if(channel3 == NULL){
      printf ("Error opening file channel3.txt\n");
      return 0;
   }
   
   wave12 = fopen("wave12.png", "w");
   if(wave12 == NULL){
      printf("Error opening file wave12.png\n");
      return 0;
   }

   wave6 = fopen("wave6.png", "w");
   if(wave6 == NULL){
      printf("Error opening file wave6.png\n");
      return 0;
   }

   wave2 = fopen("wave2.png", "w");
   if(wave2 == NULL){
      printf("Error opening file wave2.png\n");
      return 0;
   }

   printf("Client: Reading from serial port...\n");
   
   sleep(5);

   /*Store data in data_stream*/
   memset(user_msg, 0, sizeof(user_msg));
   
   int read_bytes = 0;
   while((ret = read(uart_fd, user_msg, 2048))>0){
    for(i = 0; i < ret; i++){
        fputc(user_msg[i], data_stream);
        read_bytes ++;
     }
     memset(user_msg, 0, sizeof(user_msg));
     sleep(sampling_time / 1000+1);
   }

   printf("Client: Recived %d bytes from Server\n", read_bytes);

    
   /*Write data in corresponding channel file*/
   n_values = 60/(sampling_time/1000);
   
   float times[n_values];
   float channel1_values[n_values];
   float channel2_values[n_values];
   float channel3_values[n_values];
   char line[256];
   char line_copy[256];
   char* token;
   int j;
   
   printf("Client: Measured Values:\n\n");
   fseek(data_stream, 0, SEEK_SET);
   for(int i = 0; i < n_values; i++){
       fgets(line, sizeof(line), data_stream);

       strncpy(line_copy, line, sizeof(line));
       if (line_copy == NULL) perror("Client: Error copying line\n");
       
       /* Remove newline character */
       line_copy[strcspn(line_copy, "\n")] = 0;
    
       /* Get the first token */
       token = strtok(line_copy, "-");

       /* Walk through other tokens */ 
       j=0;
       while(token != NULL && j < 4) {
        strncpy(tokens[j], token, 20);
        token = strtok(NULL, "-");
        j++;
       }
    
      if(tokens[0] == NULL || tokens[1] == NULL || tokens[2] == NULL || tokens[3] == NULL){
         printf("Error reading from serial port\n");
         return 0;
      }
       

     printf("Time:%ss | Port12:%sV | Port6:%sV | Port2:%sV\n", tokens[0], tokens[1], tokens[2], tokens[3]);
     fprintf(channel1, "Time: %ss Value: %sV\n", tokens[0], tokens[1]);
     fprintf(channel2, "Time: %ss Value: %sV\n", tokens[0], tokens[2]);
     fprintf(channel3, "Time: %ss Value: %sV\n", tokens[0], tokens[3]);
     
     times[i] = atof(tokens[0]);
     channel1_values[i] = atof(tokens[1]);
     channel2_values[i] = atof(tokens[2]);
     channel3_values[i] = atof(tokens[3]);

    }

    /* Draw the x-axis */
    gdImageLine(img1, 0, 550, 800, 550, axiscolor1);
    gdImageLine(img2, 0, 550, 800, 550, axiscolor2);
    gdImageLine(img3, 0, 550, 800, 550, axiscolor3);
    char axis_values[5];
   
    /* Draw the y-axis */
    gdImageLine(img1, 50, 0, 50, 600, axiscolor1);
    gdImageLine(img2, 50, 0, 50, 600, axiscolor2);
    gdImageLine(img3, 50, 0, 50, 600, axiscolor3);
    

    int idx = 50;
    for (int x = 0; x < 70; x+=10) {
      sprintf(axis_values, "%d", x);
      gdImageString(img1, gdFontGetSmall(), x+idx, 550 + 10, (unsigned char *)axis_values, axiscolor1);
      gdImageString(img2, gdFontGetSmall(), x+idx, 550 + 10 , (unsigned char *)axis_values, axiscolor2);
      gdImageString(img3, gdFontGetSmall(), x+idx, 550 + 10 , (unsigned char *)axis_values, axiscolor3);
      idx += (int)(750/6);
    }

    idx = 0;
    for(int y = 0; y < 6; y++){
        sprintf(axis_values, "%d", y);
        gdImageString(img1, gdFontGetSmall(), 50-10, 550 - idx, (unsigned char *)axis_values, axiscolor1);
        gdImageString(img2, gdFontGetSmall(), 50-10, 550 - idx, (unsigned char *)axis_values, axiscolor2);
        gdImageString(img3, gdFontGetSmall(), 50-10, 550 - idx, (unsigned char *)axis_values, axiscolor3);
        idx += (int)(550/5);
    }

    /* Draw the images */
    for(i = 1; i < n_values; i++){
      gdImageLine(img1, 50+times[i-1]*750/60, 550-channel1_values[i-1]*550/5, 50+times[i]*750/60, 550-channel1_values[i]*550/5, red);
      gdImageLine(img2, 50+times[i-1]*750/60, 550-channel2_values[i-1]*550/5, 50+times[i]*750/60, 550-channel2_values[i]*550/5, green);
      gdImageLine(img3, 50+times[i-1]*750/60, 550-channel3_values[i-1]*550/5, 50+times[i]*750/60, 550-channel3_values[i]*550/5, blue);
    }


   if(wave12 != NULL && wave6 != NULL && wave2 != NULL) {
        gdImagePng(img1, wave12);
        gdImagePng(img2, wave6);
        gdImagePng(img3, wave2);
        fclose(wave12);
        fclose(wave6);
        fclose(wave2);
    }

   /* Close files */
   fclose(data_stream);
   fclose(channel1);
   fclose(channel2);
   fclose(channel3);
   gdImageDestroy(img1);
   gdImageDestroy(img2);
   gdImageDestroy(img3);
   close(uart_fd);
}