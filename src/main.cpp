#include <TFT_eSPI.h>
#include "images.h"

#define BUTTON_TOP 35
#define BUTTON_BOTTOM 0

const image_t images_dino[] =
{
    dino_1,
    dino_2,
    dino_3
};

const image_t images_bump[] =
{
    bump_1,
    bump_2
};

const image_t images_enemy[] = 
{
  enemy_1,
  enemy_2
};


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
float clouds[2] = {random(0, 80), random(100, 180)};
float bumps[2];
int bumpsF[2];

float eX[2] = {random(240, 310), random(380, 460)};
int ef[2] = {0, 1};

float sped = 1;
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

  img.setColorDepth(16);
  sprite_dino.setColorDepth(16);
  sprite_enemy1.setColorDepth(16);
  sprite_enemy2.setColorDepth(16);

  img.createSprite(240, 100);
  sprite_dino.createSprite(images_dino[0].width, images_dino[0].height);
  sprite_enemy1.createSprite(images_enemy[0].width, images_enemy[0].height);
  sprite_enemy2.createSprite(images_enemy[1].width, images_enemy[1].height);

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

  tft.pushImage(0, 0, no_internet.width, no_internet.height, no_internet.data);

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
  img.drawLine(0, 84, 240, 84, TFT_BLACK);

  for (int i = 0; i < 6; i++)
  {
    img.drawLine(linesX[i], 87, linesX[i] + linesW[i], 87, TFT_BLACK);
    linesX[i] = linesX[i] - sped;
    if (linesX[i] < -14)
    {
      linesX[i] = random(245, 280);
      linesW[i] = random(1, 14);
    }
    img.drawLine(linesX2[i], 98, linesX2[i] + linesW2[i], 98, TFT_BLACK);
    linesX2[i] = linesX2[i] - sped;
    if (linesX2[i] < -14)
    {
      linesX2[i] = random(245, 280);
      linesW2[i] = random(1, 14);
    }
  }
  for (int j = 0; j < 2; j++)
  {
    img.pushImage(clouds[j], 20, cloud.width, cloud.height, cloud.data);
    clouds[j] = clouds[j] - cloudSpeed;
    if (clouds[j] < -40)
      clouds[j] = random(244, 364);
  }

  for (int n = 0; n < 2; n++)
  {
    img.pushImage(bumps[n], 80, images_bump[bumpsF[n]].width, images_bump[bumpsF[n]].height, images_bump[bumpsF[n]].data);
    bumps[n] = bumps[n] - sped;
    if (bumps[n] < -40)
    {
      bumps[n] = random(244, 364);
      bumpsF[n] = random(0, 2);
    }
  }

  for (int m = 0; m < 2; m++)
  {
    eX[m] = eX[m] - sped;
    if (eX[m] < -20)
      eX[m] = random(240, 300);
    ef[m] = random(0, 2);
  }

sprite_enemy1.pushImage(0, 0, images_enemy[0].width, images_enemy[0].height, images_enemy[0].data);
sprite_enemy2.pushImage(0, 0, images_enemy[1].width, images_enemy[1].height, images_enemy[1].data);
sprite_dino.pushImage(0, 0, images_dino[frame].width, images_dino[frame].height, images_dino[frame].data);

  sprite_enemy1.pushToSprite(&img, eX[0], 56, TFT_WHITE);
  sprite_enemy2.pushToSprite(&img, eX[1], 56, TFT_WHITE);
  sprite_dino.pushToSprite(&img, x, y, TFT_WHITE);

  score = millis() / 120;
  img.drawString(String(score), 204, 0, 2);
  img.pushSprite(0, 17);

  if (score > t + 100)
  {
    t = score;
    sped = sped + 0.1;
  }
}

void checkColision()
{
  for (int i = 0; i < 2; i++)
  {
    if (eX[i] < x + images_dino[0].width / 2 && eX[i] > x && y > 25)
    {
      gameRun = 0;
      tft.fillRect(0, 30, 240, 110, TFT_WHITE);
      tft.pushImage(10,30, game_over.width, game_over.height, game_over.data);
      tft.pushImage(10,30, game_over.width, game_over.height, game_over.data);
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
