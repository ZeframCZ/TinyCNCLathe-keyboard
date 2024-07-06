///LinuxCNC matrix Keyboard code 

#include "Keyboard.h"
#include "stdio.h"

#define NUMBER_OF_ROWS 6
#define NUMBER_OF_COLUMNS 8
#define KEY_HOLD_TIME 50
#define DIR_RELEASE_TIME 100


#define BTN_START_CYCLE 'r'
#define BTN_END_CYCLE 'p'
#define BTN_NEXT_LINE 'a'

#define BTN_ESTOP 194 //F1
#define BTN_MACHINE_ONOFF 195 //F2

#define BTN_CONSOLE 198 //F5
#define BTN_HAND 196 //F3

#define BTN_SPINDLE_BACKWARD 203 //F10
#define BTN_SPINDLE_FORWARD 202 //F9
#define BTN_SPINDLE_PLUS 205 //F12
#define BTN_SPINDLE_MINUS 204 //F11

#define BTN_ABORT_MOVE 177 //ESC
#define BTN_TOOL_TOUCHOFF 213 //END
#define BTN_RAPID KEY_RIGHT_SHIFT
#define BTN_UP KEY_UP_ARROW
#define BTN_DOWN KEY_DOWN_ARROW
#define BTN_LEFT KEY_LEFT_ARROW
#define BTN_RIGHT KEY_RIGHT_ARROW

#define BTN_SPEED_100 '0'
#define BTN_SPEED_75 '8'
#define BTN_SPEED_50 '5'
#define BTN_SPEED_25 '3'
#define BTN_SPEED_10 '2'
#define BTN_SPEED_0 '1'

//colomn pins = 16,17,18,19,20,21,22,26
unsigned char row_pins[NUMBER_OF_ROWS] = {5,4,2,6,7,8};

unsigned char all_states[NUMBER_OF_ROWS] = {0,0,0,0,0,0};

unsigned long current_time = 0;
unsigned long release_time_up = 0;
unsigned long release_time_down = 0;
unsigned long release_time_left = 0;
unsigned long release_time_right = 0;
unsigned long release_time_rapid = 0;

void setup() {
  pinMode(16, INPUT_PULLDOWN);
  pinMode(17, INPUT_PULLDOWN);
  pinMode(18, INPUT_PULLDOWN);
  pinMode(19, INPUT_PULLDOWN);
  pinMode(20, INPUT_PULLDOWN);
  pinMode(21, INPUT_PULLDOWN);
  pinMode(22, INPUT_PULLDOWN);
  pinMode(26, INPUT_PULLDOWN);

  pinMode(6, INPUT_PULLDOWN);
  pinMode(7, INPUT_PULLDOWN);
  pinMode(8, INPUT_PULLDOWN);
  pinMode(5, INPUT_PULLDOWN);
  pinMode(4, INPUT_PULLDOWN);
  pinMode(2, INPUT_PULLDOWN);

  Keyboard.begin();
}
//Gets the states of the buttons from the matrix
void get_states(unsigned char *ptr_all_states){
  for(int row = 0; row < NUMBER_OF_ROWS; row++){//REset
    ptr_all_states[row] = 0;
  }

  for(int row = 0; row < NUMBER_OF_ROWS; row++){//SET
      pinMode(row_pins[row], OUTPUT);
      digitalWrite(row_pins[row], HIGH);

      unsigned char row_states = 0;
      
      //Sets bits into one if there is signal 
      row_states |= 0x01*digitalRead(26);//0x01*1 = 0x01 -> pressed | 0x01*0 = 0x00 -> not pressed
      row_states |= 0x02*digitalRead(22);
      row_states |= 0x04*digitalRead(21);
      row_states |= 0x08*digitalRead(20);
      row_states |= 0x10*digitalRead(19);
      row_states |= 0x20*digitalRead(18);
      row_states |= 0x40*digitalRead(17);
      row_states |= 0x80*digitalRead(16);

      ptr_all_states[row] = row_states;//"return"

      digitalWrite(row_pins[row], LOW);
      pinMode(row_pins[row], INPUT_PULLDOWN);
    }
}

