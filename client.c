#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <termios.h>
#include <gd.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#define VALUES 600

int main(){

    //*****************Initialize integer variables*****************//
    int uart_fd, ret, msg_len;
    //*****************Inizialize float variables*****************//
    float user_input = 0.0;
    //*****************Initialize char variables*****************//
    char msg[1024];
    char sampling_time[1024];

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
        perror("Error opening UART device");
        return -1;
    }
    
    ret = tcgetattr(uart_fd, &settings);
    if(ret != 0){
        printf("Error getting termios settings\n");
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
   
   settings.c_cflag |= B19200 | CS8;
   ret = tcsetattr(uart_fd, TCSANOW, &settings);
   if(ret != 0){
       printf("Error settings termios settings %i %s\n", ret, strerror(errno));
       return 1;
   }

   /* Write to the serial port */
   
   printf("Enter sampling time in ms:");
   scanf("%f", &user_input);

   /* Convert float to string for transmission to the serial port */
   gcvt(user_input, 3, msg);
   strcat(msg, "\n");
   strcpy(sampling_time, msg);
   
   /* Write to the serial port */
   msg_len = strlen(msg);
   ret = write(uart_fd, msg, msg_len);

   printf("Sent %d bytes to serial port\n", ret);
   
   sleep(10);
   
   
   FILE* data_stream, *channel1, *channel2, *channel3;
   FILE *wave1, *wave2, *wave3;
   char tokens[4][20];
   char* token;
   int i = 0;
   int n_values;
   float channel1_values[VALUES];
   float channel2_values[VALUES];
   float channel3_values[VALUES];

   /* Create images for each channel */ 
    gdImagePtr img1 = gdImageCreate(800, 600);
    gdImagePtr img2 = gdImageCreate(800, 600);
    gdImagePtr img3 = gdImageCreate(800, 600);

    /* Allocate some colors */
    int white = gdImageColorAllocate(img1, 255, 255, 255);
    int black = gdImageColorAllocate(img1, 0, 0, 0);
    gdImageColorAllocate(img2, 255, 255, 255);
    gdImageColorAllocate(img2, 0, 0, 0);
    gdImageColorAllocate(img3, 255, 255, 255);
    gdImageColorAllocate(img3, 0, 0, 0);

   /* Open files */
   data_stream = fopen("data.txt", "w+");
   if(data_stream == NULL){
       printf("Error opening file data.txt\n");
       return 0;
   }

   channel1 = fopen("channel1.txt", "w");
   if(channel1 == NULL){
      printf ("Error opening file channel1.txt\n");
      return 0;
   }

   channel2 = fopen("channel2.txt", "w");
   if(channel2 == NULL){
      printf ("Error opening file channel2.txt\n");
      return 0;
   }

   channel3 = fopen("channel3.txt", "w");
   if(channel3 == NULL){
      printf ("Error opening file channel3.txt\n");
      return 0;
   }
   
   wave1 = fopen("wave1.png", "w");
   if(wave1 == NULL){
      printf("Error opening file wave1.png\n");
      return 0;
   }

   wave2 = fopen("wave2.png", "w");
   if(wave2 == NULL){
      printf("Error opening file wave2.png\n");
      return 0;
   }

   wave3 = fopen("wave3.png", "w");
   if(wave3 == NULL){
      printf("Error opening file wave3.png\n");
      return 0;
   }

   for(n_values=0 ;n_values < VALUES; n_values++){

     ret = read(uart_fd, msg, 1024);
     if(ret < 0){
        printf("Error reading from serial port\n");
        break;
     }
     
     printf("Recived %d bytes from seril port\n", ret);

     /* Get the first token */
     token = strtok(msg, "-");

     /* Walk through other tokens */
     while(token != NULL && i < 4) {
        strncpy(tokens[i], token, 20);
        token = strtok(NULL, "-");
        i++;
     }

    
     if(tokens[0] == NULL || tokens[1] == NULL || tokens[2] == NULL || tokens[3] == NULL){
         printf("Error reading from serial port\n");
         return 0;
     } 

     fprintf(data_stream, "%s %s %s %s\n", tokens[0], tokens[1], tokens[2], tokens[3]);
     fprintf(channel1, "%s %s\n", tokens[0], tokens[1]);
     fprintf(channel2, "%s %s\n", tokens[0], tokens[2]);
     fprintf(channel3, "%s %s\n", tokens[0], tokens[3]);

     channel1_values[n_values] = atof(tokens[1]);
     channel2_values[n_values] = atof(tokens[2]);
     channel3_values[n_values] = atof(tokens[3]);
  }

   /* Draw the images */
   for(i = 1; i < n_values; i++) {
        gdImageLine(img1, (i - 1) * 800 / VALUES, 600 - channel1_values[i - 1] * 600, i * 800 / VALUES, 600 - channel1_values[i] * 600, black);
        gdImageLine(img2, (i - 1) * 800 / VALUES, 600 - channel2_values[i - 1] * 600, i * 800 / VALUES, 600 - channel2_values[i] * 600, black);
        gdImageLine(img3, (i - 1) * 800 / VALUES, 600 - channel3_values[i - 1] * 600, i * 800 / VALUES, 600 - channel3_values[i] * 600, black);
    }

   if(wave1 != NULL && wave2 != NULL && wave3 != NULL) {
        gdImagePng(img1, wave1);
        gdImagePng(img2, wave2);
        gdImagePng(img3, wave3);
        fclose(wave1);
        fclose(wave2);
        fclose(wave3);
    }
   
   fclose(data_stream);
   fclose(channel1);
   fclose(channel2);
   fclose(channel3);
}