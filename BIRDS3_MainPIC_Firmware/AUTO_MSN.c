#define loop_day 70
int16 AD = 0;//DATE DATA
int16 D = 0;//Mainichi address no hozon basyo wo tuika suru. Kihonteki ni hinichi ga hueru to 5 packet saki ni susumu.
//everyday change the DL address. nomally, inclease 5 packet for each day
int8 ADH = 0;//for satellite log(int16 date high)
int8 ADL = 0;//for satellite log(int16 date low)
int32 DL_AD = 0;//Data size to increase when day changed

//@@@@@@@@@@@ HIGH SAMPLING@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
void DAY0()//2 hour high sampling sensor
{
   SAVE_SAT_LOG(0xFA,0x11,0x11);//01,01: automatical mission
   FIRST_HSSC_DONE = 1;
   STORE_FLAG_INFO();//save passed days
   WRITE_FLAG_to_EEPROM();
   HIGHSAMP_SENSOR_COLLECTION(1440);
   STORE_ADRESS_DATA_TO_FLASH();
   
   AD = PASSED_DAYS % loop_day;//1,2,3,4
   ADH = AD << 8;
   ADL = AD;
   SAVE_SAT_LOG(0xFD,ADH,ADL);//11,11: automatical mission
   UPDATE_ACK_for_COM(0x66,0x77,SECT*1639,405);//118byte*3600*2/5 (about 2.59sector) 65536*1639
   
   return;
}

void DAY1to4()//HIGH SAMPLING DL
{
   AD = PASSED_DAYS % loop_day;//1,2,3,4
   ADH = AD << 8;
   ADL = AD;
   D = AD;
   
   DL_AD = 220*D;
   DL_AD = DL_AD * 81;
   ///////////////PACKET/////////////////
   //220-224, 440-444, 660-664, 880-884
   //////////////////////////////////////
   
   
   SAVE_SAT_LOG(0xFD,ADH,ADL);//11,11: automatical mission
   UPDATE_ACK_for_COM(0x66,0x77,SECT*1639+DL_AD,405);//65536*8
   ADH=0; ADL=0; DL_AD=0; D=0;
   return;
}

void DAY1to4_ADDRESS()//HIGH SAMPLING DL
{
   AD = PASSED_DAYS % loop_day;//1,2,3,4
   ADH = AD << 8;
   ADL = AD;
   D = AD;
   
   DL_AD = 220*D;
   DL_AD = DL_AD * 81;
   ///////////////PACKET/////////////////
   //220-224, 440-444, 660-664, 880-884
   //////////////////////////////////////
   
   
   UPDATE_ACK_for_COM(0x66,0x77,SECT*1639+DL_AD,405);//65536*8
   ADH=0; ADL=0; DL_AD=0; D=0;
   return;
}

void DAY20to25()//20-25day
{
   AD = PASSED_DAYS % loop_day;//20,21,22,23,24,25
   ADH = AD << 8;
   ADL = AD;
   
   D = AD - 15;//5,6,7,8,9,10
   DL_AD = 220*D;
   DL_AD = DL_AD * 81;
   //////////////////////////////PACKET////////////////////////////////
   //1100-1104, 1320-1324, 1540-1544, 1760-1764, 1980,1984, 2200-2204
   ////////////////////////////////////////////////////////////////////
   
   SAVE_SAT_LOG(0xFD,ADH,ADL);//11,11: automatical mission
   UPDATE_ACK_for_COM(0x66,0x77,SECT*1639+DL_AD,405);//65536*8
   ADH=0; ADL=0; DL_AD=0; D=0;
   return;
}

void DAY20to25_ADDRESS()//20-25day
{
   AD = PASSED_DAYS % loop_day;//20,21,22,23,24,25
   ADH = AD << 8;
   ADL = AD;
   
   D = AD - 15;//5,6,7,8,9,10
   DL_AD = 220*D;
   DL_AD = DL_AD * 81;
   //////////////////////////////PACKET////////////////////////////////
   //1100-1104, 1320-1324, 1540-1544, 1760-1764, 1980,1984, 2200-2204
   ////////////////////////////////////////////////////////////////////
   
   UPDATE_ACK_for_COM(0x66,0x77,SECT*1639+DL_AD,405);//65536*8
   ADH=0; ADL=0; DL_AD=0; D=0;
   return;
}

