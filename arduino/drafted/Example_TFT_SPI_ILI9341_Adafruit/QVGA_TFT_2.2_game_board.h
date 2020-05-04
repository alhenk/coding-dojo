#include <Adafruit_GFX.h>    // Include core graphics library
#include <Adafruit_ILI9341.h> // Include Adafruit_ILI9341 library to drive the display

// display pins:
#define TFT_DC 9
#define TFT_RST 8
#define TFT_CS 10
// SPI for Arduino Uno  (SCK = 13 and SDA  = 11)
// SPI for Arduino Mega (SCK = 52 and MOSI = 51) MISO = 50
#define DISPLAY_WIDTH  240
#define DISPLAY_HEIGHT 320
#define BOARD_SIZE_X 12
#define BOARD_SIZE_Y 16
#define SHAPE_SIZE 4
#define SHAPE_NUMBER 5

int board_cell_size=DISPLAY_WIDTH/BOARD_SIZE_X;

int show_board(void);
int check_shape(int x, int y, int orientation, int *shape);
int erase_shape(int x, int y, int orientation, int *shape);
int merge_shape(int x, int y, int orientation, int *shape);
int show_gameover();

// Create display:
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

unsigned int colors[9]={ILI9341_BLACK, ILI9341_RED, ILI9341_ORANGE, ILI9341_YELLOW, ILI9341_GREEN, ILI9341_CYAN, ILI9341_BLUE, ILI9341_MAGENTA, ILI9341_WHITE };

int board_matrix[BOARD_SIZE_Y][BOARD_SIZE_X] = {
     {0,0,0,0, 0,0,0,0, 0,0,0,0},
     {0,0,0,0, 0,0,0,0, 0,0,0,0},
     {0,0,0,0, 0,0,0,0, 0,0,0,0},
     {0,0,0,0, 0,0,0,0, 0,0,0,0},

     {0,0,0,0, 0,0,0,0, 0,0,0,0},
     {0,0,0,0, 0,0,0,0, 0,0,0,0},
     {0,0,0,0, 0,0,0,0, 0,0,0,0},
     {0,0,0,0, 0,0,0,0, 0,0,0,0},

     {0,0,0,0, 0,0,0,0, 0,0,0,0},
     {0,0,0,0, 0,0,0,0, 0,0,0,0},
     {0,0,0,0, 0,0,0,0, 0,0,0,0},
     {0,0,0,0, 0,0,0,0, 0,0,0,0},

     {0,0,0,0, 0,0,6,6, 0,0,0,0},
     {0,0,0,0, 0,6,6,1, 0,0,0,0},
     {0,0,0,0, 0,0,0,1, 0,0,0,0},
     {0,0,0,0, 0,0,1,1, 0,0,0,0}
  };

int *shape_1_ptr,shape_1[SHAPE_SIZE*4][SHAPE_SIZE] = {
     {0,0,0,0},
     {1,1,1,0},
     {1,0,0,0},
     {0,0,0,0},

     {0,1,0,0},
     {0,1,0,0},
     {0,1,1,0},
     {0,0,0,0},

     {0,0,1,0},
     {1,1,1,0},
     {0,0,0,0},
     {0,0,0,0},

     {1,1,0,0},
     {0,1,0,0},
     {0,1,0,0},
     {0,0,0,0}     
  };
  
int *shape_2_ptr,shape_2[SHAPE_SIZE*4][SHAPE_SIZE] = {
     {0,0,0,0},
     {0,6,6,0},
     {6,6,0,0},
     {0,0,0,0},

     {0,6,0,0},
     {0,6,6,0},
     {0,0,6,0},
     {0,0,0,0},

     {0,6,6,0},
     {6,6,0,0},
     {0,0,0,0},
     {0,0,0,0},

     {6,0,0,0},
     {6,6,0,0},
     {0,6,0,0},
     {0,0,0,0}
  };
  
int *shape_3_ptr,shape_3[SHAPE_SIZE*4][SHAPE_SIZE] = {
     {0,0,0,0},
     {3,3,0,0},
     {0,3,3,0},
     {0,0,0,0},

     {0,0,3,0},
     {0,3,3,0},
     {0,3,0,0},
     {0,0,0,0},

     {3,3,0,0},
     {0,3,3,0},
     {0,0,0,0},
     {0,0,0,0},

     {0,3,0,0},
     {3,3,0,0},
     {3,0,0,0},
     {0,0,0,0}
  };
