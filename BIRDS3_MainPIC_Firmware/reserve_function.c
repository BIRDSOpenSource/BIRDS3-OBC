

#define table_size 30
unsigned int8 reserve_table[table_size+1] = {};
unsigned int8 table_num = 0;

void initialize_table()
{
   for(int num = 0; num < table_size; num++)
   {
      reserve_table[num] = 0xff;
   }
   reserve_table[30] = 0;
   return;
}

////////////////////////////////////////////////////////////////////////////////////////////////
//                          ***Sort Function***
//This is for sorting the saved reservation commands in ascending order.
//
//reserved_table[j-4] reserved_table[j-3] reserved_table[j-2]
//reserved_table[j-1]  reserved_table[j]  reserved_table[j+1]
//
//j-4 and j-1 data are the contents information(like mission)
//j-3 and j data are the time data(execute the command reserved_table[j] min later )
//j-2 and j+1 data are the contents detail information
//
//by comapring j-3 and j data,if it is needed, swhich j-4 and j-1, j-3 and j, j-2 and j+1
////////////////////////////////////////////////////////////////////////////////////////////////

int8 count_reservation()
{
   int8 counter = 0;
   for(int num = 1; num < table_size - 1; num = num + 3)
   {
      if(reserve_table[num-1] != 0xff)
      {
         counter++;
      }
   }
   return counter * 3;
}

void sort_table()//(command info, time info, mission mode)~10¨compare the time and sort
{
   for(int i = 1; i < table_size - 2; i = i + 3){

      // compare from lower site
      for(int j = table_size - 2; j > i; j = j - 3){

      // if upper value is larger, switch the values
         if(reserve_table[j] < reserve_table[j-3]){//compare the time of [j]th and [j-3]th
            int t1 = reserve_table[j+1];
            int t2 = reserve_table[j];
            int t3 = reserve_table[j-1];
            
            reserve_table[j+1] = reserve_table[j-2];
            reserve_table[j] = reserve_table[j-3];
            reserve_table[j-1] = reserve_table[j-4];
            
            reserve_table[j-2] = t1;
            reserve_table[j-3] = t2;
            reserve_table[j-4] = t3;
         }
      }
   }
   return;
}

void Save_RSV()
{
   output_low(PIN_A5);//CAM_MUX MAINSIDE
   output_low(PIN_C4);//COM_MUX MAINSIDE
   SUBSECTOR_4KB_ERASE_OF(RSV_DATA_ADDRESS);
   delay_ms(100);
   SUBSECTOR_4KB_ERASE_SCF(RSV_DATA_ADDRESS);
   delay_ms(100);
   SUBSECTOR_4KB_ERASE_SMF(RSV_DATA_ADDRESS);
   delay_ms(100);
   
   for(int num = 0; num < table_size + 1; num++)
   {
      WRITE_DATA_BYTE_OF(RSV_DATA_ADDRESS+num,reserve_table[num]);
      WRITE_DATA_BYTE_SMF(RSV_DATA_ADDRESS+num,reserve_table[num]);
      WRITE_DATA_BYTE_SCF(RSV_DATA_ADDRESS+num,reserve_table[num]);
   }
   
   WRITE_DATA_BYTE_OF(RSV_DATA_ADDRESS+31,RESERVE_MIN_FLAG);
   WRITE_DATA_BYTE_SCF(RSV_DATA_ADDRESS+31,RESERVE_MIN_FLAG);
   WRITE_DATA_BYTE_SMF(RSV_DATA_ADDRESS+31,RESERVE_MIN_FLAG);
   for(num = 0; num < table_size + 1; num++)
   {
      fprintf(PC,"%x",READ_DATA_BYTE_OF(RSV_DATA_ADDRESS + num));
   }
   output_high(PIN_C4);//COM_MUX MAINSIDE
   fprintf(PC,"\r\n");
   return;
}

void table_refresh()//everything is back to default 
{
   for(int i = 0; i < table_size; i++)
   {
      reserve_table[i] = 0xff;
   }
   reserve_table[30] = 0;
   RESERVE_MIN_FLAG = 0;
   RESERVE_SEC_FLAG = 0;
   RESERVE_CHECK = 0;
   MISSION_CONTENTS = 0;//closest mission command
   RESERVE_TARGET_FLAG = 0;//closest time data
   MISSION_DETAIL = 0;//closest mission command detail
   Save_RSV();
   return;
}