//@@@@@@@@@@@ CAM MISSION @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
void DAY5()//CAM MSN
{
   SAVE_SAT_LOG(0xC0,0x11,0x11);//11,11: automatical mission
   AUTO_CAM_DONE = 1;
   STORE_FLAG_INFO();//save flag
   WRITE_FLAG_to_EEPROM();
   CAM_TEST_OPERATION(0xA0);
   STORE_ADRESS_DATA_TO_FLASH();
   
   AD = PASSED_DAYS % loop_day;//1,2,3,4
   ADH = AD << 8;
   ADL = AD;
   SAVE_SAT_LOG(0xCD,ADH,ADL);//11,11: automatical mission
   UPDATE_ACK_for_COM(0x66,0x77,SECT*8,405);//65536*8
   return;
}

void DAY6to9()//CAM DL 1
{
   AD = PASSED_DAYS % loop_day;//6,7,8,9
   ADH = AD << 8;
   ADL = AD;
   
   D = AD - 5;//1,2,3,4
   DL_AD = D*5;
   DL_AD = DL_AD * 81;
   //////////PACKET////////////
   //5-9, 10-14, 15-19, 20-24
   ////////////////////////////
   
   SAVE_SAT_LOG(0xCD,ADH,ADL);//11,11: automatical mission
   UPDATE_ACK_for_COM(0x66,0x77,SECT*8+DL_AD,405);//65536*8
   ADH=0; ADL=0; DL_AD=0; D=0;
   return;
}

void DAY6to9_ADDRESS()//CAM DL 1
{
   AD = PASSED_DAYS % loop_day;//6,7,8,9
   ADH = AD << 8;
   ADL = AD;
   
   D = AD - 5;//1,2,3,4
   DL_AD = D*5;
   DL_AD = DL_AD * 81;
   //////////PACKET////////////
   //5-9, 10-14, 15-19, 20-24
   ////////////////////////////
   
   UPDATE_ACK_for_COM(0x66,0x77,SECT*8+DL_AD,405);//65536*8
   ADH=0; ADL=0; DL_AD=0; D=0;
   return;
}

void DAY26to46()//CAM DL 1
{
   AD = PASSED_DAYS % loop_day;//26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46
   ADH = AD << 8;
   ADL = AD;
   
   D = AD - 21;
   DL_AD = D*5;
   DL_AD = DL_AD * 81;
   ///////////////////////PACKET/////////////////////////
   //25-29,   40-34,   35-39,   40-44,   45-49
   //50-54,   55-59,   60-64,   65-69,   70-74
   //75-79,   80-84,   85-89,   90-94,   95-99
   //100-104, 105-109, 110-114, 115-119, 120-124
   //125-129
   //----------------------------------------------------
   //total is 129 packet
   //////////////////////////////////////////////////////
   
   /////////////////////////////BYTE/////////////////////////
   //2025-2429, 2430-2834, 2835-3239, 3240-3644, 3645-4049
   //4050-4454, 4455-4859, 4860-5264, 5265-5669, 5670-6074
   //6075-6479, 6480-6884, 6885-7289, 7290-7694, 7695-8099
   //8100-8504, 8505-8909, 8910-9314, 9315-9719, 9720-10124
   //10125-10530
   //--------------------------------------------------------
   //total is about 10kB
   //////////////////////////////////////////////////////////
   
   SAVE_SAT_LOG(0xCD,ADH,ADL);//11,11: automatical mission
   UPDATE_ACK_for_COM(0x66,0x77,SECT*8+DL_AD,405);//65536*8
   ADH=0; ADL=0; DL_AD=0; D=0;
   return;
}

