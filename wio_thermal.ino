/*
 * Board: Seeeduino Wio Terminal
 *
 * Library: https://github.com/Seeed-Studio/Seeed_Arduino_MLX9064x
 */

#include "TFT_eSPI.h"
#include "Free_Fonts.h"
#include "Wio_io.h"

#include "Wire.h"
#include "MLX90640_API.h"
#include "MLX9064X_I2C_Driver.h"

#define MLX90640_address 0x33
#define TA_SHIFT 8
#define MLX90640_X 32
#define MLX90640_Y 24
#define MLX90640_XY 768
#define MLX90640_EMS 0.95

TFT_eSPI tft;

float mlx90640To[MLX90640_XY];
paramsMLX90640 mlx90640;
float emissivity = MLX90640_EMS;

int cx = MLX90640_X / 2, cy = MLX90640_Y / 2;
int ignore = 4;

const uint16_t camColors[] = {0x480F,
                              0x400F, 0x400F, 0x400F, 0x4010, 0x3810, 0x3810, 0x3810, 0x3810, 0x3010, 0x3010,
                              0x3010, 0x2810, 0x2810, 0x2810, 0x2810, 0x2010, 0x2010, 0x2010, 0x1810, 0x1810,
                              0x1811, 0x1811, 0x1011, 0x1011, 0x1011, 0x0811, 0x0811, 0x0811, 0x0011, 0x0011,
                              0x0011, 0x0011, 0x0011, 0x0031, 0x0031, 0x0051, 0x0072, 0x0072, 0x0092, 0x00B2,
                              0x00B2, 0x00D2, 0x00F2, 0x00F2, 0x0112, 0x0132, 0x0152, 0x0152, 0x0172, 0x0192,
                              0x0192, 0x01B2, 0x01D2, 0x01F3, 0x01F3, 0x0213, 0x0233, 0x0253, 0x0253, 0x0273,
                              0x0293, 0x02B3, 0x02D3, 0x02D3, 0x02F3, 0x0313, 0x0333, 0x0333, 0x0353, 0x0373,
                              0x0394, 0x03B4, 0x03D4, 0x03D4, 0x03F4, 0x0414, 0x0434, 0x0454, 0x0474, 0x0474,
                              0x0494, 0x04B4, 0x04D4, 0x04F4, 0x0514, 0x0534, 0x0534, 0x0554, 0x0554, 0x0574,
                              0x0574, 0x0573, 0x0573, 0x0573, 0x0572, 0x0572, 0x0572, 0x0571, 0x0591, 0x0591,
                              0x0590, 0x0590, 0x058F, 0x058F, 0x058F, 0x058E, 0x05AE, 0x05AE, 0x05AD, 0x05AD,
                              0x05AD, 0x05AC, 0x05AC, 0x05AB, 0x05CB, 0x05CB, 0x05CA, 0x05CA, 0x05CA, 0x05C9,
                              0x05C9, 0x05C8, 0x05E8, 0x05E8, 0x05E7, 0x05E7, 0x05E6, 0x05E6, 0x05E6, 0x05E5,
                              0x05E5, 0x0604, 0x0604, 0x0604, 0x0603, 0x0603, 0x0602, 0x0602, 0x0601, 0x0621,
                              0x0621, 0x0620, 0x0620, 0x0620, 0x0620, 0x0E20, 0x0E20, 0x0E40, 0x1640, 0x1640,
                              0x1E40, 0x1E40, 0x2640, 0x2640, 0x2E40, 0x2E60, 0x3660, 0x3660, 0x3E60, 0x3E60,
                              0x3E60, 0x4660, 0x4660, 0x4E60, 0x4E80, 0x5680, 0x5680, 0x5E80, 0x5E80, 0x6680,
                              0x6680, 0x6E80, 0x6EA0, 0x76A0, 0x76A0, 0x7EA0, 0x7EA0, 0x86A0, 0x86A0, 0x8EA0,
                              0x8EC0, 0x96C0, 0x96C0, 0x9EC0, 0x9EC0, 0xA6C0, 0xAEC0, 0xAEC0, 0xB6E0, 0xB6E0,
                              0xBEE0, 0xBEE0, 0xC6E0, 0xC6E0, 0xCEE0, 0xCEE0, 0xD6E0, 0xD700, 0xDF00, 0xDEE0,
                              0xDEC0, 0xDEA0, 0xDE80, 0xDE80, 0xE660, 0xE640, 0xE620, 0xE600, 0xE5E0, 0xE5C0,
                              0xE5A0, 0xE580, 0xE560, 0xE540, 0xE520, 0xE500, 0xE4E0, 0xE4C0, 0xE4A0, 0xE480,
                              0xE460, 0xEC40, 0xEC20, 0xEC00, 0xEBE0, 0xEBC0, 0xEBA0, 0xEB80, 0xEB60, 0xEB40,
                              0xEB20, 0xEB00, 0xEAE0, 0xEAC0, 0xEAA0, 0xEA80, 0xEA60, 0xEA40, 0xF220, 0xF200,
                              0xF1E0, 0xF1C0, 0xF1A0, 0xF180, 0xF160, 0xF140, 0xF100, 0xF0E0, 0xF0C0, 0xF0A0,
                              0xF080, 0xF060, 0xF040, 0xF020, 0xF800,
                             };

