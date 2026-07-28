#include <Arduino.h>
#include "Arduino_GFX_Library.h"
#include "pin_config.h"
#include <Wire.h>

Arduino_DataBus *bus = new Arduino_ESP32QSPI(
  LCD_CS, LCD_SCLK, LCD_SDIO0, LCD_SDIO1, LCD_SDIO2, LCD_SDIO3);

Arduino_CO5300 *gfx = new Arduino_CO5300(
  bus, LCD_RESET, 0, LCD_WIDTH, LCD_HEIGHT, 0, 0, 0, 0);

const char* gua[] = {"乾为天","坤为地","水雷屯","山水蒙","水天需",
  "天水讼","地水师","水地比","风天小畜","天泽履"};
const char* gua_t[] = {"元亨利贞","元亨利牝马之贞","利建侯","匪我求童蒙",
  "需于郊","有孚窒惕","师出以律","吉","密云不雨","履虎尾"};

void setup() {
  Serial.begin(115200); Wire.begin(IIC_SDA, IIC_SCL);
  if (!gfx->begin()) { Serial.println("FAIL"); return; }
  bus->writeC8D8(0x36, 0xA0);
  gfx->fillScreen(0x0000); gfx->setBrightness(255);
  for(int i=0;i<10;i++) gfx->drawRect(i,i,480-2*i,480-2*i,0xFEA0);
  gfx->setCursor(140,100); gfx->setTextColor(0xFEA0); gfx->setTextSize(10); gfx->print("焦易");
  gfx->setCursor(170,300); gfx->setTextColor(0xFFFF); gfx->setTextSize(3); gfx->print("JIAO SHI YI LIN");
  delay(3000);
}

void loop() {
  int n=random(10);
  gfx->fillScreen(0x0000);
  gfx->drawRect(0,0,480,480,0xFEA0); gfx->drawRect(2,2,476,476,0xFEA0);
  gfx->setCursor(20,30); gfx->setTextColor(0xFEA0); gfx->setTextSize(2); gfx->print("焦氏易林");
  gfx->setCursor(350,35); gfx->setTextColor(0xFFFF); gfx->setTextSize(1); gfx->print("焦易");
  gfx->drawFastHLine(20,70,440,0xFEA0);
  gfx->setCursor(40,130); gfx->setTextColor(0xFFFF); gfx->setTextSize(4); gfx->print(gua[n]);
  gfx->setCursor(40,200); gfx->setTextColor(0x7BEF); gfx->setTextSize(2); gfx->printf("第%d卦", n+1);
  gfx->fillRoundRect(30,260,420,150,8,0x1082);
  gfx->setCursor(50,300); gfx->setTextColor(0xFFFF); gfx->setTextSize(2); gfx->print(gua_t[n]);
  gfx->setCursor(150,440); gfx->setTextColor(0x7BEF); gfx->setTextSize(1); gfx->print("摇一摇 · 起一卦");
  delay(10000);
}
