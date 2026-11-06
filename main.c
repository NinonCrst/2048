//ldd:-lncurses
#include<ncurses.h>
#include<time.h>
#include<stdlib.h>

typedef struct{
  unsigned int len;
  unsigned int len2;
  int* val;
} Tab;

typedef int key;

Tab* board;

unsigned int score = 0;

void init_board(){
  board = malloc(sizeof(Tab));
  board->len = board->len2 = 4;
  board->val = malloc(17*sizeof(int));
  
  for(unsigned int i = 0; i<board->len; i++){
    for(unsigned int j = 0; j<board->len2; j++){
      board->val[i*4+j] = 0;
    }
  }
  
  board->val[16] = '\0';
}

void display_board(){
  unsigned int t = 1;
  printw("== 2048 =============================\n====================== score:%8u\n", score);
  for (unsigned int i = 0; i < board->len; ++i) {
    printw("+--------+--------+--------+--------+\n");
    for (unsigned int j = 0; j < board->len2-1; ++j) {
      for(unsigned int k = 0; k<board->len;k++){
        printw("|");
        if((j==t)&&(board->val[i*4+k]!=0)) printw("%5d   ", board->val[i*4+k]);
        else printw("        ");
        if(k==(board->len-1)) printw("|");
      }
      printw("\n");
    }
    
    if(i==3) printw("+--------+--------+--------+--------+\n");
  }
}

void INIT_2048(){
  initscr();
  score = 0;
  init_board();
  move(0,0);
  refresh();
  display_board();
  raw();
  keypad(stdscr,true);
  noecho();
  srand(time(NULL));
}
 
void DONE_2048() {
  free(board->val);
  free(board);
  endwin();
  exit(0);
}

int count_empty(){
  int emp = 0;
  
  for(unsigned int i = 0; i<board->len; i++){
    for(unsigned int j = 0; j<board->len2; j++){
      if(board->val[i*4+j] == 0) emp++;
    }
  }
  
  return emp;
}

void add_two(int empty){
  int n = rand()%empty, idx = 0;
  
  for(unsigned int i = 0; i<board->len; i++){
    for(unsigned int j = 0; j<board->len2; j++){
      if(board->val[i*4+j] == 0){
        if(n==idx){
          board->val[i*4+j] = 2;
        }
        n--;
      }
    }
  }
}

int win(){
  int w = 0;
  
  for(unsigned int i = 0; i<board->len; i++){
    for(unsigned int j = 0; j<board->len2; j++){
      if(board->val[i*4+j] == 2048) w = 1;
    }
  }
  
  return w;
}

int game_over(int add){
  if(count_empty()==0){
    printw("============= GAME OVER =============\n============ (press a key) ===========\n");
    getch();
    return 1;
  }
  
  else if((count_empty()>=1)&&(add==true)){
    add_two(count_empty());
    move(0,0);
    display_board();
    refresh();
    
    if(win()==1){
      printw("============= WELL DONE =============\n============ (press a key) ===========\n");
      getch();
      return 1;
    }
    
    return 0;
  }
  
  else return 0;
}

void swap(int* a, int* b){
  int echange = *a;
  *a = *b;
  *b = echange;
}

int shift_board(){
  int verif = 0;
  unsigned int idx = 0, esp = 0;
  
  while(esp<board->len){
    idx = 0;
    while(idx < (board->len*board->len2)){
      for(unsigned int i = idx; i<board->len+idx-1; i++){
        if((board->val[i] == 0)&&(board->val[i+1]!=0)){
          swap(&board->val[i], &board->val[i+1]);
          verif = 1;
        }
      }
      idx+=4;
    }
    esp++;
  }
  
  return verif;
}

int update_board(){
  unsigned int idx = 0;
  int retour = 0;
  
  retour = shift_board();
  
  while(idx < (board->len*board->len2)){
    for(unsigned int i = idx; i<board->len+idx-1; i++){
      if((board->val[i] != 0)&&(board->val[i]==board->val[i+1])){
        board->val[i] *= 2;
        board->val[i+1] = 0;
        retour = 1;
        score++;
      }
    }
    idx+=4;
  }
  
  shift_board();
  
  return retour;
} 

key get_key(){
  key k = 1, fin = 0;
  
  while(fin != k){
    k = getch();
    
    switch(k){
      case KEY_UP:
      case KEY_DOWN:
      case KEY_LEFT:
      case KEY_RIGHT:
      case KEY_BACKSPACE:
        fin = k;
        break;
      default:
        continue;
    }
  }
  return k;
}

void mirror_board(){
  unsigned int idx = 0, j = 0;
  
  while(idx < (board->len*board->len2)){
    for(unsigned int i = idx; i<(board->len/2+idx); i++){
      swap(&board->val[i], &board->val[idx+board->len-1-j]);
      j++;
    }
    idx+=4;
  	j = 0;
  }
}

void pivot_board(){
  int start = 1;
  
  for(unsigned int i = 0; i<board->len; i++){
    for(unsigned int j = start; j<board->len2; j++){
      swap(&board->val[i*4+j], &board->val[j*4+i]);
    }
    start++;
  }
  
}

int play(key dir){
  int move = 1;
  
  switch(dir){
    case KEY_LEFT:
 	  move = update_board();
      break;
    case KEY_RIGHT:
      mirror_board();
	  move = update_board();
      mirror_board();
      break;
    case KEY_UP:
      pivot_board();
 	  move = update_board();
      pivot_board();
      break;
    case KEY_DOWN:
      pivot_board();
      mirror_board();
      move = update_board();
      mirror_board();
      pivot_board();
      break;
    case KEY_BACKSPACE:
      INIT_2048();
      break;
  }
  
  return move;
}

int main(){
  int add = 1;
  
  INIT_2048();
  
  while(!game_over(add)){
    add = play(get_key());
  }
  
  DONE_2048();
  
  return 0;
}