void DAY26to46_ADDRESS()//CAM DL 1
{
   AD = PASSED_DAYS % loop_day;//26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46
   ADH = AD << 8;
   ADL = AD;
   
   D = AD - 21;
   DL_AD = D*5;
   DL_AD = DL_AD * 81;
   ///////////////////////PACKET/////////////////////////
   //25-29,   40-34,   35-39,   40-44,   45-49
   //50-54,   55-59,   60-64,   65-69,   70-74
   //75-79,   80-84,   85-89,   90-94,   95-99
   //100-104, 105-109, 110-114, 115-119, 120-124
   //125-129
   //----------------------------------------------------
   //total is 129 packet
   //////////////////////////////////////////////////////
   
   /////////////////////////////BYTE/////////////////////////
   //2025-2429, 2430-2834, 2835-3239, 3240-3644, 3645-4049
   //4050-4454, 4455-4859, 4860-5264, 5265-5669, 5670-6074
   //6075-6479, 6480-6884, 6885-7289, 7290-7694, 7695-8099
   //8100-8504, 8505-8909, 8910-9314, 9315-9719, 9720-10124
   //10125-10530
   //--------------------------------------------------------
   //total is about 10kB
   //////////////////////////////////////////////////////////
   
   UPDATE_ACK_for_COM(0x66,0x77,SECT*8+DL_AD,405);//65536*8
   ADH=0; ADL=0; DL_AD=0; D=0;
   return;
}

//@@@@@@@@@@@ LoRa Demonstration MISSION @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
void DAY10()//LDM
{
   AUTO_LDM_DONE = 1;
   STORE_FLAG_INFO();//save flag
   WRITE_FLAG_to_EEPROM();
   
   SAVE_SAT_LOG(0xD0,0x11,0x11);//01,01: automatical mission
   DATACOLLECTION_TEST_OPERATION();
   delay_ms(3000);
   SAVE_SAT_LOG(0xD0,0x22,0x22);//01,01: automatical mission
   DATACOLLECTION_TEST_OPERATION();
   delay_ms(3000);
   SAVE_SAT_LOG(0xD0,0x33,0x33);//01,01: automatical mission
   DATACOLLECTION_TEST_OPERATION();
   delay_ms(3000);
   SAVE_SAT_LOG(0xD0,0x44,0x44);//01,01: automatical mission
   DATACOLLECTION_TEST_OPERATION();
   STORE_ADRESS_DATA_TO_FLASH();
   
   AD = PASSED_DAYS % loop_day;//6,7,8,9
   ADH = AD << 8;
   ADL = AD;
   SAVE_SAT_LOG(0xDD,ADH,ADL);//11,11: automatical mission
   UPDATE_ACK_for_COM(0x66,0x77,DC_STATUS_ADDRESS-332,332);//65536*162
   return;
}

void DAY11to14()//LDM DL
{
   SAVE_SAT_LOG(0xD0,0x11,0x11);//01,01: automatical mission
   DATACOLLECTION_TEST_OPERATION();
   delay_ms(3000);
   SAVE_SAT_LOG(0xD0,0x22,0x22);//01,01: automatical mission
   DATACOLLECTION_TEST_OPERATION();
   delay_ms(3000);
   SAVE_SAT_LOG(0xD0,0x33,0x33);//01,01: automatical mission
   DATACOLLECTION_TEST_OPERATION();
   delay_ms(3000);
   SAVE_SAT_LOG(0xD4,0x44,0x44);//01,01: automatical mission
   DATACOLLECTION_TEST_OPERATION();
   STORE_ADRESS_DATA_TO_FLASH();
   
   AD = PASSED_DAYS % loop_day;//11,12,13,14
   ADH = AD << 8;
   ADL = AD;
   SAVE_SAT_LOG(0xDD,ADH,ADL);
   UPDATE_ACK_for_COM(0x66,0x77,DC_STATUS_ADDRESS-332,332);//65536*8
   ADH=0; ADL=0; DL_AD=0; D=0;
   return;
}

void DAY11to14_ADDRESS()//LDM DL
{   
   AD = PASSED_DAYS % loop_day;//11,12,13,14
   ADH = AD << 8;
   ADL = AD;
   UPDATE_ACK_for_COM(0x66,0x77,DC_STATUS_ADDRESS-332,332);//65536*8
   ADH=0; ADL=0; DL_AD=0; D=0;
   return;
}

