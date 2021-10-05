//from here the mission________________________________________________________

//_______________________PAcket_download_______________________________________
Void PACKET_DOWNLOAD()
{
    if(cmd_Pckt[3]==0x10 || cmd_Pckt[3]==0x11 )                 //packets download
   {
    unsigned int32 start_adress = make32(cmd_Pckt[4],cmd_Pckt[5],cmd_Pckt[6],cmd_Pckt[7]);
    unsigned int32 pckt_num     = make32(cmd_Pckt[8],cmd_Pckt[9],cmd_Pckt[10],cmd_Pckt[11]);
    
    TX_ON();
    Delay_ms(1000);
       if(cmd_Pckt[3]==0x10)
       {
       Send_Data_Packets(start_adress,0,pckt_num,1);
       RX_BFR_ZERO();
       }
       
       if(cmd_Pckt[3]==0x11)
       {
       Send_Data_Packets(start_adress,0,pckt_num,0);
       RX_BFR_ZERO();
       }
    Delay_ms(500);    
    RX_ON();
   }
    
   return;
}


//____________________Lenth_Download___________________________________________
Void LENGTH_DOWNLOAD()
{
   if(cmd_Pckt[3]==0x13 || cmd_Pckt[3]==0x14 )                 // length download
   {
    unsigned int32 start_adress = make32(cmd_Pckt[4],cmd_Pckt[5],cmd_Pckt[6],cmd_Pckt[7]);
    unsigned int32 length       = make32(cmd_Pckt[8],cmd_Pckt[9],cmd_Pckt[10],cmd_Pckt[11]);
    
    TX_ON();
    Delay_ms(1000);
    
       if(cmd_Pckt[3]==0x13)
       {
       Send_Data_Packets(start_adress,length,0,1);
       RX_BFR_ZERO();
       }
       
       if(cmd_Pckt[3]==0x14)
       {
       Send_Data_Packets(start_adress,length,0,0);
       RX_BFR_ZERO();
       }
       
    Delay_ms(500);    
    RX_ON();
   }  
   
   
   return;

}

//____________________One_packet_download_______________________________________

Void ONE_PCKT_DOWNLOAD()
{
   if(cmd_Pckt[3]==0x15 || cmd_Pckt[3]==0x16)                 // one packet download
   {
    TX_ON();
    Delay_ms(1000);
    
       if(cmd_Pckt[3]==0x15)
       {
       Send_a_Packet(1);
       RX_BFR_ZERO();
       }
       
       if(cmd_Pckt[3]==0x16)
       {
       Send_a_Packet(0);
       RX_BFR_ZERO();
       }   
       
    Delay_ms(500);    
    RX_ON();
   } 
   
   return;
}

//__________________________SET of packet download________________________________________
Void SET_OF_PACKETS_DOWNLOAD()
{
   if(cmd_Pckt[3]== 0x40 || cmd_Pckt[3] == 0x41 )                 //packets download
   {
    unsigned int32 start_adress  = make32(cmd_Pckt[4],cmd_Pckt[5],cmd_Pckt[6],cmd_Pckt[7]);
     
    TX_ON();
    Delay_ms(1000);
       if(cmd_Pckt[3]==0x40)
       {
        PCKT_SET_SEND(start_adress,cmd_Pckt[11],cmd_Pckt[10],1);
        RX_BFR_ZERO();
       }
       
       else if(cmd_Pckt[3]==0x41)
       {
        PCKT_SET_SEND(start_adress,cmd_Pckt[11],cmd_Pckt[10],0);
        RX_BFR_ZERO();
       }
       
    Delay_ms(500);    
    RX_ON();  
   }
   
   return;
}

//set of acess downpppppppppppppppppppppppppppppppppppppppppppppppppppppppppppp
Void SET_OF_PACKETS_DOWNLOAD_WITH_ACCESS()
{
   if(cmd_Pckt[3]== 0x44)                 //packets download
   {
    unsigned int32 start_adress  = make32(cmd_Pckt[4],cmd_Pckt[5],cmd_Pckt[6],cmd_Pckt[7]);
    COMUNICATRE_WITH_MAIN_PIC(2,cmd_Pckt[8]);  //getting access
    
    TX_ON();
    Delay_ms(1000);
       
    PCKT_SET_SEND(start_adress,cmd_Pckt[11],cmd_Pckt[10],0);
    RX_BFR_ZERO();
   
       
    Delay_ms(500);    
    RX_ON();  
   }
   
   return;
}


