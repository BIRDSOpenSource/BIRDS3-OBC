void delayloop(int32 seconds)     
{ 
 int32 count = 1; 
  
   while(count <= seconds) 
   { 
    delay_ms(1000); 
    count++; 
   } 
    
 return; 
}

void first_run_mode(void)  //all missions ON
{

 fprintf(PORT1,"******Reset first run enabled*********\n\r");
 RD6 = 0; // Switch disable for COM PIC
 //fprintf(PORT1,"******COM PIC ON *********\n\r");
 RD7 = 1; // Switch enable for Main PIC
 //fprintf(PORT1,"******Main PIC ON *********\n\r");
 
 RC4 = 1; // Buckboost enable for 3V3 #1 
 RD1 = 1; // Switch enable for 3V3#1
 
 RD3 = 1; // Buckboost enable for 3V3 #2
 RD0 = 1; // Switch enable for 3V3 #2
  
 RC2 = 0; //Buckboost disable for Unreg #1
 
 RC5 = 0; //Buckboost enable for Unreg #2


 return;
 
}

void normal_mode(void)  //all missions ON
{

 fprintf(PORT1,"******Normal mode enabled*********\n\r");
 //RD6 = 1; // Switch enable for COM PIC
 //fprintf(PORT1,"******COM PIC ON *********\n\r");
 RD7 = 1; // Switch enable for Main PIC
 //fprintf(PORT1,"******Main PIC ON *********\n\r");
 
 RC4 = 1; // Buckboost enable for 3V3 #1 
 RD1 = 1; // Switch enable for 3V3#1
 
 RD3 = 1; // Buckboost enable for 3V3 #2
 RD0 = 1; // Switch enable for 3V3 #2
  
 //RC2 = 1; //Buckboost enable for Unreg #1
 
 RC5 = 0; //Buckboost enable for Unreg #2


 return;
 
}


void safe_mode(void)                //reset pic and com pic ON
{
 fprintf(PORT1,"******Safe mode enabled*********\n\r");
 //RD6 = 1; // Switch enable for COM PIC
 //fprintf(PORT1,"******COM PIC ON *********\n\r");
 RD7 = 0; // Switch enable for Main PIC
 //fprintf(PORT1,"******Main PIC OFF *********\n\r");
 
 RC4 = 1; // Buckboost enable for 3V3 #1 
 RD1 = 0; // Switch enable for 3V3#1
 
 RD3 = 1; // Buckboost enable for 3V3 #2
 RD0 = 0; // Switch enable for 3V3 #2
  
 //RC2 = 1; //Buckboost enable for Unreg #1

  return;
 
}

void reset_mode(void)
{
 fprintf(PORT1,"******Reset mode ON *********\n\r"); 
 RD6 = 0; // Switch disable for COM PIC
 RD7 = 0; // Switch disable for Main PIC
 RD1 = 0; // Switch disable  for 3V3#1
 RD0 = 0; // Switch disable for 3V3 #2
 RC2 = 0; //Buckboost disable for Unreg #1
 fprintf(PORT1,"******OFF *********\n\r"); 
 delay_ms(5000);
 fprintf(PORT1,"******ON *********\n\r"); 
 RD6 = 1; // Switch enable for COM PIC
 RD7 = 1; // Switch enable for Main PIC
 
 RC4 = 1; // Buckboost enable for 3V3 #1 
 RD1 = 1; // Switch enale for 3V3#1
 
 RD3 = 1; // Buckboost enable for 3V3 #2
 RD0 = 1; // Switch enable for 3V3 #2
  
 RC2 = 1; //Buckboost enable for Unreg #1
 
 RC5 = 0; //Buckboost enable for Unreg #2
 
 //res_flag = 0;
 
 return; 
}



void resetMain(void)
{
 RD7 = 0; // Switch disable for Main PIC
 delay_ms(1000);
 RD7 = 1; // Switch enable for Main PIC
 fprintf(PORT1,"******Main Reset *********\n\r");
 main_count  = 0;

 return;
}


