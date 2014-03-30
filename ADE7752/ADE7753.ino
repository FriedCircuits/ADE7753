//registers on ADE7753  
#define WAVEFORM 0x01
#define AENERGY 0x02
#define RAENERGY 0x03
#define LAENERGY 0x04
#define VAENERGY 0x05
#define LVAENERGY 0x06
#define LVARENERGY 0x07
#define MODE 0x09
#define IRQEN 0x0A
#define STATUS 0x0B
#define RSTSTATUS 0x0C
#define CH1OS 0x0D
#define CH2OS 0x0E
#define GAIN 0x0F
#define PHCAL 0x10
#define APOS 0x11
#define WGAIN 0x12
#define WDIV 0x12
#define CFNUM 0x14
#define CFDEN 0x15
#define IRMS 0x16
#define VRMS 0x17
#define IRMSOS 0x18
#define VRMSOS 0x19
#define VAGAIN 0x1A
#define VADIV 0x1B
#define LINECYC 0x1C
#define ZXTOUT 0x1D
#define SAGCYC 0x1E
#define SAGLVL 0x1F
#define IPKLVL 0x20
#define VPKLVL 0x21
#define IPEAK 0x22
#define RSTIPEAK 0x23
#define VPEAK 0x24
#define RSTVPEAK 0x25
#define TEMP 0x26
#define PERIOD 0x27
#define TMODE 0x3D
#define CHKSUM 0x3E
#define DIEREV 0x3F

#define DATAOUT 11//MOSI
#define DATAIN  12//MISO 
#define SPICLOCK  13//sck
#define SLAVESELECT 10//ss

//opcodes
#define WREN  6
#define WRDI  4
#define RDSR  5
#define WRSR  1
#define READ  3
#define WRITE 2

//SPCR = (1<<SPE)|(1<<MSTR)|(1<<CPOL)|(1<<CPHA)|(1<<SPR1)|(1<<SPR0); //set clock rate to 1/16th system

byte eeprom_output_data;
byte multi_byte_data[3];
byte eeprom_input_data=0;
long long_eeprom_data = 0;
byte clr;
int address=0;
//data buffer
char buffer [128];

void fill_buffer()
{
  for (int I=0;I<128;I++)
  {
    buffer[I]=I;
  }
}

char spi_transfer(volatile char data)
{
  SPDR = data;                    // Start the transmission
  while (!(SPSR & (1<<SPIF)))     // Wait the end of the transmission
  {
  };
  return SPDR;                    // return the received byte
}

void setup()
{
  Serial.begin(9600);

  pinMode(DATAOUT, OUTPUT);
  pinMode(DATAIN, INPUT);
  pinMode(SPICLOCK,OUTPUT);
  pinMode(SLAVESELECT,OUTPUT);
  digitalWrite(SLAVESELECT,HIGH); //disable device
  SPCR = (1<<SPE)|(1<<MSTR)|(1<<CPHA)|(1<<SPR1)|(1<<SPR0);
  SPSR = (0<<SPI2X);
  clr=SPSR;
  clr=SPDR;
  delay(10);
  Serial.println("init complete");
  delay(1000);

  //testrun starts here

  //utils
  //read_eeprom(address value, how many bytes)
  //write_to_eeprom(target, values, bytes to write)

  //read what is there right now
  address = LINECYC;
 Serial.print(address,HEX);
 eeprom_output_data = read_eeprom(STATUS,2);

  long TestWrite;
  TestWrite = 0xABCD;
  Serial.println(TestWrite, BIN);
  write_to_eeprom(address, TestWrite, 2);
Serial.println(eeprom_output_data, BIN);
  eeprom_output_data = read_eeprom(address, 2);
  Serial.println("Completed basic read write test");
  
  delay(10000);

}

void write_to_eeprom(int EEPROM_address, long write_buffer, int bytes_to_write)
{
  //Serial.print("Multiwrite ops to addr>");
  //Serial.println(EEPROM_address, HEX);
  //set write mode
  byte make_write_cmd = B10000000;
  byte this_write = B00000000;
  EEPROM_address = EEPROM_address|make_write_cmd;
  digitalWrite(SLAVESELECT,LOW);
  spi_transfer((char)(EEPROM_address));      //send address

  //here there should be a t7 delay, however long that is
  for (int i = 0; i<bytes_to_write; i++){
  //Serial.println(i);
  this_write = byte(write_buffer>>(8*((bytes_to_write-1)-i)));
  //Serial.println(this_write, HEX);
  spi_transfer((char)(this_write));      //send data byte
  }
  digitalWrite(SLAVESELECT,HIGH); //release chip, signal end transfer
}

long read_eeprom(int EEPROM_address, int bytes_to_read)
{
  //Serial.print("Multi-read to addr>");
  //Serial.print(EEPROM_address, HEX);
  Serial.println(" Data starts:");
  long data = 0;
  byte reader_buf = 0;
  digitalWrite(SLAVESELECT,LOW);
  spi_transfer((char)(EEPROM_address));      //send LSByte address
  for (int i = 1; i <= bytes_to_read; i++){
    reader_buf = spi_transfer(0xFF); //get data byte
    Serial.println(i);
    Serial.println(reader_buf, BIN);

    data = data|reader_buf;
    if (i< bytes_to_read) {
      data = data<<8;
    }
    }
  Serial.print("completed. data was>");
  Serial.println(data, BIN);
  digitalWrite(SLAVESELECT,HIGH); //release chip, signal end transfer
  return data;
}

void loop()
{

    eeprom_output_data = read_eeprom(STATUS,2);
   Serial.println("STATUS CHECK");
   Serial.println(eeprom_output_data, BIN);
   Serial.println(eeprom_output_data, HEX);
   delay(1000);
   eeprom_output_data = read_eeprom(LINECYC,2);
   Serial.println("LINECYC CHECK");
   Serial.println(eeprom_output_data, BIN);
   Serial.println(eeprom_output_data, HEX);   

  delay(1000);
   eeprom_output_data = read_eeprom(TEMP,1);
   Serial.println("TEMP CHECK");
   Serial.println(eeprom_output_data, BIN);
   Serial.println(eeprom_output_data, HEX);   
 delay(5000);
}
