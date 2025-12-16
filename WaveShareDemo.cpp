#include <Arduino.h>
#include "WaveShareDemo.h"
#include "Debug.h"

/******************************************************************************
  function:	According to the display area adaptive display time
  parameter:
		xStart :   X direction Start coordinates
		Ystart :   Y direction Start coordinates
		Xend   :   X direction end coordinates
		Yend   :   Y direction end coordinates
		pTime  :   Pointer to the definition of the structure
		Color  :   Set show color
  note:
******************************************************************************/

void GUI_Showtime(POINT Xstart, POINT Ystart, POINT Xend, POINT Yend,
                  DEV_TIME *pTime, COLOR Color)
{
  uint8_t value[10] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
  sFONT *Font = NULL;

  //According to the display area adaptive font size
  POINT Dx = (Xend - Xstart) / 7;//Determine the spacing between characters
  POINT Dy = Yend - Ystart;      //determine the font size
  Font = GUI_GetFontSize(Dx, Dy);
  
  if ((pTime->Sec % 10) < 10 && (pTime->Sec % 10) > 0) {
    LCD_SetArea2Color(Xstart + Dx * 6, Ystart, Xend, Yend, WHITE);// xx:xx:x0
  } else {
    if ((pTime->Sec / 10) < 6 && (pTime->Sec / 10) > 0) {
      LCD_SetArea2Color(Xstart + Dx * 5, Ystart, Xend, Yend, WHITE);// xx:xx:00
    }
    else {//sec = 60
      pTime->Min = pTime->Min + 1;
      pTime->Sec = 0;
      if ((pTime->Min % 10) < 10 && (pTime->Min % 10) > 0) {
        LCD_SetArea2Color(Xstart + Dx * 3 + Dx / 2, Ystart, Xend, Yend, WHITE);// xx:x0:00
      } 
      else {
        if ((pTime->Min / 10) < 6 && (pTime->Min / 10) > 0) {
          LCD_SetArea2Color(Xstart + Dx * 2 + Dx / 2, Ystart, Xend, Yend, WHITE);// xx:00:00
        } 
        else {  //min = 60
          pTime->Hour =  pTime->Hour + 1;
          pTime->Min = 0;
          if ((pTime->Hour % 10) < 4 && (pTime->Hour % 10) > 0 && pTime->Hour < 24) {// x0:00:00
            LCD_SetArea2Color(Xstart + Dx, Ystart, Xend, Yend, WHITE);
          } else {
            pTime->Hour = 0;
            pTime->Min = 0;
            pTime->Sec = 0;
            LCD_SetArea2Color(Xstart, Ystart, Xend, Yend, WHITE);// 00:00:00
          }
        }
      }
    }
  }

  //Write data into the cache
  GUI_DisChar(Xstart                           , Ystart, value[pTime->Hour / 10], Font, FONT_BACKGROUND, Color);
  GUI_DisChar(Xstart + Dx                      , Ystart, value[pTime->Hour % 10], Font, FONT_BACKGROUND, Color);
  GUI_DisChar(Xstart + Dx  + Dx / 4 + Dx / 2   , Ystart, ':'                    , Font, FONT_BACKGROUND, Color);
  GUI_DisChar(Xstart + Dx * 2 + Dx / 2         , Ystart, value[pTime->Min / 10] , Font, FONT_BACKGROUND, Color);
  GUI_DisChar(Xstart + Dx * 3 + Dx / 2         , Ystart, value[pTime->Min % 10] , Font, FONT_BACKGROUND, Color);
  GUI_DisChar(Xstart + Dx * 4 + Dx / 2 - Dx / 4, Ystart, ':'                    , Font, FONT_BACKGROUND, Color);
  GUI_DisChar(Xstart + Dx * 5                  , Ystart, value[pTime->Sec / 10] , Font, FONT_BACKGROUND, Color);
  GUI_DisChar(Xstart + Dx * 6                  , Ystart, value[pTime->Sec % 10] , Font, FONT_BACKGROUND, Color);
}