//@@@@@@@@@@@ ADCS MISSION @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
void DAY15()//ADCS MSN
{
   SAVE_SAT_LOG(0xA0,0x11,0x11);//01,01: automatical mission
   AUTO_ADCS_DONE = 1;
   STORE_FLAG_INFO();//save flag
   WRITE_FLAG_to_EEPROM();
   ADCS_TEST_OPERATION(5400,AUTO_ADCS_MSN);
   STORE_ADRESS_DATA_TO_FLASH();
   
   AD = PASSED_DAYS % loop_day;//11,12,13,14
   ADH = AD << 8;
   ADL = AD;
   SAVE_SAT_LOG(0xAD,ADH,ADL);//11,11: automatical mission
   UPDATE_ACK_for_COM(0x66, 0x77, SECT*1138, 405);//(9byte) + (initial 81byte) + (end 81byte)
   return;
}

void DAY16to19()//ADCS DL
{
   AD = PASSED_DAYS % loop_day;//11,12,13,14
   ADH = AD << 8;
   ADL = AD;
   D = AD - 15;
   DL_AD = 360*D;
   SAVE_SAT_LOG(0xAD,ADH,ADL);//11,11: automatical mission
   UPDATE_ACK_for_COM(0x66,0x77,SECT*1138+DL_AD,405);//65536*8
   ADH=0; ADL=0; DL_AD=0; D=0;
   return;
}

void DAY16to19_ADDRESS()//ADCS DL
{
   AD = PASSED_DAYS % loop_day;//11,12,13,14
   ADH = AD << 8;
   ADL = AD;
   D = AD - 15;
   DL_AD = 360*D;
   UPDATE_ACK_for_COM(0x66,0x77,SECT*1138+DL_AD,405);//65536*8
   ADH=0; ADL=0; DL_AD=0; D=0;
   return;
}

void DAY47to52()//ADCS DL
{
   AD = PASSED_DAYS % loop_day;//47,48,49,50,51,52
   ADH = AD << 8;
   ADL = AD;
   D = AD - 42;//5,6,7,8,9,10
   
   DL_AD = 360*D;
   DL_AD = DL_AD * 81;
   /////////////////////////////PACKET////////////////////////////////
   //1800-1804, 2160-2164, 2520-2524, 2880-2884, 3240-3244, 3600-3604
   ///////////////////////////////////////////////////////////////////
   
   SAVE_SAT_LOG(0xAD,ADH,ADL);//11,11: automatical mission
   UPDATE_ACK_for_COM(0x66,0x77,SECT*1138+DL_AD,405);//65536*8
   ADH=0; ADL=0; DL_AD=0; D=0;
   return;
}

void DAY47to52_ADDRESS()//ADCS DL
{
   AD = PASSED_DAYS % loop_day;//47,48,49,50,51,52
   ADH = AD << 8;
   ADL = AD;
   D = AD - 42;//5,6,7,8,9,10
   
   DL_AD = 360*D;
   DL_AD = DL_AD * 81;
   /////////////////////////////PACKET////////////////////////////////
   //1800-1804, 2160-2164, 2520-2524, 2880-2884, 3240-3244, 3600-3604
   ///////////////////////////////////////////////////////////////////
   
   UPDATE_ACK_for_COM(0x66,0x77,SECT*1138+DL_AD,405);//65536*8
   ADH=0; ADL=0; DL_AD=0; D=0;
   return;
}

//@@@@@@@@@@@ SAT_LOG @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
void DAY53to59()//LOG DL
{
   AD = PASSED_DAYS % loop_day;//53,54,55,56,57,58,59
   ADH = AD << 8;
   ADL = AD;
   D = AD - 53;//0,1,2,3,4,5,6
   
   DL_AD = D;
   DL_AD = DL_AD * 81;
   /////////////////////////////PACKET//////////////////////////////
   //0-4, 5-10, 10-14, 15-19, 20-24, 25-29, 30-34
   /////////////////////////////////////////////////////////////////
   
   SAVE_SAT_LOG(0xAD,ADH,ADL);//11,11: automatical mission
   UPDATE_ACK_for_COM(0x66,0x77,SECT*6+DL_AD,405);//65536*8
   ADH=0; ADL=0; DL_AD=0; D=0;
   return;
}

