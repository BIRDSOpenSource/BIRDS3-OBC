#include <16F1789.h>
BYTE next_in = 0;
int t;
#device adc=16
#fuses NOMCLR,WDT,NOPUT,NOBROWNOUT
#use delay(crystal=16mhz)
#include <COMPICREG.h>              // com pic registers header
#include <COMPIC_MAIN_SRI.h>            // com main header file

int auto_flag1 = 1;
int counti = 0;

#define CHECK_CMD (cr==pk) && ((sat_id == 0xAA)||(sat_id == 0xBB))     // (cr == pk) checking crc, AA is SL sat id, 
                                                                       // BB is the common id for all sat

#int_rda
void serial_isr()
{
   buffer[next_in]=fgetc(tr_cp);
   t=next_in;
   next_in=(next_in+1)% 17 ;   
} 


void main(void)
{
settings();                    // Every initial settings belongs to com pic is included in this function
Setup_wdt(wdt_256s);
delay_ms(5000);                  // Just delay before start
delay_ms(30000);                  // Just delay before start

RX_ON();

    While(true)
    { 
      //______________(1)______CW_RELATED_TASKS__________________________________________<<
       RSSI_VALUE = RSSI_READ();        // Reading satellite noise level
       
       COMUNICATRE_WITH_MAIN_PIC(1,0);  //__Asking CW Data
       WAIT_FOR_MAIN_PIC_RESPONSE();  
    
       SEND_CW();
       restart_wdt();                  //***********************//
   
   
   
   //______________(2)____AUTOMATIC_PACKET_TRANSMISSION_RELATED_TASKS__________________<<
      if((MAIN_TO_COM[14] == 0x77) && (counti == 0))
      {       
         rcv_byte = 0x77; 
         auto_flag1 = 1;
         counti = 1;  
      }
      
      if(MAIN_TO_COM[14] == 0x00)
      {       
         rcv_byte = 0x00; 
         auto_flag1 = 1;
         counti = 0;
         auto_address = 0;
      }
      
    //-----------------------------
      if(rcv_byte == 0x77)
      {  
         if(auto_flag1 == 1)
         {
            auto_address = make32(MAIN_TO_COM[15],MAIN_TO_COM[16],MAIN_TO_COM[17],MAIN_TO_COM[18]);
            Data_size    = make32(MAIN_TO_COM[19],MAIN_TO_COM[20],MAIN_TO_COM[21],MAIN_TO_COM[22]);
            
            COMUNICATRE_WITH_MAIN_PIC(2,1);  //getting SFM access 
            delay_ms(3000);
            FLASH_COPY(auto_address,Data_size);
            auto_flag1 = 0;
         }
      
       TX_ON();
       delay_ms(1000);
       Send_Data_Packets(auto_address,0,5,1);
       delay_ms(500);
       RX_BFR_ZERO();
       RX_ON();
      } 
    //___________________________________________________________________________________
    
    
    //___________________________________________________________________________________
     restart_wdt();            //***********************//
     RX_ON();
    //___________________________________________________________________________________
    
    
    //______________(3)____RECEIVING_MODE_START_HERE___________________________________<<   
     for (int i = 0; i < 45; i++)
     {
      BUFFER_CORECTION();
       
       if(((sat_id == 0xCC)||(sat_id == 0xDD)) && (CMD_PCKT[2] == 0x90) )
       {
        Delay_for_data_downlink();                         // CC,DD  CMD_PCKT[1] sleeping
       }
       
       if(CHECK_CMD)
       {
        if(CMD_PCKT[2] == 0xAA) { Sync(); }                // 38              sync or delay
              
        PACKET_DOWNLOAD();                                 // 10 , 11
        LENGTH_DOWNLOAD();                                 // 13 , 14
        ONE_PCKT_DOWNLOAD();                               // 15 , 16
        Data_down_from_SFM_with_access();                  // 35
        SET_OF_PACKETS_DOWNLOAD_WITH_ACCESS();             // 44
        
        SET_OF_PACKETS_DOWNLOAD();                         // 40,41           40 00 00 00 00    00 00 setsize setnumber      
        
        SEND_COM_TEMP_RSSI_TO_GS();                        // 30              30 00 00 00 00    00 00 00 00 
        FM_COPY_AND_DATA_DOWN_FROM_CFM();                  // 27              25 00 00 00 00    00 00 packet size
        
        
        FM_COPY();                                         // 17  without getting accees
        FM_COPY_WITH_ACCESS();                             // 22
        GET_SFM_ACCESS_GIVEN_TIME();                       // 42
        
        CMD_TO_MPIC();                                     // A0
        CW_SEND_BGSC();                                    // 21
        CW_UPDATE_MISSION();                               // 20 
       }   
       
      next_in=0;
      RX_BFR_ZERO();
      delay_ms(2000);
      restart_wdt();          //***********************//}  
     } // for loop finish here

      
   } //while loop finish here
}
