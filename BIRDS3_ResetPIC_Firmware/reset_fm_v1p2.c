#include <16F1789.h>
#fuses NOWDT, NOBROWNOUT, NOPROTECT
#device ADC=8;
#use delay(crystal=16MHz,clock=16MHz)
#include <PIC16F1789_registers.h>

#use rs232(baud=9600,parity=N,xmit=PIN_B7,rcv=PIN_B6,bits=8,stream=PORT1)
#use rs232(baud=9600,parity=N,xmit=PIN_D4,rcv=PIN_D5,bits=8,stream=MPIC)

unsigned int16 batvol = 0;
int8 sec = 0;
int8 min = 0;
int8 hou = 0;
unsigned int16 day_num = 0;
int8 resetData[11] = {0};
int8 MAIN_ACK;
int16 main_count = 0;
int8 res_flag = 0;
int16 modev = 2;

#include<power_enable.c>
#include<read_current.c>
#include<define_fun.c>

#INT_TIMER0
void TIMER0_isr() {  

if(!get_timer0()){
               RB2 = 1;
               delay_us(5);
               RB2 = 0; 
            }
     
}

//INTERRUPT
#INT_TIMER1
void TIMER1_isr() {   
      set_timer1(0x8000);
      main_count++;
      
      if (sec < 59){
      sec++;
      }else{
         sec = 0;
         min++;
      }
      if (min == 60){
         min = 0;
         hou++;
      }
      
      if (hou == 24){
         hou = 0;
         day_num++;
         res_flag = 1;
      }
      
      fprintf(PORT1,"%02ld", day_num);
      fprintf(PORT1,":%02d", hou);
      fprintf(PORT1,":%02d", min);
      fprintf(PORT1,":%02d\r\n", sec);
      fprintf(PORT1,"maincount:%04ld\r\n", main_count);
}
 

