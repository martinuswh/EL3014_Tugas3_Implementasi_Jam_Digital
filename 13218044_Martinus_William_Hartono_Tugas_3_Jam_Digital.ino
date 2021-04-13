//EL3014-Sistem Mikroprosesor K01
//Tugas #3 Implementasi Jam Digital
//Nama          :   Martinus William Hartono
//NIM           :   13218044
//Deskripsi     :   Kode ini merupakan source code implementasi jam digital dengan fitur pengaturan jam, display jam, stopwatch, dan alarm


#include <TimerOne.h>

const int a=A1,b=13,c=7,d=8,e=9,f=11,g=10,dp=A2;
const int dis1=5,dis2=4,dis3=12,dis4=6;
const int button[3]={A0,2,3};
const int ledPin = A3;

int jam, menit, detik, menit_stop, detik_stop, jam_alarm, menit_alarm;
int mode=0, stopWatchState=0, alarmState=0;

int pinSev[8]={a,b,c,d,e,f,g,dp};
int pinSelector[4]={dis1, dis2, dis3, dis4};
int val[6]={0,0,0,0,0,0};   //MSB Jam, LSB detik
int val_buff[4]={0,0,0,0};   //MSB Jam, LSB detik
int val_stop[4]={0,0,0,0};   //Buffer untuk stopwatch dengan MSB menit, LSB detik
int val_alarm[4]={0,0,0,0};   //Buffer untuk alarm dengan MSB jam, LSB detik

int outputSev[10][8]= {
    {0,0,0,0,0,0,1,1}, //memunculkan 0 pada 7seg
    {1,0,0,1,1,1,1,1}, //memunculkan 1 pada 7seg
    {0,0,1,0,0,1,0,1}, //memunculkan 2 pada 7seg
    {0,0,0,0,1,1,0,1}, //memunculkan 3 pada 7seg
    {1,0,0,1,1,0,0,1}, //memunculkan 4 pada 7seg
    {0,1,0,0,1,0,0,1}, //memunculkan 5 pada 7seg
    {0,1,0,0,0,0,0,1}, //memunculkan 6 pada 7seg
    {0,0,0,1,1,1,1,1}, //memunculkan 7 pada 7seg
    {0,0,0,0,0,0,0,1}, //memunculkan 8 pada 7seg
    {0,0,0,0,1,0,0,1}, //memunculkan 9 pada 7seg
  };

int buttonState[3]={0,0,0};   //0 lepas, 1 tekan

int i,j;    //iterasi
long int counterSec=0, counterStop=0, counterAlarm=0;

int time_format=24;

void setup()
{
    Serial.begin(9600);    
    pinMode(a,OUTPUT);   
    pinMode(b,OUTPUT);    
    pinMode(c,OUTPUT);  
    pinMode(d,OUTPUT);
    pinMode(e,OUTPUT);
    pinMode(f,OUTPUT);
    pinMode(g,OUTPUT);  
    pinMode(dp,OUTPUT);   
    
    pinMode(dis1,OUTPUT);
    pinMode(dis2,OUTPUT);
    pinMode(dis3,OUTPUT);
    pinMode(dis4,OUTPUT);

    pinMode(ledPin, OUTPUT);

    for(i=0;i<3;i++)
    {
      pinMode(button[i], INPUT);
    }

    //Inisialisasi interrupt timer
    //Timer1.initialize(1000000);   //Inisialisasi periode timer interrupt sebesar 1s
    Timer1.initialize(1000000);
    Timer1.attachInterrupt(secIncrement);

    attachInterrupt(1, changeMode, RISING);     //Interrupt button 3 untuk changeMode()
    attachInterrupt(0, changeStateStopWatch, RISING);   //Interrupt button 2 untuk changeStateStopWatch()
}