//Button functions
void btn_dir(unsigned char dir){
  switch(dir){
    case(0):
      Keyboard.press(KEY_UP_ARROW);
      release_time_up = current_time+DIR_RELEASE_TIME;
    break;
    case(1):
      Keyboard.press(KEY_DOWN_ARROW);
      release_time_down = current_time+DIR_RELEASE_TIME;
    break;
    case(2):
      Keyboard.press(KEY_LEFT_ARROW);
      release_time_left = current_time+DIR_RELEASE_TIME;
    break;
    case(3):
      Keyboard.press(KEY_RIGHT_ARROW);
      release_time_right = current_time+DIR_RELEASE_TIME;
    break;
    case(4):
      Keyboard.press(KEY_RIGHT_SHIFT);
      release_time_rapid = current_time+DIR_RELEASE_TIME;
    break;
  }
}

void btn_single_char(char the_char, unsigned int delay_time){
  Keyboard.write(the_char);
  delay(KEY_HOLD_TIME);
  Keyboard.release(the_char);
  delay(delay_time);
}
//key is decimal
void btn_single_key(unsigned char key, unsigned int delay_time){
  Keyboard.press(key);
  delay(KEY_HOLD_TIME);
  Keyboard.release(key);
  delay(delay_time);
}

void loop() {
  current_time = millis();
  
  //Check the time and release direction buttons that need to be released
  if(current_time>=release_time_up){Keyboard.release(BTN_UP);}
  if(current_time>=release_time_down){Keyboard.release(BTN_DOWN);}
  if(current_time>=release_time_left){Keyboard.release(BTN_LEFT);}
  if(current_time>=release_time_right){Keyboard.release(BTN_RIGHT);}
  if(current_time>=release_time_rapid){Keyboard.release(BTN_RAPID);}

  get_states(all_states);

  //Check button states
  for(int row = 0; row < NUMBER_OF_ROWS; row++){
    if((all_states[row] & (all_states[row] - 1)) == 0){
      switch(row){
        case(0):
            if(all_states[row]&0x01){btn_single_char(BTN_START_CYCLE, 500);}
            if(all_states[row]&0x02){btn_single_char(BTN_END_CYCLE, 500);}
            if(all_states[row]&0x04){btn_single_char(BTN_NEXT_LINE, 1000);}
            if(all_states[row]&0x08){btn_single_char(BTN_SPEED_0, 100);}
            if(all_states[row]&0x10){ btn_single_key(BTN_SPINDLE_BACKWARD, 1000);}
            if(all_states[row]&0x20){ btn_single_key(BTN_SPINDLE_FORWARD, 1000);}
            if(all_states[row]&0x40){ btn_single_key(BTN_ESTOP, 300);}
            if(all_states[row]&0x80){ btn_single_key(BTN_MACHINE_ONOFF, 300);}
        break;
        case(1):
          if(all_states[row]&0x08){btn_single_char(BTN_SPEED_75, 100);}
          if(all_states[row]&0x10){ btn_single_key(BTN_SPINDLE_PLUS, 200);}
        break;
        case(2):
          if(all_states[row]&0x02){btn_dir(0);}//btn_up
          if(all_states[row]&0x08){btn_single_char(BTN_SPEED_50, 100);}
          if(all_states[row]&0x10){ btn_single_key(BTN_SPINDLE_MINUS, 200);}
          if(all_states[row]&0x40){ btn_single_key(BTN_CONSOLE, 100);}
          if(all_states[row]&0x80){ btn_single_key(BTN_HAND, 100);}
        break;
        case(3):
          if(all_states[row]&0x01){btn_dir(3);}
          if(all_states[row]&0x02){btn_dir(1);}
          if(all_states[row]&0x04){btn_dir(2);}
          if(all_states[row]&0x08){btn_single_char(BTN_SPEED_25, 100);}
        break;
        case(4):
          if(all_states[row]&0x01) {btn_single_key(BTN_ABORT_MOVE, 100);}
          if(all_states[row]&0x04){btn_dir(4);}
          if(all_states[row]&0x08){btn_single_char(BTN_SPEED_10, 100);}
        break;
        case(5):
          if(all_states[row]&0x02){ btn_single_key(BTN_TOOL_TOUCHOFF, 500);}
          if(all_states[row]&0x08){btn_single_char(BTN_SPEED_0, 100);}
        break;
      }
    }
  }
}