void DAY53to59_ADDRESS()//LOG DL
{
   AD = PASSED_DAYS % loop_day;//53,54,55,56,57,58,59
   ADH = AD << 8;
   ADL = AD;
   D = AD - 53;//0,1,2,3,4,5,6
   
   DL_AD = D;
   DL_AD = DL_AD * 81;
   /////////////////////////////PACKET//////////////////////////////
   //0-4, 5-10, 10-14, 15-19, 20-24, 25-29, 30-34
   /////////////////////////////////////////////////////////////////
   
   UPDATE_ACK_for_COM(0x66,0x77,SECT*6+DL_AD,405);//65536*8
   ADH=0; ADL=0; DL_AD=0; D=0;
   return;
}

//@@@@@@@@@@@ NOMAL SAMPLING HK @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
void DAY60to69()//HK Download
{
   AD = PASSED_DAYS % loop_day;//60,61,62,63,64,65,66,67,68,69
   ADH = AD << 8;
   ADL = AD;
   D = AD - 53;//0,1,2,3,4,5,6,7,8,9
   
   DL_AD = 9798*D;
   DL_AD = DL_AD * 81;
   /////////////////////////////PACKET//////////////////////////////
   //    0-4,      9798-9802,  19596-19600, 29394-29398, 39192-39196
   //48990-48994, 58788-58792, 68586-68590, 78384-78388, 88182-88186
   /////////////////////////////////////////////////////////////////
   
   SAVE_SAT_LOG(0xAD,ADH,ADL);//11,11: automatical mission
   UPDATE_ACK_for_COM(0x66,0x77,SECT*98+DL_AD,405);//65536*8
   ADH=0; ADL=0; DL_AD=0; D=0;
   return;
}

void DAY60to69_ADDRESS()//HK Download
{
   AD = PASSED_DAYS % loop_day;//60,61,62,63,64,65,66,67,68,69
   ADH = AD << 8;
   ADL = AD;
   D = AD - 53;//0,1,2,3,4,5,6,7,8,9
   
   DL_AD = 9798*D;
   DL_AD = DL_AD * 81;
   /////////////////////////////PACKET//////////////////////////////
   //    0-4,      9798-9802,  19596-19600, 29394-29398, 39192-39196
   //48990-48994, 58788-58792, 68586-68590, 78384-78388, 88182-88186
   /////////////////////////////////////////////////////////////////
   
   UPDATE_ACK_for_COM(0x66,0x77,SECT*98+DL_AD,405);//65536*8
   ADH=0; ADL=0; DL_AD=0; D=0;
   return;
}

//@@@@@@@@@@@ ADDRESS CHANGE @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
void AUTO_MSN_AD_CHANGE()
{
   unsigned int16 D = PASSED_DAYS/loop_day;
   int8 UPDATE_TIME = (int8)D;
   SAVE_SAT_LOG(0xF9,UPDATE_TIME,UPDATE_TIME);//
   int32 DATA = 8192*D;//11byte*500 < 5500 (enough)
   SAT_LOG = SECT*6 + DATA;//2sector
   
   DATA = SECT*D;//1sector
   CAM_ADDRESS = SECT*8 + DATA;
   
   DATA = SECT*130;//124*3600*24*70/90 < 128sector
   DATA = DATA*D;
   FAB_HK_ADDRESS = SECT*98 + DATA;//1000sector
   
   DATA = SECT*5;//81*5400/1.5 < 5sector
   DATA = DATA*D;
   ADCS_SENSOR_ADDRESS = SECT*1138 + DATA;//500sector
   
   DATA = 405*5;//5packet*5
   DATA = DATA*D;
   DC_STATUS_ADDRESS = SECT*1638 + DATA;//1sector
   
   DATA = SECT*3;//124*3600*2/5 < 3sector
   DATA = DATA*D;
   HIGH_SAMP_HK_ADDRESS = SECT*1639 + DATA;//409sector
   
   STORE_ADRESS_DATA_TO_FLASH();//update
   
   return;
}


