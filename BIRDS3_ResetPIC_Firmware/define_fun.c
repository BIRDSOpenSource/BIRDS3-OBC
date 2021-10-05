#define day1H 5
#define day1L 8

#define day2H 10
#define day2L 11

#define day3H 15
#define day3L 16

#define day4H 20
#define day4L 21

#define day5H 25
#define day5L 26

#define day6H 30
#define day6L 31

#define day7H 35
#define day7L 36

#define day8H 40
#define day8L 41

#define day9H 45
#define day9L 46

#define day10H 50
#define day10L 51

//int8 daycount = 5;
int16 dy[10]={0};
//unsigned int16 day_num = 0;

void day_num_write(void)
{
   write_eeprom(day1H, (day_num >>8)& 0xff);
   write_eeprom(day1L, day_num & 0xff);

   write_eeprom(day2H, (day_num >>8) & 0xff);
   write_eeprom(day2L, day_num & 0xff);

   write_eeprom(day3H, (day_num >>8) & 0xff);
   write_eeprom(day3L, day_num & 0xff);

   write_eeprom(day4H, (day_num >>8) & 0xff);
   write_eeprom(day4L, day_num & 0xff);

   write_eeprom(day5H, (day_num >>8) & 0xff);
   write_eeprom(day5L, day_num & 0xff);
   
   write_eeprom(day6H, (day_num >>8) & 0xff);
   write_eeprom(day6L, day_num & 0xff);
   
   write_eeprom(day7H, (day_num >>8) & 0xff);
   write_eeprom(day7L, day_num & 0xff);
   
   write_eeprom(day8H, (day_num >>8) & 0xff);
   write_eeprom(day8L, day_num & 0xff);
   
   write_eeprom(day9H, (day_num >>8) & 0xff);
   write_eeprom(day9L, day_num & 0xff);
   
   write_eeprom(day10H, (day_num >>8) & 0xff);
   write_eeprom(day10L, day_num & 0xff);
   
   write_eeprom(day1H, (day_num >>8) & 0xff);
   write_eeprom(day1L, day_num & 0xff);
   
   return;
}


void day_read()
{

   dy[0] = make16( read_eeprom(day1H),read_eeprom(day1L) );
   //fprintf(PORT1,"day number1 =%04ld\n\r",dy[0]);
   dy[1] = make16( read_eeprom(day2H),read_eeprom(day2L) );   
   //fprintf(PORT1,"day number2 =%04ld\n\r",dy[1]);
   dy[2] = make16( read_eeprom(day3H),read_eeprom(day3L) );
   //fprintf(PORT1,"day number3 =%04ld\n\r",dy[2]);
   dy[3] = make16( read_eeprom(day4H),read_eeprom(day4L) );  
   //fprintf(PORT1,"day number4 =%04ld\n\r",dy[3]);
   dy[4] = make16( read_eeprom(day5H),read_eeprom(day5L) );
   //fprintf(PORT1,"day number5 =%04ld\n\r",dy[4]);
   dy[5] = make16( read_eeprom(day6H),read_eeprom(day6L) );
   //fprintf(PORT1,"day number1 =%04ld\n\r",dy[5]);
   dy[6] = make16( read_eeprom(day7H),read_eeprom(day7L) );   
   //fprintf(PORT1,"day number2 =%04ld\n\r",dy[6]);
   dy[7] = make16( read_eeprom(day8H),read_eeprom(day8L) ); 
   //fprintf(PORT1,"day number3 =%04ld\n\r",dy[7]);
   dy[8] = make16( read_eeprom(day9H),read_eeprom(day9L) );   
   //fprintf(PORT1,"day number4 =%04ld\n\r",dy[8]);
   dy[9] = make16( read_eeprom(day10H),read_eeprom(day10L) );
   //fprintf(PORT1,"day number5 =%04ld\n\r",dy[9]);
   
   return;
}


int16 return_mode()
{
    int16 returnVal = dy[0]; // stores element to be returned
    int repeatCount = 0; // counts the record number of repeats
    int prevRepCnt = 0; // temporary count for repeats

    for (int i=0; i<10; i++) { // goes through each elem

        for (int j=i; j<10; j++) { // compares to each elem after the first elem

             if (i != j && dy[i] == dy[j]) {  // if matching values
                 repeatCount++;  // gets the repeat count
               }
        }
        if (repeatCount>=prevRepCnt) {  // a higher count of repeats than before
                returnVal=dy[i]; // return that element
                prevRepCnt = repeatCount; // Keeps the highest repeat record
            }
        repeatCount=0; // resets repeat Count for next comparison
        }
    return returnVal;
}

