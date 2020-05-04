#include "QVGA_TFT_2.2_game_board.h"

volatile int x;
volatile int y;
volatile int turn;
int *shape = &shape_1[0][0];
int* shapes[5]= {&shape_1[0][0], &shape_2[0][0], &shape_3[0][0], &shape_4[0][0], &shape_5[0][0]};
volatile int shape_location_x = 5;
volatile int shape_location_y = 0;
enum fsm{INIT, START_NEW_SHAPE, CHECK_NEXT_MOVE, MERGE_SHAPE, WAIT_NEXT_MOVE, GAMEOVER, STOP};
enum fsm state;
int shape_index = 0;
volatile int orientation = 0;
volatile int timeout =0;
const unsigned int PRELOAD_TIMER = 286; //34286;
const int left_pin = 2; //Mega, Mega2560, MegaADK 2, 3, 18, 19, 20, 21
const int right_pin = 3;
const int down_pin = 20;
const int rotate_pin = 21;
volatile int left =0;
volatile int right = 0;
volatile int down = 0;
volatile int rotate = 0;
volatile int move_left =0;
volatile int move_right = 0;
volatile int move_down = 0;
volatile int rotate_clockwise = 0;
volatile int output = LOW;  
void setup()  // Start of setup
{
  Serial.begin(9600); // for logging/debugging
  Serial.println("ILI9341 Test!");
  state=INIT;
  // Display setup:
  tft.begin();  // Initialize display
  tft.fillScreen(0x0000);  // Fill screen with black
  tft.setRotation(2);  //0 - portrait mode  
  // initialize timer1 
  noInterrupts();           // disable all interrupts
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = PRELOAD_TIMER;    // preload timer 65536-16MHz/256/2Hz
  TCCR1B |= (1 << CS12);    // 256 prescaler 
  TIMSK1 |= (1 << TOIE1);   // enable timer overflow interrupt
  interrupts();             // enable all interrupts   
  attachInterrupt(digitalPinToInterrupt(left_pin), left_pin_pressed, FALLING);
  attachInterrupt(digitalPinToInterrupt(right_pin), right_pin_pressed, FALLING); 
  attachInterrupt(digitalPinToInterrupt(rotate_pin), rotate_pin_pressed, FALLING); 
  pinMode(13,OUTPUT);     
} // End of setup

void loop()  // Start of loop
{ 
  digitalWrite(13,output); 
  switch(state){
    case INIT: 
      Serial.println("INIT");
      show_board();
      state=START_NEW_SHAPE;
      break;
    case START_NEW_SHAPE: 
      Serial.println("START_NEW_SHAPE");
      shape = shapes[shape_index];
      shape_location_x = 5;
      shape_location_y = 0;
      orientation = 0;
      if (check_shape(shape_location_x, shape_location_y, orientation, shape) == 0){
        merge_shape(shape_location_x, shape_location_y, orientation, shape);
        show_board();
        state = WAIT_NEXT_MOVE;
      } else {
      state = GAMEOVER;
      }
      shape_index = (shape_index + 1) % SHAPE_NUMBER;
      break;
    case CHECK_NEXT_MOVE: 
      Serial.println("CHECK_NEXT_MOVE");
      erase_shape(shape_location_x, shape_location_y, orientation, shape); // delete shape before checking - the shape colides with itself
      x = shape_location_x + move_right - move_left;
      y = shape_location_y + move_down;
      turn = (orientation + rotate_clockwise) % 4;
      if (check_shape(x, y, turn, shape) == 0){        
        state=MERGE_SHAPE;
      } else {
        merge_shape(shape_location_x, shape_location_y, orientation, shape); // restore the last successful position
        state = START_NEW_SHAPE;
      }
      break;
    case MERGE_SHAPE:
      Serial.println("MERGE_SHAPE");
      x = shape_location_x + move_right - move_left;
      y = shape_location_y + move_down;
      turn = (orientation + rotate_clockwise) % 4;
      merge_shape(x, y, turn, shape);
      show_board();
      if( shape_location_y + 4 < BOARD_SIZE_Y){
        shape_location_x += move_right - move_left;
        shape_location_y += move_down;
        orientation = (orientation + rotate_clockwise) % 4;
        state=WAIT_NEXT_MOVE;
      } else{
        state = START_NEW_SHAPE;
      }
      move_right=0;
      move_left=0;
      move_down=0;
      rotate_clockwise=0;
      break; 
    case WAIT_NEXT_MOVE:
     // Serial.println("WAIT_NEXT_MOVE");
      if(timeout !=0 || left !=0 || right !=0 || down !=0){
        state = CHECK_NEXT_MOVE;
        move_left = left;
        move_right = right;
        move_down = down;
        rotate_clockwise = rotate;
        left = 0;
        right = 0;
        down = 0;
        rotate = 0;
        timeout = 0;
      }
      break; 
    case GAMEOVER: 
      //Serial.println("GAMEOVER");
      state=STOP;
      delay(1000);
      show_gameover();
      break;
    default: 
      ; // nothing to do - stop
  }   
}  // End of loop

ISR(TIMER1_OVF_vect)
{
  TCNT1 = PRELOAD_TIMER;// preload timer
  timeout = 1;
  down = 1;
}

void left_pin_pressed(){
  Serial.println("left_pin_pressed");
  left = 1;
  output = HIGH; 
}
void right_pin_pressed(){
  Serial.println("right_pin_pressed");
  right = 1;
  output = LOW; 
}

void rotate_pin_pressed(){
  Serial.println("right_pin_pressed");
  rotate = 1;
}