void loop()
{
  for(i=0;i<3;i++)
  {
    buttonState[i]=digitalRead(button[i]);
  };
  
  if (counterAlarm > 0)
  {
    alarmState=1;
  }
  else
  {
    alarmState=0;
  }

  //Saat counterSec >= counterAlarm -> nyalakan LED
  if ((counterSec >= counterAlarm) && (alarmState == 1) && (mode!=3))
  {
    digitalWrite(ledPin, HIGH);
  }
  else
  {
    digitalWrite(ledPin, LOW);
  }
  
  jam = (counterSec/3600);
  menit = (counterSec%3600)/60;
  detik = (counterSec%60);

  menit_stop = (counterStop%3600)/60;
  detik_stop = counterStop%60;

  jam_alarm = (counterAlarm/3600);
  menit_alarm = (counterAlarm%3600)/60;

  val[0] = jam/10;
  val[1] = jam%10;
  val[2] = menit/10;
  val[3] = menit%10;
  val[4] = detik/10;
  val[5] = detik%10;

  if(mode==0)
  {
    settingJam();
  }
  else if (mode==1)
  {
    displayJam();
  }
  else if (mode==2)
  {
    stopWatch();
  }
  else //mode>=3
  {
    alarm();
  }
  
  //Diplay 7-Segment
  for(j=0;j<4;j++)
  {
    sevSegDisp(j, val_buff[j]);
    delay(3);
  }
}

void sevSegDisp (int digit, int x)
{  
  int j=0;
  for (j=0;j<4;j++)
  {
    if(j==digit)
    {
      digitalWrite(pinSelector[j], HIGH);
    }
    else
    {
      digitalWrite(pinSelector[j], LOW);
    }
  }
  
  for(j=0;j<8;j++)
  {
    digitalWrite(pinSev[j], HIGH);
  }
  delay(3);

  for(j=0;j<8;j++)
  {
    digitalWrite(pinSev[j], outputSev[x][j]);
  }
}

//mode==0 (Atur jam)
void settingJam()
{
  if(buttonState[0]==1)
  {
    counterSec+=60;
  }
  //else do nothing
  val_buff[0] = val[0];
  val_buff[1] = val[1];
  val_buff[2] = val[2];
  val_buff[3] = val[3];
};


//mode==1 (Display jam)
void displayJam()
{
  if (buttonState[0]==1 && mode==1)
  {
    //display menit detik saat ditekan
    val_buff[0] = val[2];
    val_buff[1] = val[3];
    val_buff[2] = val[4];
    val_buff[3] = val[5];
  }
  else
  {
    //display jam menit saat tidak ditekan
    val_buff[0] = val[0];
    val_buff[1] = val[1];
    val_buff[2] = val[2];
    val_buff[3] = val[3];
  }
};

//mode==2 (Stopwatch)
void stopWatch()
{
    val_stop[0] = menit_stop/10;
    val_stop[1] = menit_stop%10;
    val_stop[2] = detik_stop/10;
    val_stop[3] = detik_stop%10;
    
    val_buff[0] = val_stop[0];
    val_buff[1] = val_stop[1];
    val_buff[2] = val_stop[2];
    val_buff[3] = val_stop[3];
}

//mode==3 (Alarm)
void alarm()
{  
  if(buttonState[0]==1)
  {
    counterAlarm+=60;
  }
  //else do nothing

  val_alarm[0] = jam_alarm/10;
  val_alarm[1] = jam_alarm%10;
  val_alarm[2] = menit_alarm/10;
  val_alarm[3] = menit_alarm%10;
  
  val_buff[0] = val_alarm[0];
  val_buff[1] = val_alarm[1];
  val_buff[2] = val_alarm[2];
  val_buff[3] = val_alarm[3];
};

void secIncrement()
{
  if (mode != 0)
  {
    counterSec=counterSec+1;
  }
  else    //mode=0
  {
    counterSec=counterSec;
  };

  if (stopWatchState == 1)    //counterStop running
  {
    counterStop++;
  }
  else    //stopWatchState == 0
  {
    counterStop = counterStop;
  };
  
  if (counterSec == 24*3600 && mode!=0)      //Saat pergantian hari, terjadi overflow dan kembali ke nilai 0
  {
    counterSec=0;
  };

  if (counterStop == (60*60 -1))
  {
    counterStop == 60*60 - 1;
  };
};

//end mode

void changeStateStopWatch()
{
  if (stopWatchState == 0)
  {
    stopWatchState = 1;   //running stopwatch
  }
  else //stopWatchState == 1
  {
    stopWatchState = 0;
  }
}

void changeMode()   //ganti mode, button 3 interrupt
{
  mode++;
  if (mode>3)
  {
    mode=0;
  }
}