//@@@@@@@@@@@ AUTO MISSION EXECUTING FUNCTION @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void AUTOMATICAL_MSN_EXE()
{
   //PASSED_DAYS = make16(reset_bffr[4],reset_bffr[5]);
   if(((PASSED_DAYS%loop_day) >= 0 && UPLINK_SUCCESS == 0) && FIRST_HSSC_DONE == 0)//automated high samp mission
   {
      DAY0();
   }else if(PASSED_DAYS < make16(reset_bffr[4],reset_bffr[5]))//if the date changed(next day)
   {
      PASSED_DAYS = make16(reset_bffr[4],reset_bffr[5]);//update passed days
      STORE_FLAG_INFO();//save passed days
      WRITE_FLAG_to_EEPROM();
      
      if(((PASSED_DAYS%loop_day) >= 1 && (PASSED_DAYS%loop_day) <= 4) && UPLINK_SUCCESS == 0){//1-4th day high samp mission download 
         DAY1to4();
      }else if(((PASSED_DAYS%loop_day) >= 5 && UPLINK_SUCCESS == 0) && AUTO_CAM_DONE == 0){//5th day (CAM MSN)
         DAY5();
      }else if(((PASSED_DAYS%loop_day) >= 6 && (PASSED_DAYS%loop_day) <= 9) && UPLINK_SUCCESS == 0){//download cam data
         DAY6to9();
      }else if(((PASSED_DAYS%loop_day) >= 10 && UPLINK_SUCCESS == 0) && AUTO_LDM_DONE == 0){//LDM and download
         DAY10();
      }else if(((PASSED_DAYS%loop_day) >= 11 && (PASSED_DAYS%loop_day) <= 14) && UPLINK_SUCCESS == 0){//LDM and download
         DAY11to14();
      }else if(((PASSED_DAYS%loop_day) >= 15 && UPLINK_SUCCESS == 0) && AUTO_ADCS_DONE == 0){//ADCS Mission and Download
         DAY15();
      }else if(((PASSED_DAYS%loop_day) >= 16 && (PASSED_DAYS%loop_day) <= 19) && UPLINK_SUCCESS == 0){//ADCS Download
         DAY16to19();
      }else if(((PASSED_DAYS%loop_day) >= 20 && (PASSED_DAYS%loop_day) <= 25) && UPLINK_SUCCESS == 0){//High sampling sensor collecting mission download
         DAY20to25();
      }else if(((PASSED_DAYS%loop_day) >= 26 && (PASSED_DAYS%loop_day) <= 46) && UPLINK_SUCCESS == 0){//CAM Mission Download
         DAY26to46();
      }else if(((PASSED_DAYS%loop_day) >= 47 && (PASSED_DAYS%loop_day) <= 52) && UPLINK_SUCCESS == 0){//ADCS Mission Data Download
         DAY47to52();
      }else if(((PASSED_DAYS%loop_day) >= 53 && (PASSED_DAYS%loop_day) <= 59) && UPLINK_SUCCESS == 0){//Satellite LOG Download
         DAY53to59();
      }else if(((PASSED_DAYS%loop_day) >= 60 && (PASSED_DAYS%loop_day) <= 69) && UPLINK_SUCCESS == 0){//Normal HK Download
         DAY60to69();
      }
   }
   if(BC_ATTEMPT_FLAG == 0)//at this moment, this flag should be over 1
   {
      BC_ON_30min();
   }
   
   FAB_FLAG = 0;
   return;
}

