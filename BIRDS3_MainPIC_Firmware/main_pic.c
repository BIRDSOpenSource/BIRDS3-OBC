////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//FM_version_2.3
//Final Flight Model Code
//2019_02_16
//Yuta KAKIMOTO
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <main pic1.h>
//#include <mainpic_testing.c>
#int_timer0
void intval()
{
   static int32 counter = 0;
   counter++;
   if (counter > 60)
   {
      counter = 0;
//!      nowtime++;
//!      
//!      if(nowtime > 0)
//!      {
         //fprintf(PC,"%ld",nowtime);
         FAB_FLAG++;
         RESERVE_SEC_FLAG++;
         currenttime++;
         //fprintf(PC,"Done\r\n");
//!      }
      if(RESERVE_SEC_FLAG > 59)
      {
         RESERVE_SEC_FLAG = 0;
         RESERVE_MIN_FLAG++;
      }
   }
}

#INT_rda//PC Interrupt
void UART1_RXD(void)
{
   CMD_FROM_PC[PC_DATA] = fgetc(PC);
   PC_DATA = ((PC_DATA + 1) % 6);
   //fputc(fgetc(PC),PC);
}

#INT_rda2 HIGH //COM Interrupt
void UART2_RXD(void)
{
   //SerialDataReceive();  //get 32 byte
   //COM_ONEBYTE_COMMAND = fgetc(COM);
   in_bffr_main[COM_DATA] = fgetc(COM);
   COM_DATA = ((COM_DATA + 1) % 16);
} 

#INT_rda3 //FAB Interrupt
void UART3_RXD(void)
{
   //collect_HK_from_FAB();
   in_HK[FAB_DATA] = fgetc(FAB);
   FAB_DATA = ((FAB_DATA + 1) % FAB_SENSOR_size);
} 

#INT_rda4//reset Interrupt
void UART4_RXD(void)
{
   
   reset_bffr[RESET_DATA] = fgetc(reset);
   RESET_DATA = ((RESET_DATA + 1) % RESET_size);
   
   if(reset_bffr[0] == 0xaa)//get ready for reset satellite
   {
      reset_flag = 1;
      RESET_DATA = 0;
   }
   
} 

void settings()
{
   set_tris_b(0b11010000);
   enable_interrupts(global);
   enable_interrupts(INT_rda);   // Main to PC
   enable_interrupts(INT_rda2);  // Main to COM PIC
   enable_interrupts(INT_rda3);  // Main to FAB PIC
   enable_interrupts(INT_rda4);  // Main to RESET PIC
   
   setup_timer_0(RTCC_INTERNAL | RTCC_DIV_256 | RTCC_8_BIT);
   set_timer0(0);
   
   READ_WRTITING_ADDRESS_LOCATION();
   RESERVE_CHECK = 0;
   RESET_DATA = 0;

   CHECK_FLAG_INFO();
   CHECK_ADDRESS_DATA();
   
   BC_SETUP();
   
   output_low(PIN_A4);//kill switch off
   Turn_OFF_CAM();
   Turn_OFF_ADCS();
   Turn_OFF_DC();
   Turn_OFF_BC();

   fprintf(PC,"\r\n");
   CURRENT_FLAG_STATUS();
   fprintf(PC,"\r\n");
   CHANGE_ADDRESS_WRITING_LOCATION();
   
   output_high(PIN_C4); //MUX: COM (MAIN-COM)
   output_low(PIN_A5); //MUX: CAM (MAIN-MISSION)
   
   Get_RSV();//read the reservation table info from flash memory
   SAVE_SAT_LOG(0x25,0x25,0x25);//reset log
   
   return;
}

