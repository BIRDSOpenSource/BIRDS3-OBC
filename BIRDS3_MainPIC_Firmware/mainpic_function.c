//caution
//port of kbhit in CAM Mission is PC--
//TRANSFER_DATA_NBYTE_TOPC_SCF(65536,65536); is comment out--
//adcs address test make32 function--
//
//reset battery voltage 0x87, over 0x87 is nomal mode
//reset 

static int16 currenttime = 0;

void GIVE_ACCESS_SCF_Nsec(int8 min)
{
   fprintf(PC,"give access to COM\r\n");
   fputc(0x35,COM);
   output_high(PIN_C4);
   if(min > 5)//for accidentaly send the long time delay, maximum should be 5 min
   {
      min = 5;
   }
   for(int16 i = 0; i < min * 60; i++)
   {
      delay_ms(1000); //wait 60 sec
      if(in_bffr_main[4] == 0xab || CMD_FROM_PC[0] == 0xab)
      {
         CMD_FROM_PC[0] = 0;
         CMD_FROM_PC[1] = 0;
         PC_DATA = 0;
         break;
      }
   }
   fprintf(PC,"Back to Nomal Operation\r\n");
   return;
}

//--------MAIN PIC Buffer------------------------------------------------------
void SerialDataReceive() //get buffer data one by one
{
   int num = 0;
   for(num = 0 ;num < 16; num++)
   {
    in_bffr_main[num] = fgetc(com);
   }     
   return;
}

void DELETE_CMD_FROM_PC()
{
   for(int num = 0; num < 6; num++)
   {
      CMD_FROM_PC[num] = 0;
   }
   return;
}

void Delete_Buffer() //delete com command buffer
{
   int num = 0;
   for(num = 0;num < 16; num++)
   {
    in_bffr_main[num] = 0x00;
   }
   COM_DATA = 0;
   return;
}

void Transfer_Buffer(int PORT_NUM) //get buffer data one by one
                                   //1:PC 2:COM 3:RESET
{
   int num = 0;
   switch(PORT_NUM)
   {
      case 1:
         for(;num < 16; num++)
         {
         fputc(in_bffr_main[num],PC);
         }
         break;
      case 2:
         for(;num < 16; num++)
         {
         fputc(in_bffr_main[num],COM);
         }
         break;
      case 3:
         for(;num < 16; num++)
         {
         fputc(in_bffr_main[num],RESET);
         }
         break;
   }
   return;
}

void TRANSFER_Buffer_OF(unsigned INT32 TO_ADRESS) //for testing
{
   for (int i=0 ; i < 16 ; i++)
   {
      WRITE_DATA_BYTE_OF(TO_ADRESS, in_bffr_main[i]);
      TO_ADRESS++;
   }
   
   return;
}

//void

//--------BC Function----------------------------------------------------------
//unsigned int8 BC_temp_data[2] = {};
unsigned int16 BC_temp_data_h = 0;
unsigned int16 BC_temp_data_l = 0;
unsigned int16 BC_TEMP = 0;
float  temp = 0;
float initial_temp = 0;
float MAXTEMP = 0;
int16 UNLEG2 = 0;


void BC_SETUP()
{
   ANCON2= 0x01; // AN9 as analog, all other pins are digital
   ADCON1L = 0x00; // SAMP bit = 0 ends sampling and starts converting
   ADCHS0L = 0x09; // Connect AN9 as S/H+ input
   // AN9 is the input
   ADCON2H = 0x00;
   ADRC = 1;
   ADCSS0L = 0;
   ADCON3L = 0x02; // Manual Sample, Tad = 3Tcy
      //setup_adc(ADC_CLOCK_DIV_32); //VSS_VDD
   ADCON2L = 0;
   return;
}

void BC_READ_TO_PC()
{
   ADON = 1;
   SAMP = 1;
   delay_ms(100);
   
   SAMP = 0; // start converting
   while (!DONE){};
   delay_ms(100);
   //BC_temp_data[0] = ADCBUF0H;//read_adc();
   //BC_temp_data[1] = ADCBUF0L;
   
   BC_temp_data_h = ADCBUF0H;
   BC_temp_data_l = ADCBUF0L;
   //temp = ((data_h << 8 ) | data_l);
   fprintf(PC,"data_l:%x \r\n", BC_temp_data_l);
   fprintf(PC,"data_h:%x \r\n", BC_temp_data_h);
   BC_TEMP = BC_temp_data_h | BC_temp_data_l;
   //fprintf(PC,"temp:%f\r\n",temp);
   temp = BC_TEMP/1024*3.31*100-50;    //VDD: 3.31V(MEASURED)
   //fprintf(PC,"%1.1f \r\n", temp);
   return;
}

void CHECK_BC_TEMP()
{
   BC_SETUP();
   ADON = 1;
   SAMP = 1;
   delay_ms(10);
   
   SAMP = 0; // start converting
   while (!DONE){};
   delay_ms(10);
   BC_temp_data_h = ADCBUF0H;//read_adc();
   BC_temp_data_l = ADCBUF0L;
   //fprintf(PC,"%x%x\r\n",ADCBUF0H,BC_temp_data_l);
   BC_temp = BC_temp_data_h << 8 | BC_temp_data_l;
   //fprintf(PC,"HEX : %x%x\r\n",BC_temp>>8,BC_temp);

   temp = BC_temp;//float
   temp = temp/1024*3.25*100-50;
   fprintf(PC,"Current Temp: %1.1f\r\n",temp);
   
   if(MAXTEMP < temp)
   {
      MAXTEMP = temp;
   }
   fprintf(PC,"%f\r\n",temp);
   return;
}

void MEASURE_BC_TEMP()
{
   BC_SETUP();
   ADON = 1;
   SAMP = 1;
   delay_ms(1);
   
   SAMP = 0; // start converting
   while (!DONE){};
   delay_ms(1);
   BC_temp_data_h = ADCBUF0H;//read_adc();
   BC_temp_data_l = ADCBUF0L;

   return;
}

void Turn_ON_BC()
{
   output_high(PIN_D5); //switch signal on
   return;
}

void Turn_OFF_BC()
{
   output_low(PIN_D5); //switch signal off
   return;
}

void BC_OPERATION()
{
   CHECK_BC_TEMP();
   Turn_ON_BC();
   fprintf(PC,"Turned ON BC\r\n");
   currenttime = 0;
   int8 counter = 0;
   while(currenttime < 30)
   {
      for(int i = 0; i < 10; i++)
      {
         COLLECT_RESET_DATA();
         if(reset_bffr[0] == 0x8e)
         {
            for(i = 0; i < 10; i++);
            {
               fprintf(PC,"%x,",reset_bffr[i]);
            }
            fprintf(PC,"%x\r\n",reset_bffr[10]);
            break;
         }
      }
      CHECK_BC_TEMP();
      UNLEG2 = UNLEG2 + RESET_bffr[10]; //add the unleg2 current every second
      counter++;
      delay_ms(900);
   }
      
   Turn_OFF_BC();
   delay_ms(500);
   CHECK_BC_TEMP();
   if(MAXTEMP-initial_temp > 5)
   {
      ANT_DEP_STATUS = 1;
   }
   fprintf(PC,"MAXIMUM TEMPERATURE is %1.1f\r\n\r\n",MAXTEMP);
   MAXTEMP = 0;
   return;
}

void CLEAR_BC_FLAG()
{
   BC_ATTEMPT_FLAG = 0;
   fprintf(PC,"\r\nFlag delete done\r\n");
   return;
}

void MAKE_BC_FLAG_1()
{
   BC_ATTEMPT_FLAG = 1;
   fprintf(PC,"\r\nFlag:1\r\n");
   return;
}

void MAKE_BC_FLAG_2()
{
   BC_ATTEMPT_FLAG = 2;
   fprintf(PC,"\r\nFlag:2\r\n");
   return;
}

void MAKE_BC_FLAG_3()
{
   BC_ATTEMPT_FLAG = 3;
   fprintf(PC,"\r\nFlag:3\r\n");
   return;
}

void MAKE_BC_FLAG_4()
{
   BC_ATTEMPT_FLAG = 4;
   fprintf(PC,"\r\nFlag:4\r\n");
   return;
}

void Antenna_Deploy()
{
   fprintf(PC,"%x\r\n",BC_ATTEMPT_FLAG);
   
   if(BC_ATTEMPT_FLAG < 4 && BC_ATTEMPT_FLAG != 0)
   {
      //SAVE_SAT_LOG(0xBC,BC_ATTEMPT_FLAG,BC_ATTEMPT_FLAG);
      fprintf(PC,"send command to RESET PIC\r\n");
      for(int num = 0; num < 50; num++)
      {
         fputc(0xBC,reset);
         delay_ms(10);
      }
      delay_ms(1000);
      if(reset_bffr[0] == 0xCB)
      {
         RESET_DATA = 0;
         BC_OPERATION();//antenna deployment
         BC_ATTEMPT_FLAG++;
         STORE_FLAG_INFO();
         WRITE_FLAG_to_EEPROM();
         STORE_ADRESS_DATA_TO_FLASH();
         reset_bffr[0] = 0;
         CMD_FROM_PC[1] = 0;
         RESET_DATA = 0;
         delay_ms(1000);
         delay_ms(20000);//wait until RESET goes back to nomal loop
         SAVE_SAT_LOG(0xBC,BC_ATTEMPT_FLAG,BC_ATTEMPT_FLAG);
      }
   }
   return;
}