//MAIN
void main() {

   TRISC2 = 0; RC2 = 0;
   TRISC3 = 0; RC3 = 0;
   TRISC4 = 0; RC4 = 0;
   TRISC5 = 0; RC5 = 0;
   
   TRISD0 = 0; RD0 = 0;
   TRISD1 = 0; RD1 = 0;
   TRISD2 = 0; RD2 = 0;
   TRISD3 = 0; RD3 = 0;
   TRISD6 = 0; RD6 = 0;
   TRISD7 = 0; RD7 = 0;
   
   TRISB2 = 0; RB2 = 0;
   TRISB5 = 0; RB5 = 1;       
    
   SETUP_ADC(ADC_CLOCK_INTERNAL);
   SETUP_ADC_PORTS(sAN0|sAN1|sAN2|sAN3|sAN4|sAN5|sAN6);
   
   clear_interrupt(int_timer1);
   setup_timer_1(T1_EXTERNAL | T1_DIV_BY_1);
   T1OSCEN = 1;
   enable_interrupts(INT_TIMER1);
   enable_interrupts(INT_TIMER0);
   enable_interrupts(GLOBAL);
   
   set_timer1(0x8000);
 
   setup_timer_0(RTCC_DIV_256 | RTCC_INTERNAL);
   set_timer0(0xFF);

   fprintf(PORT1,"*********************************************************\n\r");
   fprintf(PORT1,"******     THIS IS RESET PIC FM VERSION S/W     *********\n\r");
   fprintf(PORT1,"*********************************************************\n\r");

   day_read(); 
   
   if(make16( read_eeprom(day1H),read_eeprom(day1L) ) == -1)
   {
      day_num = 0;
      day_num_write();
   }
   
   
   if (dy[0]==dy[1] && dy[1]==dy[2] && dy[2]==dy[3] && dy[3]==dy[4] && dy[4]==dy[5] && dy[5]==dy[6] && dy[6]==dy[7] && dy[7]==dy[8] && dy[8]==dy[9])
   {
      day_num = dy[0];   
      //fprintf(PORT1,"day number1###### =%ld\n\r",day_num);
   }
   
   else 
   {
      day_num = return_mode();
      // fprintf(PORT1,"day number1***** =%ld\n\r",day_num);
   }
 
   delay_ms(500);
   
   first_run_mode();
   
  while (TRUE)
      {
      
     resetData[0] = 0x8E;
     resetData[1] = sec;
     resetData[2] = min;
     resetData[3] = hou;   
      
     resetData[4] = (day_num >> 8) & 0xFF;
     resetData[5] = day_num & 0x00FF; 

     adcreading_reset();

     if((min==30) && (hou <=1))
     {
      RD6 = 1; // Switch enable for COM PIC
      RC2 = 1; //Buckboost enable for Unreg #1
      delayloop(60);
     }

     
   //===============================UART WITH MAIN PIC==========================      
         for(int32 num = 0; num < 100; num++)                                                      
         {                                                                                             //
           if(kbhit(MPIC))
           {
               MAIN_ACK = fgetc(MPIC);   //UART receive from main pic
               break;//
           }                                                                                          //
                                                                                                     //
         }                                                                                             //
//======================================================================================================= 

         if(MAIN_ACK == 0x28) //receiving command from main pic for sending reset data
         {   
           
            fprintf(PORT1,"******Sending data to main *********\n\r");
            MAIN_ACK=0;
            main_count = 0;
            
            for(int i=0;i<11;i++)
            { 
               fputc(resetData[i],MPIC);  
               //delay_ms(10);              
            }
            fprintf(PORT1,"******data sent *********\n\r");
          }       
            
         else if(MAIN_ACK == 0xBC) //receiving command from main pic for turning on Unreg 2
          {            
             fprintf(PORT1,"******Unreg ON *********\n\r");
             MAIN_ACK=0;
             main_count = 0;
             RC5 = 1;
             fputc(0xCB,MPIC); //unreg 2 On signal to Main PIC
             delayloop(45);
             RC5 = 0;
             fprintf(PORT1,"******Unreg OFF *********\n\r");
          }      
          
        else if(MAIN_ACK == 0x27) //receiving command from main pic for resetting the system
          {            
             MAIN_ACK=0;
             main_count = 0;
             fprintf(PORT1,"******Received command for satellite reset *********\n\r");
             reset_mode();
          }   
          
        else if(main_count >= 7200)
           {
            resetMain();
           }
            
  
//__(1)___________________mode selection part______________________________________________________________________<<
     for(int k=0; k<100; k++)
       {
         SET_ADC_CHANNEL(0); //  routing nth channel to adc
         delay_us(1);
         batvol = batvol + READ_ADC(); 
         delay_us(2);
        }
        
       batvol= batvol/100;

      //fprintf(PORT1,"batt vol:%04lX\n\r",batvol);
      
      if((batvol <= 0x87) &&  (modev == 2) )             // safe mode   --- 0
      {
            RD1 = 0; //3V3#1 turned off first (to prevent FAB interrupt)
            do
            {
                 fputc(0xAA,MPIC);
                 
            }while(fgetc(MPIC) == 0x27);
            
            //tel mpic to wait delay 20ms
            fprintf(PORT1,"******Received acknoledgement for safe mode*********\n\r");

            MAIN_ACK =0;
            main_count = 0;   
            safe_mode();
            modev = 0;
      }
      
      else if((batvol >= 0x87) && (modev == 0 ) )            // normal mode   --- 2
      {
         normal_mode();
         modev = 2;
      }
      
      batvol = 0;
          
 //=================================24hr reset part=========================================================================      
         if(res_flag == 1)
         {
            day_num_write();
            RD1 = 0; //3V3#1 turned off first (to prevent FAB interrupt)
            do
            {
               fputc(0xAA,MPIC);
                 
            }while(fgetc(MPIC) == 0x27);
            
            //tel mpic to wait delay 20ms
            fprintf(PORT1,"******Received acknoledgement for reset*********\n\r");
            
             MAIN_ACK =0;
             main_count = 0;
             reset_mode();
             res_flag = 0;
         }            
               
      }
  
}