void setup() {
  int i;

  Serial.begin(115200);
  ioInitPin();

  tft.begin();
  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);

  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setFreeFont(FSS12);
  tft.drawString("MLX90640", 10, 10);

  Wire.begin();
//  Wire.setClock(400000);
  Wire.setClock(1000000);

  delay(100);
  Wire.beginTransmission(0); // general call
  Wire.write(0x06); // reset
  Wire.endTransmission();
  delay(100);

    if (isConnected() == false) {
        tft.drawString("MLX9064x not detected", 10, 30);
        Serial.println("MLX9064x not detected at default I2C address. Please check wiring. Freezing.");
        while (1);
    }
    //Get device parameters - We only have to do this once
    int status;
    uint16_t eeMLX90640[832];
    status = MLX90640_DumpEE(MLX90640_address, eeMLX90640);
    if (status != 0) {
        tft.drawString("Failed to load system parameters", 10, 30);
        Serial.println("Failed to load system parameters");
        while (1);
    }
 
    status = MLX90640_ExtractParameters(eeMLX90640, &mlx90640);
    if (status != 0) {
        tft.drawString("Parameter extraction failed", 10, 30);
        Serial.println("Parameter extraction failed");
        while (1);
    }
 
    //Once params are extracted, we can release eeMLX90640 array
 
    //MLX90640_SetRefreshRate(MLX90640_address, 0x02); //Set rate to 2Hz
    MLX90640_SetRefreshRate(MLX90640_address, 0x03); //Set rate to 4Hz
    //MLX90640_SetRefreshRate(MLX90640_address, 0x07); //Set rate to 64H

  tft.fillScreen(TFT_BLACK);
  for (i = 0; i < 256; i ++) {
    tft.drawLine(32 + i, 218, 32 + i, 220, camColors[i]);
  }
  tft.setFreeFont(FF1);
  tft.setTextColor(TFT_DARKGREY, TFT_BLACK);
  tft.drawString("(EMS", 150, 222);
  tft.drawFloat(emissivity, 2, 200, 222);
  tft.drawString(")", 245, 222);
  wioBeep(100);
}

