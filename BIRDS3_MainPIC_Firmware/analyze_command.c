void UPLINK_SUCCESS_CHECK()
{
   if(UPLINK_SUCCESS == 0)
   {
      UPLINK_SUCCESS = 1;
      STORE_FLAG_INFO();
      WRITE_FLAG_to_EEPROM();
   }
   return;
}

void UPLINK_SUCCESS_ADCS()
{
   if(UPLINK_SUCCESS == 0)
   {
      UPLINK_SUCCESS = 1;
      AUTO_ADCS_DONE = 1;
      STORE_FLAG_INFO();
      WRITE_FLAG_to_EEPROM();
   }else if(AUTO_ADCS_DONE == 0){
      AUTO_ADCS_DONE = 1;
      STORE_FLAG_INFO();
      WRITE_FLAG_to_EEPROM();
   }
   return;
}

void UPLINK_SUCCESS_HSSC()
{
   if(UPLINK_SUCCESS == 0)
   {
      UPLINK_SUCCESS = 1;
      FIRST_HSSC_DONE = 1;
      STORE_FLAG_INFO();
      WRITE_FLAG_to_EEPROM();
   }else if(FIRST_HSSC_DONE == 0){
      FIRST_HSSC_DONE = 1;
      STORE_FLAG_INFO();
      WRITE_FLAG_to_EEPROM();
   }
   return;
}

void UPLINK_SUCCESS_LDM()
{
   if(UPLINK_SUCCESS == 0)
   {
      UPLINK_SUCCESS = 1;
      AUTO_LDM_DONE = 1;
      STORE_FLAG_INFO();
      WRITE_FLAG_to_EEPROM();
   }else if(AUTO_LDM_DONE == 0){
      AUTO_LDM_DONE = 1;
      STORE_FLAG_INFO();
      WRITE_FLAG_to_EEPROM();
   }
   return;
}

void UPLINK_SUCCESS_CAM()
{
   if(UPLINK_SUCCESS == 0)
   {
      AUTO_CAM_DONE = 1;
      UPLINK_SUCCESS = 1;
      STORE_FLAG_INFO();
      WRITE_FLAG_to_EEPROM();
   }else if(AUTO_CAM_DONE == 0){
      AUTO_CAM_DONE = 1;
      STORE_FLAG_INFO();
      WRITE_FLAG_to_EEPROM();
   }
   return;
}