//--------FAB HK collection----------------------------------------------------
#define HK_size 124  //HK FORMAT ARRAY SIZE
#define CW_size 5  //CW FORMAT ARRAY SIZE
#define HIGH_SAMP_HK_size 124 //High Sampling HK FORMAT ARRAY SIZE
#define FAB_SENSOR_size 45 //HK FAB Part

static unsigned int8 CW_FORMAT[CW_size] = {};
unsigned int8 in_HK[FAB_SENSOR_size] = {};
unsigned int8 HKDATA[HK_size] ={};
unsigned int8 ACK_for_COM[24] = {0xAA,0,0,0,0,0,0,0,0,0,
                                 0,0,0,0,0,0,0,0,0,0,
                                 0,0,0,0xBB};
//int8 in_High_HK[HIGH_SAMP_HK_size] = {};
BYTE FAB_DATA = 0;
//int t;
static int8 FAB_MEASUERING_FLAG= 0;
static int8 HIGH_SAMP_FAB_MEASUERING_FLAG = 0;
int32 FAB_FLAG = 0;
int8 CHECK_FAB_RESPONSE = 0;

#define buffer_from_FAB  (in_bffr_main[0]==0x33)

void REFLESH_CW_ACK_for_COM()
{
   for(int i = 1; i < 7; i++)
   {
      ACK_for_COM[i] = 0x00;
   }
   return;
}

void REFLESH_MSN_ACK_for_COM()
{
   for(int i = 12; i < 23; i++)
   {
      ACK_for_COM[i] = 0x00;
   }
   return;
}

void CW_RESPOND()
{
   REFLESH_CW_ACK_for_COM();
   delay_ms(100);
   ACK_for_COM[0] = 0xAA;
   ACK_for_COM[1] = 0x50;
   ACK_for_COM[2] = CW_FORMAT[0];
   ACK_for_COM[3] = CW_FORMAT[1];
   ACK_for_COM[4] = CW_FORMAT[2];
   ACK_for_COM[5] = CW_FORMAT[3];
   ACK_for_COM[6] = CW_FORMAT[4];
   ACK_for_COM[23] = 0xBB;
   for(int a = 0; a < 24; a++)//send the latest CW data to COM PIC
   {
      fputc(ACK_for_COM[a],COM);
   }
   fprintf(PC,"\r\nANS:");
   for(a = 0; a < 23; a++)//send the latest CW data to COM PIC
   {
      fprintf(PC,"%x,",ACK_for_COM[a]);
   }
   fprintf(PC,"%x\r\n",ACK_for_COM[23]);
   return;
}

void CHECK_50_and_CW_RESPOND()
{
   if(in_bffr_main[4] == 0x50)
   {
      CW_RESPOND();
      Delete_Buffer();
      COM_DATA = 0;
   }
   return;
}

//--------ADCS MISSION---------------------------------------------------------
//////////////////////////////////////////////////
//ADCS MISSION ID: 0x45                         //
//ADCS MISSION DATA COLLECT: 55                 //
//ADCS SENSOR SAMPLING: 0x34                    //  
//ADCS ACK: 0x55                                //
//ADCS Finish Storing: 0x67                     //
//////////////////////////////////////////////////

void Turn_On_ADCS()
{
   output_high(PIN_D6);
   return;
}

void Turn_Off_ADCS()
{
   output_low(PIN_D6);
   return;
}

void Send_Command_to_ADCS(int8 data) //testing for EM integration
{
   fputc(data,ADCS);
   return;
}

void DELETE_ADCS_SENSOR()
{
   for(int a = 0; a < ADCS_SENSOR_SIZE; a++)
   {
      ADCS_SENSOR_DATA[a] = 0;
   }
   return;
}

void ADCS_SETTINGS()//initiallize
{
   output_high(PIN_C4);
   Delete_Buffer();
   DELETE_ADCS_SENSOR();
   return;
}

void DISPLAY_ADCS_SENSOR_TO_PC()//for testing
{
   for(int b = 0; b < ADCS_SENSOR_SIZE; b++)//for checking
   {
      fputc(ADCS_SENSOR_DATA[b],PC);
   }
   return;
}

void Get_ADCS_MISSION_Data()
{
   
   Delete_ADCS_SENSOR(); //initialize
   CHECK_50_and_CW_RESPOND();
   fputc(0x55,ADCS);
   fputc(0x55,PC);
   int i = 0;
   for(int32 num = 0; num < 1000000 ;num++)
   {
      if(kbhit(ADCS) && fgetc(ADCS) == 0x67)
      {
         for(num = 0; num < 10000; num++)
         {
            if(Kbhit(ADCS))
            {
               ADCS_SENSOR_DATA[i] = fgetc(ADCS);
               i++;
               if(i == 60)
               {
                  break;
               }
            }
         }
         ADCS_ACK = 0x67;
         break;
      }
   }
   if(num == 1000000)
   {
      fprintf(PC,"NO DATA\r\n");
   }
   CHECK_50_and_CW_RESPOND();
   return;
}

int8 compare_before_after(int32 beginning, int32 end)
{
   int8 data[12] = {};
   for(int i = 0; i < 6; i++)
   {
      data[i] = READ_DATA_BYTE_OF(beginning+222+10+i);
   }
   for(i = 0; i < 6; i++)
   {
      data[i+6] = READ_DATA_BYTE_OF(end-101+i);
   }
   signed int16 Bx = make16(data[0],data[1]);
   signed int16 By = make16(data[2],data[3]);
   signed int16 Bz = make16(data[4],data[5]);
   
   signed int16 Ax = make16(data[6],data[7]);
   signed int16 Ay = make16(data[8],data[9]);
   signed int16 Az = make16(data[10],data[11]);
   
   signed int16 Total_B = Bx + By + Bz;
   signed int16 Total_A = Ax + Ay + Az;
   Total_B = abs(Total_B);
   Total_A = abs(Total_A);
   fprintf(PC,"%lx,%lx,%lx\r\n",Bx,By,Bz);
   fprintf(PC,"%lx,%lx,%lx\r\n",Ax,Ay,Az);
   fprintf(PC,"%lud,%lud\r\n",Total_B,Total_A);
   if(Total_B >= Total_A)
   {
      return 1;//declease --> continue
   }else{
      return 0;//inclease --> stop
   }
}

void GET_ADCS_SENSOR_DATA()//after that, method will changed (ADCS make format and just send to MAIN PIC)
{
   CHECK_50_and_CW_RESPOND();
   fputc(0x34,adcs);//send command (just collect sensor)
   //delay_ms(700);
   ADCS_ACK = 0;
   for(int32 num = 0; num < 1000000; num++)
   {
      if(kbhit(ADCS))
      {
         ADCS_ACK = fgetc(ADCS);
         //fprintf(PC,"GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG\r\n");
         break;
      }
      
   }
   int8 counter = 0;
   if(ADCS_ACK == 0x55)//acknowledge
   {
      for(int32 num = 0; num < 1000000; num++)
      {
         if(kbhit(ADCS))
         {
            ADCS_SENSOR_DATA[counter] = fgetc(ADCS);
            counter++;
            if(counter == 61)
            {
               break;
            }
         }
      }
   }
   CHECK_50_and_CW_RESPOND();
   if(ADCS_SENSOR_DATA[0] == 0x57)
   {
      fprintf(PC,"GET ADCS DATA\r\n");   
   }else{
      fprintf(PC,"Data was wrong\r\n");
      ADCS_ACK_COMMING = 0;
      ADCS_ACK = 0;
   }
   return;
}


void Delete_in_HK()
{
   int num;
   for(num = 0; num < HK_size; num++)
   {
      in_HK[num] = 0;
   }
   return;
}
//////////////////////////////////////
void Delete_HKDATA()
{
   int num;
   for(num = 0; num < HK_size; num++)
   {
      HKDATA[num] = 0;
   }
   return;
}

void Delete_CWFORMAT()
{
   for(int num = 0; num < CW_size; num++)
   {
      CW_FORMAT[num] = 0;
   }
   return;
}

int8 CHECK_SUNSHINE(int8 current)
{
   if(current > EX_PANEL_THRESHHOLD)
   {
      return 1;
   }else{
      return 0;
   }
}

int8 CONVERT_16bit_GYRO_to_8bit_X(int8 HIGH_8bit, int8 LOW_8bit)//caribrated for X
{
   signed int16 row_data = make16(HIGH_8bit, LOW_8bit);
   float ang_v = row_data*0.00875-0.11853;
   fprintf(PC,"ang:%.4f\r\n",ang_v);
   return (int8)ang_v; 
}

int8 CONVERT_16bit_GYRO_to_8bit_Y(int8 HIGH_8bit, int8 LOW_8bit)//caribrated for Y
{
   signed int16 row_data = make16(HIGH_8bit, LOW_8bit);
   float ang_v = row_data*0.00875-0.24552;
   fprintf(PC,"ang:%.4f\r\n",ang_v);
   return (int8)ang_v; 
}

int8 CONVERT_16bit_GYRO_to_8bit_Z(int8 HIGH_8bit, int8 LOW_8bit)//caribrated for Z
{
   signed int16 row_data = make16(HIGH_8bit, LOW_8bit);
   float ang_v = row_data*0.00875-0.40486;
   fprintf(PC,"ang:%.4f\r\n",ang_v);
   return (int8)ang_v; 
}

int8 OPERATION_MODE_VALUE(int8 Bat_Vol)
{//0x94HKDATA[116] 
   if(Bat_Vol > 0x94){//more than 3.8
      OPERATION_MODE = 0x11;
      return 96;
   }else if((Bat_Vol > 0x88) && (Bat_Vol <= 0x94)){//3.5 to 3.8
      OPERATION_MODE = 0x10;
      return 64;
   }else if(Bat_Vol <= 0x88){//less than 3.5
      OPERATION_MODE = 0x00;
      return 16;
   }
}