void loop() {
  int i;

    wioLed(false);
    long startTime = millis();
    for (byte x = 0 ; x < 2 ; x++) {
        uint16_t mlx90640Frame[834];
        int status = MLX90640_GetFrameData(MLX90640_address, mlx90640Frame);
 
        float vdd = MLX90640_GetVdd(mlx90640Frame, &mlx90640);
        float Ta = MLX90640_GetTa(mlx90640Frame, &mlx90640);
 
        float tr = Ta - TA_SHIFT; //Reflected temperature based on the sensor ambient temperature
//        float emissivity = MLX90640_EMS;

        MLX90640_CalculateTo(mlx90640Frame, &mlx90640, emissivity, tr, mlx90640To);
    }
    long stopTime = millis();
    wioLed(true);

  int x, y, c;
  float n, n_min = mlx90640To[0], n_max = mlx90640To[0];
  for (i = 0; i < MLX90640_XY; i ++) {
    if (n_min > mlx90640To[i]) n_min = mlx90640To[i];
    if (n_max < mlx90640To[i]) n_max = mlx90640To[i];
  }

  // mirror x
  for (y = 0; y < MLX90640_Y; y ++) {
    for (x = 0 ; x < MLX90640_X / 2; x ++) {
      n = mlx90640To[y * MLX90640_X + x];
      mlx90640To[y * MLX90640_X + x] = mlx90640To[y * MLX90640_X + (MLX90640_X - 1 - x)];
      mlx90640To[y * MLX90640_X + (MLX90640_X - 1 - x)] = n;
    }
  }

  // draw thermal
  for (y = 0; y < MLX90640_Y; y ++) {
    for (x = 0 ; x < MLX90640_X; x ++) {
      n = mlx90640To[y * MLX90640_X + x];
      c = ((n - n_min) / (n_max - n_min)) * 255;
      tft.fillRect(16 + x * 9, y * 9, 9, 9, camColors[c]);
    }
  }

  if (ignore) {
    ignore --;
    return;
  }

  // button
  if (digitalRead(WIO_5S_UP) == LOW) {
    if (emissivity < 1.0) emissivity += 0.01;
  } else
  if (digitalRead(WIO_5S_DOWN) == LOW) {
    if (emissivity > 0.7) emissivity -= 0.01;
  } else
  if (digitalRead(WIO_5S_LEFT) == LOW || digitalRead(WIO_KEY_C) == LOW) {
    for (i = 0; i < MLX90640_XY; i ++) {
      if (n_min == mlx90640To[i]) {
        cx = i % MLX90640_X;
        cy = i / MLX90640_X;
        break;
      }
    }
  } else
  if (digitalRead(WIO_5S_RIGHT) == LOW || digitalRead(WIO_KEY_A) == LOW) {
    for (i = 0; i < MLX90640_XY; i ++) {
      if (n_max == mlx90640To[i]) {
        cx = i % MLX90640_X;
        cy = i / MLX90640_X;
        break;
      }
    }
  } else
  if (digitalRead(WIO_KEY_B) == LOW) {
    cx = 16;
    cy = 12;
    emissivity = MLX90640_EMS;
  }

  // closs line
  tft.drawLine(16 + cx * 9, cy * 9 + 4, 16 + cx * 9 + 8, cy * 9 + 4, TFT_WHITE);
  tft.drawLine(16 + cx * 9 + 4, cy * 9, 16 + cx * 9 + 4, cy * 9 + 8, TFT_WHITE);

  // status
  tft.setFreeFont(FF1);
  tft.setTextColor(TFT_BLUE, TFT_BLACK);
  tft.drawFloat(n_min, 1, 10, 222);
  tft.setTextColor(TFT_RED, TFT_BLACK);
  tft.drawFloat(n_max, 1, 265, 222);
  n = mlx90640To[cx + cy * 32];
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawFloat(n, 1, 80, 222);
  tft.drawString("`c", 125, 222);

  tft.setTextColor(TFT_DARKGREY, TFT_BLACK);
  tft.drawFloat(emissivity, 2, 200, 222);
}

boolean isConnected() {
  Wire.beginTransmission((uint8_t)MLX90640_address);
  if (Wire.endTransmission() != 0) {
    return false;    //Sensor did not ACK
  }
  return true;
}
