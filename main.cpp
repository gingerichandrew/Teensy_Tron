#include <Adafruit_NeoPixel.h>

#ifdef __AVR__
 #include <avr/power.h> 
#endif

#define NUMPIXELS 256 // Number of pixels

#define PIN             0 // NeoPixel data
#define PIN_VRX_L        A1 // Joystick x-input
#define PIN_VRY_L        A0 // Joystick y-input
#define PIN_SW_L         41 // Joystick press

#define PIN_VRX_R        A10 // Joystick x-input
#define PIN_VRY_R        A11 // Joystick y-input
#define PIN_SW_R         26 // Joystick press

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

IntervalTimer input_handler_R;
IntervalTimer input_handler_L;

volatile int s_dir_R = 0;     // Player R direction -1 DOWN & 1 UP | -2 LEFT & 2 RIGHT
volatile int s_dir_L = 0;     // Player L direction -1 DOWN & 1 UP | -2 LEFT & 2 RIGHT
volatile bool b_pressed_R = false; // Joystick's R button press, 0 not | 1 pressed
volatile bool b_pressed_L = false; // Joystick's L button press, 0 not | 1 pressed

int s_dir_Copy_R = 0;
int s_dir_Copy_L = 0;
int player_lengths = 1;
int position_map[32][8];
int size_counter = 0;

struct tron_bike *player_one;
struct tron_bike *player_two;

struct tron_bike{
  int pos[2];
  struct tron_bike *next;
};

void reset_players(){
  player_one->next = NULL;
  player_two->next = NULL;

  player_one->pos[0] = 1;
  player_one->pos[1] = 1;
  player_one->next = NULL;
  
  player_two->pos[0] = 30;
  player_two->pos[1] = 6;
  player_two->next = NULL;

  s_dir_R = 0;     // Player R direction -1 DOWN & 1 UP | -2 LEFT & 2 RIGHT
  s_dir_L = 0;     // Player L direction -1 DOWN & 1 UP | -2 LEFT & 2 RIGHT

  player_lengths = 1;
  size_counter = 0;
}

void flash_board(int x_hit, int y_hit){
  draw_players();
  pixels.setPixelColor(position_map[x_hit][y_hit], pixels.Color(0, 50, 0));
  pixels.show();   // Send the updated pixel colors to the hardware.
  delay(2000);
  reset_players();
}

