#include"Ap_29demo.h"
//IO settings
int BUSY_Pin = A14; 
int RES_Pin = A15; 
int DC_Pin = A16; 
int CS_Pin = A17; 
int SCK_Pin = A18; 
int SDI_Pin = A19; 
#define EPD_W21_MOSI_0  digitalWrite(SDI_Pin,LOW)
#define EPD_W21_MOSI_1  digitalWrite(SDI_Pin,HIGH) 

#define EPD_W21_CLK_0 digitalWrite(SCK_Pin,LOW)
#define EPD_W21_CLK_1 digitalWrite(SCK_Pin,HIGH)

#define EPD_W21_CS_0 digitalWrite(CS_Pin,LOW)
#define EPD_W21_CS_1 digitalWrite(CS_Pin,HIGH)

#define EPD_W21_DC_0  digitalWrite(DC_Pin,LOW)
#define EPD_W21_DC_1  digitalWrite(DC_Pin,HIGH)
#define EPD_W21_RST_0 digitalWrite(RES_Pin,LOW)
#define EPD_W21_RST_1 digitalWrite(RES_Pin,HIGH)
#define isEPD_W21_BUSY digitalRead(BUSY_Pin)
////////FUNCTION//////
void SPI_Write(unsigned char value);
void EPD_W21_WriteDATA(unsigned char command);
void EPD_W21_WriteCMD(unsigned char command);
//EPD    // //send red data
    // send_command(0x13);
    // for (uint j = 0; j < height; j++) {
    //     for (uint i = 0; i < width; i++) {
    //         send_data(0xff);
    //     }
    // }
void EPD_init(void);
void PIC_display(const unsigned char* picData_old,const unsigned char* picData_new);
void EPD_sleep(void);
void EPD_refresh(void);
void lcd_chkstatus(void);
void PIC_display_Clean(void);
void EPD_partial_display_Color(unsigned int x_start,unsigned int y_start,const unsigned char *old_data,const unsigned char *new_data,unsigned int PART_COLUMN,unsigned int PART_LINE,unsigned char mode); //partial display 
//LUT
void EPD_init_LUT(void);

void setup() {
   pinMode(BUSY_Pin, INPUT); 
   pinMode(RES_Pin, OUTPUT);  
   pinMode(DC_Pin, OUTPUT);    
   pinMode(CS_Pin, OUTPUT);    
   pinMode(SCK_Pin, OUTPUT);    
   pinMode(SDI_Pin, OUTPUT);    
}
//Tips//
/*When the electronic paper is refreshed in full screen, the picture flicker is a normal phenomenon, and the main function is to clear the display afterimage in the previous picture.
  When the local refresh is performed, the screen does not flash.*/
/*When you need to transplant the driver, you only need to change the corresponding IO. The BUSY pin is the input mode and the others are the output mode. */

void loop() {
  while(1)
  {
    EPD_init(); //EPD init
    PIC_display(gImage_BW1,gImage_R1);//EPD_picture1
    EPD_sleep();//EPD_sleep,Sleep instruction is necessary, please do not delete!!!


    EPD_init_LUT(); //EPD init  LUT
    EPD_partial_display_Color(0,136,gImage_num[0],gImage_num[0],32,32,0);  //x,y,old_data,new_data,W,L,mode 
    EPD_partial_display_Color(32,136,gImage_num[1],gImage_num[1],32,32,0);  //x,y,old_data,new_data,W,L,mode 
    EPD_partial_display_Color(64,136,gImage_num[2],gImage_num[2],32,32,0);  //x,y,old_data,new_data,W,L,mode 

    EPD_partial_display_Color(0,136,gImage_num[0],gImage_num[1],32,32,1);  //x,y,old_data,new_data,W,L,mode 
    EPD_partial_display_Color(32,136,gImage_num[1],gImage_num[2],32,32,1);  //x,y,old_data,new_data,W,L,mode 
    EPD_partial_display_Color(64,136,gImage_num[2],gImage_num[3],32,32,1);  //x,y,old_data,new_data,W,L,mode  

    EPD_partial_display_Color(0,136,gImage_num[1],gImage_num[2],32,32,1);  //x,y,old_data,new_data,W,L,mode 
    EPD_partial_display_Color(32,136,gImage_num[2],gImage_num[3],32,32,1);  //x,y,old_data,new_data,W,L,mode 
    EPD_partial_display_Color(64,136,gImage_num[3],gImage_num[4],32,32,1);  //x,y,old_data,new_data,W,L,mode   

    EPD_sleep();//EPD_sleep,Sleep instruction is necessary, please do not delete!!!
    delay(1000);  
    //EPD_Clean
    EPD_init(); //EPD init
    PIC_display_Clean();//EPD_Clean 
    EPD_sleep();//EPD_sleep,Sleep instruction is necessary, please do not delete!!!
    while(1);
  }
}