void MAKE_CW1_FORMAT()//when you inprement, go to line
{
   Delete_CWFORMAT();
   //CHECK_50_and_CW_RESPOND();
   CW_FORMAT[0] = HKDATA[44];//battery volatge
   CW_FORMAT[1] = HKDATA[45]<<4|HKDATA[46]>>4;//battery current
   CW_FORMAT[2] = HKDATA[47];//battery temp
   
//   CW_FORMAT[3] = CW_FORMAT[3];//0:CW1
//   CW_FORMAT[3] = CW_FORMAT[3] + RESERVE_CHECK * 64;
   CW_FORMAT[3] = CW_FORMAT[3] + OPERATION_MODE_VALUE(HKDATA[116]); //96(=64+32-->0x01100000):nomal, 64(0x01000000):low, 16(0x00100000):safe
   KILL_FLAG_FAB = (HKDATA[49] & 0x10)>>4;
   KILL_FLAG_MAIN = HKDATA[49] & 0x01;
   CW_FORMAT[3] = CW_FORMAT[3] + KILL_FLAG_MAIN * 16;
   CW_FORMAT[3] = CW_FORMAT[3] + KILL_FLAG_FAB * 8;
   CW_FORMAT[3] = CW_FORMAT[3] + ANT_DEP_STATUS * 4;
   CW_FORMAT[3] = CW_FORMAT[3] + CHECK_SUNSHINE(HKDATA[34]) * 2;//+X Panel
   CW_FORMAT[3] = CW_FORMAT[3] + CHECK_SUNSHINE(HKDATA[35]) * 1;//-Y Panel
   
   CW_FORMAT[4] = CW_FORMAT[4] + CHECK_SUNSHINE(HKDATA[36]) * 128;//-Z Panel
   CW_FORMAT[4] = CW_FORMAT[4] + CHECK_SUNSHINE(HKDATA[37]) * 64;//+Y Panel
   CW_FORMAT[4] = CW_FORMAT[4] + CHECK_SUNSHINE(HKDATA[38]) * 32;//+Z Panel
   CW_FORMAT[4] = CW_FORMAT[4] + (HKDATA[4] & 0b00011111);//time data
   
   CW_IDENTIFIER = 0;
   CHECK_50_and_CW_RESPOND();
   ACK_for_COM[0] = 0xAA;//for safety (this byte should be always 0)
   ACK_for_COM[23] = 0xBB;//for safety (this byte should be always 0)
   return;
}
void MAKE_CW2_FORMAT()//when you inprement, go to line
{
   Delete_CWFORMAT();
   //CHECK_50_and_CW_RESPOND();
   CW_FORMAT[0] = CONVERT_16bit_GYRO_to_8bit_X(HKDATA[59], HKDATA[60]); //GYRO X axis
   CW_FORMAT[1] = CONVERT_16bit_GYRO_to_8bit_Y(HKDATA[61], HKDATA[62]); //GYRO Y axis
   CW_FORMAT[2] = CONVERT_16bit_GYRO_to_8bit_Z(HKDATA[63], HKDATA[64]); //GYRO Z axis
   
   CW_FORMAT[3] = CW_FORMAT[3] + 128; //1:CW2
   CW_FORMAT[3] = CW_FORMAT[3] + FIRST_HSSC_DONE * 64; //High Sampling Sensor Collection Flag 0:not done, 1:done
   CW_FORMAT[3] = CW_FORMAT[3] + AUTO_CAM_DONE * 32; //AUTO CAM MISSION DONE 0:not done, 1:done
   CW_FORMAT[3] = CW_FORMAT[3] + AUTO_LDM_DONE * 16; //AUTO LDM MISSION DONE 0:not done, 1:done
   CW_FORMAT[3] = CW_FORMAT[3] + AUTO_ADCS_DONE * 8; //AUTO ADCS MISSION DONE 0:not done, 1:done
   CW_FORMAT[3] = CW_FORMAT[3] + HKDATA[48] * 4; //Heater 0:OFF, 1:ON
   CW_FORMAT[3] = CW_FORMAT[3] + RESERVE_CHECK * 2;//RSV Flag
   CW_FORMAT[3] = CW_FORMAT[3] + UPLINK_SUCCESS;//UPLINK SUCCESS
   
   CW_FORMAT[4] = HKDATA[20]<<4|HKDATA[21]>>4;//CPLD_TEMP
   
   CW_IDENTIFIER = 1;
   CHECK_50_and_CW_RESPOND();
   ACK_for_COM[0] = 0xAA;//for safety (this byte should be always 0)
   ACK_for_COM[23] = 0xBB;//for safety (this byte should be always 0)
   
   return;
}

void MAKE_CW_FORMAT()
{
   if(CW_IDENTIFIER)
   {
      MAKE_CW1_FORMAT();
   }else{
      MAKE_CW2_FORMAT();
   }
}

void Send_COMMAND_TO_FAB(int8 cmd)
{
   fputc(cmd,FAB);
   return;
}

void COMMAND_TO_FAB(int32 delaytime)
{
   FAB_DATA = 0;
   enable_interrupts(INT_rda3);//FAB
   Send_Command_to_FAB(0x61);  //send command to fab
   fprintf(PC,"Sending Done\r\n");
   int32 a = 0;
   while(FAB_DATA == 0){
      a++;
      if(a > 50000)
      {
         break;
      }
   }
   waiting(delaytime);
   disable_interrupts(INT_rda3);
   return;
}

int8 ACK_FROM_FAB()
{
   int8 ack;
   ack = fgetc(CAM);
   return ack;
}










void SAVE_HKDATA_TO_SCF(unsigned int32 Memory_Adress)
{
   output_low(PIN_C4);
   for(int8 num = 0; num < HK_size; num++)
   {
      WRITE_DATA_BYTE_SCF(Memory_Adress+num, HKDATA[num]);
   }
   output_high(PIN_C4);
   return;
}

void Send_HKDATA_to_SCF(int32 adress)
{
   Save_HKDATA_to_SCF(adress);  //save HK to COM PIC
   CHECK_50_and_CW_RESPOND();
   return;
}

void SAVE_HKDATA_TO_SMF(unsigned int32 Memory_Adress)
{
   output_low(PIN_A5);
   for(int8 num = 0; num < HK_size; num++)
   {
      WRITE_DATA_BYTE_SMF(Memory_Adress+num, HKDATA[num]);
   }
   return;
}
void Send_HKDATA_to_SMF(int32 adress)
{
   Save_HKDATA_to_SMF(adress);  //save HK to COM PIC
   CHECK_50_and_CW_RESPOND();
   return;
}

void SAVE_HKDATA_TO_OF(unsigned int32 Memory_Adress)
{
   int num;
   for(num = 0; num < HK_size; num++)
   {
      WRITE_DATA_BYTE_OF(Memory_Adress+num, HKDATA[num]);
   }
   return;
}
void Send_HKDATA_to_OF(int32 adress)
{
   Save_HKDATA_to_OF(adress);  //save HK to COM PIC
   CHECK_50_and_CW_RESPOND();
   return;
}








void SAVE_CWFORMAT_TO_SCF(unsigned int32 Memory_Adress)
{
   output_low(PIN_C4);
   for(int8 num = 0; num < CW_size; num++)
   {
      WRITE_DATA_BYTE_SCF(Memory_Adress+num, CW_FORMAT[num]);
   }
   output_high(PIN_C4);
   return;
}
void SEND_CWFORMAT_TO_SCF(int32 adress)
{
   SAVE_CWFORMAT_TO_SCF(adress);  //save HK to COM PIC
   CHECK_50_and_CW_RESPOND();
   return;
}

void SAVE_CWFORMAT_TO_SMF(unsigned int32 Memory_Adress)
{
   output_low(PIN_A5);
   for(int8 num = 0; num < CW_size; num++)
   {
      WRITE_DATA_BYTE_SMF(Memory_Adress+num, CW_FORMAT[num]);
   }
   return;
}
void SEND_CWFORMAT_TO_SMF(int32 adress)
{
   SAVE_CWFORMAT_TO_SMF(adress);  //save HK to COM PIC
   CHECK_50_and_CW_RESPOND();
   fprintf(PC,"\r\nSAVE CW\r\n");
   return;
}

void SAVE_CWFORMAT_TO_OF(unsigned int32 Memory_Adress)
{
   for(int8 num = 0; num < CW_size; num++)
   {
      WRITE_DATA_BYTE_OF(Memory_Adress+num, CW_FORMAT[num]);
   }
   return;
}
void SEND_CWFORMAT_TO_OF(int32 adress)
{
   SAVE_CWFORMAT_TO_SCF(adress);  //save HK to COM PIC
   TRANSFER_DATA_NBYTE_TOPC_OF(adress,CW_size); //for checking whether the data saved correctly
   CHECK_50_and_CW_RESPOND();
   return;
}








void CHECK_HKDATA(int8 in,int32 delaytime)///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
   fprintf(PC,"GET SENSOR DATA\r\n");
   Delete_HKDATA();
   waiting(delaytime);
   CHECK_50_and_CW_RESPOND();
   for(int num = 1; num < 11 - in; num++) //[FAB] +X,-Y,-Z,+Y,-Xtemp_high,low(array[10] to [17])
   {
      HKDATA[num + 5+4] = in_HK[num + 2 - in];
      fputc(HKDATA[num + 5+4],PC);
   }
   
   MEASURE_BC_TEMP();
   CHECK_50_and_CW_RESPOND();
   HKDATA[14+4] = BC_temp_data_h;//+X temp high[18]
   HKDATA[15+4] = BC_temp_data_l;//+X temp low[19]
   
   for(num = 9; num < FAB_SENSOR_size - 2; num++) //[FAB] from CPLD temp to Kill status(array[20] to [49])
   {
      HKDATA[num + 7+4] = in_HK[num + 2 - in];
      fputc(HKDATA[num + 7+4],PC);
   }
   FAB_DATA = 0;
   return;
}