//________________________FM_data_copy_________________________________________
Void FM_COPY()
{
   if(cmd_Pckt[3]==0x17)                 // copy
   {
    SEND_SUCCESS_ACK();
    unsigned int32 start_adress = make32(cmd_Pckt[4],cmd_Pckt[5],cmd_Pckt[6],cmd_Pckt[7]);
    unsigned int32 length       = make32(cmd_Pckt[8],cmd_Pckt[9],cmd_Pckt[10],cmd_Pckt[11]);
    
    FLASH_COPY(start_adress,length);
    
    SEND_SUCCESS_ACK();
    RX_BFR_ZERO(); 
    
    RX_ON();
   }
    
   return;
}

//________________________Copy With Access_________________________________________________
Void FM_COPY_WITH_ACCESS()
{
   if(cmd_Pckt[3]==0x22)                 // copy
   {
    unsigned int32 start_adress = make32(cmd_Pckt[4],cmd_Pckt[5],cmd_Pckt[6],cmd_Pckt[7]);
    unsigned int16 packet       = make16(cmd_Pckt[10],cmd_Pckt[11]);                         //as 256byte packets
    
    COMUNICATRE_WITH_MAIN_PIC(2,cmd_Pckt[8]);  //getting accees
    WAIT_FOR_MAIN_PIC_RESPONSE();
    
       if(MAIN_TO_COM[12] == 0x66)
       {
        SEND_SUCCESS_ACK();          // success of access
        delay_ms(1000);
        FLASH_COPY(start_adress,packet*256);
        SEND_SUCCESS_ACK();          // success of copying
       }
       
       else SEND_NOT_SUCCESS_ACK();
 
    RX_BFR_ZERO(); 
    RX_ON();
   }
   
   return;
}



//________________________Copy With Access_________________________________________________
Void FM_COPY_AND_DATA_DOWN_FROM_CFM()
{
   if(cmd_Pckt[3]==0x27)                 // copy
   {
    unsigned int32 start_adress = make32(cmd_Pckt[4],cmd_Pckt[5],cmd_Pckt[6],cmd_Pckt[7]);
    unsigned int16 packet       = make16(cmd_Pckt[10],cmd_Pckt[11]);                         //as 81byte packets
    
    COMUNICATRE_WITH_MAIN_PIC(2,cmd_Pckt[8]);  //getting accees for 1 min
    WAIT_FOR_MAIN_PIC_RESPONSE();
    
    
       if(MAIN_TO_COM[12] == 0x66)
       {
        FLASH_COPY(start_adress,packet*81);
        
        TX_ON();
        Delay_ms(1000);
        Send_Data_Packets(start_adress,0,packet,1); 
        Delay_ms(500);
        RX_ON();
       }
       
       else
       { 
        TX_ON();
        Delay_ms(1000);
        SEND_NOT_SUCCESS_ACK();
        Delay_ms(500);
        RX_ON();
       }
 
    RX_BFR_ZERO();
   }
   
   return;
}

//999999999999999999999999999999999999999999999999999999999999999999999999999999999999999
//________________________Copy With Access_________________________________________________
Void Data_down_from_SFM_with_access()
{
   if(cmd_Pckt[3]==0x35)                 // copy
   {
    unsigned int32 start_adress = make32(cmd_Pckt[4],cmd_Pckt[5],cmd_Pckt[6],cmd_Pckt[7]);
    unsigned int16 packet       = make16(cmd_Pckt[10],cmd_Pckt[11]);                         //as 81byte packets
    
    COMUNICATRE_WITH_MAIN_PIC(2,cmd_Pckt[8]);  //getting access 
    WAIT_FOR_MAIN_PIC_RESPONSE();
    
    Delay_ms(2000);
    
       if(MAIN_TO_COM[12] == 0x66)
       {    
        
        TX_ON();
        Delay_ms(1000);
        Send_Data_Packets(start_adress,0,packet,0); 
        Delay_ms(500);
        RX_ON();
       }
 
    RX_BFR_ZERO();
   }
   
   return;
}


