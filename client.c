#include <termios.h>

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
        printf("Error opening UART device\n");
        return -1;
    }
    
    ret = tcgettermios(uart_fd, &settigs);
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
   
   settings.c_cflag |= B19200 | CS8
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
   msg_len = write(uart_fd, msg, msg_len);

   printf("Sent %d bytes to serial port\n", msg_len);

   /* Operazione server */
}