//////////////////////SPI///////////////////////////////////
void SPI_Write(unsigned char value)                                    
{                                                           
    unsigned char i;
    for(i=0; i<8; i++)   
    {
        EPD_W21_CLK_0;
        if(value & 0x80)
          EPD_W21_MOSI_1;
        else
          EPD_W21_MOSI_0;   
        value = (value << 1); 
        EPD_W21_CLK_1; 
    }
}

void EPD_W21_WriteCMD(unsigned char command)
{
  EPD_W21_CS_0;                   
  EPD_W21_DC_0;   // command write
  SPI_Write(command);
  EPD_W21_CS_1;
}
void EPD_W21_WriteDATA(unsigned char data)
{
  EPD_W21_CS_0;                   
  EPD_W21_DC_1;   // command write
  SPI_Write(data);
  EPD_W21_CS_1;
}



/////////////////EPD settings Functions/////////////////////
void EPD_W21_Init(void)
{
  EPD_W21_RST_0;    // Module reset
  delay(10); //At least 10ms
  EPD_W21_RST_1;
  delay(10);

  EPD_W21_RST_0;    // Module reset
  delay(10); //At least 10ms
  EPD_W21_RST_1;
  delay(10);  

  EPD_W21_RST_0;    // Module reset
  delay(10); //At least 10ms
  EPD_W21_RST_1;
  delay(10);  
}

void EPD_init(void)
{
    EPD_W21_Init(); //Electronic paper IC reset
  
    EPD_W21_WriteCMD(0x04);  
    lcd_chkstatus();//waiting for the electronic paper IC to release the idle signal

    EPD_W21_WriteCMD(0x00);     //panel setting
    EPD_W21_WriteDATA(0x0f);    //LUT from OTP£¬128x296
    EPD_W21_WriteDATA(0x89);    //Temperature sensor, boost and other related timing settings

    EPD_W21_WriteCMD(0x61);     //resolution setting
    EPD_W21_WriteDATA (0x80);     //128      
    EPD_W21_WriteDATA (0x01);     //296
    EPD_W21_WriteDATA (0x28);
    
    EPD_W21_WriteCMD(0X50);     //VCOM AND DATA INTERVAL SETTING      
    EPD_W21_WriteDATA(0x77);    //WBmode:VBDF 17|D7 VBDW 97 VBDB 57   WBRmode:VBDF F7 VBDW 77 VBDB 37  VBDR B7
}

void EPD_refresh(void)
{
    EPD_W21_WriteCMD(0x12);     //DISPLAY REFRESH   
    delay(100);          //!!!The delay here is necessary, 200uS at least!!!     
    lcd_chkstatus();
} 
void EPD_sleep(void)
{
    EPD_W21_WriteCMD(0X50);  //VCOM AND DATA INTERVAL SETTING  
    EPD_W21_WriteDATA(0xf7);     
    EPD_W21_WriteCMD(0X02);   //power off
    lcd_chkstatus();
    EPD_W21_WriteCMD(0X07);   //deep sleep
    EPD_W21_WriteDATA(0xA5);
}