void VERIFY_FABDATA(int32 delaytime1,int32 delaytime2)
{
   for(int8 num = 0; num < 3; num++)
   {
      COMMAND_TO_FAB(delaytime1);
      CHECK_50_and_CW_RESPOND();
      //FAB_DATA = 0;
      if(in_HK[0] == 0x33)//gather sensor data by interrupt
      {
         CHECK_HKDATA(2,delaytime2);
         CHECK_50_and_CW_RESPOND();
         CHECK_FAB_RESPONSE = 1; //1 is succeeded to get response from FAB
         break;
      }else if(in_HK[1] == 0x33){
         //delay_ms(200);
         CHECK_HKDATA(1,delaytime2);
         CHECK_50_and_CW_RESPOND();
         CHECK_FAB_RESPONSE = 1; //1 is succeeded to get response from FAB
         break;
      }else if(in_HK[2] == 0x33){
         //delay_ms(200);
         CHECK_HKDATA(0,delaytime2);
         CHECK_50_and_CW_RESPOND();
         CHECK_FAB_RESPONSE = 1; //1 is succeeded to get response from FAB
         break;
      }
   }
   return;
}




void GET_RESET_DATA()
{
   RESET_DATA = 0;
   for(int i = 0; i < 6; i++)
   {
      COLLECT_RESET_DATA();
      if(reset_bffr[0] == 0x8e)
      {
         break;
      }
   }
   CHECK_50_and_CW_RESPOND();
   if(RESET_bffr[0] == 0x8e)
   {
      fprintf(PC,"GET RESET\r\n");
      for(int num = 0; num < 5; num++)//timedata
      {
         HKDATA[num + 2] = reset_bffr[num + 1];
         fputc(HKDATA[num + 2],PC);
      }
   
      for(num = 0; num < 5; num++)//reset sensor data
      {
         HKDATA[num + 116] = reset_bffr[num + 6];//HKDATA[116] = reset[6]
         fputc(HKDATA[num + 116],PC);
      }
   }else{
      fprintf(PC,"NO RESET\r\n");
   }
   //Delete_Reset();
   return;
}

void MAKE_ADCS_HKDATA()
{
   GET_ADCS_SENSOR_DATA();
   CHECK_50_and_CW_RESPOND();
   for(int num = 53; num < 113; num++)//12byte(MAG6,GYRO6)+48byte(GPS) = 60
   {
      HKDATA[num] = ADCS_SENSOR_DATA[num - 52];//ADCS[1] to ADCS[60]
   }
//!   for(num = 65; num < 107; num++)//60byte(GPS)
//!   {
//!      HKDATA[num] = ADCS_SENSOR_DATA[num - 34];//HKDATA[54] = ADCS_SENSOR_DATA[21]-->ADCS[21] to ADCS[62]
//!   }
   return;
}

void DISPLAY_CW()
{
   fprintf(PC,"\r\nCW:\r\n");
   for(int8 i = 0; i < 5; i++)
   {
      fprintf(PC,"%x",CW_FORMAT[i]);
   }
   fprintf(PC,"\r\n");
   CHECK_50_and_CW_RESPOND();
   return;
}

void SET_IDENTIFIER()
{
   CHECK_50_and_CW_RESPOND();
   HKDATA[0] = 0x33;
   HKDATA[1] = 0x33;
   HKDATA[7] = 0xAA;
   HKDATA[8] = 0xAA;
   HKDATA[9] = 0xAA;
   HKDATA[50] = 0xBB;
   HKDATA[51] = 0xBB;
   HKDATA[52] = 0xBB;
   HKDATA[113] = 0xCC;
   HKDATA[114] = 0xCC;
   HKDATA[115] = 0xCC;
   HKDATA[122] = 0x44;
   HKDATA[123] = 0x44;
   return;
}

void FAB_TEST_OPERATION()
{
   Delete_in_HK();
   LOOP_FAB_HK_ADDRESS();
   CHECK_50_and_CW_RESPOND();
   LOOP_FAB_CW_ADDRESS();
   
   //LOOP_FAB_HK_ADDRESS();
   //CHECK_50_and_CW_RESPOND();
   //LOOP_FAB_CW_ADDRESS();
   CHECK_50_and_CW_RESPOND();
   Turn_ON_ADCS();
   fprintf(PC,"FAB communication start\r\n");
   waiting(200000);
   FAB_MEASUERING_FLAG++;//count until 7(it means 10 min)
   FAB_DATA = 0;   //for initialize
   
   fprintf(PC,"SENDING COMMAND TO FAB\r\n");
   CHECK_50_and_CW_RESPOND();
   VERIFY_FABDATA(130000,2000);
   CHECK_50_and_CW_RESPOND();
   
   GET_RESET_DATA();
   MAKE_ADCS_HKDATA();
   Turn_OFF_ADCS();
   CHECK_50_and_CW_RESPOND();
   MAKE_CW_FORMAT();
   CHECK_50_and_CW_RESPOND();
   SET_IDENTIFIER();
   if(CHECK_FAB_RESPONSE)
   {
      fprintf(PC,"\r\nCollecting HK and Making CWFormat Done\r\n\r\n");
      fprintf(PC,"\r\n\r\nSensor:");
      for(int num = 0; num < 65; num++)//array[0] to [64](until gyro data)
      {
         fprintf(PC,"%x,",HKDATA[num]);
      }
      CHECK_50_and_CW_RESPOND();
      for(num = 65; num < 113; num++)//GPS 60 byte should be shown as char type
      {
         fputc(HKDATA[num],PC);
      }
      CHECK_50_and_CW_RESPOND();
      for(num = 113; num < HK_Size; num++)//array[125] to [135]
      {
         fprintf(PC,",%x",HKDATA[num]);
      }
      CHECK_50_and_CW_RESPOND();
      
      DISPLAY_CW();
      CHECK_FAB_RESPONSE = 0;
      output_low(PIN_C4);
      output_low(PIN_A5);
      //sector_erase_SCF(65536*50);
      //sector_erase_SCF(65536*51);
      SEND_HKDATA_to_SCF(FAB_HK_ADDRESS);
      SEND_HKDATA_to_SMF(FAB_HK_ADDRESS);
      SEND_HKDATA_to_OF(FAB_HK_ADDRESS);
      SEND_CWFORMAT_TO_SCF(FAB_CW_ADDRESS);
      SEND_CWFORMAT_TO_SMF(FAB_CW_ADDRESS);
      SEND_CWFORMAT_TO_OF(FAB_CW_ADDRESS);
      FAB_HK_ADDRESS = FAB_HK_ADDRESS + HK_size;//prepare for next storing address
      FAB_CW_ADDRESS = FAB_CW_ADDRESS + CW_size;//prepare for next storing address
      fprintf(PC,"\r\nSENSOR SAVING DONE\r\n");
      output_high(PIN_C4);
      CHECK_50_and_CW_RESPOND();
   }else{
      SEND_HKDATA_to_SCF(FAB_HK_ADDRESS);
      SEND_HKDATA_to_SMF(FAB_HK_ADDRESS);
      SEND_HKDATA_to_OF(FAB_HK_ADDRESS);
      SEND_CWFORMAT_TO_SCF(FAB_CW_ADDRESS);
      SEND_CWFORMAT_TO_SMF(FAB_CW_ADDRESS);
      SEND_CWFORMAT_TO_OF(FAB_CW_ADDRESS);
      FAB_HK_ADDRESS = FAB_HK_ADDRESS + HK_size;//prepare for next storing address
      FAB_CW_ADDRESS = FAB_CW_ADDRESS + CW_size;//prepare for next storing address
      fprintf(PC,"NO RESPONSE FROM FAB\r\n\r\n");  
      CHECK_50_and_CW_RESPOND();
      for(int num = 0; num < HK_size; num++)
      {
         fprintf(PC,"%x,",HKDATA[num]);
      }
      CHECK_50_and_CW_RESPOND();
      
   }
   
   return;
}

void SAVE_ADCS_INITIAL_and_FINAL(int32 ADDRESS)
{
   output_low(PIN_C4);
   output_low(PIN_A5);
   for(int i = 0; i < 90; i++)//put initial 90byte(9byte for time + 81byte for MSN)
   {
      WRITE_DATA_BYTE_OF(ADCS_SENSOR_ADDRESS+i,READ_DATA_BYTE_OF(ADDRESS+i));
   }
   for(i = 0; i < 90; i++)//put initial 90byte(9byte for time + 81byte for MSN)
   {
      WRITE_DATA_BYTE_SCF(ADCS_SENSOR_ADDRESS+i,READ_DATA_BYTE_OF(ADDRESS+i));
   }
   for(i = 0; i < 90; i++)//put initial 90byte(9byte for time + 81byte for MSN)
   {
      WRITE_DATA_BYTE_SMF(ADCS_SENSOR_ADDRESS+i,READ_DATA_BYTE_OF(ADDRESS+i));
   }
   
   ADCS_SENSOR_ADDRESS = ADCS_SENSOR_ADDRESS + 90;
   
   for(i = 0; i < 81; i++)//put final 81 byte
   {
      WRITE_DATA_BYTE_OF(ADCS_SENSOR_ADDRESS+i,READ_DATA_BYTE_OF(ADCS_SENSOR_ADDRESS-171+i));
   }
   for(i = 0; i < 81; i++)//put final 81 byte
   {
      WRITE_DATA_BYTE_SCF(ADCS_SENSOR_ADDRESS+i,READ_DATA_BYTE_OF(ADCS_SENSOR_ADDRESS-171+i));
   }
   for(i = 0; i < 81; i++)//put final 81 byte
   {
      WRITE_DATA_BYTE_SMF(ADCS_SENSOR_ADDRESS+i,READ_DATA_BYTE_OF(ADCS_SENSOR_ADDRESS-171+i));
   }
   ADCS_SENSOR_ADDRESS = ADCS_SENSOR_ADDRESS + 81;
   output_high(PIN_C4);
   return;
}

