/************************************************************************************

Copyright (c) 2001-2016  University of Washington Extension.

Module Name:

tasks.c

Module Description:

The tasks that are executed by the test application.

2016/2 Nick Strathy adapted it for NUCLEO-F401RE 

2020/2 Edwin Kaburu adapted it for  STM 32L475VG

************************************************************************************/


#include <stdarg.h>

#include "bsp.h"
#include "print.h"
#include "mp3Util.h"

// SD Card Library
#include "SD.h"

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ILI9341.h>
#include <Adafruit_FT6206.h>

Adafruit_ILI9341 lcdCtrl = Adafruit_ILI9341(); // The LCD controller

Adafruit_FT6206 touchCtrl = Adafruit_FT6206(); // The touch controller


Adafruit_GFX_Button playBottom;

Adafruit_GFX_Button pauseBottom;

Adafruit_GFX_Button nextBottom;

Adafruit_GFX_Button previousBottom;

Adafruit_GFX_Button haltBottom;

Adafruit_GFX_Button VolIncBottom;

Adafruit_GFX_Button VolDesBottom;



#define PENRADIUS 3

long MapTouchToScreen(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
#include "train_crossing.h"

#define BUFSIZE 256

// Music Previous Capacity
// Should be modified to Total Numbers of Music Files you Have.
const INT8U CAPACITY = 3;

const INT8U QUEUE_CAPACITY = CAPACITY - 1;

/************************************************************************************

Allocate the stacks for each task.
The maximum number of tasks the application can have is defined by OS_MAX_TASKS in os_cfg.h

************************************************************************************/

static OS_STK   LcdTouchTaskStk[APP_CFG_TASK_START_STK_SIZE];
static OS_STK   Mp3SDTaskStk[APP_CFG_TASK_START_STK_SIZE];

static OS_STK   ControlTaskStk[APP_CFG_TASK_START_STK_SIZE];

static OS_STK   DisplayTaskStk[APP_CFG_TASK_START_STK_SIZE];

// ---------------- Task prototypes  -------------------------------------------
void LcdTouchTask(void* pdata); // Change (LcdTouchDemoTask) to Touch Task, Assert of buttons

void ControlTask(void* pdata); // Actions Taken By Buttons

void DisplayTask(void* pdata); // UI Display

// Include ---- Display Task ---- 

void Mp3SDTask(void* pdata);


// -----------------------------------------------------------------------------
OS_EVENT *musicStatus;

// Useful functions
void PrintToLcdWithBuf(char *buf, int size, char *format, ...);

// Globals
BOOLEAN nextSong = OS_FALSE;

BOOLEAN stopSong = OS_TRUE;

BOOLEAN prevSong = OS_FALSE;

BOOLEAN haltPlayer = OS_TRUE; 

// Inside ISR_M
BOOLEAN isr_M = OS_FALSE;

//BOOLEAN Read_Message = OS_FALSE;

//  New Message To Read.
BOOLEAN Read_Update = OS_FALSE;

BOOLEAN Read_MailUpdate = OS_FALSE;

BOOLEAN After_Start = OS_FALSE;

BOOLEAN SystemVolUp = OS_FALSE;

//BOOLEAN isr_items = OS_FALSE;

// Event flags for synchronizing mailbox messages
//OS_FLAG_GRP *btnFlags = 0;

// Mutext
OS_EVENT *pauseMutex;

// Mail Box
OS_EVENT * buttonMBox;

// Mail Box
//OS_EVENT * songDisplay;

// Mail Box
OS_EVENT * mstatus_Change; // Interrupted Music Status

// Mail Box

OS_EVENT * volume_Change; // Volume Change

// Define Music Status
void *qMusicStatus[QUEUE_CAPACITY];

OS_EVENT *queueMusic;

static HANDLE hMp3;

//INT16U rdOnce;
//char *prevLists[] = {"music1.mp3", "music2.mp3", "music3.mp3"};

//char *prevSong[] = { "music1", "music2", "music3" };

//char *prevmusic[CAPACITY];

File prevFiles[CAPACITY];

typedef enum
{
  VOLUP_COMMAND,
  VOLDW_COMMAND,
  PAUSE_COMMAND,
  PLAY_COMMAND,
  NEXT_COMMAND,
  PREVIOUS_COMMAND,
  HALT_COMMAND
}ButtonControlsEnum;

INT8U DefVolume =  0x00;

//char volumeDigit[3];

/************************************************************************************

This task is the initial task running, started by main(). It starts
the system tick timer and creates all the other tasks. Then it deletes itself.

************************************************************************************/
void StartupTask(void* pdata)
{
  // INT8U err;
  
  char buf[BUFSIZE];
  
  //INT8U err;
  PjdfErrCode pjdfErr;
  INT32U length;
  static HANDLE hSD = 0;
  static HANDLE hSPI = 0;
  
  PrintWithBuf(buf, BUFSIZE, "StartupTask: Begin\n");
  // PrintWithBuf(buf, BUFSIZE, "StartupTask: Starting timer tick\n");
  
  // Start the system tick
  SetSysTick(OS_TICKS_PER_SEC);
  
  // ----------- Create Flags and Mutex -------------------------------
  
  // btnFlags = OSFlagCreate(0x0,&err);
  
  buttonMBox = OSMboxCreate(NULL);
  
  // songDisplay = OSMboxCreate(NULL);
  
  //music_Stats = OSMboxCreate(NULL);
  
  mstatus_Change = OSMboxCreate(NULL);
  
  volume_Change = OSMboxCreate(NULL);
  
  // Create Music Queue
  queueMusic = OSQCreate(qMusicStatus, QUEUE_CAPACITY);
  
  // Initialize SD card
  //PrintWithBuf(buf, PRINTBUFMAX, "Opening handle to SD driver: %s\n", PJDF_DEVICE_ID_SD_ADAFRUIT);
  hSD = Open(PJDF_DEVICE_ID_SD_ADAFRUIT, 0);
  if (!PJDF_IS_VALID_HANDLE(hSD)) while(1);
  
  
  PrintWithBuf(buf, PRINTBUFMAX, "Opening SD SPI driver: %s\n", SD_SPI_DEVICE_ID);
  // We talk to the SD controller over a SPI interface therefore
  // open an instance of that SPI driver and pass the handle to 
  // the SD driver.
  hSPI = Open(SD_SPI_DEVICE_ID, 0);
  if (!PJDF_IS_VALID_HANDLE(hSPI)) while(1);
  
  length = sizeof(HANDLE);
  pjdfErr = Ioctl(hSD, PJDF_CTRL_SD_SET_SPI_HANDLE, &hSPI, &length);
  if(PJDF_IS_ERROR(pjdfErr)) while(1);
  
  if (!SD.begin(hSD)) {
    PrintWithBuf(buf, PRINTBUFMAX, "Attempt to initialize SD card failed.\n");
  }
  
  // Create the test tasks
  PrintWithBuf(buf, BUFSIZE, "StartupTask: Creating the application tasks\n");
  
  // The maximum number of tasks the application can have is defined by OS_MAX_TASKS in os_cfg.h
  uint16_t task_prio =APP_TASK_TEST2_PRIO;
  
  // OSTaskCreate(Mp3DemoTask, (void*)0, &Mp3DemoTaskStk[APP_CFG_TASK_START_STK_SIZE-1], task_prio++);
  
  OSTaskCreate(LcdTouchTask, (void*)0, &LcdTouchTaskStk[APP_CFG_TASK_START_STK_SIZE-1], task_prio++);
  
  OSTaskCreate(Mp3SDTask, (void*)0, &Mp3SDTaskStk[APP_CFG_TASK_START_STK_SIZE-1], task_prio++);
  
  // Control Task Creation
  OSTaskCreate(ControlTask, (void*)0, &ControlTaskStk[APP_CFG_TASK_START_STK_SIZE-1], task_prio++);
  
  // DisplayTask Creation
  OSTaskCreate(DisplayTask, (void*)0, &DisplayTaskStk[APP_CFG_TASK_START_STK_SIZE-1], task_prio++);
  
  //pauseMutex = OSMutexCreate(task_prio++, &err);
  
  // Delete ourselves, letting the work be done in the new tasks.
  //PrintWithBuf(buf, BUFSIZE, "StartupTask: deleting self\n");
  OSTaskDel(OS_PRIO_SELF);
}

static void DrawLcdContents()
{
  char buf[BUFSIZE];
  OS_CPU_SR cpu_sr;
  
  // allow slow lower pri drawing operation to finish without preemption
  OS_ENTER_CRITICAL(); 
  
  lcdCtrl.fillScreen(ILI9341_BLACK);
  
  // Print a message on the LCD
  lcdCtrl.setCursor(0, 0);
  lcdCtrl.setTextColor(ILI9341_WHITE);  
  lcdCtrl.setTextSize(2);
  PrintToLcdWithBuf(buf, BUFSIZE, "Music Player");
  
  OS_EXIT_CRITICAL();
  
}

void DisplaySong(char* string, INT16S x, INT16S y, INT16S w, INT16S h)
{
  char buf[BUFSIZE];
  OS_CPU_SR cpu_sr;
  
  // allow slow lower pri drawing operation to finish without preemption
  OS_ENTER_CRITICAL(); 
  
  // X, Y, W, H, COLOR
  //lcdCtrl.fillRect(0,40,120,20,ILI9341_NAVY);
  
  lcdCtrl.fillRect(x,y,w, h,ILI9341_NAVY);
  
  // Print a message on the LCD
  lcdCtrl.setCursor(x, y); // This was 0, 40
  lcdCtrl.setTextColor(ILI9341_WHITE);  
  lcdCtrl.setTextSize(2);
  PrintToLcdWithBuf(buf, BUFSIZE, string);
  
  OS_EXIT_CRITICAL();
  
}


/*******************************************************************************
RUN SD TASK CODE
********************************************************************************/

void Mp3SDTask(void* pdata)
{
  INT8U err;
  
  PjdfErrCode pjdfErr;
  INT32U length;
  
  char buf[BUFSIZE];
  PrintWithBuf(buf, BUFSIZE, "Mp3DemoTask: starting\n");
  
  // PrintWithBuf(buf, BUFSIZE, "Opening MP3 driver: %s\n", PJDF_DEVICE_ID_MP3_VS1053);
  // Open handle to the MP3 decoder driver
  hMp3 = Open(PJDF_DEVICE_ID_MP3_VS1053, 0);
  if (!PJDF_IS_VALID_HANDLE(hMp3)) while(1);
  
  PrintWithBuf(buf, BUFSIZE, "Opening MP3 SPI driver: %s\n", MP3_SPI_DEVICE_ID);
  // We talk to the MP3 decoder over a SPI interface therefore
  // open an instance of that SPI driver and pass the handle to 
  // the MP3 driver.
  HANDLE hSPI = Open(MP3_SPI_DEVICE_ID, 0);
  if (!PJDF_IS_VALID_HANDLE(hSPI)) while(1);
  
  length = sizeof(HANDLE);
  pjdfErr = Ioctl(hMp3, PJDF_CTRL_MP3_SET_SPI_HANDLE, &hSPI, &length);
  if(PJDF_IS_ERROR(pjdfErr)) while(1);
  
  // Send initialization data to the MP3 decoder and run a test
  PrintWithBuf(buf, BUFSIZE, "Starting MP3 device test\n");
  Mp3Init(hMp3);
  int count = 0;
  
  
  char *music_status = "Halting";
  
  
  //Read_Message = OS_TRUE;
  
  //Read_Update = OS_TRUE;
  
  //Read_MailUpdate = OS_TRUE;
  Read_Update = OS_TRUE;
  
  
  // Insert Into Mail Box;
  
  // Add To Mail Box
  //OSMboxPost(mstatus_Change,(void*)music_status);
  
  err = OSQPost(queueMusic, (void*)music_status); // Display Name
  
  err = OSQPost(queueMusic, (void*)music_status); // Display Music Status
  
  // Add To Mail Box. HaltPlayer is Set To True
  //OSMboxPost(mstatus_Change,(void*)music_status);
  
  //DrawBackGround();
  
  //OS_TIME()
  
  File dir = SD.open("/");
  
  while (1)
  {
    // Reset Values
    INT8U counter = 0u;
    
    // ISR-M 
    INT8U at_counter = 0u; // After Counter
    INT8U lp_counter = 0u; // Looping Counter
    
    //INT8U diff = 0;
    
    // Set Music Status To Playing
    
    if(haltPlayer)
    {
      OSTimeDly(300);
    }
    else
    {
      
      After_Start = OS_TRUE;
      // Loop To Play Song
      
      while (1)
      {
        
        //if(haltPlayer == OS_FALSE)
        //{
        
        
        File entry;
        
        music_status = "Playing";
        
        //char *test_name;
        
        // Check if we are in isr_M, and run those files, else read from SD Card.
        
        if(isr_M)
        { 
          
          if(prevSong)
          {
            // Previous is True, Decrement LP_Counter;
            
            if(lp_counter != 0)
            {
              lp_counter--;
            }
            
            
            // Get File using Lp_Counter 
            entry = prevFiles[lp_counter];
            
            // test_name = prevmusic[lp_counter];
            
            // Play Song 
            PrintWithBuf(buf, BUFSIZE, "\nP-Begin streaming isr file: %d \n", lp_counter);       
            
            // PrintString(entry.name());
            
            //  Mp3StreamSDFile(hMp3, entry.name());           
            
            //  PrintWithBuf(buf, BUFSIZE, "\nP-Done streaming isr file  count=%d\n", ++count);     
            
            // Reset Previous Button
            prevSong = OS_FALSE;
            
          }
          else
          {
            if(nextSong || nextSong == OS_FALSE)
            {
              if(nextSong)
              {
                // Reset Button
                nextSong = OS_FALSE;
              }
              
              if(lp_counter < at_counter)
              {
                lp_counter++;
                
                // Only Play when Lpcounter != at_counter
                // Get File using Lp_Counter 
                entry = prevFiles[lp_counter];
                
                // Play Song 
                PrintWithBuf(buf, BUFSIZE, "\nNP-Begin streaming isr file: %d\n", lp_counter);   
                
              }
              else 
              {
                if(lp_counter == at_counter)
                {
                  isr_M = OS_FALSE; // This will exit ISR_M
                }
              }
              
            }
            
          }
          
          //PrintString(entry.name());
          
          //OSMboxPost(songDisplay,(void*)entry.name());
          
          // Add To Queue : For Display Purposes
          //music_status
          
          //  Read_Message = OS_FALSE;
          
          Read_Update = OS_TRUE;
          
          err = OSQPost(queueMusic, (void*)entry.name()); // Display Name
          
          err = OSQPost(queueMusic, (void*)music_status); // Display Music Status
          
          
         
          
          //PrintWithBuf(buf, BUFSIZE,"\n%d \n", err);
          
          Mp3StreamSDFile(hMp3, entry.name());          
          
          PrintWithBuf(buf, BUFSIZE, "\nDone streaming isr file at: %d", lp_counter);
         // DefVolume =  0x00;
          continue;
          
        }
        else
        {
          entry = dir.openNextFile();
          
          if (!entry)
          {
            break;
          }
          if (entry.isDirectory())  // skip directories
          {
            entry.close();
            continue;
          }
          
          // Play Song 
          PrintWithBuf(buf, BUFSIZE, "\nBegin streaming sd file  count=%d\n", ++count);    
          
          // Send Song Name
          //OSMboxPost(songDisplay,(void*)entry.name());
          //songDisplay
          
          // PrintString(entry.name());
          
          
          //Read_Message = OS_FALSE;
          
          Read_Update = OS_TRUE;
          
          err = OSQPost(queueMusic, (void*)entry.name()); // Display Name
          
          err = OSQPost(queueMusic, (void*)music_status); // Display Music Status
         
          
          
          Mp3StreamSDFile(hMp3, entry.name()); 
          
          PrintWithBuf(buf, BUFSIZE, "\nDone streaming sd file  count=%d\n", ++count);    
          
         // DefVolume =  0x00;
          
          //if(nextSong || prevSong)
          // {        
          counter = counter % CAPACITY; // Will Reset is Counter is Above Capacitor
          
          //prevmusic[counter] = entry.name();
          
          prevFiles[counter] = entry;
          
          // Increment Counter, Something In Previous List 
          counter++;
          
          if(nextSong) // This was Next Button CLick
          {
            // Reset Next_Song to False
            nextSong = OS_FALSE;
          }
          
          // Insert Ourself Into isr_M
          // This is our entry point of intersection 
          if(isr_M == OS_FALSE && counter > 0 && prevSong)
          {  
            isr_M = OS_TRUE;
            
            // Set Up AC And LP
            at_counter = counter - 1 ;
            lp_counter = at_counter;
            
            PrintWithBuf(buf, BUFSIZE, "\nInserted at_co: %d lp_co : %d\n", at_counter, lp_counter);  
            
            //diff = lp_counter;
          }
          
          PrintWithBuf(buf, BUFSIZE, "FileSV =%d\n", counter);
          
          //}
        }
        
        entry.close();
        
        
        //}
        
      }
      
      
    }
    
    
    dir.seek(0); // reset directory file to read again;
    
  }
}

/*******************************************************************************

Runs Control Task code

*******************************************************************************/

void ControlTask(void* pdata)
{
  INT8U err;
  char buf[BUFSIZE];
  ButtonControlsEnum *BtnControl_type;
  
  

  
  PrintWithBuf(buf, BUFSIZE,"Control Task building\n");
  
  //OS_CPU_SR  cpu_sr;
  
  //OS_ENTER_CRITICAL();
  
  
  while(1)
  {
    
    BtnControl_type = (ButtonControlsEnum*)OSMboxPend(buttonMBox,0,&err);
    
    ButtonControlsEnum* value = BtnControl_type;
    
    PrintWithBuf(buf, BUFSIZE,"Value: %d \t %d \n", value, (*value));
    
    char *music_status;
    
    char volumeDigit[7];
    
    INT8U displayVolume;
    
    OS_CPU_SR  cpu_sr;
    switch((int)BtnControl_type)
    {
    case VOLUP_COMMAND:
      //PrintString("None Command");
      
      // De-Assert Volume
      VolIncBottom.press(0);
      
      // Increase Volume
      SystemVolUp = OS_TRUE;
      
      if(DefVolume != 0x00)
      {
        // Increase Volume
         DefVolume =  DefVolume - 0xA; // subtract 10
         
         OS_ENTER_CRITICAL();
         
         Mp3VolumeUpDown(hMp3);
     
         OS_EXIT_CRITICAL();
      }
      
      // Send To Display
      
      Read_Update = OS_TRUE;
      
      Read_MailUpdate = OS_TRUE;
      
     // sprintf(volumeDigit,"%d",DefVolume);
      
      
      //PrintWithBuf(buf, BUFSIZE,"\nVolume: %s\n", volumeDigit);
      
        // Display Volume
      
      displayVolume = 0x64 - DefVolume;
      
      snprintf(volumeDigit,7,"%d %s", displayVolume, "%%");
      
      OSMboxPost(volume_Change, (void*) volumeDigit);
            
      OSTimeDly(100);
      
      break;
    case VOLDW_COMMAND:
      
      // De-Assert Volume
      VolDesBottom.press(0);
      
      // Decrese Volume
      SystemVolUp = OS_TRUE;
      
      if(DefVolume < 0x64) // used to be 0x90
      {
         // Decrease Volume
         DefVolume =  DefVolume + 0xA; // Add 10
         
         OS_ENTER_CRITICAL();
         
         Mp3VolumeUpDown(hMp3);
      
         OS_EXIT_CRITICAL();
      }
      
      Read_Update = OS_TRUE;
      
      Read_MailUpdate = OS_TRUE;
      
      // Display Volume
      
      displayVolume = 0x64 - DefVolume;
      
      snprintf(volumeDigit,7,"%d %s", displayVolume, "%%");
      //PrintWithBuf(buf, BUFSIZE,"\nVolume: %s\n", volumeDigit);
        
      OSMboxPost(volume_Change, (void*) volumeDigit);
      
      OSTimeDly(100);
      
      break;
    case PAUSE_COMMAND:
      playBottom.press(0); stopSong = OS_TRUE;
      
      //Read_Message = OS_TRUE;
      
      Read_Update = OS_TRUE;
      
      Read_MailUpdate = OS_TRUE;
      
      music_status = "Paused";
      // Add To Mail Box
      OSMboxPost(mstatus_Change,(void*)music_status);
      break;
    case PLAY_COMMAND:
      pauseBottom.press(0);  
      haltBottom.press(0);
      
      haltPlayer = OS_FALSE;
      
      stopSong = OS_FALSE;
      
      if(After_Start)
      {
        Read_Update = OS_TRUE;
      
      Read_MailUpdate = OS_TRUE;
      
      music_status = "Playing";
      // Add To Mail Box
      OSMboxPost(mstatus_Change,(void*)music_status);
      }
     
      break;
    case NEXT_COMMAND:
      prevSong = OS_FALSE;  nextSong = OS_TRUE;
      
      // De-assert Button Pressed. 
      nextBottom.press(0); 
      
      pauseBottom.press(0);
      
      // Un-Pause Song if paused.
      // Condition will be taken care off by the Button Click Capture.
      // We are not un-Halting the entire music player, just the song.
      stopSong = OS_FALSE;
      
      
      break;
    case PREVIOUS_COMMAND:
      
      prevSong = OS_TRUE; nextSong = OS_FALSE;
      
      previousBottom.press(0); 
      pauseBottom.press(0);
      
      // Un-Pause Song if paused.
      // Condition will be taken care off by the Button Click Capture.
      // We are not un-Halting the entire music player, just the song.
      stopSong = OS_FALSE;
            
      break;      
    default:
      // Set Halt Player  To True
      
      haltPlayer = OS_TRUE;
      // Set Stop Song To True
      stopSong = OS_TRUE;
      
      // Release Play Button if Locked
      playBottom.press(0);
      
     // Read_Message = OS_TRUE;
      
      Read_Update = OS_TRUE;
      
      Read_MailUpdate = OS_TRUE;
      
      music_status = "Halting";
      // Add To Mail Box
      OSMboxPost(mstatus_Change,(void*)music_status);
      
      break;
    }
  }
  
  //OS_EXIT_CRITICAL();
  //while (1) OSTimeDly(1000);
  
}

/*******************************************************************************

Runs Display Task Code

*******************************************************************************/
void DisplayTask(void* pdata)
{
  // Future reference: Song Status
  
  // Get Message Box Data
  
  INT8U err;
  
  INT16U rdOnce = 0; // Read Once
  //char *displayData;
  //char *displayStats;
  char *display_name;
  char *display_status;
  char *display_volume = "100 %%";
  
  char buf[BUFSIZE];
  PrintWithBuf(buf, BUFSIZE,"Display Task building\n");
 
  
  while(1)
  {
    if(Read_Update)
    {
      if(Read_MailUpdate)
      {
        // Could be Halt Command or Pause Command or Stop Command or Begining of 
        // Music Player
        
        
        // Check for Volume Update
        
        if(SystemVolUp )
        {
          // Capture Volume Data in MailBox
          
          display_volume = (char*)OSMboxPend(volume_Change,100,&err);
          
          //DisplaySong(display_volume, 0, 140, 100,20);
          
        }
        else
        {
           // Read from Mail Box
           display_status = (char*)OSMboxPend(mstatus_Change,100,&err);
        }
                
      }
      else
      {
        // Read From Queue 
               
        display_name = (char*)OSQPend(queueMusic,0,&err);
        
        display_status = (char*)OSQPend(queueMusic,0,&err);
        
        rdOnce = 0;
        
      }
      
      // Display Values
      if(rdOnce == 0)
      {
        DisplaySong(display_name,0,40,150,20);
      }
      
      DisplaySong(display_status,0,90,150,20);
     
      // Display Volume
      DisplaySong(display_volume, 0, 140, 100,20);
      
      // Reset Booleans
      rdOnce++;
      Read_MailUpdate = OS_FALSE;
      Read_Update = OS_FALSE;
      
      SystemVolUp = OS_FALSE;
      
    }
  
     OSTimeDly(100);
  
  }
  
}

/*******************************************************************************

Runs LCD/Touch  code

*******************************************************************************/
void LcdTouchTask(void* pdata)
{
  PjdfErrCode pjdfErr;
  // INT8U err;
  INT32U length;
  
  
  //ButtonControlsEnum BtnControl_type;
  
  char buf[BUFSIZE];
  PrintWithBuf(buf, BUFSIZE, "LcdTouchDemoTask: starting\n");
  
  PrintWithBuf(buf, BUFSIZE, "Opening LCD driver: %s\n", PJDF_DEVICE_ID_LCD_ILI9341);
  // Open handle to the LCD driver
  HANDLE hLcd = Open(PJDF_DEVICE_ID_LCD_ILI9341, 0);
  if (!PJDF_IS_VALID_HANDLE(hLcd)) while(1);
  
  PrintWithBuf(buf, BUFSIZE, "Opening LCD SPI driver: %s\n", LCD_SPI_DEVICE_ID);
  // We talk to the LCD controller over a SPI interface therefore
  // open an instance of that SPI driver and pass the handle to 
  // the LCD driver.
  HANDLE hSPI = Open(LCD_SPI_DEVICE_ID, 0);
  if (!PJDF_IS_VALID_HANDLE(hSPI)) while(1);
  
  length = sizeof(HANDLE);
  pjdfErr = Ioctl(hLcd, PJDF_CTRL_LCD_SET_SPI_HANDLE, &hSPI, &length);
  if(PJDF_IS_ERROR(pjdfErr)) while(1);
  
  PrintWithBuf(buf, BUFSIZE, "Initializing LCD controller\n");
  lcdCtrl.setPjdfHandle(hLcd);
  lcdCtrl.begin();
  
  DrawLcdContents();
  
  PrintWithBuf(buf, BUFSIZE, "Initializing FT6206 touchscreen controller\n");
  
  // DRIVER TODO
  // Open a HANDLE for accessing device PJDF_DEVICE_ID_I2C1
  // <your code here>
  HANDLE hSPI1 = Open(PJDF_DEVICE_ID_I2C,0);
  
  if(!PJDF_IS_VALID_HANDLE(hSPI1))
  {
    while(1);
  }
  
  // Call Ioctl on that handle to set the I2C device address to FT6206_ADDR
  // <your code here>
  //(void *)FT6206_ADDR
  uint8_t address = FT6206_ADDR;
  pjdfErr =  Ioctl(hSPI1, PJDF_CTRL_I2C_SET_DEVICE_ADDRESS, &address , &length);
  if(PJDF_IS_ERROR(pjdfErr)) while(1);
  
  // Call setPjdfHandle() on the touch contoller to pass in the I2C handle
  // <your code here>
  touchCtrl.setPjdfHandle(hSPI1);
  touchCtrl.begin();
  
  
  if (! touchCtrl.begin(40)) {  // pass in 'sensitivity' coefficient
    //PrintWithBuf(buf, BUFSIZE, "Couldn't start FT6206 touchscreen controller\n");
    while (1);
  }
  
  int currentcolor = ILI9341_RED;
  
  //  Adafruit_GFX_Button pauseBottom
  pauseBottom = Adafruit_GFX_Button(); 
  
  // Adafruit_GFX_Button playBottom
  playBottom  = Adafruit_GFX_Button();
  
  // Adafruit_GFX_Button nextBottom
  nextBottom = Adafruit_GFX_Button();
  
  // Adafruit_GFX_Button haltBottom
  haltBottom = Adafruit_GFX_Button();
  
  // Adafruit_GFX_Button previousBottom
  previousBottom = Adafruit_GFX_Button();
  
  // Adafruit_GFX_Bitton VolIncBottom
  VolIncBottom = Adafruit_GFX_Button();
  
  // Adafruit_GFX_Bitton VolDesBottom
  VolDesBottom = Adafruit_GFX_Button();
  
  pauseBottom.initButton(&lcdCtrl, ILI9341_TFTWIDTH-30, ILI9341_TFTHEIGHT-30, // x, y center of button
                         60, 50, // width, height
                         ILI9341_YELLOW, // outline
                         ILI9341_BLACK, // fill
                         ILI9341_YELLOW, // text color
                         "Pause", // label
                         1); // text size
  pauseBottom.drawButton(0);
  
  
  
  playBottom.initButton(&lcdCtrl, ILI9341_TFTWIDTH-100, ILI9341_TFTHEIGHT-30, // x, y center of button
                        60, 50, // width, height
                        ILI9341_YELLOW, // outline
                        ILI9341_BLACK, // fill
                        ILI9341_YELLOW, // text color
                        "Play", // label
                        1); // text size
  playBottom.drawButton(0);
  
  nextBottom.initButton(&lcdCtrl, ILI9341_TFTWIDTH-170, ILI9341_TFTHEIGHT-30, // x, y center of button
                        60, 50, // width, height
                        ILI9341_YELLOW, // outline
                        ILI9341_BLACK, // fill
                        ILI9341_YELLOW, // text color
                        "Next", // label
                        1); // text size
  nextBottom.drawButton(0);
  
  previousBottom.initButton(&lcdCtrl, ILI9341_TFTWIDTH-170, ILI9341_TFTHEIGHT-90, // x, y center of button
                            60, 50, // width, height
                            ILI9341_YELLOW, // outline
                            ILI9341_BLACK, // fill
                            ILI9341_YELLOW, // text color
                            "Previous", // label
                            1); // text size
  previousBottom.drawButton(0);
  
  
  haltBottom.initButton(&lcdCtrl, ILI9341_TFTWIDTH-30, ILI9341_TFTHEIGHT-90, // x, y center of button
                        60, 50, // width, height
                        ILI9341_YELLOW, // outline
                        ILI9341_BLACK, // fill
                        ILI9341_YELLOW, // text color
                        "Stop", // label
                        1); // text size
  haltBottom.drawButton(0);
  
  VolIncBottom.initButton(&lcdCtrl, ILI9341_TFTWIDTH-30, ILI9341_TFTHEIGHT-150, // x, y center of button
                        60, 50, // width, height
                        ILI9341_YELLOW, // outline
                        ILI9341_BLACK, // fill
                        ILI9341_YELLOW, // text color
                        "Vol Up", // label
                        1); // text size
  VolIncBottom.drawButton(0);
  
  
  VolDesBottom.initButton(&lcdCtrl, ILI9341_TFTWIDTH-30, ILI9341_TFTHEIGHT-210, // x, y center of button
                        60, 50, // width, height
                        ILI9341_YELLOW, // outline
                        ILI9341_BLACK, // fill
                        ILI9341_YELLOW, // text color
                        "Vol Down", // label
                        1); // text size
  VolDesBottom.drawButton(0);
  
  
  //VolDesBottom
  
  // By Default this will be 0 - False.
  // Meaning that is was Released
  
  pauseBottom.press(0);
  
  playBottom.press(0);
  
  nextBottom.press(0);
  
  previousBottom.press(0);
  
  VolIncBottom.press(0);
  
  while (1) { 
    boolean touched;
    
    touched = touchCtrl.touched();
    
    if (! touched) {
      OSTimeDly(5);
      continue;
    }
    
    TS_Point point;
    
    point = touchCtrl.getPoint();
    if (point.x == 0 && point.y == 0)
    {
      continue; // usually spurious, so ignore
    }
    
    // transform touch orientation to screen orientation.
    TS_Point p = TS_Point();
    p.x = MapTouchToScreen(point.x, 0, ILI9341_TFTWIDTH, ILI9341_TFTWIDTH, 0);
    p.y = MapTouchToScreen(point.y, 0, ILI9341_TFTHEIGHT, ILI9341_TFTHEIGHT, 0);
    
    lcdCtrl.fillCircle(p.x, p.y, PENRADIUS, currentcolor);
    
    // Don't Accept Buttons when HaltPlayer is set To True, Apart from Play Button
    if(pauseBottom.contains(p.x, p.y) && pauseBottom.isPressed() == 0 && !haltPlayer)
    {
      
      if(pauseBottom.justReleased() == 1 || pauseBottom.justPressed() == 0)
      {
        
        // Assert Pause Button
        pauseBottom.press(1); 
        
        if(pauseBottom.isPressed() == 1)
        {
          // Send Control Message to Mailbox
          
          PrintString("\nPause Asserted\n");
          
          // ButtonControlsEnum BtnControl_type1 = PAUSE_COMMAND ;
          
          OSMboxPost(buttonMBox,(void*)PAUSE_COMMAND);
          
        }
        
      } //OSTimeDly(100);
      
    }
    else if(playBottom.contains(p.x, p.y) && playBottom.isPressed() == 0)
    { 
      
      if(playBottom.justReleased() == 1 || playBottom.justPressed() == 0)
      {
        // Assert Play Button
        playBottom.press(1); 
        
        // Call Interrupt
        if(playBottom.isPressed() == 1)
        {
          // Send Control Message to Mailbox
          
          PrintString("\nPlay Asserted\n");
          
          // ButtonControlsEnum BtnControl_type = PLAY_COMMAND ;
          
          OSMboxPost(buttonMBox,(void*)PLAY_COMMAND);
          
        }
      }
      
    }
    else if (nextBottom.contains(p.x, p.y) && nextBottom.isPressed() == 0 && !haltPlayer)
    {
      if(nextBottom.justReleased() == 1 || nextBottom.justPressed() == 0)
      {
        // Assert Next Button
        nextBottom.press(1); 
        
        // Call Interrupt
        if(nextBottom.isPressed() == 1)
        {
          
          // Send Control Message to Mailbox
          
          PrintString("\nNext Asserted\n");
          
          // NEXT_COMMAND, PREVIOUS_COMMAND, previousBottom
          
          OSMboxPost(buttonMBox,(void*)NEXT_COMMAND);
          
        }
      }
    }
    else if (previousBottom.contains(p.x, p.y) && previousBottom.isPressed() == 0 && !haltPlayer)
    {
      if(previousBottom.justReleased() == 1 || previousBottom.justPressed() == 0)
      {
        // Assert Previous Button
        previousBottom.press(1); 
        
        // Call Interrupt
        if(previousBottom.isPressed() == 1)
        {
          // Send Control Message to Mailbox
          
          PrintString("\nPrevious Asserted\n");
          
          // NEXT_COMMAND, PREVIOUS_COMMAND, previousBottom
          
          OSMboxPost(buttonMBox,(void*)PREVIOUS_COMMAND);
          
        }
      }
    }//haltBottom
    else if(haltBottom.contains(p.x, p.y) && haltBottom.isPressed() == 0 && !haltPlayer)
    {
      if(haltBottom.justReleased() == 1 || haltBottom.justPressed() == 0)
      {
        // Assert Halt Button
        haltBottom.press(1);
        
        // Send Button Clicked to Control Task
        if(haltBottom.isPressed() == 1)
        {
          PrintString("\nHalt Asserted\n");
          
          OSMboxPost(buttonMBox,(void*)HALT_COMMAND);
        }
      }
    }
    else if(VolIncBottom.contains(p.x, p.y) && VolIncBottom.isPressed() == 0 && !haltPlayer)
    {
      if(VolIncBottom.justReleased() == 1 || VolIncBottom.justPressed() == 0)
      {
        // Assert Vol Up Button
        VolIncBottom.press(1);
        
        // Send Button Clicked to Control Task
        if(VolIncBottom.isPressed() == 1)
        {
          PrintString("\nIncrease Volume \n");
          
          OSMboxPost(buttonMBox,(void*)VOLUP_COMMAND);
        }
      }
    }
    else if(VolDesBottom.contains(p.x, p.y) && VolDesBottom.isPressed() == 0 && !haltPlayer)
    {
      if(VolDesBottom.justReleased() == 1 || VolDesBottom.justPressed() == 0)
      {
        // Assert Vol Down Button
        VolDesBottom.press(1);
        
        // Send Button Clicked to Control Task
        if(VolDesBottom.isPressed() == 1)
        {
          PrintString("\nDecrease Volume \n");
          
          OSMboxPost(buttonMBox,(void*)VOLDW_COMMAND);
        }
      }
    }
    
    OSTimeDly(100);
    
  } 
  
}

/************************************************************************************

Runs MP3 demo code

************************************************************************************/
/*void Mp3DemoTask(void* pdata)
{
PjdfErrCode pjdfErr;
INT32U length;

char buf[BUFSIZE];
PrintWithBuf(buf, BUFSIZE, "Mp3DemoTask: starting\n");

PrintWithBuf(buf, BUFSIZE, "Opening MP3 driver: %s\n", PJDF_DEVICE_ID_MP3_VS1053);
// Open handle to the MP3 decoder driver
HANDLE hMp3 = Open(PJDF_DEVICE_ID_MP3_VS1053, 0);
if (!PJDF_IS_VALID_HANDLE(hMp3)) while(1);

PrintWithBuf(buf, BUFSIZE, "Opening MP3 SPI driver: %s\n", MP3_SPI_DEVICE_ID);
// We talk to the MP3 decoder over a SPI interface therefore
// open an instance of that SPI driver and pass the handle to 
// the MP3 driver.
HANDLE hSPI = Open(MP3_SPI_DEVICE_ID, 0);
if (!PJDF_IS_VALID_HANDLE(hSPI)) while(1);

length = sizeof(HANDLE);
pjdfErr = Ioctl(hMp3, PJDF_CTRL_MP3_SET_SPI_HANDLE, &hSPI, &length);
if(PJDF_IS_ERROR(pjdfErr)) while(1);

// Send initialization data to the MP3 decoder and run a test
PrintWithBuf(buf, BUFSIZE, "Starting MP3 device test\n");
Mp3Init(hMp3);
int count = 0;

while (1)
{
OSTimeDly(500);

//if(stopSong == OS_FALSE)
// {
PrintWithBuf(buf, BUFSIZE, "Begin streaming sound file  count=%d\n", ++count);
Mp3Stream(hMp3, (INT8U*)Train_Crossing, sizeof(Train_Crossing)); 
PrintWithBuf(buf, BUFSIZE, "Done streaming sound file  count=%d\n", count);
// }

  }
}
*/

// Renders a character at the current cursor position on the LCD
static void PrintCharToLcd(char c)
{
  lcdCtrl.write(c);
}

/************************************************************************************

Print a formated string with the given buffer to LCD.
Each task should use its own buffer to prevent data corruption.

************************************************************************************/
void PrintToLcdWithBuf(char *buf, int size, char *format, ...)
{
  va_list args;
  va_start(args, format);
  PrintToDeviceWithBuf(PrintCharToLcd, buf, size, format, args);
  va_end(args);
}