void PIC_display(const unsigned char* picData_old,const unsigned char* picData_new)
{
    unsigned int i;
    EPD_W21_WriteCMD(0x10);        //Transfer old data
    for(i=0;i<4736;i++)      
  {
    EPD_W21_WriteDATA(pgm_read_byte(&picData_old[i]));
  }
  
    EPD_W21_WriteCMD(0x13);        //Transfer new data
    for(i=0;i<4736;i++)      
  {
    EPD_W21_WriteDATA(pgm_read_byte(&picData_new[i]));
  }
  EPD_refresh();
}
void PIC_display_Clean(void)
{
    unsigned int i;
    EPD_W21_WriteCMD(0x10);        //Transfer old data
    for(i=0;i<4736;i++)      
  {
    EPD_W21_WriteDATA(0xff);
  }
  
    EPD_W21_WriteCMD(0x13);        //Transfer new data
    for(i=0;i<4736;i++)      
  {
    EPD_W21_WriteDATA(0xff);
  }
   EPD_refresh();
}
void lcd_chkstatus(void)
{
  unsigned char busy;
  do
  {
    EPD_W21_WriteCMD(0x71);
    busy = isEPD_W21_BUSY;
    busy =!(busy & 0x01);        
  }
  while(busy);   
  delay(10);                       
}










/***************************partial display function*************************************/
/////////////////LUT
/////////////////////////////////////partial screen update LUT///////////////////////////////////////////
const unsigned char lut_vcom1[] PROGMEM={
0x00  ,0x1F ,0x01 ,0x00 ,0x00 ,0x01,
0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,
0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,
0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,
0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,
0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,
0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00
  ,0x00 ,0x00,          };
const unsigned char lut_ww1[] PROGMEM={
0x00  ,0x1F ,0x01 ,0x00 ,0x00 ,0x01,
0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,
0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,
0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,
0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,
0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,
0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,};
const unsigned char lut_bw1[] PROGMEM={
0x80  ,0x1F ,0x01 ,0x00 ,0x00 ,0x01,  
0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,  
0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,  
0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,  
0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,  
0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,  
0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,  };
const unsigned char lut_wb1[] PROGMEM={
0x40  ,0x1F ,0x01 ,0x00 ,0x00 ,0x01,  
0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,  
0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,  
0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,  
0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,  
0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,  
0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,  };
const unsigned char lut_bb1[] PROGMEM={
0x00  ,0x1F ,0x01 ,0x00 ,0x00 ,0x01,  
0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,  
0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,  
0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,  
0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,  
0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,  
0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,  };

void lut1(void)
{
  unsigned int count;
  EPD_W21_WriteCMD(0x20);
  for(count=0;count<44;count++)      
    {EPD_W21_WriteDATA(pgm_read_byte(&lut_vcom1[count]));}

  EPD_W21_WriteCMD(0x21);
  for(count=0;count<42;count++)      
    {EPD_W21_WriteDATA(pgm_read_byte(&lut_ww1[count]));}   
  
  EPD_W21_WriteCMD(0x22);
  for(count=0;count<42;count++)      
    {EPD_W21_WriteDATA(pgm_read_byte(&lut_bw1[count]));} 

  EPD_W21_WriteCMD(0x23);
  for(count=0;count<42;count++)      
    {EPD_W21_WriteDATA(pgm_read_byte(&lut_wb1[count]));} 

  EPD_W21_WriteCMD(0x24);
  for(count=0;count<42;count++)      
    {EPD_W21_WriteDATA(pgm_read_byte(&lut_bb1[count]));}   
}