void Get_RSV()
{
   //fprintf(PC,"%x");
   for(int num = 0; num < table_size+1; num++)
   {
      reserve_table[num]=READ_DATA_BYTE_OF(RSV_DATA_ADDRESS + num);//import reservation table info
   }
   reserve_table[30] = count_reservation();//count the number of the data
//   RESERVE_MIN_FLAG = READ_DATA_BYTE_OF(RSV_DATA_ADDRESS+31);
   if(reserve_table[30] == 0xFF || reserve_table[30] == 0x00)//if no data
   {
      initialize_table();//table info --> 0xff, reserve_table[30] = 0x00
   }
   table_num = reserve_table[30];
   if(table_num != 0x00)//yes-->get reserved command and target time
   {
      RESERVE_CHECK = 1;
      MISSION_CONTENTS = reserve_table[0];//closest mission command
      RESERVE_TARGET_FLAG = reserve_table[1];//closest time data
      MISSION_DETAIL = reserve_table[2];//closest mission command detail
   }else{
      RESERVE_CHECK = 0;
      MISSION_CONTENTS = 0xff;//closest mission command
      RESERVE_TARGET_FLAG = 0xff;//closest time data
      MISSION_DETAIL = 0xff;//closest mission command detail
   }
   return;
}

void Disp_RSV()//show the reserve status (reserve command and reserve table)
{
   Get_RSV();//get the info from the main flash
   for(int num = 0; num < table_size + 1; num++)
   {
      fprintf(PC,"%x",reserve_table[num]);
   }
   fprintf(PC,"\r\n");
   fprintf(PC,"RESERVE_CHECK = %x\r\n",RESERVE_CHECK);
   fprintf(PC,"NEXT MISSION_CONTENTS = %x\r\n",MISSION_CONTENTS);
   fprintf(PC,"RESERVE_TARGET_FLAG = %x\r\n",RESERVE_TARGET_FLAG);
   fprintf(PC,"NEXT MISSION DETAIL = %x\r\n",MISSION_DETAIL);
   fprintf(PC,"RESERVE_MIN_FLAG = %x\r\n",RESERVE_MIN_FLAG);
   fprintf(PC,"table_num = %x\r\n",table_num);
   
   return;
}



void input_reservation_COM()//put the command from COM into the table and sort
{
//!   reserve_table[table_num] = CMD_FROM_PC[0];//input the data about mission content
//!   reserve_table[table_num + 1] = CMD_FROM_PC[1];//input the data about time data
//!   reserve_table[table_num + 2] = CMD_FROM_PC[2];//input the data about mission detail mode
   fprintf(PC,"%x\r\n",reserve_table[30]);
   table_num = reserve_table[30];
   if(reserve_table[30] == 30)
   {
      fprintf(PC,"Reservation Full!\r\n\r\n");
   }else{
      if(RESERVE_CHECK != 0x00)
      {
         for(int num = 1; num < table_size - 1; num = num + 3)
         {
            if(reserve_table[num-1] != 0xff)//check whether command data is placed
            {
               if(reserve_table[num] > RESERVE_MIN_FLAG)
               {
                  reserve_table[num] = reserve_table[num] - RESERVE_MIN_FLAG;//update the time of all commands
               }else{
                  reserve_table[num] = 0;
               }
            }
         }
         reserve_table[table_num] = in_bffr_main[4];//input the data about mission content
         reserve_table[table_num + 1] = in_bffr_main[5];
         reserve_table[table_num + 2] = in_bffr_main[6];//input the data about mission detail mode
         if(RESERVE_MIN_FLAG != 0)//RESERVE_MIN_FLAG = 0 --> no need to update the target flag
         {
            RESERVE_TARGET_FLAG = RESERVE_TARGET_FLAG - RESERVE_MIN_FLAG;
            RESERVE_MIN_FLAG = 0;
         }
      }else{//if first reservation
         RESERVE_MIN_FLAG = 0;
         RESERVE_SEC_FLAG = 0;
         reserve_table[table_num] = in_bffr_main[4];//input the data about mission content
         reserve_table[table_num + 1] = in_bffr_main[5];//input time data
         reserve_table[table_num + 2] = in_bffr_main[6];//input the data about mission detail mode
      }
      //table_num = table_num + 3;
      //reserve_table[30] = table_num;
      sort_table();
//!      for(int num = 0; num < table_size+1; num++)
//!      {
//!         fprintf(PC,"%x",reserve_table[num]);//show for the test
//!      }
   }
   reserve_table[30] = count_reservation();//update the table number data
   return;
}