void GET_INITIAL_TIME()
{
   COLLECT_RESET_DATA();
   
   WRITE_DATA_BYTE_OF(ADCS_SENSOR_ADDRESS,0xdd);//header
   WRITE_DATA_BYTE_OF(ADCS_SENSOR_ADDRESS + 1,0xdd);//header
   for(int i = 1; i < 6; i++){
      WRITE_DATA_BYTE_OF(ADCS_SENSOR_ADDRESS+1+i,reset_bffr[i]);
   }
   WRITE_DATA_BYTE_OF(ADCS_SENSOR_ADDRESS + 7,0xdd);//footer
   WRITE_DATA_BYTE_OF(ADCS_SENSOR_ADDRESS + 8,0xdd);//footer
   
   output_low(PIN_C4);//COM_MUX MAINSIDE
   WRITE_DATA_BYTE_SCF(ADCS_SENSOR_ADDRESS,0xdd);//header
   WRITE_DATA_BYTE_SCF(ADCS_SENSOR_ADDRESS + 1,0xdd);//header
   for(i = 1; i < 6; i++){
      WRITE_DATA_BYTE_SCF(ADCS_SENSOR_ADDRESS+1+i,reset_bffr[i]);
   }
   WRITE_DATA_BYTE_SCF(ADCS_SENSOR_ADDRESS + 7,0xdd);//footer
   WRITE_DATA_BYTE_SCF(ADCS_SENSOR_ADDRESS + 8,0xdd);//footer
   output_high(PIN_C4);
   
   output_low(PIN_A5);//CAM_MUX MAINSIDE
   WRITE_DATA_BYTE_SMF(ADCS_SENSOR_ADDRESS,0xdd);//heade
   WRITE_DATA_BYTE_SMF(ADCS_SENSOR_ADDRESS + 1,0xdd);//header
   for(i = 1; i < 6; i++){
      WRITE_DATA_BYTE_SMF(ADCS_SENSOR_ADDRESS+1+i,reset_bffr[i]);
   }
   WRITE_DATA_BYTE_SMF(ADCS_SENSOR_ADDRESS + 7,0xdd);//footer
   WRITE_DATA_BYTE_SMF(ADCS_SENSOR_ADDRESS + 8,0xdd);//footer
   
   ADCS_SENSOR_ADDRESS = ADCS_SENSOR_ADDRESS + 9;//total 9byte for time info
   //5 byte time info and 2 byte header and 2 byte footer
   return;
}

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
void GET_ADCS_MSN_DATA()
{
   int a = 1;
   if(kbhit(ADCS)&&fgetc(ADCS) == 0x57)
   {
      for(int32 n= 0; n < 300000; n++)
      {
         if(kbhit(ADCS))
         {
            ADCS_SENSOR_DATA[a] = fgetc(ADCS);//[1] to [72]
            a++;//after input everything, a = 73
         }
         if(a == 73)
         {
            break;
         }
      }
      for(int i = 76; i > 70 ; i--)//6 byte [76] to [70] --> Duty XYZ
      {
         ADCS_SENSOR_DATA[i] = ADCS_SENSOR_DATA[i-4];//ADCS_SENSOR_DATA[76] is ADCS_SENSOR_DATA[72]
      }
      for(i = 68; i > 20 ; i--)//48 byte [68] to [21] --> GPS
      {
         ADCS_SENSOR_DATA[i] = ADCS_SENSOR_DATA[i-2];//ADCS_SENSOR_DATA[68] is ADCS_SENSOR_DATA[66]
      }
         
      ADCS_SENSOR_DATA[0] = 0x57;
      ADCS_SENSOR_DATA[19] = 0xee;
      ADCS_SENSOR_DATA[20] = 0xee;
      ADCS_SENSOR_DATA[69] = 0x58;
      ADCS_SENSOR_DATA[70] = 0x58;
      ADCS_SENSOR_DATA[77] = 0x59;
            
      int8 a = (int8)(currenttime >> 8);
      int8 b = (int8)(currenttime);
      output_low(PIN_C4);
      output_low(PIN_A5);
      WRITE_DATA_BYTE_OF(ADCS_SENSOR_ADDRESS,0x56);//header
      WRITE_DATA_BYTE_SCF(ADCS_SENSOR_ADDRESS,0x56);//header
      WRITE_DATA_BYTE_SMF(ADCS_SENSOR_ADDRESS,0x56);//header
            
      WRITE_DATA_BYTE_OF(ADCS_SENSOR_ADDRESS+1,a);
      WRITE_DATA_BYTE_OF(ADCS_SENSOR_ADDRESS+2,b);
      WRITE_DATA_BYTE_SCF(ADCS_SENSOR_ADDRESS+1,a);
      WRITE_DATA_BYTE_SCF(ADCS_SENSOR_ADDRESS+2,b);
      WRITE_DATA_BYTE_SMF(ADCS_SENSOR_ADDRESS+1,a);
      WRITE_DATA_BYTE_SMF(ADCS_SENSOR_ADDRESS+2,b);
      for(i = 0; i < ADCS_SENSOR_SIZE; i++)// save everything to frash memory
      {
         WRITE_DATA_BYTE_OF(ADCS_SENSOR_ADDRESS + 3 + i,ADCS_SENSOR_DATA[i]);
         WRITE_DATA_BYTE_SCF(ADCS_SENSOR_ADDRESS + 3 + i,ADCS_SENSOR_DATA[i]);
         WRITE_DATA_BYTE_SMF(ADCS_SENSOR_ADDRESS + 3 + i,ADCS_SENSOR_DATA[i]);
      }
      output_high(PIN_C4);
      fprintf(PC,"\r\n%ld,",currenttime);
      fprintf(PC,"%x",READ_DATA_BYTE_OF(ADCS_SENSOR_ADDRESS+3)/*ADCS_SENSOR_DATA[i]*/);//ack
      fprintf(PC,",");
            
      for(i = 4; i < 22; i++)// show the data-->Magnetometer XYZ, Gyro XYZ, Cur sensor XYZ
      {
         fprintf(PC,"%x",READ_DATA_BYTE_OF(ADCS_SENSOR_ADDRESS+i)/*ADCS_SENSOR_DATA[i]*/);
         i++;
         fprintf(PC,"%x,",READ_DATA_BYTE_OF(ADCS_SENSOR_ADDRESS+i)/*ADCS_SENSOR_DATA[i]*/);
         ADCS_SENSOR_DATA[i] = 0;
      }
            
      fprintf(PC,"%x",READ_DATA_BYTE_OF(ADCS_SENSOR_ADDRESS+22)/*ADCS_SENSOR_DATA[i]*/);//0xEE
      fprintf(PC,"%x",READ_DATA_BYTE_OF(ADCS_SENSOR_ADDRESS+23)/*ADCS_SENSOR_DATA[i]*/);//0xEE
      fprintf(PC,",");
            
      for(i = 24; i < 72; i++)// show the data(GPS)-->48byte
      {
         //fprintf(PC,"%c",READ_DATA_BYTE_OF(ADCS_SENSOR_ADDRESS+i)/*ADCS_SENSOR_DATA[i]*/);
         fputc(READ_DATA_BYTE_OF(ADCS_SENSOR_ADDRESS+i),PC);
         ADCS_SENSOR_DATA[i] = 0;
      }
            
      fprintf(PC,",");
      fprintf(PC,"%x",READ_DATA_BYTE_OF(ADCS_SENSOR_ADDRESS+84)/*ADCS_SENSOR_DATA[i]*/);//0x58
      fprintf(PC,"%x",READ_DATA_BYTE_OF(ADCS_SENSOR_ADDRESS+85)/*ADCS_SENSOR_DATA[i]*/);//0x58
      fprintf(PC,",");
            
      for(i = 74; i < ADCS_SENSOR_SIZE+2; i++)// show the duty data-->6byte
      {
         fprintf(PC,"%x,",(READ_DATA_BYTE_OF(ADCS_SENSOR_ADDRESS+i))/*ADCS_SENSOR_DATA[i]*/);
         ADCS_SENSOR_DATA[i] = 0;
         i++;
         fprintf(PC,"%x,",(READ_DATA_BYTE_OF(ADCS_SENSOR_ADDRESS+i))/*ADCS_SENSOR_DATA[i]*/);
         ADCS_SENSOR_DATA[i] = 0;
      }
      ADCS_SENSOR_ADDRESS = ADCS_SENSOR_ADDRESS + ADCS_SENSOR_SIZE + 3;
   }
   return;
}
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

