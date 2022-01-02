#include <TFT_eSPI.h>
#include "images.h"

#define BUTTON_TOP 35
#define BUTTON_BOTTOM 0

constexpr auto font_16pt = 2;
constexpr auto font_26pt = 4;
constexpr auto font_48pt = 6;
constexpr auto font_48pt_lcd = 7;

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite img = TFT_eSprite(&tft);
TFT_eSprite sprite_dino = TFT_eSprite(&tft);
TFT_eSprite sprite_enemy1 = TFT_eSprite(&tft);
TFT_eSprite sprite_enemy2 = TFT_eSprite(&tft);

// Backlight
const int pwmFreq = 5000;
const int pwmResolution = 8;
const int pwmLedChannelTFT = 0;

byte brightness[6] = {70, 100, 130, 160, 200, 220};
byte brightness_level = 1;

float linesX[6];
int linesW[6];
float linesX2[6];
int linesW2[6];
float clouds[2] = {(float)random(0, 80), (float)random(100, 180)};
float bumps[2];
int bumpsF[2];

float eX[2] = {(float)random(TFT_HEIGHT, 310), (float)random(380, 460)};
int ef[2] = {0, 1};

float speed = 1;
int x = 30;
int y = 58;
float dir = -1.4;
bool pressed = 0;
int frames = 0;
int f = 0;
float cloudSpeed = 0.4;
bool gameRun = 1;
int score = 0;
int t = 0;
int press2 = 0;

void setup()
{
  pinMode(BUTTON_BOTTOM, INPUT_PULLUP);
  pinMode(BUTTON_TOP, INPUT_PULLUP);

  tft.init();
  tft.setSwapBytes(true);
  tft.fillScreen(TFT_WHITE);
  tft.setRotation(1);

  img.setTextColor(TFT_BLACK,TFT_WHITE);

  img.setColorDepth(16);
  sprite_dino.setColorDepth(16);
  sprite_enemy1.setColorDepth(16);
  sprite_enemy2.setColorDepth(16);

  img.createSprite(TFT_HEIGHT, 100);
  sprite_dino.createSprite(image_dino.width, image_dino.height);
  sprite_enemy1.createSprite(image_enemy.width, image_enemy.height);
  sprite_enemy2.createSprite(image_enemy.width, image_enemy.height);

  tft.fillScreen(TFT_WHITE);

  ledcSetup(pwmLedChannelTFT, pwmFreq, pwmResolution);
  ledcAttachPin(TFT_BL, pwmLedChannelTFT);
  ledcWrite(pwmLedChannelTFT, brightness[brightness_level]);

  for (int i = 0; i < 6; i++)
  {
    linesX[i] = random(i * 40, (i + 1) * 40);
    linesW[i] = random(1, 14);
    linesX2[i] = random(i * 40, (i + 1) * 40);
    linesW2[i] = random(1, 14);
  }

  for (int n = 0; n < 2; n++)
  {
    bumps[n] = random(n * 90, (n + 1) * 120);
    bumpsF[n] = random(0, 2);
  }

  tft.pushImage(0, 0, image_no_internet.width, image_no_internet.height, image_no_internet.data);

  while (digitalRead(BUTTON_BOTTOM) != 0)
  {
    if (digitalRead(BUTTON_TOP) == 0)
    {
      if (press2 == 0)
      {
        press2 = 1;
        brightness_level++;
        brightness_level %= 6;
        ledcWrite(pwmLedChannelTFT, brightness[brightness_level]);
      }
      delay(200);
    }
    else
      press2 = 0;
  }
  tft.fillScreen(TFT_WHITE);
}

void drawS(int x, int y, int frame)
{
  img.fillSprite(TFT_WHITE);
  img.drawLine(0, 84, TFT_HEIGHT, 84, TFT_BLACK);

  for (int i = 0; i < 6; i++)
  {
    img.drawLine(linesX[i], 87, linesX[i] + linesW[i], 87, TFT_BLACK);
    linesX[i] = linesX[i] - speed;
    if (linesX[i] < -14)
    {
      linesX[i] = random(245, 280);
      linesW[i] = random(1, 14);
    }
    img.drawLine(linesX2[i], 98, linesX2[i] + linesW2[i], 98, TFT_BLACK);
    linesX2[i] = linesX2[i] - speed;
    if (linesX2[i] < -14)
    {
      linesX2[i] = random(245, 280);
      linesW2[i] = random(1, 14);
    }
  }
  for (int j = 0; j < 2; j++)
  {
    img.pushImage(clouds[j], 20, image_cloud.width, image_cloud.height, image_cloud.data);
    clouds[j] = clouds[j] - cloudSpeed;
    if (clouds[j] < -40)
      clouds[j] = random(244, 364);
  }

  for (int n = 0; n < 2; n++)
  {
    img.pushImage(bumps[n], 80, image_bump.width, image_bump.height, image_bump.data[bumpsF[n]]);
    bumps[n] = bumps[n] - speed;
    if (bumps[n] < -40)
    {
      bumps[n] = random(244, 364);
      bumpsF[n] = random(0, 2);
    }
  }

  for (int m = 0; m < 2; m++)
  {
    eX[m] = eX[m] - speed;
    if (eX[m] < -20)
      eX[m] = random(TFT_HEIGHT, 300);
    ef[m] = random(0, 2);
  }

  sprite_enemy1.pushImage(0, 0, image_enemy.width, image_enemy.height, image_enemy.data[0]);
  sprite_enemy2.pushImage(0, 0, image_enemy.width, image_enemy.height, image_enemy.data[1]);
  sprite_dino.pushImage(0, 0, image_dino.width, image_dino.height, image_dino.data[frame]);

  sprite_enemy1.pushToSprite(&img, eX[0], 56, TFT_WHITE);
  sprite_enemy2.pushToSprite(&img, eX[1], 56, TFT_WHITE);
  sprite_dino.pushToSprite(&img, x, y, TFT_WHITE);

  score = millis() / 120;
  img.drawRightString(String(score), TFT_HEIGHT, 0, font_16pt);
  img.pushSprite(0, 17);

  if (score > t + 100)
  {
    t = score;
    speed = speed + 0.1;
  }
}

void checkColision()
{
  for (int i = 0; i < 2; i++)
  {
    if (eX[i] < x + image_dino.width / 2 && eX[i] > x && y > 25)
    {
      gameRun = 0;
      tft.fillRect(0, 30, TFT_HEIGHT, 110, TFT_WHITE);
      tft.pushImage(10, 30, image_game_over.width, image_game_over.height, image_game_over.data);
      delay(500);
    }
  }
}

void loop()
{
  if (gameRun == 1)
  {
    if (digitalRead(BUTTON_BOTTOM) == 0 && pressed == 0)
    {
      pressed = 1;
      f = 0;
    }

    if (pressed == 1)
    {
      y = y + dir;
      if (y == 2)
        dir = -dir;

      if (y == 58)
      {
        pressed = 0;
        dir = -dir;
      }
    }

    if (frames < 8 && pressed == 0)
      f = 1;

    if (frames > 8 && pressed == 0)
      f = 2;

    drawS(x, y, f);
    frames++;
    if (frames == 16)
      frames = 0;

    checkColision();
  }

  if (digitalRead(BUTTON_TOP) == 0)
  {
    if (press2 == 0)
    {
      press2 = 1;
      brightness_level++;
      brightness_level %= 6;
      ledcWrite(pwmLedChannelTFT, brightness[brightness_level]);
    }
  }
  else
    press2 = 0;
}