void update_players(){
  tron_bike* t_player_one = player_one;
  tron_bike* t_player_two = player_two;
  
  int prev_pos_R[2];
  prev_pos_R[0] = t_player_two->pos[0];
  prev_pos_R[1] = t_player_two->pos[1];
  if(s_dir_Copy_R == 0 || s_dir_Copy_L == 0){
    return;
  }
  switch(s_dir_R){
    case 1:
      t_player_two->pos[1] = t_player_two->pos[1] + 1;
      if(t_player_two->pos[1] > 7)flash_board(t_player_two->pos[0],  t_player_two->pos[1] - 1);
      break;
    case -1:
      t_player_two->pos[1] = t_player_two->pos[1] - 1;
      if(t_player_two->pos[1] < 0) flash_board(t_player_two->pos[0],  t_player_two->pos[1] + 1);
      break;
    case 2:
      t_player_two->pos[0] = t_player_two->pos[0] + 1;
      if(t_player_two->pos[0] > 31) flash_board(t_player_two->pos[0] - 1,  t_player_two->pos[1]);
      break;    
    case -2:
      t_player_two->pos[0] = t_player_two->pos[0] - 1;
      if(t_player_two->pos[0] < 0) flash_board(t_player_two->pos[0] + 1,  t_player_two->pos[1]);
      break;
  }
  t_player_two = t_player_two->next;
  while (t_player_two != NULL) {
    int prev_pos_temp_R[2];
    prev_pos_temp_R[0] = t_player_two->pos[0];
    prev_pos_temp_R[1] = t_player_two->pos[1];
    t_player_two->pos[0] = prev_pos_R[0];
    t_player_two->pos[1] = prev_pos_R[1];

    prev_pos_R[0] = prev_pos_temp_R[0];
    prev_pos_R[1] = prev_pos_temp_R[1];
    
    t_player_two = t_player_two->next;
  }

  int prev_pos_L[2];
  prev_pos_L[0] = t_player_one->pos[0];
  prev_pos_L[1] = t_player_one->pos[1];
  if(s_dir_Copy_L == 0){
    return;
  }
  switch(s_dir_L){
    case 1:
      t_player_one->pos[1] = t_player_one->pos[1] + 1;
      if(t_player_one->pos[1] > 7) flash_board(t_player_one->pos[0],  t_player_one->pos[1]- 1);
      break;
    case -1:
      t_player_one->pos[1] = t_player_one->pos[1] - 1;
      if(t_player_one->pos[1] < 0) flash_board(t_player_one->pos[0],  t_player_one->pos[1] + 1);
      break;
    case 2:
      t_player_one->pos[0] = t_player_one->pos[0] + 1;
      if(t_player_one->pos[0] > 31) flash_board(t_player_one->pos[0] - 1,  t_player_one->pos[1]);
      break;    
    case -2:
      t_player_one->pos[0] = t_player_one->pos[0] - 1;
      if(t_player_one->pos[0] < 0) flash_board(t_player_one->pos[0] + 1,  t_player_one->pos[1]);
      break;
  }
  t_player_one = t_player_one->next;
  while (t_player_one != NULL) {
    int prev_pos_temp_L[2];
    prev_pos_temp_L[0] = t_player_one->pos[0];
    prev_pos_temp_L[1] = t_player_one->pos[1];
    t_player_one->pos[0] = prev_pos_L[0];
    t_player_one->pos[1] = prev_pos_L[1];

    prev_pos_L[0] = prev_pos_temp_L[0];
    prev_pos_L[1] = prev_pos_temp_L[1];
    
    t_player_one = t_player_one->next;
  }
  
}

void draw_players(){
  tron_bike* t_player_one = player_one;
  tron_bike* t_player_two = player_two;
  
  int i = 50;
  while (t_player_one != NULL) {
     if(i > 5){
       i -= 2;
     }
    int pos = position_map[t_player_one->pos[0]][t_player_one->pos[1]];
    pixels.setPixelColor(pos, pixels.Color(i, 0, 0));
    t_player_one = t_player_one->next;
  }
  i = 50;
  while (t_player_two != NULL) {
    if(i > 5){
       i -= 2;
     }
    int pos = position_map[t_player_two->pos[0]][t_player_two->pos[1]];
    pixels.setPixelColor(pos, pixels.Color(0, 0, i));
    t_player_two = t_player_two->next;
  }

}

void fill_position_map(){
  int counter = 0;
  for (int i=0; i < 32; ++i) {
    if (i%2 == 0) {
      // for even rows, print as we normally would
      for (int j=0; j < 9; ++j) {
        position_map[i][j] = counter;
        counter++;
      }
      counter--;
    }
    else if(i%2 != 0) {
      for (int j=7; j >=0; j--) {
        position_map[i][j] = counter;
        counter++;
      }
    }
  }
}

void get_input_R(){
  int prev_d = s_dir_R;
  int sw = digitalRead(PIN_SW_R);
  int x = analogRead(PIN_VRY_R);
  int y = analogRead(PIN_VRX_R);
  if(sw == LOW){
    b_pressed_R = true;
  }
  if(x > 520 || x < 480 || y < 300 || y > 700){
    int absX = abs(x - 512);
    int absY = abs(y - 512);
    
    if(absX > absY){// LEFT -> RIGHT movement
      if(x > 512 && s_dir_Copy_R != -2){
        s_dir_R = 2;  // RIGHT
      }else if(x < 512 && s_dir_Copy_R != 2) {
        s_dir_R = -2; // LEFT
      }
    }
    else {       // else then UP -> DOWN movement
      if(y > 512 && s_dir_Copy_R != -1){
        s_dir_R = 1; // UP
      }else if(y < 512 && s_dir_Copy_R != 1){
        s_dir_R = -1;  // DOWN
      }
    }
  }
  else{
    s_dir_R = s_dir_R;
  }
}

