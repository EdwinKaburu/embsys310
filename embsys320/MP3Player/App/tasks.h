#ifndef UC_TASK
#define UC_TASK

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

#endif