void EXECUTE_MISSION_from_COM(int8 CMD1,int8 CMD2,int8 CMD3,int8 CMD4,int8 CMD5)
{
   switch(CMD1)
   {
      case 0xC0:
         REPLY_TO_COM(0x66,0);
         SAVE_SAT_LOG(CMD1, 0, CMD2);
         
         UPLINK_SUCCESS_CAM();
         ACK_for_COM[14] = 0x00;
         
         CAM_TEST_OPERATION(CMD2);
         STORE_ADRESS_DATA_TO_FLASH();
         break;
      case 0xA0:
         REPLY_TO_COM(0x66,0);
         SAVE_SAT_LOG(CMD1,0,CMD2);
         unsigned int16 duration = (unsigned int16)CMD2*60;
         
         UPLINK_SUCCESS_ADCS();
         ACK_for_COM[14] = 0x00;
         
         ADCS_TEST_OPERATION(duration,NOMAL_ADCS_MSN);
         STORE_ADRESS_DATA_TO_FLASH();
         break;
      case 0xD0:
         REPLY_TO_COM(0x66,0);
         SAVE_SAT_LOG(CMD1, 0, CMD2);
         
         UPLINK_SUCCESS_LDM();
         ACK_for_COM[14] = 0x00;
         
         DATACOLLECTION_TEST_OPERATION();
         STORE_ADRESS_DATA_TO_FLASH();
         delay_ms(10);
         break;
      case 0xaa:
         SAVE_SAT_LOG(CMD1, 0, CMD2);
         STORE_ADRESS_DATA_TO_FLASH();
         
         UPLINK_SUCCESS_CHECK();
         ACK_for_COM[14] = 0x00;
         
         STORE_FLAG_INFO();
         WRITE_FLAG_to_EEPROM();
         break;
      case 0x70:
         UPLINK_SUCCESS_CHECK();
         ACK_for_COM[14] = 0x00;
         REPLY_TO_COM(0x66,0);
         
         SAVE_SAT_LOG(CMD1, 0, CMD2);
         
         SEND_CMD_FOR_RESET_SATELLITE();
         break;
      case 0xb0:
         CLEAR_BC_FLAG();
         
         UPLINK_SUCCESS_CHECK();
         ACK_for_COM[14] = 0x00;
         
         STORE_ADRESS_DATA_TO_FLASH();
         STORE_FLAG_INFO();
         WRITE_FLAG_to_EEPROM();
         break;
      case 0xb1:
         MAKE_BC_FLAG_1();
         
         UPLINK_SUCCESS_CHECK();
         ACK_for_COM[14] = 0x00;
         
         STORE_ADRESS_DATA_TO_FLASH();
         STORE_FLAG_INFO();
         WRITE_FLAG_to_EEPROM();
         break;
      case 0xb2:
         MAKE_BC_FLAG_2();
         
         UPLINK_SUCCESS_CHECK();
         ACK_for_COM[14] = 0x00;
         
         STORE_ADRESS_DATA_TO_FLASH();
         STORE_FLAG_INFO();
         WRITE_FLAG_to_EEPROM();
         break;
      case 0xb3:
         MAKE_BC_FLAG_3();
         
         UPLINK_SUCCESS_CHECK();
         ACK_for_COM[14] = 0x00;
         
         STORE_ADRESS_DATA_TO_FLASH();
         STORE_FLAG_INFO();
         WRITE_FLAG_to_EEPROM();
         break;
      case 0xb4:
         MAKE_BC_FLAG_4();
         
         UPLINK_SUCCESS_CHECK();
         ACK_for_COM[14] = 0x00;
         
         STORE_ADRESS_DATA_TO_FLASH();
         STORE_FLAG_INFO();
         WRITE_FLAG_to_EEPROM();
         break;
      case 0x71:
         REPLY_TO_COM(0x66,0);
         UPLINK_SUCCESS_CHECK();
         ACK_for_COM[14] = 0x00;
         
         Count_Kill_Flag();
         SAVE_SAT_LOG(CMD1, KILL_COUNTER, CMD2);
         break;
      case 0xbb:
         REPLY_TO_COM(0x66,0);
         UPLINK_SUCCESS_CHECK();
         ACK_for_COM[14] = 0x00;
      
         Disable_Kill();
         SAVE_SAT_LOG(CMD1, KILL_COUNTER, CMD2);
         break;
      case 0xfa:
      
         UPLINK_SUCCESS_HSSC();//High Sampling Sensor Collect mission
         ACK_for_COM[14] = 0x00;
      
         REPLY_TO_COM(0x66,0);
         SAVE_SAT_LOG(CMD1, 0, CMD2);
         
         duration = (unsigned int16)CMD2*12;//CMD2 is operation time(min), maximum duration = 1440
         if(duration > 1440){duration = 1440;}
         HIGHSAMP_SENSOR_COLLECTION(duration);
         STORE_ADRESS_DATA_TO_FLASH();
         break;
      case 0x59:
      
         //UPLINK_SUCCESS_CHECK();
         //ACK_for_COM[14] = 0x00;
      
         REPLY_TO_COM(0x66,0);
         GIVE_ACCESS_SCF_Nsec(CMD2);
         break;
      case 0x88:
      
         //UPLINK_SUCCESS_CHECK();
         //ACK_for_COM[14] = 0x00;
      
         CURRENT_FLAG_STATUS();
         break;
      case 0x9f:
      
         //UPLINK_SUCCESS_CHECK();
         //ACK_for_COM[14] = 0x00;
      
         Disp_RSV();
         break;
      case 0x50:
         if(UPLINK_SUCCESS == 0){ACK_for_COM[14]=0x77;}
         CW_RESPOND();
         break;
      case 0x73:
      
         UPLINK_SUCCESS_CHECK();
         ACK_for_COM[14] = 0x00;
      
         REFRESH_SECTOR_OF(CMD2,CMD3,CMD4,CMD5);//this function contains saving log
         break;
      case 0x74:
      
         UPLINK_SUCCESS_CHECK();
         ACK_for_COM[14] = 0x00;
      
         REFRESH_SECTOR_SCF(CMD2,CMD3,CMD4,CMD5);//this function contains saving log
         break;
      case 0x75:
      
         UPLINK_SUCCESS_CHECK();
         ACK_for_COM[14] = 0x00;
         
         REFRESH_SECTOR_SMF(CMD2,CMD3,CMD4,CMD5);//this function contains saving log
         break;
      case 0x76:
      
         UPLINK_SUCCESS_CHECK();
         ACK_for_COM[14] = 0x00;
      
         TRANSFER_SECTOR_OF2SCF(CMD2,CMD3,CMD4,CMD5);//this function contains saving log
         break;
      case 0x77:
      
         UPLINK_SUCCESS_CHECK();
         ACK_for_COM[14] = 0x00;
      
         TRANSFER_SECTOR_OF2SMF(CMD2,CMD3,CMD4,CMD5);//this function contains saving log
         break;
      case 0x78:
      
         UPLINK_SUCCESS_CHECK();
         ACK_for_COM[14] = 0x00;
         
         TRANSFER_SECTOR_SCF2OF(CMD2,CMD3,CMD4,CMD5);//this function contains saving log
         break;
      case 0x79:
      
         UPLINK_SUCCESS_CHECK();
         ACK_for_COM[14] = 0x00;
         
         TRANSFER_SECTOR_SCF2SMF(CMD2,CMD3,CMD4,CMD5);//this function contains saving log
         break;
      case 0x7A:
      
         UPLINK_SUCCESS_CHECK();
         ACK_for_COM[14] = 0x00;
         
         TRANSFER_SECTOR_SMF2OF(CMD2,CMD3,CMD4,CMD5);//this function contains saving log
         break;
      case 0x7B:
      
         UPLINK_SUCCESS_CHECK();
         ACK_for_COM[14] = 0x00;
      
         TRANSFER_SECTOR_SMF2SCF(CMD2,CMD3,CMD4,CMD5);//this function contains saving log
         break;
      
      
   }
   return;
}