void get_input_L(){
  int prev_d = s_dir_L;
  int sw = digitalRead(PIN_SW_L);
  int x = analogRead(PIN_VRX_L);
  int y = analogRead(PIN_VRY_L);
  if(sw == LOW){
    b_pressed_L = true;
  }
  if(x > 520 || x < 480 || y < 300 || y > 700){
    int absX = abs(x - 512);
    int absY = abs(y - 512);
    
    if(absX > absY){// LEFT -> RIGHT movement
      if(x > 512 && s_dir_Copy_L != 2){
        s_dir_L = -2;  // LEFT
      }else if(x < 512 && s_dir_Copy_L != -2) {
        s_dir_L = 2; // RIGHT
      }
    }
    else {       // else then UP -> DOWN movement
      if(y > 512 && s_dir_Copy_L != 1){
        s_dir_L = -1; // DOWN
      }else if(y < 512 && s_dir_Copy_L != -1){
        s_dir_L = 1;  // UP
      }
    }
  }
  else{
    s_dir_L = s_dir_L;
  }
}

void generate_players(){
  player_one = (struct tron_bike*)malloc(sizeof(struct tron_bike)); // Create head of snake
  player_one->pos[0] = 7;
  player_one->pos[1] = 1;
  player_one->next = NULL;

  player_two = (struct tron_bike*)malloc(sizeof(struct tron_bike)); // Create head of snake
  player_two->pos[0] = 24;
  player_two->pos[1] = 6;
  player_two->next = NULL;
  
  struct tron_bike *player_one_tail_previous = player_one;
  struct tron_bike *player_two_tail_previous = player_two;
  
  for(int i = 0; i < 6; i++){ 
    struct tron_bike *player_one_tail = (struct tron_bike*)malloc(sizeof(struct tron_bike)); // Create head of snake
    struct tron_bike *player_two_tail = (struct tron_bike*)malloc(sizeof(struct tron_bike)); // Create head of snake
  
    player_one_tail->pos[0] = player_one_tail_previous->pos[0] - 1;
    player_one_tail->pos[1] = player_one_tail_previous->pos[1];
    player_one_tail->next = NULL;
  
    player_two_tail->pos[0] = player_two_tail_previous->pos[0] + 1;
    player_two_tail->pos[1] = player_two_tail_previous->pos[1];
    player_two_tail->next = NULL;

    player_one_tail_previous->next = player_one_tail;
    player_two_tail_previous->next = player_two_tail;

    player_one_tail_previous = player_one_tail;
    player_two_tail_previous = player_two_tail;
  }
}
void check_collision(){
  tron_bike* t_player_one = player_one->next;
  tron_bike* t_player_two = player_two->next;
  int player_one_pos[player_lengths][2];
  int player_two_pos[player_lengths][2];
  int i;
  player_one_pos[0][0] = player_one->pos[0];
  player_one_pos[0][1] = player_one->pos[1];

  player_two_pos[0][0] = player_two->pos[0];
  player_two_pos[0][1] = player_two->pos[1];
  i = 1;
  while (t_player_one != NULL) {    // 0
    if(player_one->pos[0] == t_player_one->pos[0] && player_one->pos[1] == t_player_one->pos[1]){
      flash_board(player_one->pos[0], player_one->pos[1]);
    }
    player_one_pos[i][0] = t_player_one->pos[0];
    player_one_pos[i][1] = t_player_one->pos[1];
    i++;
    t_player_one = t_player_one->next;
  }
  
  i = 1;
  while (t_player_two != NULL) {   // 1
    if(player_two->pos[0] == t_player_two->pos[0] && player_two->pos[1] == t_player_two->pos[1]){
      flash_board(player_two->pos[0], player_two->pos[1]);
    }
    player_two_pos[i][0] = t_player_two->pos[0];
    player_two_pos[i][1] = t_player_two->pos[1];
    i++;
    t_player_two = t_player_two->next;
  }

  for(int i = 0; i < player_lengths; i++){
    if((player_one->pos[0] == player_two_pos[i][0]) && (player_one->pos[1] == player_two_pos[i][1])){
      flash_board(player_one->pos[0], player_one->pos[1]);
    }
  }
  
  for(int i = 0; i < player_lengths; i++){
      if((player_two->pos[0] == player_one_pos[i][0]) && (player_two->pos[1] == player_one_pos[i][1])){
        flash_board(player_two->pos[0], player_two->pos[1]);
    }
  }
}