void input_reservation_PC()//put the command form PC into the table and sort
{
//!   reserve_table[table_num] = in_bffr_main[4];
//!   reserve_table[table_num + 1] = in_bffr_main[5];
//!   reserve_table[table_num + 2] = in_bffr_main[6];
   fprintf(PC,"%x\r\n",reserve_table[30]);
   table_num = reserve_table[30];
   if(reserve_table[30] == 30)
   {
      fprintf(PC,"Reservation Full!\r\n\r\n");
   }else{
      if(RESERVE_CHECK != 0x00)//if new reservation is faster than target time
      {
         for(int num = 1; num < table_size - 2; num = num + 3)
         {
            if(reserve_table[num-1] != 0xff)//check whether command data is placed
            {
               if(reserve_table[num] > RESERVE_MIN_FLAG)
               {
                  reserve_table[num] = reserve_table[num] - RESERVE_MIN_FLAG;//update the time of all commands
               }else{
                  reserve_table[num] = 0;
               }
            }
         }
         reserve_table[table_num] = CMD_FROM_PC[0];//input the data about mission content
         reserve_table[table_num + 1] = CMD_FROM_PC[1];
         reserve_table[table_num + 2] = CMD_FROM_PC[2];//input the data about mission detail mode
         if(RESERVE_MIN_FLAG != 0)//RESERVE_MIN_FLAG = 0 --> no need to update the target flag
         {
            RESERVE_TARGET_FLAG = RESERVE_TARGET_FLAG - RESERVE_MIN_FLAG;
            RESERVE_MIN_FLAG = 0;
         }
      }else{//if first reservation
         RESERVE_MIN_FLAG = 0;
         RESERVE_SEC_FLAG = 0;
         reserve_table[table_num] = CMD_FROM_PC[0];//input the data about mission content
         reserve_table[table_num + 1] = CMD_FROM_PC[1];
         reserve_table[table_num + 2] = CMD_FROM_PC[2];//input the data about mission detail mode
      }
      //table_num = table_num + 3;
      //reserve_table[30] = table_num;
      sort_table();
//!      for(int num = 0; num < table_size+1; num++)
//!      {
//!         fprintf(PC,"%x",reserve_table[num]);//show for the test
//!      }
   }
   reserve_table[30] = count_reservation();//update the table number data
   return;
}



void Remove_1_Reservation()//after executing the mission, it should be finished
{
   for(int i = 1; i < table_size - 2; i = i + 3)
   {
      if(reserve_table[i-1] != 0xff)//check whether command data is placed
      {
         reserve_table[i] = reserve_table[i] - RESERVE_TARGET_FLAG;//update the time of all commands
      }
   }
   reserve_table[0] = 0xff;//delete the finished mission data
   reserve_table[1] = 0xff;//delete the finished mission data
   reserve_table[2] = 0xff;//delete the finished mission data 
   sort_table();
   reserve_table[30] = count_reservation();//update the table number data
   
   fprintf(PC,"remove finished command:");
   Save_RSV();
   fprintf(PC,"\r\n");
   return;
}

void Reserve_command_COM()//reservation from COM
{
   fprintf(PC,"reserve\r\n");
   input_reservation_COM();//put reserve table buffer and sort
   Save_RSV();//save reserve table to flash memory
   RESERVE_CHECK = 1;
   if(reserve_table[30] == 0x00 || reserve_table[30] == 0xff)
   {
      RESERVE_MIN_FLAG = 0; //reset time flag
   }
   MISSION_CONTENTS = reserve_table[0];//closest mission command
   RESERVE_TARGET_FLAG = reserve_table[1];//closest time data
   MISSION_DETAIL = reserve_table[2];//closest mission command detail
}



void Reserve_command_PC()//reservation from PC
{
   fprintf(PC,"reserve\r\n");
   input_reservation_PC();//put reserve table buffer and sort
   Save_RSV();//save reserve table to flash memory
   RESERVE_CHECK = 1;
   if(reserve_table[30] == 0x00 || reserve_table[30] == 0xff)//for only first reservation
   {
      RESERVE_MIN_FLAG = 0; //reset time flag
   }
   MISSION_CONTENTS = reserve_table[0];//closest mission command
   RESERVE_TARGET_FLAG = reserve_table[1];//closest time data
   MISSION_DETAIL = reserve_table[2];//closest mission command detail
}



void Reserve_next()//after finishing executing the reservation command, this function should be execute
{
   fprintf(PC,"reserve next\r\n");
   //Save_RSV();//save reserve table to flash memory
   
   //RESERVE_CHECK = 1;
   RESERVE_MIN_FLAG = 0; //reset time flag
   MISSION_CONTENTS = reserve_table[0];//update closest mission command
   RESERVE_TARGET_FLAG = reserve_table[1];//update closest time data (it is the time after 1 mission)
   MISSION_DETAIL = reserve_table[2];//closest mission command detail
}


