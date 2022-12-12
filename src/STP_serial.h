#ifndef serial_h

#define serial_h

#ifndef RX_BUFFER_SIZE
  #define RX_BUFFER_SIZE 128
#endif
#ifndef TX_BUFFER_SIZE
  #define TX_BUFFER_SIZE 64
#endif


//Serial Communication Functions

int serial_read();

void parse_command();







#endif