/******************************************************************************
  function:	GUI_Show
  note:
	Clear,
	Draw Line,
	Draw Rectangle,
	Draw Rings,
	Draw Olympic Rings,
	Display String,
	Show Pic
******************************************************************************/
void GUI_Show(void)
{
  GUI_Clear(WHITE);
  if (sLCD_DIS.LCD_Dis_Column > sLCD_DIS.LCD_Dis_Page) { //Horizontal screen display

    DEBUG("Draw Horizontal Lines\r\n");
    GUI_DrawLine(0, 10, LCD_WIDTH, 10, RED, LINE_DOTTED, DOT_PIXEL_8X8);
    GUI_DrawLine(0, 20, LCD_WIDTH, 20, RED, LINE_DOTTED, DOT_PIXEL_DFT);
    GUI_DrawLine(0, 300, LCD_WIDTH, 300, RED, LINE_DOTTED, DOT_PIXEL_DFT);
    GUI_DrawLine(0, 310, LCD_WIDTH, 310, RED, LINE_DOTTED, DOT_PIXEL_4X4);

    DEBUG("Draw Rectangle\r\n");
    GUI_DrawRectangle(10, 30, sLCD_DIS.LCD_Dis_Column - 10, sLCD_DIS.LCD_Dis_Page - 30, BLUE, DRAW_EMPTY, DOT_PIXEL_4X4);
    GUI_DrawRectangle(20, 40, sLCD_DIS.LCD_Dis_Column - 20, 60, BLUE, DRAW_FULL, DOT_PIXEL_DFT);
    GUI_DrawRectangle(20, 65, sLCD_DIS.LCD_Dis_Column - 25, sLCD_DIS.LCD_Dis_Page - 40, MAGENTA, DRAW_EMPTY, DOT_PIXEL_6X6, LINE_DOTTED);

    DEBUG("Draw Olympic Rings\r\n");
    uint16_t Cx1 = 190, Cy1 = 240, Cr = 20;
    uint16_t Cx2 = Cx1 + (2.5 * Cr), Cy2 = Cy1;
    uint16_t Cx3 = Cx1 + (5 * Cr), Cy3 = Cy1;
    uint16_t Cx4 = ( Cx1 + Cx2 ) / 2, Cy4 = Cy1 + Cr;
    uint16_t Cx5 = ( Cx2 + Cx3 ) / 2, Cy5 = Cy1 + Cr;

    GUI_DrawCircle( Cx1, Cy1, Cr, BLUE, DRAW_EMPTY, DOT_PIXEL_3X3);
    GUI_DrawCircle( Cx2, Cy2, Cr, BLACK, DRAW_EMPTY, DOT_PIXEL_3X3);
    GUI_DrawCircle( Cx3, Cy3, Cr, RED, DRAW_EMPTY, DOT_PIXEL_3X3);
    GUI_DrawCircle( Cx4, Cy4, Cr, YELLOW, DRAW_EMPTY, DOT_PIXEL_3X3);
    GUI_DrawCircle( Cx5, Cy5, Cr, GREEN, DRAW_EMPTY, DOT_PIXEL_3X3);

    DEBUG("Draw Realistic circles\r\n");
    GUI_DrawCircle(58, 250, 30, CYAN, DRAW_FULL, DOT_PIXEL_DFT);
    GUI_DrawCircle(sLCD_DIS.LCD_Dis_Column - 55, 250, 30, CYAN, DRAW_EMPTY, DOT_PIXEL_4X4);

    DEBUG("Display String\r\n");
    GUI_DisString_EN(80, 80, "WaveShare Electronic", &Font24, LCD_BACKGROUND, BLUE);
    GUI_DisString_EN(80, 120, "3.5inch TFTLCD", &Font20, YELLOW, BLUE);

    DEBUG("Display Nummber\r\n");
    GUI_DisNum(80, 150, 1234567890, &Font16, LCD_BACKGROUND, BLUE);

  } else { //Vertical screen display

    DEBUG("Draw Line\r\n");
    GUI_DrawLine(0, 10, sLCD_DIS.LCD_Dis_Column , 10, RED, LINE_SOLID, DOT_PIXEL_2X2);
    GUI_DrawLine(0, 20, sLCD_DIS.LCD_Dis_Column , 20, RED, LINE_DOTTED, DOT_PIXEL_DFT);
    GUI_DrawLine(0, sLCD_DIS.LCD_Dis_Page - 20, sLCD_DIS.LCD_Dis_Column , sLCD_DIS.LCD_Dis_Page - 20, RED, LINE_DOTTED, DOT_PIXEL_DFT);
    GUI_DrawLine(0, sLCD_DIS.LCD_Dis_Page - 10, sLCD_DIS.LCD_Dis_Column , sLCD_DIS.LCD_Dis_Page - 10, RED, LINE_SOLID, DOT_PIXEL_2X2);

    DEBUG("Draw Rectangle\r\n");
    GUI_DrawRectangle(10, 30, sLCD_DIS.LCD_Dis_Column - 10, sLCD_DIS.LCD_Dis_Page - 30, BLUE, DRAW_EMPTY, DOT_PIXEL_DFT);
    GUI_DrawRectangle(20, 40, sLCD_DIS.LCD_Dis_Column - 20, 60, BLUE, DRAW_FULL, DOT_PIXEL_DFT);

    DEBUG("Draw Olympic Rings\r\n");
    uint16_t Cx1 = 120, Cy1 = 300, Cr = 20;
    uint16_t Cx2 = Cx1 + (2.5 * Cr), Cy2 = Cy1;
    uint16_t Cx3 = Cx1 + (5 * Cr), Cy3 = Cy1;
    uint16_t Cx4 = ( Cx1 + Cx2 ) / 2, Cy4 = Cy1 + Cr;
    uint16_t Cx5 = ( Cx2 + Cx3 ) / 2, Cy5 = Cy1 + Cr;

    GUI_DrawCircle( Cx1, Cy1, Cr, BLUE, DRAW_EMPTY, DOT_PIXEL_2X2);
    GUI_DrawCircle( Cx2, Cy2, Cr, BLACK, DRAW_EMPTY, DOT_PIXEL_2X2);
    GUI_DrawCircle( Cx3, Cy3, Cr, RED, DRAW_EMPTY, DOT_PIXEL_2X2);
    GUI_DrawCircle( Cx4, Cy4, Cr, YELLOW, DRAW_EMPTY, DOT_PIXEL_2X2);
    GUI_DrawCircle( Cx5, Cy5, Cr, GREEN, DRAW_EMPTY, DOT_PIXEL_2X2);

    DEBUG("Draw Realistic circles\r\n");
    GUI_DrawCircle(50, 400, 30, CYAN, DRAW_FULL, DOT_PIXEL_DFT);
    GUI_DrawCircle(sLCD_DIS.LCD_Dis_Column - 50, 400, 30, CYAN, DRAW_FULL, DOT_PIXEL_DFT);

    DEBUG("Display String\r\n");
    GUI_DisString_EN(40, 120, "WaveShare Electronic", &Font24, LCD_BACKGROUND, BLUE);
    GUI_DisString_EN(40, 180, "3.5inch TFTLCD", &Font20, RED, BLUE);

    DEBUG("Display Nummber\r\n");
    GUI_DisNum(40, 210, 1234567890, &Font16, LCD_BACKGROUND, BLUE);

  }
}

