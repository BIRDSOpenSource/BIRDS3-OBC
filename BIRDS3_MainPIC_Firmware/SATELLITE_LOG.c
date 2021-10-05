void waiting(int32 counterval)//about 200000 : 1s
{
   for(int32 num = 0; num < counterval; num++)
   {}
   return;
}

//--------RESET function-------------------------------------------------------


void DELETE_RESET()
{
   for(int8 num = 0; num < Reset_size; num++)
   {
      reset_bffr[num] = 0;
   }
   return;
}

void SEND_CMD_FOR_RESET_SATELLITE()
{  
   STORE_FLAG_INFO();
   Save_RSV();
   STORE_ADRESS_DATA_TO_FLASH();
   while(true)
   {
      //fputc(0x27,PC);
      fprintf(PC,"%x,",READ_DATA_BYTE_OF(FAB_HK_ADDRESS-122));
      fprintf(PC,"%x,",READ_DATA_BYTE_OF(FAB_HK_ADDRESS-121));
      fprintf(PC,"%x,",READ_DATA_BYTE_OF(FAB_HK_ADDRESS-120));
      fprintf(PC,"%x",READ_DATA_BYTE_OF(FAB_HK_ADDRESS-119));
      fprintf(PC,"%x\r\n",READ_DATA_BYTE_OF(FAB_HK_ADDRESS-118));
      for(int i = 0; i<100; i++)
      {
         fputc(0x27,reset);
         delay_ms(10);
      }
      //delay_ms(100);//if it is needed
//!      fputc(READ_DATA_BYTE_OF(FAB_HK_ADDRESS-122),reset);
//!      fputc(READ_DATA_BYTE_OF(FAB_HK_ADDRESS-121),reset);
//!      fputc(READ_DATA_BYTE_OF(FAB_HK_ADDRESS-120),reset);
//!      fputc(READ_DATA_BYTE_OF(FAB_HK_ADDRESS-119),reset);
//!      fputc(READ_DATA_BYTE_OF(FAB_HK_ADDRESS-118),reset);
      RESET_DATA = 0;
      fprintf(PC,"get ready for reset");
      delay_ms(1000);
   }
   return;
}

void RESET_SATELLITE()//if get reset command from reset, prepare for resetting
{
   if(reset_flag == 1)//get ready for reset satellite
   {
      SEND_CMD_FOR_RESET_SATELLITE();
   }
   DELETE_RESET();
   RESET_DATA = 0;
   return;
}

void COLLECT_RESET_DATA()
{
   DELETE_RESET();
   RESET_DATA = 0;
   for(int16 num = 0; num < 50; num++)
   {
      fputc(0x28,reset);
      delay_us(800);
   }
   waiting(10000);
   return;
}


//@@@@@@@@@@@for satellite log@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#define logdata_size 11
int8 satellitelog[logdata_size] = {};

void SAVE_SAT_LOG(int8 data1, int8 data2, int8 data3)
{
   LOOP_SAT_LOG();
   RESET_DATA = 0;
   COLLECT_RESET_DATA();
   if(RESET_bffr[0] == 0x8e)
   {
      satellitelog[0] = 0xda;//header of the satellite log
      satellitelog[1] = 0xda;//header of the satellite log
   
      satellitelog[2] = RESET_bffr[1];
      satellitelog[3] = RESET_bffr[2];
      satellitelog[4] = RESET_bffr[3];
      satellitelog[5] = RESET_bffr[4];
      satellitelog[6] = RESET_bffr[5];
      
      satellitelog[7] = data1;
      satellitelog[8] = data2;
      satellitelog[9] = data3;
      satellitelog[10] = 0xef;//footer
      
      output_low(PIN_A5);//CAM_MUX
      output_low(PIN_C4);//COM_MUX
      for(int i = 0; i < logdata_size; i++)//write to the 
      {
         WRITE_DATA_BYTE_OF(SAT_LOG + i,satellitelog[i]);
         WRITE_DATA_BYTE_SCF(SAT_LOG + i,satellitelog[i]);
         WRITE_DATA_BYTE_SMF(SAT_LOG + i,satellitelog[i]);
      }
      output_high(PIN_C4);//COM_MUX
      SAT_LOG = SAT_LOG + logdata_size;
      fprintf(PC,"LOG saving done\r\n");
   }
   
   return;
}