int32 beginning_of_operation = 0;
//OPERATION_MODE
#define NOMAL_ADCS_MSN 0x22
#define AUTO_ADCS_MSN 0x27
void ADCS_TEST_OPERATION(unsigned int16 duration, int8 OPERATION_MODE) //Integration Testing With ADCS
{
   int8 count = 0;
   LOOP_ADCS_SENSOR_ADDRESS();
   ADCS_SETTINGS();
   beginning_of_operation = ADCS_SENSOR_ADDRESS;
   
   GET_INITIAL_TIME();
   fprintf(PC,"Turn On ADCS\r\n");
   Turn_On_ADCS(); //Trun on ADCS
   delay_ms(8000);
   fprintf(PC,"Send Command to ADCS\r\n");

   fputc(OPERATION_MODE,adcs);//for execute stabilizse mission

   CMD_FROM_PC[0] = 0;//prepare to get MAIN command
   PC_DATA = 0;//prepare to get MAIN command
   currenttime = 0;//prepare to measure the operation time
   while(true)
   {
      for(int safe = 0; safe < 100; safe++)
      {
         GET_ADCS_MSN_DATA();
      }
      if(currenttime > duration)//600-->600 seconds (10min)
      {
         fprintf(PC,"done\r\n");
         break;
      }
      for(safe = 0; safe < 100; safe++)
      {
         GET_ADCS_MSN_DATA();
      }
      if((currenttime%90)==0)
      {
         COLLECT_RESET_DATA();
      }
      
      for(safe = 0; safe < 100; safe++)
      {
         GET_ADCS_MSN_DATA();
      }
      
      if(CMD_FROM_PC[0] == 0xab)
      {
         CMD_FROM_PC[0] = 0;
         PC_DATA = 0;
         Turn_off_ADCS();
         fprintf(PC,"Stop ADCS Mission\r\n");
         break;
      }
      for(safe = 0; safe < 100; safe++)
      {
         GET_ADCS_MSN_DATA();
      }
      RESET_SATELLITE();//if reset pic send command for resetting the satellite, OBC will response soon
   }   
   Turn_OFF_ADCS();  //turn off the mux(give access to MAIN)
   fprintf(PC,"\r\n\r\nTrun_OFF_ADCS\r\n\r\n\r\n");
   
   fprintf(PC,"%lx",beginning_of_operation);
   delay_ms(1000);
   SAVE_ADCS_INITIAL_and_FINAL(beginning_of_operation);
   output_low(PIN_C4);
   for(int32 num = 0; num < 81+90; num++)
   {
      fprintf(PC,"%x",READ_DATA_BYTE_SCF(beginning_of_operation + num));
   }
   //TRANSFER_DATA_NBYTE_TOPC_SCF(ADCS_SENSOR_ADDRESS-ATTEMPT_TIME*100, ATTEMPT_TIME*100);
   output_high(PIN_C4);
   RESET_SATELLITE();//if reset pic send command for resetting the satellite, OBC will response soon
   if(count > 89)
   {
      count = 0;
      COLLECT_RESET_DATA();
   }else{
      count++;
   } 
   return;
}



//--------HIGH SAMPLING HK collection------------------------------------------

void CHECK_HIGH_SAMP_FABDATA(int8 in)//FAB sensor data collect
{
   fprintf(PC,"GET SENSOR DATA\r\n");
   Delete_HKDATA();
   for(int num = 1; num < 11; num++) //Collect HK DATA
   {
      HKDATA[num + 5+4] = in_HK[num + 2 - in];
      //fputc(HKDATA[num],PC);
   }
   MEASURE_BC_TEMP();
   HKDATA[18] = BC_temp_data_h;//-X Panel Temp
   HKDATA[19] = BC_temp_data_l;//-X Panel Temp
   for(num = 9; num < FAB_SENSOR_size - 2; num++) //[FAB] from CPLD temp to Kill status(array[20] to [49])
   {
      HKDATA[num + 7+4] = in_HK[num + 2 - in];
      fputc(HKDATA[num + 7+4],PC);
   }
   FAB_DATA = 0;
}

void VERIFY_HIGH_SAMP_FABDATA(int32 delaytime)
{
   for(int8 num = 0; num < 3; num++)
   {
      COMMAND_TO_FAB(delaytime);
      //FAB_DATA = 0;
      //waiting(1000);
      if(in_HK[0] == 0x33)//gather sensor data by interrupt
      {
         CHECK_HIGH_SAMP_FABDATA(2);
         CHECK_FAB_RESPONSE = 1; //1 is succeeded to get response from FAB
         break;
      }else if(in_HK[1] == 0x33){
         //delay_ms(200);
         CHECK_HIGH_SAMP_FABDATA(1);
         CHECK_FAB_RESPONSE = 1; //1 is succeeded to get response from FAB
         break;
      }else if(in_HK[2] == 0x33){
         //delay_ms(200);
         CHECK_HIGH_SAMP_FABDATA(0);
         CHECK_FAB_RESPONSE = 1; //1 is succeeded to get response from FAB
         break;
      }
      FAB_DATA = 0;
   }
   return;
}



void GET_HIGH_SAMP_RESET_DATA()
{
   RESET_DATA = 0;
   COLLECT_RESET_DATA();
   if(RESET_bffr[0] == 0x8e)
   {
fprintf(PC,"GET RESET\r\n");
      for(int num = 0; num < 5; num++)//timedata
      {
         HKDATA[num + 2] = reset_bffr[num + 1];
         fputc(HKDATA[num + 2],PC);
      }
   
      for(num = 0; num < 5; num++)//reset sensor data
      {
         HKDATA[num + 116] = reset_bffr[num + 6];//HKDATA[116] = reset[6]
         fputc(HKDATA[num + 116],PC);
      }
   }else{
      fprintf(PC,"NO RESET\r\n");
   }
   //Delete_Reset();
}

void MAKE_HIGH_SAMP_ADCS_FORMAT()
{
   GET_ADCS_SENSOR_DATA();
   for(int num = 53; num < 113; num++)//12byte+48byte = 60 byte
   {
      HKDATA[num] = ADCS_SENSOR_DATA[num - 52];//ADCS[1] to ADCS[60]
   }
//!   for(num = 65; num < 107; num++)
//!   {
//!      HKDATA[num] = ADCS_SENSOR_DATA[num - 34];//HKDATA[54] = ADCS_SENSOR_DATA[21]
//!   }
   return;
}

void MAKE_HIGH_SAMPLING_FORMAT()
{
   CHECK_50_and_CW_RESPOND();
   if(in_HK[0] == 0x33)//gather sensor data by interrupt
   {
      CHECK_HIGH_SAMP_FABDATA(2);
      CHECK_FAB_RESPONSE = 1; //1 is succeeded to get response from FAB
   }else if(in_HK[1] == 0x33){
      CHECK_HIGH_SAMP_FABDATA(1);
      CHECK_FAB_RESPONSE = 1; //1 is succeeded to get response from FAB
   }else if(in_HK[2] == 0x33){;
      CHECK_HIGH_SAMP_FABDATA(0);
      CHECK_FAB_RESPONSE = 1; //1 is succeeded to get response from FAB
   }

   CHECK_50_and_CW_RESPOND();
   if(RESET_bffr[0] == 0x8e)
   {
      for(int num = 0; num < 5; num++)//timedata
      {
         HKDATA[num] = reset_bffr[num + 1];
         //fputc(HKDATA[num],PC);
      }
   
      for(num = 0; num < 5; num++)//reset sensor data
      {
         HKDATA[num + 91] = reset_bffr[num + 6];
         //fputc(HKDATA[num + 91],PC);
      }
   }
   
   CHECK_50_and_CW_RESPOND();
   for(int num = 0; num < 12; num++)//put into sensor array
   {
      HKDATA[num + 38] = ADCS_SENSOR_DATA[num];
   }
   for(num = 0; num < 41; num++)//put into sensor array
   {
      HKDATA[num + 50] = ADCS_SENSOR_DATA[num + 21];
   }
   return;
}

void SAVE_HIGH_SAMP_DATA_TO_EACH_MEMORY()
{
   output_low(PIN_C4);
   CHECK_50_and_CW_RESPOND();
   for(int i = 0; i < HIGH_SAMP_HK_size; i++)
   {
      WRITE_DATA_BYTE_SCF(HIGH_SAMP_HK_ADDRESS + i,HKDATA[i]);
   }
   output_high(PIN_C4); 
   
   CHECK_50_and_CW_RESPOND();
   for(i = 0; i < HIGH_SAMP_HK_size; i++)
   {
      WRITE_DATA_BYTE_OF(HIGH_SAMP_HK_ADDRESS + i,HKDATA[i]);
   }
   
   CHECK_50_and_CW_RESPOND();
   output_low(PIN_A5);
   for(i = 0; i < HIGH_SAMP_HK_size; i++)
   {
      WRITE_DATA_BYTE_SMF(HIGH_SAMP_HK_ADDRESS + i,HKDATA[i]);
   }  
   
   CHECK_50_and_CW_RESPOND();
   HIGH_SAMP_HK_ADDRESS = HIGH_SAMP_HK_ADDRESS + HIGH_SAMP_HK_size;//prepare for next storing address
   
   return;
}