///////////TEST PART//////////////////////////
//#org DEFAULT 
void main()
{
   settings();
   enable_interrupts(INT_TIMER0);
   fprintf(PC,"Start Operating\r\n");
   FAB_TEST_OPERATION();

   STORE_ADRESS_DATA_TO_FLASH();
   CURRENT_ADDRESS_OF_FLASH();
   STORE_FLAG_INFO();
   WRITE_FLAG_to_EEPROM();
   
   fprintf(PC,"CW:");
   for(int i = 0; i < 4; i++)
   {
      fprintf(PC,"%x,",CW_FORMAT[i]);
   }
   fprintf(PC,"%x\r\n",CW_FORMAT[4]);
   
   
   Antenna_Deploy();
   //try 5 times for reliability
   AUTOMATICAL_MISSION_CHECK();
   AUTOMATICAL_MISSION_CHECK();
   AUTOMATICAL_MISSION_CHECK();
   AUTOMATICAL_MISSION_CHECK();
   AUTOMATICAL_MISSION_CHECK();
  
   RESET_DATA = 0;
   while(true)
   {
      BC_ON_30min();//if satellite shut down during automatical MSN, this function will be used after start again 
      RESET_SATELLITE();//check reset command from RESET PIC
      
      if(FAB_FLAG > 89)//every 90 sec, OBC gather sensor data and update CW format 
      {
         FAB_FLAG = 0;
         fprintf(PC,"90sec passed\r\n");
         FAB_TEST_OPERATION();//get all sensor data
         COM_DATA = 0;
         STORE_ADRESS_DATA_TO_FLASH();//for store the address info
         fprintf(PC,"CW:");
         for(int i = 0; i < 4; i++)//show CW format
         {
            fprintf(PC,"%x,",CW_FORMAT[i]);
         }
         fprintf(PC,"%x\r\n",CW_FORMAT[4]);
         DELETE_CMD_FROM_PC();//delete PC command
         PC_DATA = 0;//reset interrupt data for safety
         COM_DATA = 0;//reset interrupt data for safety
         RESET_DATA = 0;//reset interrupt data for safety
      }   
      
      if((RESERVE_MIN_FLAG >= RESERVE_TARGET_FLAG) && RESERVE_CHECK == 1)//check the reservation command, if time came, execute
      {
         fprintf(PC,"execute reserved command\r\n");
         MISSION_CONTENTS = CHECK_MEMORY_FUNCTION(MISSION_CONTENTS);
         EXECUTE_MISSION_from_COM(MISSION_CONTENTS,MISSION_DETAIL,0,0,0);//execute command
         Remove_1_Reservation();//remove the finished command and sort again and save updated command table
         if(reserve_table[30] != 0x00)//if next reservation is registered, wait until time will be come
         {
            Reserve_next();
         }else{//if all reservation finished, reset flag about reservation
            RESERVE_TARGET_FLAG = 0; //reset flag
            RESERVE_CHECK = 0;
            MISSION_CONTENTS = 0;
            MISSION_DETAIL = 0;
         }
      }
      
      if(buffer_from_com)
      {
         for(COM_DATA = 0; COM_DATA < 16; COM_DATA++)//display command
         {
            fputc(in_bffr_main[COM_DATA],PC);
         }
         COM_DATA = 0;
         fprintf(PC,"COM COMMAND\r\n");
         if(in_bffr_main[5] == 0)//if time data is not placed
         {
            EXECUTE_MISSION_from_COM(in_bffr_main[4],in_bffr_main[6],in_bffr_main[7],in_bffr_main[8],in_bffr_main[9]);
         }else{//if time data is placed (that means reservation command)
            SAVE_SAT_LOG(in_bffr_main[4],in_bffr_main[5],in_bffr_main[6]);//reservation command log
            Reserve_command_COM();
         }
         DELETE_CMD_FROM_PC();
         Delete_Buffer();
         CMD_FROM_PC[1] = 0;
         COM_DATA = 0;
         PC_DATA = 0;
      }
      
      
      
      if(CMD_FROM_PC[0])
      {
         fprintf(PC,"PC COMMAND:");
         for(int m = 0; m < 6; m++)
         {
            fprintf(PC,"%x",CMD_FROM_PC[m]);
         }
         fprintf(PC,"\r\n");
         
         if(CMD_FROM_PC[1] == 0)//if time data is not placed
         {
            EXECUTE_MISSION_from_PC(CMD_FROM_PC[0],CMD_FROM_PC[2],CMD_FROM_PC[3],CMD_FROM_PC[4],CMD_FROM_PC[5]);//execute mission
         }else{//if time data is placed (that means reservation command)
            SAVE_SAT_LOG(CMD_FROM_PC[0],CMD_FROM_PC[1],CMD_FROM_PC[2]);//reservation command log
            Reserve_command_PC();//mission command reservation
         }
         DELETE_CMD_FROM_PC();
         Delete_Buffer();
         CMD_FROM_PC[1] = 0;
         COM_DATA = 0;
         PC_DATA = 0;
      }
      
      if(COM_DATA != 0 || PC_DATA != 0)
      {
         delay_ms(500);
         if(PC_DATA != 0)
         {
            CMD_FROM_PC[0] = 0;
            CMD_FROM_PC[1] = 0;
            CMD_FROM_PC[2] = 0;
            CMD_FROM_PC[3] = 0;
            CMD_FROM_PC[4] = 0;
            CMD_FROM_PC[5] = 0;
            COM_DATA = 0;
            PC_DATA = 0;
         }
      }
      delay_ms(400);
   }
}
