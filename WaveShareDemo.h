#include "LCD_Driver.h"
#include "LCD_GUI.h"
#include "LCD_Touch.h"

void GUI_Showtime(POINT Xstart, POINT Ystart, POINT Xend, POINT Yend, DEV_TIME *pTime, COLOR Color);
//show
void GUI_Show(void);

void TP_Dialog(void);
void TP_DrawBoard(void);