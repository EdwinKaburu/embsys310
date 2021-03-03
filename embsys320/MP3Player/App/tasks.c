/************************************************************************************

Copyright (c) 2001-2016  University of Washington Extension.

Module Name:

tasks.c

Module Description:

The tasks that are executed by the test application.

2016/2 Nick Strathy adapted it for NUCLEO-F401RE 

************************************************************************************/


#include <stdarg.h>

#include "bsp.h"
#include "print.h"
#include "mp3Util.h"
#include "button_ui.h"

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


#define PENRADIUS 3

long MapTouchToScreen(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
#include "train_crossing.h"

#define BUFSIZE 256

/************************************************************************************

Allocate the stacks for each task.
The maximum number of tasks the application can have is defined by OS_MAX_TASKS in os_cfg.h

************************************************************************************/

static OS_STK   LcdTouchDemoTaskStk[APP_CFG_TASK_START_STK_SIZE];
static OS_STK   Mp3SDTaskStk[APP_CFG_TASK_START_STK_SIZE];

// ---------------- Task prototypes  -------------------------------------------
void LcdTouchDemoTask(void* pdata);

void Mp3SDTask(void* pdata);


// -----------------------------------------------------------------------------
OS_EVENT *musicStatus;

// Useful functions
void PrintToLcdWithBuf(char *buf, int size, char *format, ...);

// Globals
BOOLEAN nextSong = OS_FALSE;

BOOLEAN stopSong = OS_FALSE;

// Event flags for synchronizing mailbox messages
OS_FLAG_GRP *btnFlags = 0;

// Mutext
OS_EVENT *pauseMutex;



/************************************************************************************

This task is the initial task running, started by main(). It starts
the system tick timer and creates all the other tasks. Then it deletes itself.

************************************************************************************/
void StartupTask(void* pdata)
{
  INT8U err;
  
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
  
  btnFlags = OSFlagCreate(0x0,&err);
  
  
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
  OSTaskCreate(Mp3SDTask, (void*)0, &Mp3SDTaskStk[APP_CFG_TASK_START_STK_SIZE-1], task_prio++);
  
  OSTaskCreate(LcdTouchDemoTask, (void*)0, &LcdTouchDemoTaskStk[APP_CFG_TASK_START_STK_SIZE-1], task_prio++);
  
 
  
 // OSTaskCreate(PauseButtonTask, (void*)0, &PauseTaskStk[APP_CFG_TASK_START_STK_SIZE-1], task_prio++);
 // OSTaskCreate(PlayButtonTask, (void*)0, &PlayTaskStk[APP_CFG_TASK_START_STK_SIZE-1], task_prio++);
  
  pauseMutex = OSMutexCreate(task_prio++, &err);
  
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
  lcdCtrl.setCursor(40, 60);
  lcdCtrl.setTextColor(ILI9341_WHITE);  
  lcdCtrl.setTextSize(2);
  PrintToLcdWithBuf(buf, BUFSIZE, "Hello World!");
  
  OS_EXIT_CRITICAL();
  
}


/*******************************************************************************
Button Handlers
*******************************************************************************/

void PlayBtn_Handler(void)
{
  
  INT8U err;
  
  OS_CPU_SR  cpu_sr;
  
  OS_ENTER_CRITICAL();
  
  // Non-Blocking Wait
  OSFlagAccept(btnFlags ,0x2,OS_FLAG_WAIT_SET_ANY,&err );
  
  pauseBottom.press(0);   stopSong = OS_FALSE;
  
  // Active Play Button
  OSFlagPost(btnFlags ,0x1,OS_FLAG_CLR ,&err);
  
  OS_EXIT_CRITICAL();
  
  OSIntExit();
  
}

void PauseBtn_Handler(void)
{
  INT8U err;
  
  OS_CPU_SR  cpu_sr;
  
  OS_ENTER_CRITICAL();
  
  // Redirected from Interrupt
  // PauseButton must Active at the end.
  // 1 - Means Active (Set)
  // 0 - Means Deactive (Clear)
    
  
  // Pause
  // Bit 1 or the Bit second ( 2 ) - Play Button Must be deactivated before continuing on.
  // Will be cleared on Pause-Button Click.
  OSFlagAccept(btnFlags ,0x1,OS_FLAG_WAIT_SET_ANY,&err );
  
  // If that is the case we can release the playBottom before
  // Activating the Pause Button
  
  playBottom.press(0);stopSong = OS_TRUE;
  
  // Active Pause Button, by setting the Bit 0
  OSFlagPost(btnFlags ,0x2,OS_FLAG_CLR,&err);
  
  OS_EXIT_CRITICAL();
  
  OSIntExit();
  
  
}


/*******************************************************************************
RUN SD TASK CODE
********************************************************************************/

void Mp3SDTask(void* pdata)
{
  PjdfErrCode pjdfErr;
  INT32U length;
  
  char buf[BUFSIZE];
  PrintWithBuf(buf, BUFSIZE, "Mp3DemoTask: starting\n");
  
 // PrintWithBuf(buf, BUFSIZE, "Opening MP3 driver: %s\n", PJDF_DEVICE_ID_MP3_VS1053);
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
  // int count = 0;
  
  //DrawBackGround();
  
  File dir = SD.open("/");
  while (1)
  {
    while (1)
    {
      File entry = dir.openNextFile();
      if (!entry)
      {
        break;
      }
      if (entry.isDirectory())  // skip directories
      {
        entry.close();
        continue;
      } 
      
      //DrawLcdContents(entry.name(),40, 100);
      
      // PrintWithBuf(buf, BUFSIZE, "Begin streaming sd file  count=%d\n", ++count);                
      Mp3StreamSDFile(hMp3, entry.name()); 
      //  PrintWithBuf(buf, BUFSIZE, "Done streaming sound file  count=%d\n", count);
     
      entry.close();
    }
    dir.seek(0); // reset directory file to read again;
  }
}

/************************************************************************************

Runs LCD/Touch demo code

************************************************************************************/
void LcdTouchDemoTask(void* pdata)
{
  PjdfErrCode pjdfErr;
  INT8U err;
  INT32U length;
  
  char buf[BUFSIZE];
  //PrintWithBuf(buf, BUFSIZE, "LcdTouchDemoTask: starting\n");
  
 // PrintWithBuf(buf, BUFSIZE, "Opening LCD driver: %s\n", PJDF_DEVICE_ID_LCD_ILI9341);
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
  
  
  // Declare a Pause button
  
  //  Adafruit_GFX_Button pauseBottom
  pauseBottom = Adafruit_GFX_Button(); 
  
  // Declare a Play button
  //Adafruit_GFX_Button playBottom
  playBottom  = Adafruit_GFX_Button(); 
  
  
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
  
  
  // By Default this will be 0 - False.
  // Meaning that is was Released
   
  pauseBottom.press(0);
  
  playBottom.press(0);
  
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
    
    /*
    PauseButton : This will assert the PauseButton,
    */
    
    if(pauseBottom.contains(p.x, p.y) && pauseBottom.isPressed() == 0)
    {
      // If So, Check the JustRelease for PauseBtn Indicating, it was released by the interrupt
      
     /*   int is_Pressed = pauseBottom.isPressed() ? 1:0;
        
        PrintString("\nPause-IsPressed \t");
        Print_uint32(is_Pressed);
        PrintString("\n");
        
        int is_justReleased = pauseBottom.justReleased() ? 1: 0;
        
        PrintString("nPause-JustReleased \t");
        Print_uint32(is_justReleased);
        PrintString("\n");
        
        int is_justPressed =  pauseBottom.justPressed() ? 1:0;
        
        PrintString("nPause-JustPressed \t");
        Print_uint32(is_justPressed);
        PrintString("\n");*/
      
      if(pauseBottom.justReleased() == 1 || pauseBottom.justPressed() == 0)
      {
        // Bit 1 (Pause Btn): Will be 0, indicating PauseBtn is deactived, so we can accept the input 
        // Bit 2 (Play Btn): Will be 1, indicating PlayBtn is activate and we are running
        // Check Flags, Bit 0 must be 0 (False), to activate the Pause Btn
        
        OSFlagPend(btnFlags ,0x1,OS_FLAG_WAIT_CLR_ALL,0,&err );
        
        // If So, Both isPressed and JustPressed must be 0. Enter into a Mutex to Assert the PauseBtn.
        
        OSMutexPend(pauseMutex,0, &err);
        
        // Assert Pause Button
        
        pauseBottom.press(1); 
        
        // Exit Mutex
        OSMutexPost(pauseMutex); 
        
        // Pause Button Is Active . So Bit 1 : becames 0 , meaning Playing is Deactived.
        
        OSFlagPost(btnFlags ,0x1,OS_FLAG_SET, &err);
        
        
        if(pauseBottom.isPressed() == 1)
        {
          // Enter Interrupt
          OSIntEnter();
          PauseBtn_Handler();
          
         // OSTimeDly(10);
          
          //OSTimeDly(10);
        }
        
      }
      
    }
    else if(playBottom.contains(p.x, p.y) && playBottom.isPressed() == 0)
    {      
      
       /* int is_Pressed = playBottom.isPressed() ? 1:0;
        
        PrintString("\nPlay-IsPressed \t");
        Print_uint32(is_Pressed);
        PrintString("\n");
        
        int is_justReleased = playBottom.justReleased() ? 1: 0;
        
        PrintString("Play-JustReleased \t");
        Print_uint32(is_justReleased);
        PrintString("\n");
        
        int is_justPressed =  playBottom.justPressed() ? 1:0;
        
        PrintString("Play-JustPressed \t");
        Print_uint32(is_justPressed);
        PrintString("\n");
      */
      if(playBottom.justReleased() == 1 || playBottom.justPressed() == 0)
      {
        
        // Play Button Is Deactived
        // Pause Button is Active OS_FLAG_WAIT_SET_ALL
        // OSFlagPend(btnFlags ,0x1,OS_FLAG_WAIT_SET_ALL,0,&err );
        
        OSFlagPend(btnFlags ,0x2,OS_FLAG_WAIT_CLR_ALL,0,&err );
        
        // If So, Both isPressed and JustPressed must be 0. 
        // Enter into a Mutex to Assert the PauseBtn.
        
        OSMutexPend(pauseMutex,0, &err);
        
        // Assert Pause Button
        playBottom.press(1); 
        
        // Exit Mutex
        OSMutexPost(pauseMutex);
        
        // Play Button Is Active .
        // So Bit 1 : becames 0 , meaning Pausing is Deactived.
        
        OSFlagPost(btnFlags ,0x2,OS_FLAG_SET,&err);
        
        // Call Interrupt
        if(playBottom.isPressed() == 1)
        {
          // Enter Interrupt
          OSIntEnter();
          PlayBtn_Handler();
        }
      }
      nextBottom
    }
    else if (nextBottom.contains(p.x, p.y) && nextBottom.isPressed() == 0)
    {
      if(playBottom.justReleased() == 1 || playBottom.justPressed() == 0)
      {
         playBottom.press(1);
         
         // Enter Interrupt
      }
    }
    OSTimeDly(5);
  } 
  
   //OSTimeDly(15);
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



