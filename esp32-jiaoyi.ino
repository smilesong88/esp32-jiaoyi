// 焦氏易林 - ESP32-S3 480x480 AMOLED · 四庫全書本
// Data at flash 0x400000 (yilin.bin, 141KB, 4026 entries)

#include <Arduino.h>
#include "Arduino_GFX_Library.h"
#include "pin_config.h"
#include <Wire.h>
#include "esp_flash.h"
#define U8G2_USE_LARGE_FONTS
#include "font/u8g2_font_unifont_h_cjk.h"

Arduino_DataBus *bus = new Arduino_ESP32QSPI(
  LCD_CS, LCD_SCLK, LCD_SDIO0, LCD_SDIO1, LCD_SDIO2, LCD_SDIO3);

Arduino_CO5300 *gfx = new Arduino_CO5300(
  bus, LCD_RESET, 0, LCD_WIDTH, LCD_HEIGHT, 0, 0, 0, 0);

#define BLACK   0x0000
#define GOLD    0xFEA0
#define WHITE   0xFFFF
#define GRAY    0x7BEF
#define RED     0xF800
#define DARKBG  0x1082
#define DATA_ADDR  0x400000

uint32_t num_entries, num_strs, num_hex, entry_base_addr;

uint32_t read_u32(uint32_t addr) { uint32_t v; esp_flash_read(NULL, &v, addr, 4); return v; }
uint8_t read_u8(uint32_t addr) { uint8_t v; esp_flash_read(NULL, &v, addr, 1); return v; }

void get_hex_name(uint8_t idx, char* buf) {
  esp_flash_read(NULL, (uint8_t*)buf, DATA_ADDR + 16 + idx * 7, 7);
}

void get_verse(uint16_t idx, char* buf, size_t maxlen) {
  uint32_t str_off = read_u32(DATA_ADDR + 16 + num_hex * 7 + idx * 4);
  uint32_t addr = DATA_ADDR + 16 + num_hex * 7 + num_strs * 4 + str_off;
  size_t i = 0;
  while (i < maxlen - 1) {
    char c; esp_flash_read(NULL, &c, addr + i, 1);
    if (c == 0) break;
    buf[i++] = c;
  }
  buf[i] = 0;
}

void init_data() {
  char magic[5] = {0}; esp_flash_read(NULL, magic, DATA_ADDR, 4);
  if (strncmp(magic, "YILN", 4) != 0) {
    gfx->fillScreen(BLACK);
    gfx->setFont(u8g2_font_unifont_h_cjk);
    gfx->setCursor(80, 200); gfx->setTextColor(RED); gfx->setTextSize(2);
    gfx->println("NO DATA - Flash yilin.bin at 0x400000");
    while(1) delay(1000);
  }
  num_entries = read_u32(DATA_ADDR + 4);
  num_strs = read_u32(DATA_ADDR + 8);
  num_hex = read_u32(DATA_ADDR + 12);
  uint32_t last_off = read_u32(DATA_ADDR + 16 + num_hex * 7 + (num_strs - 1) * 4);
  uint32_t str_base = DATA_ADDR + 16 + num_hex * 7 + num_strs * 4;
  uint32_t last_addr = str_base + last_off;
  size_t llen = 0;
  while (1) { char c; esp_flash_read(NULL, &c, last_addr + llen, 1); if (c == 0) break; llen++; }
  entry_base_addr = last_addr + llen + 1;
}

void show_splash() {
  gfx->fillScreen(BLACK);
  for (int i = 0; i < 10; i++) gfx->drawRect(i, i, 480 - 2*i, 480 - 2*i, GOLD);
  gfx->setFont(u8g2_font_unifont_h_cjk);
  gfx->setCursor(140, 100); gfx->setTextColor(GOLD); gfx->setTextSize(3); gfx->print("焦易");
  gfx->setCursor(100, 260); gfx->setTextColor(WHITE); gfx->setTextSize(2); gfx->print("JIAO SHI YI LIN");
  gfx->setCursor(130, 320); gfx->setTextColor(GRAY); gfx->setTextSize(1);
  gfx->printf("%d Verses", num_entries);
  delay(3000);
}

void draw_gua(uint32_t idx) {
  uint32_t addr = entry_base_addr + idx * 4;
  uint8_t base = read_u8(addr), target = read_u8(addr + 1);
  char nb[7], tb[7], vb[256];
  get_hex_name(base, nb); nb[6] = 0;
  get_hex_name(target, tb); tb[6] = 0;
  uint16_t vi = read_u8(addr + 2) | (read_u8(addr + 3) << 8);
  get_verse(vi, vb, sizeof(vb));

  gfx->fillScreen(BLACK);
  gfx->drawRect(0, 0, 480, 480, GOLD); gfx->drawRect(2, 2, 476, 476, GOLD);
  gfx->setFont(u8g2_font_unifont_h_cjk);
  gfx->setCursor(10, 20); gfx->setTextColor(GOLD); gfx->setTextSize(2); gfx->print("焦氏易林");
  gfx->setCursor(320, 25); gfx->setTextColor(GRAY); gfx->setTextSize(1); gfx->printf("#%d", idx + 1);
  gfx->drawFastHLine(10, 55, 460, GOLD);

  int nw = strlen(nb) * 48;
  gfx->setCursor(40, 80); gfx->setTextColor(WHITE); gfx->setTextSize(4); gfx->print(nb);
  gfx->setCursor(40 + nw + 12, 88); gfx->setTextColor(GOLD); gfx->setTextSize(2); gfx->print("之");
  gfx->setCursor(40 + nw + 48, 80); gfx->setTextColor(WHITE); gfx->setTextSize(4); gfx->print(tb);
  gfx->setCursor(40, 145); gfx->setTextColor(GRAY); gfx->setTextSize(2); gfx->printf("%s之%s", nb, tb);

  gfx->fillRoundRect(15, 185, 450, 245, 8, DARKBG);
  gfx->setTextColor(WHITE); gfx->setTextSize(2);
  int cpl = 14, y = 205, len = strlen(vb), s = 0;
  while (s < len && y < 415) {
    int e = s + cpl; if (e >= len) e = len;
    else while (e > s && (vb[e] & 0xC0) == 0x80) e--;
    if (e <= s) break;
    gfx->setCursor(30, y);
    char sv = vb[e]; vb[e] = 0; gfx->print(&vb[s]); vb[e] = sv;
    s = e; y += 30;
  }

  gfx->setCursor(140, 445); gfx->setTextColor(GRAY); gfx->setTextSize(1); gfx->print("摇一摇 · 起一卦");
}

void setup() {
  Serial.begin(115200); Wire.begin(IIC_SDA, IIC_SCL);
  if (!gfx->begin()) { Serial.println("FAIL"); return; }
  bus->writeC8D8(0x36, 0xA0); gfx->setBrightness(255);
  init_data();
  randomSeed(esp_random());
  show_splash();
}

void loop() {
  draw_gua(random(num_entries));
  delay(10000);
}