void HIGH_SAMP_FAB_OPERATION()
{
   Turn_ON_ADCS();
   CHECK_50_and_CW_RESPOND();
   HIGH_SAMP_FAB_MEASUERING_FLAG++;//count until 90(it means 10 min)
   //disable_interrupts(INT_rda2);
   if(HIGH_SAMP_FAB_MEASUERING_FLAG > 17)//HIGH_SAMP_FAB_MEASUERING_FLAG=18 --> 18*5 = 90 (sec)
      {
         
         CHECK_50_and_CW_RESPOND();
         
         fprintf(PC,"\r\n\90sec\r\n");
         Delete_in_HK();
         VERIFY_FABDATA(10000,10);
         GET_RESET_DATA();
         MAKE_ADCS_HKDATA();
         SET_IDENTIFIER();
         MAKE_CW_FORMAT();
         output_low(PIN_C4);
         output_low(PIN_A5);
         
         SEND_HKDATA_to_SCF(FAB_HK_ADDRESS);
         SEND_HKDATA_to_SMF(FAB_HK_ADDRESS);
         SEND_HKDATA_to_OF(FAB_HK_ADDRESS);
         SEND_CWFORMAT_TO_SCF(FAB_CW_ADDRESS);
         SEND_CWFORMAT_TO_SMF(FAB_CW_ADDRESS);
         SEND_CWFORMAT_TO_OF(FAB_CW_ADDRESS);
         
         CHECK_50_and_CW_RESPOND();
         FAB_HK_ADDRESS = FAB_HK_ADDRESS + HK_size;//prepare for next storing address
         FAB_CW_ADDRESS = FAB_CW_ADDRESS + CW_size;//prepare for next storing address
         
         SAVE_HIGH_SAMP_DATA_TO_EACH_MEMORY();
         
         CHECK_50_and_CW_RESPOND();
         
         fprintf(PC,"\r\nSuccess%x\r\n",HIGH_SAMP_FAB_MEASUERING_FLAG);
         HIGH_SAMP_FAB_MEASUERING_FLAG = 0;
         
      }else{
         Delete_in_HK();
         GET_RESET_DATA();
         VERIFY_HIGH_SAMP_FABDATA(10000);   
         GET_HIGH_SAMP_RESET_DATA();
         MAKE_HIGH_SAMP_ADCS_FORMAT();
         SET_IDENTIFIER();
         //MAKE_CW_FORMAT();
              
         fprintf(PC,"\r\n");
         for(int num = 0; num < 65; num++)//array[0] to [64](until gyro data)
         {
            fprintf(PC,"%x,",HKDATA[num]);
         }
         CHECK_50_and_CW_RESPOND();
         for(num = 65; num < 113; num++)
         {
            fputc(HKDATA[num],PC);
         }
         CHECK_50_and_CW_RESPOND();
         for(num = 113; num < HK_Size; num++)
         {
            fprintf(PC,",%x",HKDATA[num]);
         } 
         CHECK_50_and_CW_RESPOND();
         
         fprintf(PC,"\r\nCOUNT:%x\r\n",HIGH_SAMP_FAB_MEASUERING_FLAG);
         CHECK_FAB_RESPONSE = 0;
         SAVE_HIGH_SAMP_DATA_TO_EACH_MEMORY();
      }    
      //Delete_in_HK();

   FAB_DATA = 0;
   output_high(PIN_C4);

   return;
}


//--------CAM MISSION----------------------------------------------------------
//#define STORE_TO_SING AA
//////////////////////////////////////////////////
//CAM MISSION ID: 0x54                          //
//CAM ACK: 0x61                                 //
//CAM Finish Storing: 0x62                      //
//////////////////////////////////////////////////

int8 CAM_ACK = 0;


void Turn_On_CAM()
{
   output_high(PIN_D7);
   return;
}

void Turn_Off_CAM()
{
   output_low(PIN_D7);
   return;
}

void Send_Command_to_CAM(int8 data) //testing for EM integration
{
   fputc(data,CAM);
   return;
}

void CAM_SETTINGS()
{
   //disable_interrupts(INT_rda);   // Main to PC
   fprintf(PC,"CAM Start\r\n");
   COM_ONEBYTE_COMMAND = 0;
   fprintf(PC,"Delete Buffer\r\n");

   output_high(PIN_A5); //turn on MUX (give access to CAM)
   delay_ms(100);
   Turn_On_CAM(); //Trun on CAM
   fprintf(PC,"Turned On MUX And CAM\r\n");
   delay_ms(10);
   return;
}

void CHECK_ACK_FROM_CAM_10k() //check 10000 times (keep listening about 10 sec)
{
   for(int32 num = 0; num < 1000000; num++) 
   {
      if(kbhit(CAM)/*kbhit(PC)*/)
      {
         CAM_ACK = fgetc(CAM/*PC*/);
         break;
      }
      //delay_ms(1);
   }
   return;
}

void CHECK_ACK_FROM_CAM_200k() //check 200000 times(keep listening about 200 sec)
{
   for(int32 num = 0; num < 40000000; num++) 
   {
      if(kbhit(CAM)/*kbhit(PC)*/)
      {
         CAM_ACK = fgetc(CAM/*PC*/);
         break;
      }
      //delay_ms(1);
   }
   return;
}

int8 ACK_from_CAM()
{
   int8 ack;
   ack = fgetc(CAM | PC);
   return ack;
}

void Transfer_Photo() //transfer photo from Shared Mission Flash to Shared COM Flash
{
   output_low(PIN_A5);
   output_low(PIN_C4);
   TRANSFER_DATA_NBYTE_SMFtoSCF(0,CAM_ADDRESS,65536); //from: sector0, to: sector1, datasize: 64kbyte(this value is for testing)
   CAM_ADDRESS = CAM_ADDRESS + 65536;
   return;
}

void CAM_TEST_OPERATION(int8 mode) //Integration Testing With CAM
{
   CAM_SETTINGS();  //initialize_CAM
   delay_ms(5000);
   

   fprintf(PC,"Send Command to CAM\r\n");
   delay_ms(10);
   Send_Command_to_CAM(0x54);

   //fputc(0x54,PC);
   CHECK_ACK_FROM_CAM_10k();
   
   if(CAM_ACK == 0x61)
   {
      CAM_ACK = 0;
      LOOP_CAM_ADDRESS();
      fprintf(PC,"ACK Get\r\n\r\n");
      delay_ms(1000);
      fprintf(PC,"Send the mode data\r\n");
      fputc(mode,CAM);
      CHECK_ACK_FROM_CAM_10k();
      if(CAM_ACK == mode)
      {
            fprintf(PC,"Wait Next Command...\r\n\r\n");
         
         CHECK_ACK_FROM_CAM_200k();
         
         if(CAM_ACK == 0x62)
         {
            CAM_ACK = 0;
            fprintf(PC,"PHOTO SAVING DONE\r\n");
            
            output_low(PIN_A5);  //turn off the mux(give access to MAIN)
            delay_ms(10);
            //TRANSFER_DATA_NBYTE_TOPC_SMF(0x00000000,256);  //for checking
            output_low(PIN_C4);
            delay_ms(10);
            fprintf(PC,"Copy and Read Start\r\n");
            delay_ms(1000);
            
            if(mode == 0xB0 || mode == 0xB1 || mode == 0xB2)//if cam size will be large
            {
               sector_erase_SCF(CAM_ADDRESS);
               sector_erase_SCF(CAM_ADDRESS + 65536);
               TRANSFER_DATA_NBYTE_SMFtoSCF(0x00080000,CAM_ADDRESS,80000); //From(SMF): sector0, To(SCF): sector1
               TRANSFER_DATA_NBYTE_SMFtoOF(0x00080000,CAM_ADDRESS,80000); //From(SMF): sector0, To(SCF): sector1
               TRANSFER_DATA_NBYTE_TOPC_SCF(CAM_ADDRESS,90000);
               CAM_ADDRESS = CAM_ADDRESS + 65536*2;
               delay_ms(10);
            }else{
               sector_erase_SCF(CAM_ADDRESS);
               sector_erase_SCF(CAM_ADDRESS + 65536);
               TRANSFER_DATA_NBYTE_SMFtoSCF(0x00080000,CAM_ADDRESS,65536); //From(SMF): sector0, To(SCF): sector1
               TRANSFER_DATA_NBYTE_SMFtoOF(0x00080000,CAM_ADDRESS,65536); //From(SMF): sector0, To(SCF): sector1
               TRANSFER_DATA_NBYTE_TOPC_SCF(CAM_ADDRESS,65536);
               CAM_ADDRESS = CAM_ADDRESS + 65536;
               delay_ms(10);
            }
            
         }else{
            CAM_ACK = 0;
            fprintf(PC,"SAVING FAILURED\r\n");
         
         }
      
      }   

   }else{
      CAM_ACK = 0;
      fprintf(PC,"ACK FAILURED\r\n");
   }
   
   
   

   
//*****************************************************************************
//!   int8 CAMSIZE[4] = {};
//!   int32 CAM_ADD;
//!   while(true)
//!   {
//!      if(fgetc(CAM) == 0x23)
//!      {
//!         for(int a = 0; a < 4; a++)
//!         {
//!            CAMSIZE[a] = fgetc(CAM);
//!         }
//!      }
//!   }
//!   CAM_ADD = ((CAMSIZE[0]<<24 | CAMSIZE[1]<<16 | CAMSIZE[2]<<8 | CAMSIZE[3]) & 0xffffffff);
//!   
//!   fprintf(PC,"data adress is %x\r\n", CAM_ADD);//for testing
//*****************************************************************************
  
   Turn_Off_CAM();
   fprintf(PC,"Turned OFF CAM\r\n");
   output_high(PIN_C4); //Give Access to COM PIC
   //fputc(0x69,COM); 
   //enable_interrupts(INT_rda);   // Main to PC
   return;
}



//--------Data Collection MISSION----------------------------------------------

int8 DC_ACK = 0;
int8 LDM_DATA[83] = {};

void Turn_On_DC()
{
   output_high(PIN_F5);
   return;
}

void Turn_Off_DC()
{
   output_low(PIN_F5);
   return;
}

void CHECK_ACK_FROM_DC_10k()
{
   for(int32 num = 0; num < 5000000 ;num++)
   {
      if(kbhit(DC))
      {
         if(fgetc(DC) == 0x20)
         {
            DC_ACK = 0x20;
            break;
         }
//!         DC_ACK = fgetc(DC);
//!         fprintf(PC,"%x",DC_ACK);
//!         break;
      }
      //delay_ms(1);
   }   
   return;
}

