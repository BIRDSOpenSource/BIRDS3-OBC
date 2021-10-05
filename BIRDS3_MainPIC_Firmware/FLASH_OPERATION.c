

void REFRESH_SECTOR_OF(int8 data1,int8 data2,int8 data3,int8 data4)
{
   fprintf(PC,"ERASE OF:%x%x%x%x\r\n",data1,data2,data3,data4);
   int32 address = make32(data1,data2,data3,data4);
   sector_erase_OF(address);
   
   int16 DATA = address/SECT;//maximum 2048
   int8 D1 = DATA >> 8;
   int8 D2 = DATA;
   
   SAVE_SAT_LOG(0x73, D1, D2);//0x73 + 16bit sector info
   
   return;
}
void REFRESH_SECTOR_SCF(int8 data1,int8 data2,int8 data3,int8 data4)
{
   fprintf(PC,"ERASE SCF:%x%x%x%x\r\n",data1,data2,data3,data4);
   int32 address = make32(data1,data2,data3,data4);
   output_low(PIN_C4);
   sector_erase_SCF(address);
   output_high(PIN_C4);
   
   int16 DATA = address/SECT;//maximum 2048
   int8 D1 = DATA >> 8;
   int8 D2 = DATA;
   
   SAVE_SAT_LOG(0x74, D1, D2);//0x74+16bit sector info
   return;
}
void REFRESH_SECTOR_SMF(int8 data1,int8 data2,int8 data3,int8 data4)
{
   fprintf(PC,"ERASE SMF:%x%x%x%x\r\n",data1,data2,data3,data4);
   int32 address = make32(data1,data2,data3,data4);
   output_low(PIN_A5);
   sector_erase_SMF(address);
   
   int16 DATA = address/SECT;//maximum 2048
   int8 D1 = DATA >> 8;
   int8 D2 = DATA;
   
   SAVE_SAT_LOG(0x75, D1, D2);//0x75+16bit sector info
   return;
}





void TRANSFER_SECTOR_OF2SCF(int8 data1,int8 data2,int8 data3,int8 data4)//command 0x76
{
   REFRESH_SECTOR_SCF(data1,data2,data3,data4);
   delay_ms(500);
   fprintf(PC,"transfer:%x%x%x%x\r\n",data1,data2,data3,data4);
   int32 address = make32(data1,data2,data3,data4);
   address = address - (address%SECT);
   output_low(PIN_C4);
   for(int32 i = 0; i < SECT; i++)
   {
      WRITE_DATA_BYTE_SCF(address + i, READ_DATA_BYTE_OF(address + i));
   }
   delay_ms(100);
   output_high(PIN_C4);
   
   int16 DATA = address/SECT;//maximum 2048
   int8 D1 = DATA >> 8;
   int8 D2 = DATA;
   
   SAVE_SAT_LOG(0x76, D1, D2);//0x76+16bit sector info
   return;
}

void TRANSFER_SECTOR_OF2SMF(int8 data1,int8 data2,int8 data3,int8 data4)
{
   REFRESH_SECTOR_SMF(data1,data2,data3,data4);
   delay_ms(500);
   fprintf(PC,"transfer:%x%x%x%x\r\n",data1,data2,data3,data4);
   int32 address = make32(data1,data2,data3,data4);
   address = address - (address%SECT);
   output_low(PIN_A5);
   for(int32 i = 0; i < SECT; i++)
   {
      WRITE_DATA_BYTE_SMF(address + i, READ_DATA_BYTE_OF(address + i));
   }
   delay_ms(100);
   
   int16 DATA = address/SECT;//maximum 2048
   int8 D1 = DATA >> 8;
   int8 D2 = DATA;
   
   SAVE_SAT_LOG(0x77, D1, D2);//0x77+16bit sector info
   
   return;
}


void TRANSFER_SECTOR_SCF2OF(int8 data1,int8 data2,int8 data3,int8 data4)
{
   REFRESH_SECTOR_OF(data1,data2,data3,data4);
   delay_ms(500);
   fprintf(PC,"transfer:%x%x%x%x\r\n",data1,data2,data3,data4);
   int32 address = make32(data1,data2,data3,data4);
   address = address - (address%SECT);
   output_low(PIN_C4);
   for(int32 i = 0; i < SECT; i++)
   {
      WRITE_DATA_BYTE_OF(address + i, READ_DATA_BYTE_SCF(address + i));
   }
   delay_ms(100);
   output_high(PIN_C4);
   
   int16 DATA = address/SECT;//maximum 2048
   int8 D1 = DATA >> 8;
   int8 D2 = DATA;
   
   SAVE_SAT_LOG(0x78, D1, D2);//0x78+16bit sector info
   
   return;
}

void TRANSFER_SECTOR_SCF2SMF(int8 data1,int8 data2,int8 data3,int8 data4)
{
   REFRESH_SECTOR_SMF(data1,data2,data3,data4);
   delay_ms(500);
   fprintf(PC,"transfer:%x%x%x%x\r\n",data1,data2,data3,data4);
   int32 address = make32(data1,data2,data3,data4);
   address = address - (address%SECT);
   output_low(PIN_C4);
   output_low(PIN_A5);
   for(int32 i = 0; i < SECT; i++)
   {
      WRITE_DATA_BYTE_SMF(address + i, READ_DATA_BYTE_SCF(address + i));
   }
   delay_ms(100);
   output_high(PIN_C4);
   
   int16 DATA = address/SECT;//maximum 2048
   int8 D1 = DATA >> 8;
   int8 D2 = DATA;
   
   SAVE_SAT_LOG(0x79, D1, D2);//0x79+16bit sector info
   
   return;
}

void TRANSFER_SECTOR_SMF2OF(int8 data1,int8 data2,int8 data3,int8 data4)
{
   REFRESH_SECTOR_OF(data1,data2,data3,data4);
   delay_ms(500);
   fprintf(PC,"transfer:%x%x%x%x\r\n",data1,data2,data3,data4);
   int32 address = make32(data1,data2,data3,data4);
   address = address - (address%SECT);
   output_low(PIN_A5);
   for(int32 i = 0; i < SECT; i++)
   {
      WRITE_DATA_BYTE_OF(address + i, READ_DATA_BYTE_SMF(address + i));
   }
   delay_ms(100);
   
   int16 DATA = address/SECT;//maximum 2048
   int8 D1 = DATA >> 8;
   int8 D2 = DATA;
   
   SAVE_SAT_LOG(0x7A, D1, D2);//0x7A+16bit sector info
   
   return;
}

void TRANSFER_SECTOR_SMF2SCF(int8 data1,int8 data2,int8 data3,int8 data4)//0x7B
{
   REFRESH_SECTOR_SCF(data1,data2,data3,data4);
   delay_ms(500);
   fprintf(PC,"transfer:%x%x%x%x\r\n",data1,data2,data3,data4);
   int32 address = make32(data1,data2,data3,data4);
   address = address - (address%SECT);
   output_low(PIN_C4);
   output_low(PIN_A5);
   for(int32 i = 0; i < SECT; i++)
   {
      WRITE_DATA_BYTE_SCF(address + i, READ_DATA_BYTE_SMF(address + i));
   }
   delay_ms(100);
   output_high(PIN_C4);
   
   int16 DATA = address/SECT;//maximum 2048
   int8 D1 = DATA >> 8;
   int8 D2 = DATA;
   
   SAVE_SAT_LOG(0x7B, D1, D2);//0x7B+16bit sector info
   return;
}