void PREPARE_AUTOMATICAL_MSN_ADDRESS()
{
   if((PASSED_DAYS%loop_day) == 0 && UPLINK_SUCCESS == 0){//0th day high samp mission download
      UPDATE_ACK_for_COM(0x66,0x77,SECT*1639,405);
   }else if(((PASSED_DAYS%loop_day) >= 1 && (PASSED_DAYS%loop_day) <= 4) && UPLINK_SUCCESS == 0){//1-4th day high samp mission download 
      DAY1to4_ADDRESS();
   }else if(((PASSED_DAYS%loop_day) >= 5 && UPLINK_SUCCESS == 0) && AUTO_CAM_DONE == 0){//5th day (CAM MSN)
      UPDATE_ACK_for_COM(0x66,0x77,SECT*8,405);//65536*8;
   }else if(((PASSED_DAYS%loop_day) >= 6 && (PASSED_DAYS%loop_day) <= 9) && UPLINK_SUCCESS == 0){//download cam data
      DAY6to9_ADDRESS();
   }else if(((PASSED_DAYS%loop_day) >= 10 && UPLINK_SUCCESS == 0) && AUTO_LDM_DONE == 0){//LDM and download
      UPDATE_ACK_for_COM(0x66,0x77,DC_STATUS_ADDRESS-332,332);//65536*162;
   }else if(((PASSED_DAYS%loop_day) >= 11 && (PASSED_DAYS%loop_day) <= 14) && UPLINK_SUCCESS == 0){//LDM and download
      DAY11to14_ADDRESS();
   }else if(((PASSED_DAYS%loop_day) >= 15 && UPLINK_SUCCESS == 0) && AUTO_ADCS_DONE == 0){//ADCS Mission and Download
      UPDATE_ACK_for_COM(0x66, 0x77, SECT*1138, 405);//ADCS
   }else if(((PASSED_DAYS%loop_day) >= 16 && (PASSED_DAYS%loop_day) <= 19) && UPLINK_SUCCESS == 0){//ADCS Download
      DAY16to19_ADDRESS();//ADCS DL
   }else if(((PASSED_DAYS%loop_day) >= 20 && (PASSED_DAYS%loop_day) <= 25) && UPLINK_SUCCESS == 0){//High sampling sensor collecting mission download
      DAY20to25_ADDRESS();
   }else if(((PASSED_DAYS%loop_day) >= 26 && (PASSED_DAYS%loop_day) <= 46) && UPLINK_SUCCESS == 0){//CAM Mission Download
      DAY26to46_ADDRESS();
   }else if(((PASSED_DAYS%loop_day) >= 47 && (PASSED_DAYS%loop_day) <= 52) && UPLINK_SUCCESS == 0){//ADCS Mission Data Download
      DAY47to52_ADDRESS();
   }else if(((PASSED_DAYS%loop_day) >= 53 && (PASSED_DAYS%loop_day) <= 59) && UPLINK_SUCCESS == 0){//Satellite LOG Download
      DAY53to59_ADDRESS();
   }else if(((PASSED_DAYS%loop_day) >= 60 && (PASSED_DAYS%loop_day) <= 69) && UPLINK_SUCCESS == 0){//Normal HK Download
      DAY60to69_ADDRESS();
   }
   return;
}


//@@@@@@@@@@@ AUTO MISSION OPERATION @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
void AUTOMATICAL_MISSION_CHECK()
{
   int c;
   for(c = 0; c < 6; c++)
   {
      COLLECT_RESET_DATA();
      if(reset_bffr[0] == 0x8e)
      {
         break;
      }
   }
   
   for(c = 0; c < 10; c++)
   {
      fprintf(PC,"%x,",reset_bffr[c]);
   }
   fprintf(PC,"%x\r\n",reset_bffr[10]);
   
   if(((PASSED_DAYS%loop_day==0)&&(PASSED_DAYS>69))&&UPLINK_SUCCESS == 0)//after 60 day, auto mission again
   {
      FIRST_HSSC_DONE = 0;
      AUTO_CAM_DONE = 0;
      AUTO_LDM_DONE = 0;
      AUTO_ADCS_DONE = 0;
      //AUTO_MSN_AD_CHANGE();
      MEMORY_ERASE();
      FLAG_DATA_ADDRESS = SECT*4;//1sector
      RSV_DATA_ADDRESS=SECT*5;//1sector
      SAT_LOG = SECT*6;//2sector
      CAM_ADDRESS = SECT*8;//90sector
      FAB_HK_ADDRESS = SECT*98;//1000sector
      FAB_CW_ADDRESS = SECT*1098;//40sector
      ADCS_SENSOR_ADDRESS = SECT*1138;//500sector
      DC_STATUS_ADDRESS = SECT*1638;//1sector
      HIGH_SAMP_HK_ADDRESS = SECT*1639;//409sector
      FLAG_ADDRESS_EEPROM = 0x18000;//from 75 percent of the programming memory
      FLASH_AD_ADDRESS_EEPROM = 0x18000 + 28;//after flag info
      STORE_FLAG_INFO();
      WRITE_FLAG_to_EEPROM();
   }
   AUTOMATICAL_MSN_EXE();
   PREPARE_AUTOMATICAL_MSN_ADDRESS();
   return;
}