void DELETE_LDM_DATA()
{
   for(int8 i = 0; i < 83; i++)
   {
      LDM_DATA[i] = 0;
   }
   return;
}

void GET_LDM_DATA()
{
   fputc(0x67,DC);
   int16 n = 0;
   for(int32 i = 0; i < /*10000000*/1000000; i++)
   {
      if(kbhit(DC))
      {
         LDM_DATA[n] = fgetc(DC);
         n++;
//!         if(n>83 && LDM_DATA[0]==0xAA && LDM_DATA[82]==0xBB )
//!         {
//!            n = 0;
//!            fprintf(PC,"Get data from LDM\r\n");
//!            delay_ms(2000);
//!            for(int m = 0; m < 83; m++)
//!            {
//!               fprintf(PC,"%x",LDM_DATA[m]);
//!            }
//!            fprintf(PC,"\r\n");
//!            break;
//!         }else{
//!            fprintf(PC,"Data were not correct\r\n");
//!            for(int m = 0; m < 83; m++)
//!            {
//!               fprintf(PC,"%x",LDM_DATA[m]);
//!            }
//!            fprintf(PC,"\r\n");
//!            break;
//!         }
//!       //break;
      }
   }
   for(int m = 0; m < 83; m++)
   {
      fprintf(PC,"%x",LDM_DATA[m]);
   }
   fprintf(PC,"\r\n");
   output_low(PIN_C4);
   output_low(PIN_A5);
   for(m = 0; m < 83; m++)
   {
      WRITE_DATA_BYTE_OF(DC_STATUS_ADDRESS + m,LDM_DATA[m]);
      WRITE_DATA_BYTE_SCF(DC_STATUS_ADDRESS + m,LDM_DATA[m]);
      WRITE_DATA_BYTE_SMF(DC_STATUS_ADDRESS + m,LDM_DATA[m]);
   }
   output_high(PIN_C4);
   DC_STATUS_ADDRESS = DC_STATUS_ADDRESS + 83;
   
   return;
}

void DATACOLLECTION_TEST_OPERATION()
{
   DELETE_LDM_DATA();//prepare for mission
   LOOP_DC_STATUS_ADDRESS();
   fprintf(PC,"Trun ON LDM\r\n");
   Turn_On_DC();
   delay_ms(30);
      //for(int32 num = 0; num < 10000 ;num++){}
   //fputc(0x25,DC);
   //while(true)
   //{
      //fputc(0x69,DC);
      //fputc(fgetc(DC),PC);
   //}
   //fprintf(PC,"SEND COMMAND AND WAIT FOR RESPONSE\r\n");
   CHECK_ACK_FROM_DC_10k();
   if(DC_ACK == 0x20)
   {
      
      fprintf(PC,"Get ACK From LDM\r\n");
      //delay_ms(1000);
      DC_ACK = 0;
      
      //fprintf(PC,"WAITING NEXT RESPONSE\r\n");
      GET_LDM_DATA();
//!      CHECK_ACK_FROM_DC_10k();
//!      if(DC_ACK == 0x25)
//!      {
//!         fprintf(PC,"DC WORKING\r\n");
//!         output_low(PIN_C4);
//!         Write_DATA_BYTE_SCF(DC_STATUS_ADDRESS,0xBB);
//!         Write_DATA_BYTE_SMF(DC_STATUS_ADDRESS,0xBB);
//!         output_high(PIN_C4);
//!         fprintf(PC,"COM FLASH ADDRESS:%x, Status:BB ",DC_STATUS_ADDRESS);
//!         DC_STATUS_ADDRESS++;
//!         DC_ACK = 0;
//!      }else{
//!         DC_ACK = 0;
//!         fprintf(PC,"FAILUED GETTING ACK From DC\r\n");
//!         output_low(PIN_C4);
//!         Write_DATA_BYTE_SCF(DC_STATUS_ADDRESS,0xCC);
//!         Write_DATA_BYTE_SMF(DC_STATUS_ADDRESS,0xCC);
//!         output_high(PIN_C4);
//!         fprintf(PC,"COM FLASH ADDRESS:%x, Status:CC ",DC_STATUS_ADDRESS);
//!         DC_STATUS_ADDRESS++;
//!         DC_ACK = 0;
//!      }
   }else{
      DC_ACK = 0;
      fprintf(PC,"FAILUED GETTING ACK From DC\r\n");
   }
   
   fprintf(PC,"Turn OFF...\r\n");
   Turn_Off_DC();
   output_high(PIN_C4);
   
   return;
}

//-------Kill Switch-----------------------------------------------------------
Static int8 KILL_COUNTER = 0;
void Count_Kill_Flag()
{
   KILL_COUNTER++;
   if(Kill_COUNTER > 4)
   {
      Kill_FLAG_MAIN = 1;
      output_high(PIN_A4); //Kill Switch ON
      fputc(0x17,FAB);
   }
   fprintf(PC,"Kill counter: %x\r\n",KILL_COUNTER);
   return;
}

void Disable_Kill()
{
   KILL_COUNTER = 0;
   
   Kill_FLAG_MAIN = 0;
   output_low(PIN_A4); //Kill Switch ON
   fputc(0x18,FAB);
   
   fprintf(PC,"Kill counter: %x\r\n",KILL_COUNTER);
   return;
}


//-------Reset Function--------------------------------------------------------


void BC_ON_30min()
{
   if(currenttime > 1805 && BC_ATTEMPT_FLAG == 0)//if first attempt and 30 min(1800sec) passed
   {
      fprintf(PC,"send command to RESET PIC\r\n");
      for(int16 num = 0; num < 200; num++)
      {
         fputc(0xBC,reset);
         delay_ms(5);
      }
      delay_ms(1000);
      if(reset_bffr[0] == 0xCB)
      {
         BC_OPERATION();
         BC_ATTEMPT_FLAG++;
         WRITE_FLAG_to_EEPROM();
         STORE_FLAG_INFO();
         STORE_ADRESS_DATA_TO_FLASH();
         reset_bffr[0] = 0;
         RESET_DATA = 0;
         delay_ms(1000);
         delay_ms(20000);//wait until RESET goes back to nomal loop
         SAVE_SAT_LOG(0xBC,0x30,0x30);//first 30 min antenna deployment
      }
   }
   return;
}



void HIGHSAMP_SENSOR_COLLECTION(int16 times)
{
   LOOP_HIGH_SAMP_HK_ADDRESS();
   CHECK_50_and_CW_RESPOND();
   LOOP_FAB_HK_ADDRESS();
   CHECK_50_and_CW_RESPOND();
   LOOP_FAB_CW_ADDRESS();
   int32 num = 0;
   while(num < times)
   {
      CHECK_50_and_CW_RESPOND();
      if(FAB_FLAG > 4)
      {
         FAB_FLAG = 0;
         HIGH_SAMP_FAB_OPERATION();
         
         num++;
         if((num % 20) == 0)//once in the 20 times(every 100sec save the address to the flash)
         {
            STORE_ADRESS_DATA_TO_FLASH();//for store the flag
         } 
      }
      BC_ON_30min();
      if((CMD_FROM_PC[0] == 0xAB)||(in_bffr_main[4] == 0xAB))//forced termination function
      {
         Delete_Buffer();
         break;
      }
      PC_DATA = 0;
      RESET_SATELLITE();
   }
   fprintf(PC,"HSSC DONE\r\n");
   return;
}

void REPLY_TO_COM(int8 data1,int8 data2)
{
   if(ACK_for_COM[12] == 0x66 && ACK_for_COM[14] == 0x77)
   {
      for(int n = 0; n < 24; n++)//send back the acknowledge
      {
         fputc(ACK_for_COM[n],COM);
      }
      for(n = 0; n < 24; n++)//send back the acknowledge
      {
         fprintf(PC,"%x,",ACK_for_COM[n]);
      }
      fprintf(PC,",AUTO\r\n");
   }
   else{
      //REFLESH_MSN_ACK_for_COM();
//!      for(int n = 7; n < 24; n++)//send back the acknowledge
//!      {
//!         ACK_for_COM[n] = 0xff;
//!      }
      ACK_for_COM[0] = 0xAA;
      ACK_for_COM[12] = data1;
      ACK_for_COM[13] = data2;
      ACK_for_COM[23] = 0xBB;
      for(int n = 0; n < 24; n++)//send back the acknowledge
      {
         fputc(ACK_for_COM[n],COM);
      }
      for(n = 0; n < 24; n++)//send back the acknowledge
      {
         fprintf(PC,"%x,",ACK_for_COM[n]);
      }
      fprintf(PC,",NOMAL\r\n");
   }

}

void UPDATE_ACK_for_COM(int8 data1,int8 data2,int32 address, int16 size)//for automatical mission
{
   REFLESH_MSN_ACK_for_COM();
   ACK_for_COM[15] = address >> 24;
   ACK_for_COM[16] = address >> 16;
   ACK_for_COM[17] = address >> 8;
   ACK_for_COM[18] = address;
   
   ACK_for_COM[19] = size >> 24;
   ACK_for_COM[20] = size >> 16;
   ACK_for_COM[21] = size >> 8;
   ACK_for_COM[22] = size;
   
   ACK_for_COM[0] = 0xAA;
   ACK_for_COM[12] = data1;
   ACK_for_COM[14] = data2;
   ACK_for_COM[23] = 0xBB;
   for(int n = 0; n < 24; n++)//send back the acknowledge
   {
      fputc(ACK_for_COM[n],COM);
   }
}