void increase_snake(){
  tron_bike* t_player_one = player_one;
  tron_bike* t_player_two = player_two;
  
  struct tron_bike *player_one_node = (struct tron_bike*)malloc(sizeof(struct tron_bike)); // Create node of snake
  struct tron_bike *player_two_node = (struct tron_bike*)malloc(sizeof(struct tron_bike)); // Create node of snake
  
  while (t_player_one != NULL) {
    if(t_player_one->next == NULL){
      t_player_one->next = player_one_node;
      player_one_node->pos[0] = t_player_one->pos[0];
      player_one_node->pos[1] = t_player_one->pos[1];
      player_one_node->next = NULL;
      break;
    }
    t_player_one = t_player_one->next;
  }
  while (t_player_two != NULL) {
    if(t_player_two->next == NULL){
      t_player_two->next = player_two_node;
      player_two_node->pos[0] = t_player_two->pos[0];
      player_two_node->pos[1] = t_player_two->pos[1];
      player_two_node->next = NULL;
      break;
    }
    t_player_two = t_player_two->next;
  }
  player_lengths++;
}

void setup() {
  Serial.begin(9600);        // open the serial port at 9600 bps:
  pixels.clear();            // Set all pixel colors to 'off'
  pixels.begin();            // INITIALIZE NeoPixel strip object

  pinMode(A0, INPUT);        // Set A0 to input, pin 14
  pinMode(A1, INPUT);        // Set A1 to input, pin 15
  pinMode(PIN_SW_L, INPUT);  // Set PIN_SW to input, pin 41
  
  pinMode(A10, INPUT);       // Set A0 to input, pin 24
  pinMode(A11, INPUT);       // Set A1 to input, pin 25
  pinMode(PIN_SW_R, INPUT);  // Set PIN_SW to input, pin 26

  fill_position_map();              // Fill up position matrix
  player_one = (struct tron_bike*)malloc(sizeof(struct tron_bike)); // Create head of snake
  player_one->pos[0] = 1;
  player_one->pos[1] = 1;
  player_one->next = NULL;

  player_two = (struct tron_bike*)malloc(sizeof(struct tron_bike)); // Create head of snake
  player_two->pos[0] = 30;
  player_two->pos[1] = 6;
  player_two->next = NULL;
   
  input_handler_R.begin(get_input_R, 10000);  // Asych get input R - 10ms
  input_handler_L.begin(get_input_L, 10000);  // Asych get input L - 10ms
}


void loop() {
  pixels.clear();
  update_players();
  draw_players();
  check_collision();
  if(s_dir_R != 0 && s_dir_L !=0 && size_counter < 30){
      increase_snake();
      size_counter++;
  }
  pixels.show();   // Send the updated pixel colors to the hardware.
  s_dir_Copy_R = s_dir_R;
  s_dir_Copy_L = s_dir_L;
  delay(100);
}