/*******************************************************************************
  function:
        Paint the Delete key and paint color choose area
*******************************************************************************/
void TP_Dialog(void)
{
  LCD_Clear(LCD_BACKGROUND);
  DEBUG("Drawing...\r\n");
  //Horizontal screen display
  if (sLCD_DIS.LCD_Dis_Column > sLCD_DIS.LCD_Dis_Page) {
    //Clear screen
    GUI_DisString_EN(sLCD_DIS.LCD_Dis_Column - 60, 0,
                     "CLEAR", &Font16, RED, BLUE);
    //adjustment
    GUI_DisString_EN(sLCD_DIS.LCD_Dis_Column - 120, 0,
                     "AD", &Font24, RED, BLUE);
    //choose the color
    GUI_DrawRectangle(sLCD_DIS.LCD_Dis_Column - 50, 20,
                      sLCD_DIS.LCD_Dis_Column, 70,
                      BLUE, DRAW_FULL, DOT_PIXEL_1X1);
    GUI_DrawRectangle(sLCD_DIS.LCD_Dis_Column - 50, 80,
                      sLCD_DIS.LCD_Dis_Column, 130,
                      GREEN, DRAW_FULL, DOT_PIXEL_1X1);
    GUI_DrawRectangle(sLCD_DIS.LCD_Dis_Column - 50, 140,
                      sLCD_DIS.LCD_Dis_Column, 190,
                      RED, DRAW_FULL, DOT_PIXEL_1X1);
    GUI_DrawRectangle(sLCD_DIS.LCD_Dis_Column - 50, 200,
                      sLCD_DIS.LCD_Dis_Column, 250,
                      YELLOW, DRAW_FULL, DOT_PIXEL_1X1);
    GUI_DrawRectangle(sLCD_DIS.LCD_Dis_Column - 50, 260,
                      sLCD_DIS.LCD_Dis_Column, 310,
                      BLACK, DRAW_FULL, DOT_PIXEL_1X1);
    
    typedef struct {
      int x1, y1, x2, y2;
    } Line;
    
    Line lines[100];
    int x = 240;
    int y = 160;
    int length = 10;
    int dir = 0;
    int count = 0;

    for (int i = 0; i < 100; i++) {
      int x2 = x;
      int y2 = y;
      switch (dir)
      {
        /* code */
        case 0: x2 = x + length; break;
        case 1: y2 = y + length; break;
        case 2: x2 = x - length; break;
        case 3: y2 = y - length; break;
      }
      GUI_DrawLine(x, y, x2, y2, BLACK, LINE_SOLID, DOT_PIXEL_3X3);
      delay(200);

      lines[count].x1 = x;
      lines[count].y1 = y;
      lines[count].x2 = x2;
      lines[count].y2 = y2;
      count++;

      x = x2;
      y = y2;
      if (i % 2 == 1)
        length += 5;
      dir = (dir + 1) % 4;
    }
    
    for (int i = count - 1; i >=0; i--){
      GUI_DrawLine(lines[i].x1, lines[i].y1, lines[i].x2, lines[i].y2, WHITE, LINE_SOLID, DOT_PIXEL_3X3);
      delay(200);
    }
    
  } else { //Vertical screen display
    GUI_DisString_EN(sLCD_DIS.LCD_Dis_Column - 60, 0,
                     "CLEAR", &Font16, RED, BLUE);
    GUI_DisString_EN(sLCD_DIS.LCD_Dis_Column - 120, 0,
                     "AD", &Font24, RED, BLUE);
    GUI_DrawRectangle(20, 20, 70, 70, BLUE, DRAW_FULL, DOT_PIXEL_1X1);
    GUI_DrawRectangle(80, 20, 130, 70, GREEN, DRAW_FULL, DOT_PIXEL_1X1);
    GUI_DrawRectangle(140, 20, 190, 70, RED, DRAW_FULL, DOT_PIXEL_1X1);
    GUI_DrawRectangle(200, 20, 250, 70, YELLOW, DRAW_FULL, DOT_PIXEL_1X1);
    GUI_DrawRectangle(260, 20, 310, 70, BLACK, DRAW_FULL, DOT_PIXEL_1X1);
  }
}