void EPD_init_LUT(void)
{
    EPD_W21_Init();
    EPD_W21_WriteCMD(0x01);     //POWER SETTING
    EPD_W21_WriteDATA (0x03);
    EPD_W21_WriteDATA (0x00);      
    EPD_W21_WriteDATA (0x2b);                                  
    EPD_W21_WriteDATA (0x2b);   
    EPD_W21_WriteDATA (0x13);

    EPD_W21_WriteCMD(0x06);         //booster soft start
    EPD_W21_WriteDATA (0x17);   //A
    EPD_W21_WriteDATA (0x17);   //B
    EPD_W21_WriteDATA (0x17);   //C 
    
    EPD_W21_WriteCMD(0x04);
    lcd_chkstatus();
    
    EPD_W21_WriteCMD(0x00);     //panel setting
    EPD_W21_WriteDATA(0xbf);    //KW-bf   KWR-2F  BWROTP 0f BWOTP 1f
    
    EPD_W21_WriteCMD(0x30);     //PLL setting
    EPD_W21_WriteDATA (0x3c);       //100hz 
    
    EPD_W21_WriteCMD(0x61);     //resolution setting
    EPD_W21_WriteDATA (0x80);     //128      
    EPD_W21_WriteDATA (0x01);     //296
    EPD_W21_WriteDATA (0x28);

    EPD_W21_WriteCMD(0x82);     //vcom_DC setting
    EPD_W21_WriteDATA (0x12);

    EPD_W21_WriteCMD(0X50);     //VCOM AND DATA INTERVAL SETTING      
    EPD_W21_WriteDATA(0x97); //VBDF 17|D7 VBDW 97 VBDB 57   VBDF F7 VBDW 77 VBDB 37  VBDR B7
   lut1(); //Power settings

  }
void EPD_partial_display_Color(unsigned int x_start,unsigned int y_start,const unsigned char *old_data,const unsigned char *new_data,unsigned int PART_COLUMN,unsigned int PART_LINE,unsigned char mode) //partial display 
{
  unsigned int i,count;  
  unsigned int x_end,y_start1,y_start2,y_end1,y_end2;
  x_start=x_start;
  x_end=x_start+PART_LINE-1; 
  
  y_start1=0;
  y_start2=y_start;
  if(y_start>=256)
  {
    y_start1=y_start2/256;
    y_start2=y_start2%256;
  }
  y_end1=0;
  y_end2=y_start+PART_COLUMN-1;
  if(y_end2>=256)
  {
    y_end1=y_end2/256;
    y_end2=y_end2%256;    
  }   
  
count=PART_COLUMN*PART_LINE/8;
  
    EPD_W21_WriteCMD(0x82);     //vcom_DC setting   
    EPD_W21_WriteDATA (0x08); 
    EPD_W21_WriteCMD(0X50);
    EPD_W21_WriteDATA(0x47);    

    EPD_W21_WriteCMD(0x91);   //This command makes the display enter partial mode
    EPD_W21_WriteCMD(0x90);   //resolution setting
    EPD_W21_WriteDATA (x_start);   //x-start     
    EPD_W21_WriteDATA (x_end);   //x-end  

    EPD_W21_WriteDATA (y_start1);
    EPD_W21_WriteDATA (y_start2);   //y-start    
    
    EPD_W21_WriteDATA (y_end1);   
    EPD_W21_WriteDATA (y_end2);  //y-end
    EPD_W21_WriteDATA (0x28); 

    EPD_W21_WriteCMD(0x10);        //writes Old data to SRAM for programming
 
  if(mode==0)
  for(i=0;i<count;i++)       
  {
   EPD_W21_WriteDATA(0x00);  
  }
  else
   for(i=0;i<count;i++)      
  {
   EPD_W21_WriteDATA(~pgm_read_byte(&old_data[i]));  
  }  
  
  EPD_W21_WriteCMD(0x13);        //writes New data to SRAM.
  if(mode!=2) //new  datas
  {
    for(i=0;i<count;i++)       
   {
    EPD_W21_WriteDATA(~pgm_read_byte(&new_data[i]));  
   }
  }
  else  //white
  {
    for(i=0;i<count;i++)       
    {
     EPD_W21_WriteDATA(0x00);  
    }   
  }   
      
    EPD_W21_WriteCMD(0x12);    //DISPLAY REFRESH                 
    delay(1);     //!!!The delay here is necessary, 200uS at least!!!     
    lcd_chkstatus();
    
}
//////////////////////////////////END/////////////////////////////////////////

