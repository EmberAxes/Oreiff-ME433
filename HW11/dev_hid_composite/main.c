/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Ha Thach (tinyusb.org)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "hardware/gpio.h"
#include <time.h>

#include "bsp/board_api.h"
#include "tusb.h"

#include "usb_descriptors.h"

//--------------------------------------------------------------------+
// MACRO CONSTANT TYPEDEF PROTYPES
//--------------------------------------------------------------------+

/* Blink pattern
 * - 250 ms  : device not mounted
 * - 1000 ms : device mounted
 * - 2500 ms : device is suspended
 */
enum  {
  BLINK_NOT_MOUNTED = 250,
  BLINK_MOUNTED = 1000,
  BLINK_SUSPENDED = 2500,
};

static uint32_t blink_interval_ms = BLINK_NOT_MOUNTED;
int mode = 0;    // mode switch back and forth

uint u = 10;      // Button pins
uint l = 11;
uint r = 12;
uint d = 13;
uint m = 18;

int c_v = 0;     // counters for buttons
int c_h = 0;
int c_m = 0;

int v_speed = 1; // vertical speed
int h_speed = 1; // horizontal speed

void led_blinking_task(void);
void hid_task(void);
void buttons_init();
void led_init();

/*------------- MAIN -------------*/
int main(void)
{
  board_init();
  
  // init device stack on configured roothub port
  tud_init(BOARD_TUD_RHPORT);

  if (board_init_after_tusb) {
    board_init_after_tusb();
  }

  buttons_init();
  led_init();

  while (1)
  {
    tud_task(); // tinyusb device task
    if (gpio_get(18)==0){     // If the mode button is pressed
      mode +=1;               // Change mode
      sleep_ms(100);
    }
    hid_task();

  }
}

//--------------------------------------------------------------------+
// Device callbacks
//--------------------------------------------------------------------+

// Invoked when device is mounted
void tud_mount_cb(void)
{
  blink_interval_ms = BLINK_MOUNTED;
}

// Invoked when device is unmounted
void tud_umount_cb(void)
{
  blink_interval_ms = BLINK_NOT_MOUNTED;
}

// Invoked when usb bus is suspended
// remote_wakeup_en : if host allow us  to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
void tud_suspend_cb(bool remote_wakeup_en)
{
  (void) remote_wakeup_en;
  blink_interval_ms = BLINK_SUSPENDED;
}

// Invoked when usb bus is resumed
void tud_resume_cb(void)
{
  blink_interval_ms = tud_mounted() ? BLINK_MOUNTED : BLINK_NOT_MOUNTED;
}

//--------------------------------------------------------------------+
// USB HID
//--------------------------------------------------------------------+

static void send_hid_report(uint8_t report_id, uint32_t btn)
{
  // skip if hid is not ready yet
  if ( !tud_hid_ready() ) return;

  switch(report_id)
  {
    case REPORT_ID_KEYBOARD:
    {
      // use to avoid send multiple consecutive zero report for keyboard
      static bool has_keyboard_key = false;

      if ( btn )
      {
        uint8_t keycode[6] = { 0 };
        keycode[0] = HID_KEY_A;

        tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, keycode);
        has_keyboard_key = true;
      }else
      {
        // send empty key report if previously has key pressed
        if (has_keyboard_key) tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, NULL);
        has_keyboard_key = false;
      }
    }
    break;

    case REPORT_ID_MOUSE:
    {

      int8_t rl;
      int8_t ud;
      if (mode%2 == 0){
        gpio_put(16,0);
        gpio_put(17,1);

        if (gpio_get(u) == 0 | gpio_get(d) == 0){    // up down
          c_v += 1;
          if (c_v % 25 == 0 && v_speed < 5){v_speed += 1;} // increase to max
          if (gpio_get(u)==0){ud = -v_speed;}
          if (gpio_get(d)==0){ud = v_speed;}
        }else{v_speed = 1; ud = 0; c_v = 0;}

        if (gpio_get(l) == 0 | gpio_get(r) == 0){    // left right
          c_h += 1;
          if (c_h % 25 == 0 && h_speed < 5){h_speed += 1;} // increase to max
          if (gpio_get(l)==0){rl = -h_speed;}
          if (gpio_get(r)==0){rl = h_speed;}
        }else{h_speed = 1; rl = 0; c_h = 0;}

      }else{
        gpio_put(16,1);
        gpio_put(17,0);
        
        if (c_m < 10) { rl = 5; ud = 0; }
        else if (c_m < 20) { rl = 3; ud = -3; }
        else if (c_m < 30) { rl = 0; ud = -5; }
        else if (c_m < 40) { rl = -3; ud = -3; }
        else if (c_m < 50) { rl = -5; ud = 0; }
        else if (c_m < 60) { rl = -3; ud = 3; }
        else if (c_m < 70) { rl = 0; ud = 5; }
        else if (c_m < 80) { rl = 3; ud = 3; }

        c_m += 1;
        if (c_m > 80){c_m = 0;}
      }
    
      // rl > 0 = move right     rl < 0 = move left
      // ud > 0 = move down      ud < 0 = move up
      tud_hid_mouse_report(REPORT_ID_MOUSE, 0x00, rl, ud, 0, 0);
    }
    break;

    case REPORT_ID_CONSUMER_CONTROL:
    {
      // use to avoid send multiple consecutive zero report
      static bool has_consumer_key = false;

      if ( btn )
      {
        // volume down
        uint16_t volume_down = HID_USAGE_CONSUMER_VOLUME_DECREMENT;
        tud_hid_report(REPORT_ID_CONSUMER_CONTROL, &volume_down, 2);
        has_consumer_key = true;
      }else
      {
        // send empty key report (release key) if previously has key pressed
        uint16_t empty_key = 0;
        if (has_consumer_key) tud_hid_report(REPORT_ID_CONSUMER_CONTROL, &empty_key, 2);
        has_consumer_key = false;
      }
    }
    break;

    case REPORT_ID_GAMEPAD:
    {
      // use to avoid send multiple consecutive zero report for keyboard
      static bool has_gamepad_key = false;

      hid_gamepad_report_t report =
      {
        .x   = 0, .y = 0, .z = 0, .rz = 0, .rx = 0, .ry = 0,
        .hat = 0, .buttons = 0
      };

      if ( btn )
      {
        report.hat = GAMEPAD_HAT_UP;
        report.buttons = GAMEPAD_BUTTON_A;
        tud_hid_report(REPORT_ID_GAMEPAD, &report, sizeof(report));

        has_gamepad_key = true;
      }else
      {
        report.hat = GAMEPAD_HAT_CENTERED;
        report.buttons = 0;
        if (has_gamepad_key) tud_hid_report(REPORT_ID_GAMEPAD, &report, sizeof(report));
        has_gamepad_key = false;
      }
    }
    break;

    default: break;
  }
}