int *shape_4_ptr,shape_4[SHAPE_SIZE*4][SHAPE_SIZE] = {
     {0,0,0,0},
     {0,4,4,0},
     {0,4,4,0},
     {0,0,0,0},

     {0,0,0,0},
     {0,4,4,0},
     {0,4,4,0},
     {0,0,0,0},

     {0,0,0,0},
     {0,4,4,0},
     {0,4,4,0},
     {0,0,0,0},

     {0,0,0,0},
     {0,4,4,0},
     {0,4,4,0},
     {0,0,0,0}
  };
int *shape_5_ptr,shape_5[SHAPE_SIZE*4][SHAPE_SIZE] = {
     {0,5,0,0},
     {0,5,0,0},
     {0,5,0,0},
     {0,5,0,0},

     {0,0,0,0},
     {5,5,5,5},
     {0,0,0,0},
     {0,0,0,0},

     {0,5,0,0},
     {0,5,0,0},
     {0,5,0,0},
     {0,5,0,0},

     {0,0,0,0},
     {5,5,5,5},
     {0,0,0,0},
     {0,0,0,0}
  };
int show_board(void){
   for (int j=0;j<BOARD_SIZE_Y;j++){
        for(int i=0;i<BOARD_SIZE_X;i++){
          tft.fillRect(i*board_cell_size, j*board_cell_size, board_cell_size, board_cell_size, colors[board_matrix[j][i]]);
          tft.drawRect(i*board_cell_size, j*board_cell_size, board_cell_size, board_cell_size, ILI9341_BLACK);
        }
      }
   return 0;
}

int check_shape(int x, int y, int orientation, int *shape){
  int count=0;
  int cell_value=0;
  //Serial.println("Checking cells");
  for(int j = 0; j < SHAPE_SIZE; j++){
    for(int i = 0; i<SHAPE_SIZE; i++){
      cell_value = *(shape + count + orientation*SHAPE_SIZE*SHAPE_SIZE);
      //Serial.print(board_matrix[j + y][i + x]);
      //Serial.print(" -> ");
      //Serial.print(cell_value);
      //Serial.print("  ");
      if (board_matrix[j + y][i + x] != 0 && cell_value != 0){
        return -1;
      }
      count++;
    }
    //Serial.println();
  }
  return 0;
}

 int erase_shape(int x, int y, int orientation, int *shape){
  int count=0;
  int cell_value=0;
  //Serial.println("Erasing shape");
  for(int j=0;j<SHAPE_SIZE;j++){
    for(int i=0;i<SHAPE_SIZE;i++){
      cell_value=*(shape + count + orientation*SHAPE_SIZE*SHAPE_SIZE);
      //Serial.print(board_matrix[j+y][i+x]);
      //Serial.print(" -> ");
      //Serial.print(cell_value);
      //Serial.print(" = ");
      if (cell_value != 0) {     
               board_matrix[j+y][i+x] = 0;  
      }else{
 
      }
      //Serial.print(board_matrix[j+y][i+x]);
      //Serial.print("  ");
      count++;
    }
    //Serial.println();
  }
  return 0;
 }

int show_gameover(){
  tft.fillRect(0, 140, 240, 60, ILI9341_BLACK);
  tft.setCursor(10, 160);
  tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(3);
  tft.println("GAME OVER!");
}

int merge_shape(int x, int y, int orientation, int *shape){
  int count = 0;
  int cell_value = 0;
  //Serial.println("Putting shape");
  for(int j = 0; j < SHAPE_SIZE; j++){
    for(int i = 0; i<SHAPE_SIZE; i++){
      cell_value = *(shape + count + orientation*SHAPE_SIZE*SHAPE_SIZE);
      //Serial.print(board_matrix[j+y][i+x]);
      //Serial.print(" -> ");
      //Serial.print(cell_value);
      //Serial.print(" = ");
      if (board_matrix[j + y][i + x] == 0) {
        board_matrix[j + y][i + x] = cell_value;
      }
      //Serial.print(board_matrix[j+y][i+x]);
      //Serial.print("  ");
      count++;
    }
    //Serial.println();
  }
  return 0;
 }