void sync()
{
 unsigned int8 delay = CMD_PCKT[11];
 
   if(delay <= 120)
   {
    SEND_SUCCESS_ACK();
      for(int i = 0; i <delay ; i++)
      {
         delay_ms(5000);
         restart_wdt();          //***********************//} 
      }
   }
   
   else {SEND_NOT_SUCCESS_ACK();}
   
 RX_BFR_ZERO();     
 return;
}

// ___________________CMD to main PIC___________________________________________________
Void CMD_TO_MPIC()
{
   if(CMD_PCKT[3]==0xA0)    // main pic commands
    {      
       COMUNICATRE_WITH_MAIN_PIC(0,0);   
       WAIT_FOR_MAIN_PIC_RESPONSE();
       
       if(MAIN_TO_COM[12] == 0x66) SEND_SUCCESS_ACK();
       
       if(MAIN_TO_COM[12] == 0x00) SEND_NOT_SUCCESS_ACK();
       
       RX_BFR_ZERO();
    }
    
    return;
}

//_________________________GET_FM_ACCESS_________________________________________________
Void GET_SFM_ACCESS_GIVEN_TIME()
{
   if(CMD_PCKT[3]==0x42 && ((CMD_PCKT[11]==0x01) || (CMD_PCKT[11]==0x05) || (CMD_PCKT[11]==0x0A) ))    // main pic comands
    {   
       
       
       COMUNICATRE_WITH_MAIN_PIC(2,cmd_Pckt[11]);
       
       
       WAIT_FOR_MAIN_PIC_RESPONSE();
       
       if(MAIN_TO_COM[12] == 0x66) SEND_SUCCESS_ACK();
       
       if(MAIN_TO_COM[12] == 0x00) SEND_NOT_SUCCESS_ACK();
       
       
       RX_BFR_ZERO();
    }
    
    return;
}


//__________________________SPECIAL______________________________________________________
Void Delay_for_data_downlink()
{
 SEND_SUCCESS_ACK();
    
      for(int i = 0; i <30; i++)
      {
         delay_ms(20000);
         restart_wdt();          //***********************//} 
      }
      
 RX_BFR_ZERO();     
 return;
}




//__________________force CW send________________________________________________________
Void CW_SEND_BGSC()
{

   if(CMD_PCKT[3]==0x21)    // cw send
    {
       CW_ON();
       
       CW_PCKT();
       delay_ms(500);
       
       RX_ON();
       RX_BFR_ZERO();
    }
    
    return;
}

//__________________CW update mission____________________________________________________
void CW_UPDATE_MISSION()
{
   if(CMD_PCKT[3]==0x20)     // cw update
   {
       CF_SECTOR_ERASE(0x00000000);
       
       CF_BYTE_WRITE(0x00000000,CMD_PCKT[10]);   //should be ff
       
       CF_BYTE_WRITE(0x00000001,CMD_PCKT[4]);
       CF_BYTE_WRITE(0x00000002,CMD_PCKT[5]);
       CF_BYTE_WRITE(0x00000003,CMD_PCKT[6]);
       CF_BYTE_WRITE(0x00000004,CMD_PCKT[7]);
       CF_BYTE_WRITE(0x00000005,CMD_PCKT[8]);
       CF_BYTE_WRITE(0x00000006,CMD_PCKT[9]);
       
       
       
       SEND_SUCCESS_ACK();
       RX_BFR_ZERO();
    }
 
 return;
}


//_______________________________SEND temp rssi__________________________________________
Void SEND_COM_TEMP_RSSI_TO_GS()
{
   if(CMD_PCKT[3]==0x30)    
    {      
       SEND_COM_DATA();
       RX_BFR_ZERO();
    }
    
    return;
}


//_______________________________CW_send_________________________________________________
Void SEND_CW()
{
   CW_ON();   
   CW_PCKT();
   RX_ON();
   RX_BFR_ZERO();  
   return;
}