// Every 10ms, we will sent 1 report for each HID profile (keyboard, mouse etc ..)
// tud_hid_report_complete_cb() is used to send the next report after previous one is complete
void hid_task(void)
{
  // Poll every 10ms
  const uint32_t interval_ms = 10;
  static uint32_t start_ms = 0;

  if ( board_millis() - start_ms < interval_ms) return; // not enough time
  start_ms += interval_ms;

  uint32_t const btn = board_button_read();

  // Remote wakeup
  if ( tud_suspended() && btn )
  {
    // Wake up host if we are in suspend mode
    // and REMOTE_WAKEUP feature is enabled by host
    tud_remote_wakeup();
  }else
  {
    // Send the 1st of report chain, the rest will be sent by tud_hid_report_complete_cb()
    send_hid_report(REPORT_ID_MOUSE, btn);
  }
}

// Invoked when sent REPORT successfully to host
// Application can use this to send the next report
// Note: For composite reports, report[0] is report ID
void tud_hid_report_complete_cb(uint8_t instance, uint8_t const* report, uint16_t len)
{
  (void) instance;
  (void) len;

  uint8_t next_report_id = report[0] + 1u;

  if (next_report_id < REPORT_ID_COUNT)
  {
    send_hid_report(next_report_id, board_button_read());
  }
}

// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen)
{
  // TODO not Implemented
  (void) instance;
  (void) report_id;
  (void) report_type;
  (void) buffer;
  (void) reqlen;

  return 0;
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize)
{
  (void) instance;

  if (report_type == HID_REPORT_TYPE_OUTPUT)
  {
    // Set keyboard LED e.g Capslock, Numlock etc...
    if (report_id == REPORT_ID_KEYBOARD)
    {
      // bufsize should be (at least) 1
      if ( bufsize < 1 ) return;

      uint8_t const kbd_leds = buffer[0];

      if (kbd_leds & KEYBOARD_LED_CAPSLOCK)
      {
        // Capslock On: disable blink, turn led on
        blink_interval_ms = 0;
        board_led_write(true);
      }else
      {
        // Caplocks Off: back to normal blink
        board_led_write(false);
        blink_interval_ms = BLINK_MOUNTED;
      }
    }
  }
}

//--------------------------------------------------------------------+
// BLINKING TASK
//--------------------------------------------------------------------+
void led_blinking_task(void)
{
  static uint32_t start_ms = 0;
  static bool led_state = false;

  // blink is disabled
  if (!blink_interval_ms) return;

  // Blink every interval ms
  if ( board_millis() - start_ms < blink_interval_ms) return; // not enough time
  start_ms += blink_interval_ms;

  board_led_write(led_state);
  led_state = 1 - led_state; // toggle
}

// --------------------------------------------------------------------+
// INITIALIZING BUTTONS
// --------------------------------------------------------------------+
// initialize the buttons as inputs and also pull up
void buttons_init(){  

  gpio_init(u);                // UP button
  gpio_pull_up(u);             // Pull-up
  gpio_set_dir(u,GPIO_IN);     // input

  gpio_init(l);                // LEFT button
  gpio_pull_up(l);             // Pull-up
  gpio_set_dir(l,GPIO_IN);     // input

  gpio_init(r);                // RIGHT button
  gpio_pull_up(r);             // Pull-up
  gpio_set_dir(r,GPIO_IN);     // input

  gpio_init(d);                // DOWN button
  gpio_pull_up(d);             // Pull-up
  gpio_set_dir(d,GPIO_IN);     // input

  gpio_init(m);                // MODE button
  gpio_pull_up(m);             // Pull-up
  gpio_set_dir(m,GPIO_IN);     // input

  // Comment this out when test is done
    // while(gpio_get(m) == 1){       // until mode button is pushed
    //   if (gpio_get(u)==0){
    //     break;
    //    }
    //   if (gpio_get(l)==0){
    //     break;
    //   }
    //   if (gpio_get(r)==0){
    //     break;
    //   }
    //   if (gpio_get(d)==0){
    //     break;
    //   }
    //   sleep_ms(100);
    // }

}

// --------------------------------------------------------------------+
// INITIALIZING LEDs
// --------------------------------------------------------------------+
// initialize the buttons as inputs and also pull up
void led_init(){
  uint yel,gre;
  yel = 17;       // yellow (regular mode)
  gre = 16;       // green (remote mode)

  gpio_init(yel);                // yellow LED
  gpio_set_dir(yel,GPIO_OUT);     // output

  gpio_init(gre);                // green LED
  gpio_set_dir(gre,GPIO_OUT);     // output
}