void EXECUTE_MISSION_from_PC(int8 CMD1,int8 CMD2,int8 CMD3,int8 CMD4,int8 CMD5)
{
   switch(CMD1)
   {
      case 0xC0:
      
         UPLINK_SUCCESS_CAM();
         ACK_for_COM[14] = 0x00;
      
         SAVE_SAT_LOG(CMD1,0,CMD2);
         CAM_TEST_OPERATION(CMD2);
         STORE_ADRESS_DATA_TO_FLASH();
         UPLINK_SUCCESS_CHECK();
         break;
      case 0xA0:
      
         UPLINK_SUCCESS_ADCS();
         ACK_for_COM[14] = 0x00;
      
         SAVE_SAT_LOG(CMD1,0,CMD2);
         unsigned int16 duration = (unsigned int16)CMD2*60;
         UPLINK_SUCCESS_CHECK();
         ADCS_TEST_OPERATION(duration, NOMAL_ADCS_MSN);
         STORE_ADRESS_DATA_TO_FLASH();
         break;
      case 0xD0:
      
         UPLINK_SUCCESS_LDM();
         ACK_for_COM[14] = 0x00;
      
         SAVE_SAT_LOG(CMD1,0,CMD2);
         DATACOLLECTION_TEST_OPERATION();
         delay_ms(10);
         STORE_ADRESS_DATA_TO_FLASH();
         break;
      case 0x99:
      
         //UPLINK_SUCCESS_CHECK();
         ACK_for_COM[14] = 0x00;
      
         SAVE_SAT_LOG(CMD1,0,CMD2);
         CURRENT_ADDRESS_OF_FLASH();
         break;
      case 0xaa:
      
         //UPLINK_SUCCESS_CHECK();
         ACK_for_COM[14] = 0x00;
      
         SAVE_SAT_LOG(CMD1,0,CMD2);
         STORE_ADRESS_DATA_TO_FLASH();
         STORE_FLAG_INFO();
         WRITE_FLAG_to_EEPROM();
         break;
      case 0x70:
      
         //UPLINK_SUCCESS_CHECK();
         //ACK_for_COM[14] = 0x00;
      
         SAVE_SAT_LOG(CMD1,0,CMD2);
         SEND_CMD_FOR_RESET_SATELLITE();
         break;
      case 0xb0:
      
         UPLINK_SUCCESS_CHECK();
         ACK_for_COM[14] = 0x00;
      
         CLEAR_BC_FLAG();
         STORE_ADRESS_DATA_TO_FLASH();
         STORE_FLAG_INFO();
         WRITE_FLAG_to_EEPROM();
         break;
      case 0xb1:
      
         UPLINK_SUCCESS_CHECK();
         ACK_for_COM[14] = 0x00;
      
         MAKE_BC_FLAG_1();
         STORE_ADRESS_DATA_TO_FLASH();
         STORE_FLAG_INFO();
         WRITE_FLAG_to_EEPROM();
         break;
      case 0xb2:
      
         UPLINK_SUCCESS_CHECK();
         ACK_for_COM[14] = 0x00;
      
         MAKE_BC_FLAG_2();
         STORE_ADRESS_DATA_TO_FLASH();
         STORE_FLAG_INFO();
         WRITE_FLAG_to_EEPROM();
         break;               
      case 0xb3:
      
         UPLINK_SUCCESS_CHECK();
         ACK_for_COM[14] = 0x00;
         
         MAKE_BC_FLAG_3();
         STORE_ADRESS_DATA_TO_FLASH();
         STORE_FLAG_INFO();
         WRITE_FLAG_to_EEPROM();
         break;
      case 0xb4:
      
         UPLINK_SUCCESS_CHECK();
         ACK_for_COM[14] = 0x00;
      
         MAKE_BC_FLAG_4();
         STORE_ADRESS_DATA_TO_FLASH();
         STORE_FLAG_INFO();
         WRITE_FLAG_to_EEPROM();
         break;
      case 0x71:
      
         UPLINK_SUCCESS_CHECK();
         ACK_for_COM[14] = 0x00;
      
         SAVE_SAT_LOG(CMD1,0,CMD2);
         Count_Kill_Flag();
         break;
      case 0xbb:
      
         UPLINK_SUCCESS_CHECK();
         //ACK_for_COM[14] = 0x00;
      
         SAVE_SAT_LOG(CMD1, 0, CMD2);
         Disable_Kill();
         break;
      case 0xfa:
      
         UPLINK_SUCCESS_HSSC();//High Sampling Sensor Collect mission
         ACK_for_COM[14] = 0x00;
      
         SAVE_SAT_LOG(CMD1,0,CMD2);
         
         duration = (unsigned int16)CMD2*12;//CMD2 is operation time(min), maximum duration = 1440
         if(duration > 1440){duration = 1440;}
         HIGHSAMP_SENSOR_COLLECTION(duration);
         Turn_OFF_ADCS();
         STORE_ADRESS_DATA_TO_FLASH();
         break;
      case 0x59:
      
         UPLINK_SUCCESS_CHECK();
         ACK_for_COM[14] = 0x00;
      
         GIVE_ACCESS_SCF_Nsec(CMD2);
         break;
      case 0x72:
      
         //UPLINK_SUCCESS_CHECK();
         //ACK_for_COM[14] = 0x00;
      
         ERASE_EEPROM_INFO();
         MEMORY_ERASE();
         break;
      case 0x88:
      
         //UPLINK_SUCCESS_CHECK();
         //ACK_for_COM[14] = 0x00;
      
         CURRENT_FLAG_STATUS();
         break;
      case 0x9f:
      
         //UPLINK_SUCCESS_CHECK();
         //ACK_for_COM[14] = 0x00;
      
         Disp_RSV();
         break;
      case 0xcf:
      
         //UPLINK_SUCCESS_CHECK();
         //ACK_for_COM[14] = 0x00;
      
         table_refresh();
         break;
      case 0x6a:
      
         UPLINK_SUCCESS_CHECK();
         ACK_for_COM[14] = 0x00;
      
         RESET_FLAG_DATA();
         break;
      case 0x73:
      
         UPLINK_SUCCESS_CHECK();
         ACK_for_COM[14] = 0x00;
      
         REFRESH_SECTOR_OF(CMD2,CMD3,CMD4,CMD5);//this function contains saving log
         break;
      case 0x74:
      
         UPLINK_SUCCESS_CHECK();
         ACK_for_COM[14] = 0x00;
      
         REFRESH_SECTOR_SCF(CMD2,CMD3,CMD4,CMD5);//this function contains saving log
         break;
      case 0x75:
      
         UPLINK_SUCCESS_CHECK();
         ACK_for_COM[14] = 0x00;
      
         REFRESH_SECTOR_SMF(CMD2,CMD3,CMD4,CMD5);//this function contains saving log
         break;
      case 0x76:
      
         UPLINK_SUCCESS_CHECK();
         ACK_for_COM[14] = 0x00;
      
         TRANSFER_SECTOR_OF2SCF(CMD2,CMD3,CMD4,CMD5);//this function contains saving log
         break;
      case 0x7B:
      
         UPLINK_SUCCESS_CHECK();
         ACK_for_COM[14] = 0x00;
      
         TRANSFER_SECTOR_SMF2SCF(CMD2,CMD3,CMD4,CMD5);//this function contains saving log
         break;
      case 0x78:
      
         UPLINK_SUCCESS_CHECK();
         ACK_for_COM[14] = 0x00;
      
         TRANSFER_SECTOR_SCF2OF(CMD2,CMD3,CMD4,CMD5);//this function contains saving log
         break;
   }
   return;
}



int8 CHECK_MEMORY_FUNCTION(int8 data)
{
   if((data == 0x73)||(data == 0x74)||(data == 0x75))
   {
      data = 0;
   }
   if((data == 0x76)||(data == 0x7B)||(data == 0x78))
   {
      data = 0;
   }
   return data;
}