/*******************************************************************************
  function:
        Draw Board
*******************************************************************************/
void TP_DrawBoard(void)
{
  //  sTP_DEV.chStatus &= ~(1 << 6);
  TP_Scan(0);
  if (sTP_DEV.chStatus & TP_PRESS_DOWN) {     //Press the button
    //Horizontal screen
    if (sTP_Draw.Xpoint < sLCD_DIS.LCD_Dis_Column &&
        //Determine whether the law is legal
        sTP_Draw.Ypoint < sLCD_DIS.LCD_Dis_Page) {
      //Judgment is horizontal screen
      if (sLCD_DIS.LCD_Dis_Column > sLCD_DIS.LCD_Dis_Page) {
        if (sTP_Draw.Xpoint > (sLCD_DIS.LCD_Dis_Column - 60) &&
            sTP_Draw.Ypoint < 16) {     //Clear Board
          TP_Dialog();
        } else if (sTP_Draw.Xpoint > (sLCD_DIS.LCD_Dis_Column - 120) &&
                   sTP_Draw.Xpoint < (sLCD_DIS.LCD_Dis_Column - 80) &&
                   sTP_Draw.Ypoint < 24) { //afresh adjustment
          TP_Calibration();
          TP_Dialog();
        } else if (sTP_Draw.Xpoint > (sLCD_DIS.LCD_Dis_Column - 50) &&
                   sTP_Draw.Xpoint < sLCD_DIS.LCD_Dis_Column &&
                   sTP_Draw.Ypoint > 20 &&
                   sTP_Draw.Ypoint < 70) {
          sTP_Draw.Color = BLUE;
        } else if (sTP_Draw.Xpoint > (sLCD_DIS.LCD_Dis_Column - 50) &&
                   sTP_Draw.Xpoint < sLCD_DIS.LCD_Dis_Column &&
                   sTP_Draw.Ypoint > 80 &&
                   sTP_Draw.Ypoint < 130) {
          sTP_Draw.Color = GREEN;
        } else if (sTP_Draw.Xpoint > (sLCD_DIS.LCD_Dis_Column - 50) &&
                   sTP_Draw.Xpoint < sLCD_DIS.LCD_Dis_Column &&
                   sTP_Draw.Ypoint > 140 &&
                   sTP_Draw.Ypoint < 190) {
          sTP_Draw.Color = RED;
        } else if (sTP_Draw.Xpoint > (sLCD_DIS.LCD_Dis_Column - 50) &&
                   sTP_Draw.Xpoint < sLCD_DIS.LCD_Dis_Column &&
                   sTP_Draw.Ypoint > 200 && sTP_Draw.Ypoint < 250) {
          sTP_Draw.Color = YELLOW;
        } else if (sTP_Draw.Xpoint > (sLCD_DIS.LCD_Dis_Column - 50) &&
                   sTP_Draw.Xpoint < sLCD_DIS.LCD_Dis_Column &&
                   sTP_Draw.Ypoint > 260 &&
                   sTP_Draw.Ypoint < 310) {
          sTP_Draw.Color = BLACK;
        } else {
          GUI_DrawPoint(sTP_Draw.Xpoint, sTP_Draw.Ypoint,
                        sTP_Draw.Color , DOT_PIXEL_1X1, DOT_FILL_RIGHTUP);
          GUI_DrawPoint(sTP_Draw.Xpoint + 1, sTP_Draw.Ypoint,
                        sTP_Draw.Color , DOT_PIXEL_1X1, DOT_FILL_RIGHTUP);
          GUI_DrawPoint(sTP_Draw.Xpoint, sTP_Draw.Ypoint + 1,
                        sTP_Draw.Color , DOT_PIXEL_1X1, DOT_FILL_RIGHTUP);
          GUI_DrawPoint(sTP_Draw.Xpoint + 1, sTP_Draw.Ypoint + 1,
                        sTP_Draw.Color , DOT_PIXEL_1X1, DOT_FILL_RIGHTUP);
          GUI_DrawPoint(sTP_Draw.Xpoint, sTP_Draw.Ypoint,
                        sTP_Draw.Color , DOT_PIXEL_2X2, DOT_FILL_RIGHTUP);
        }
        //Vertical screen
      } else {
        if (sTP_Draw.Xpoint > (sLCD_DIS.LCD_Dis_Column - 60) &&
            sTP_Draw.Ypoint < 16) {//Clear Board
          TP_Dialog();
        } else if (sTP_Draw.Xpoint > (sLCD_DIS.LCD_Dis_Column - 120) &&
                   sTP_Draw.Xpoint < (sLCD_DIS.LCD_Dis_Column - 80) &&
                   sTP_Draw.Ypoint < 24) { //afresh adjustment
          TP_Calibration();
          TP_Dialog();
        } else if (sTP_Draw.Xpoint > 20 && sTP_Draw.Xpoint < 70 &&
                   sTP_Draw.Ypoint > 20 && sTP_Draw.Ypoint < 70) {
          sTP_Draw.Color = BLUE;
        } else if (sTP_Draw.Xpoint > 80 && sTP_Draw.Xpoint < 130 &&
                   sTP_Draw.Ypoint > 20 && sTP_Draw.Ypoint < 70) {
          sTP_Draw.Color = GREEN;
        } else if (sTP_Draw.Xpoint > 140 && sTP_Draw.Xpoint < 190 &&
                   sTP_Draw.Ypoint > 20 && sTP_Draw.Ypoint < 70) {
          sTP_Draw.Color = RED;
        } else if (sTP_Draw.Xpoint > 200 && sTP_Draw.Xpoint < 250 &&
                   sTP_Draw.Ypoint > 20 && sTP_Draw.Ypoint < 70) {
          sTP_Draw.Color = YELLOW;
        } else if (sTP_Draw.Xpoint > 260 && sTP_Draw.Xpoint < 310 &&
                   sTP_Draw.Ypoint > 20 && sTP_Draw.Ypoint < 70) {
          sTP_Draw.Color = BLACK;
        } else {
          GUI_DrawPoint(sTP_Draw.Xpoint, sTP_Draw.Ypoint,
                        sTP_Draw.Color , DOT_PIXEL_2X2,
                        DOT_FILL_RIGHTUP );
        }
      }
    }
  }
}