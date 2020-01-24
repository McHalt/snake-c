#include <stdio.h>
#include <windows.h>
#include <conio.h>
#include <time.h>

struct Snake_Element{
    int x, y;
    struct Snake_Element *next, *prev;
};

struct Snake{
    struct Snake_Element *head;
    struct Snake_Element *end;
    int length;
};

struct Snake_Element *Snake_create_element(){
    struct Snake_Element *E = malloc(sizeof(struct Snake_Element));
    E->x = 0;
    E->y = 0;
    E->prev = NULL;
    E->next = NULL;
    return E;
};

void get_config(int *config, int length){
    FILE *f;
    if(!(f = fopen("snake.conf", "r"))) exit(0);
    int i;
    char temp[256];
    for(i = 0; i < length; i++){
        fscanf(f, "%s", temp);
        config[i] = atoi(strtok(temp, ":"));
    }
    fclose(f);
}

void save_config(int *config){
    FILE *f;
    if(!(f = fopen("snake.conf", "w"))) exit(0);
    fprintf(f, "%d:speed", config[0]);
    fprintf(f, "\n");
    fprintf(f, "%d:board_type", config[1]);
    fprintf(f, "\n");
    fprintf(f, "%d:board_size", config[2]);
    fclose(f);
}

void gotoxy(int x, int y){
    COORD coord = {0, 0};
    coord.X = x; coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

int get_terminal_height(){
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    int height;

    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    return height;
}

int get_terminal_width(){
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    int width;

    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    return width;
}

void Snake_construct(struct Snake *snake){
    snake->head = Snake_create_element();
    snake->head->prev = Snake_create_element();
    snake->end = Snake_create_element();
    snake->head->prev->prev = snake->end;
    snake->head->prev->next = snake->head;
    snake->end->next = snake->head->prev;
    snake->length = 3;
}

void Snake_add_element(struct Snake *snake, int x, int y){
    struct Snake_Element *el;
    el = Snake_create_element();
    el->x = x;
    el->y = y;
    el->next = snake->end;
    snake->end->prev = el;
    snake->end = el;
    snake->length++;
}

char border[6] = {201, 187, 188, 200, 205, 186};

int is_it_border(int val){
    int i;
    for(i = 0; i < 6; i++) if(border[i] == val) return 1;
    return 0;
}

int is_it_snake(struct Snake snake, int x, int y){
    struct Snake_Element *temp;
    temp = snake.head;
    if(temp->x == x && temp->y == y) return 3;
    temp = temp->prev;
    if(temp->x == x && temp->y == y) return 2;
    temp = temp->prev;
    while(temp){
        if(temp->x == x && temp->y == y) return 1;
        temp = temp->prev;
    }
    return 0;
}

void print_character(int amount, char character){
    int i;
    for(i = 0; i < amount; i++){
        printf("%c", character);
    }
}

char **create_board(int width, int height){
    char **board = malloc(sizeof(int*)*height);
    int i;
    for(i = 0; i < height; i++){
        board[i] = malloc(sizeof(int)*width);
    }
    return board;
}

int fill_board_square(char **board, int board_size){
    int height = get_terminal_height(), width = get_terminal_width();
    int side_len;
    switch(board_size){
        case 1:{
            if(height > width/2) side_len = (width/2)*0.4;
            else side_len = height*0.4;
            break;
        }
        case 2:{
            if(height > width/2) side_len = (width/2)*0.60;
            else side_len = height*0.60;
            break;
        }
        case 3:{
            if(height > width/2) side_len = (width/2)*0.8;
            else side_len = height*0.8;
            break;
        }
        default:{
            if(height > width/2) side_len = (width/2)*0.60;
            else side_len = height*0.60;
            break;
        }
    }
    int i, j, k, l;
    for(i = 0; i < height; i++) for(j = 0; j < width; j++) board[i][j] = 1;
    i = ((height-side_len)/2-1);
    j = (width/2)-side_len;
    board[i][j++] = border[0];
    for(k = 0; k < (side_len*2)-1; k++) board[i][j++] = border[4];
    board[i++][j] = border[1];
    for(l = 0; l < side_len; l++){
        j = (width/2)-side_len;
        board[i][j++] = border[5];
        for(k = 0; k < (side_len*2)-1; k++) board[i][j++] = 32;
        board[i++][j] = border[5];
    }
    j = (width/2)-side_len;
    board[i][j++] = border[3];
    for(k = 0; k < (side_len*2)-1; k++) board[i][j++] = border[4];
    board[i][j] = border[2];
    return side_len*((side_len*2)-1);
}

int fill_board(char **board, int board_type, int board_size){
    switch(board_type){
        case 1:{
            return fill_board_square(board, board_size);
        }
        case 2:{
            //rectangle
            break;
        }
        case 3:{
            //irregular
            break;
        }
        default:{
            return fill_board_square(board, board_size);
        }
    }
}

void print_board(char **board){
    int height = get_terminal_height(), width = get_terminal_width();
    int i, j;
    for(i = 0; i < height; i++){
        for(j = 0; j < width; j++){
            if(board[i][j] == 1){
                SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 0x80);
                printf(" ");
            }else if(is_it_border(board[i][j])) {
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x00);
                printf(" ");
            }else if(board[i][j] == 43){
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0xF1);
                printf("%c", board[i][j]);
            }else{
                SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 0xF0);
                printf("%c", board[i][j]);
            }
        }
    }
    SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 0x07);
}

void print_game_over(int score){
    int l = 6;
    int i, j, width = get_terminal_width(), height = get_terminal_height();
    int bckg_count[3] = {width, (width-62)/2, (width-62)/2};
    if(width % 2) bckg_count[2] = (width-61)/2;
    if(score < 10) l = 1;
    else if(score < 100) l = 2;
    else if(score < 1000) l = 3;
    else if(score < 10000) l = 4;
    char game_over[6][100] = {
            "  _____          __  __ ______    ______      ________ _____  "
            , " / ____|   /\\   |  \\/  |  ____|  / __ \\ \\    / /  ____|  __ \\ "
            , "| |  __   /  \\  | \\  / | |__    | |  | \\ \\  / /| |__  | |__) |"
            , "| | |_ | / /\\ \\ | |\\/| |  __|   | |  | |\\ \\/ / |  __| |  _  / "
            , "| |__| |/ ____ \\| |  | | |____  | |__| | \\  /  | |____| | \\ \\ "
            , " \\_____/_/    \\_\\_|  |_|______|  \\____/   \\/   |______|_|  \\_\\"
    };

    for(i = 0; i < (get_terminal_height()-8)/2; i++){
        Sleep(25);
        print_character(bckg_count[0], 35);
    }

    for(i = 0; i < 6; i++){
        Sleep(25);
        print_character(bckg_count[1], 35);
        printf("%s", game_over[i]);
        print_character(bckg_count[2], 35);
    }
    Sleep(25);
    print_character(bckg_count[1], 35);
    for(j = 0; j < 62; j++) printf(" ");
    print_character(bckg_count[2], 35);
    Sleep(25);
    print_character(bckg_count[1], 35);
    for(j = 0; j < (50-l)/2; j++) printf(" ");
    printf("S C O R E : %d", score);
    for(j = 0; j < (50-l)/2; j++) printf(" ");
    if(l%2) printf(" ");
    print_character(bckg_count[2], 35);

    for(i = 0; i < (height-8)/2; i++){
        Sleep(25);
        if(height % 2) print_character(bckg_count[0], 35);
    }
    if(height % 2) print_character(bckg_count[0], 35);
    gotoxy(0,0);
    getch();
}

void set_initial_snake_position(struct Snake *snake){
    int height = get_terminal_height(), width = get_terminal_width();
    snake->head->x = width/2+1;
    snake->head->y = height/2;
    snake->head->prev->x = width/2;
    snake->head->prev->y = height/2;
    snake->end->x = width/2-1;
    snake->end->y = height/2;
}

int motion(struct Snake *snake, char **board, int direction){
    struct Snake_Element *temp;
    int newx, newy;
    if(direction != 0){
        if(direction == 72){
            newy = snake->head->y-1;
            newx = snake->head->x;
        }else if(direction == 77){
            newy = snake->head->y;
            newx = snake->head->x+1;
        }else if(direction == 80){
            newy = snake->head->y+1;
            newx = snake->head->x;
        }else if(direction == 75){
            newy = snake->head->y;
            newx = snake->head->x-1;
        }
        if(is_it_snake(*snake, newx, newy) == 2) direction = 0;
    }
    if(direction == 0) {
        if (snake->head->y == snake->head->prev->y) {
            if (snake->head->x > snake->head->prev->x) newx = snake->head->x + 1;
            else newx = snake->head->x-1;
            newy = snake->head->y;
        } else {
            newx = snake->head->x;
            if (snake->head->y > snake->head->prev->y) newy = snake->head->y + 1;
            else newy = snake->head->y-1;
        }
    }
    int s = is_it_snake(*snake, newx, newy);
    temp = snake->end;
    int oldx = temp->x, oldy = temp->y;
    while(temp->next){
        temp->x = temp->next->x;
        temp->y = temp->next->y;
        temp = temp->next;
    }
    temp->x = newx;
    temp->y = newy;
    if(is_it_border(board[newy][newx])) return 0;
    if(board[newy][newx] == 43){
        Snake_add_element(snake, oldx, oldy);
        board[newy][newx] = 32;
        return 2;
    }
    if(s != 0 && s != 3) return 0;
    gotoxy(oldx, oldy);
    printf(" ");
    return 1;
}

void set_food(struct Snake snake, char **board, COORD *free_space, int free_space_count){
    int r, x, y;
    do{
        r = rand()%free_space_count;
        x = free_space[r].X;
        y = free_space[r].Y;
    }while(is_it_snake(snake,y, x));
    board[x][y] = 43;
    gotoxy(y, x);
    SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 0xF1);
    printf("%c", 43);
}

void print_snake(struct Snake Snake){
    SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 0xF5);
    struct Snake_Element *temp;
    gotoxy(Snake.head->x, Snake.head->y);
    printf("@");
    temp = Snake.head->prev;
    while(temp){
        gotoxy(temp->x, temp->y);
        printf("o");
        temp = temp->prev;
    }
}

void countdown(int sec){
    int height = get_terminal_height(), width = get_terminal_width();
    for(; sec > 0; sec--){
        gotoxy(width/2, (height/2)-2);
        printf("%d", sec);
        gotoxy(0,0);
        Sleep(1000);
    }
    gotoxy(width/2, (height/2)-2);
    printf(" ");
    gotoxy(0,0);
}

void fill_free_space_array(char **board, COORD *free_space){
    int i, j, r = 0;
    int height = get_terminal_height(), width = get_terminal_width();
    for(i = 0; i < height; i++){
        for(j = 0; j < width; j++){
            if(board[i][j] == 32){
                free_space[r].X = i;
                free_space[r++].Y = j;
            }
        }
    }
}

void game(){
    int config[3];
    get_config(config, sizeof(config)/sizeof(int));
    unsigned int R;
    srand(R*time(NULL));
    COORD *free_space;
    struct Snake snake;
    Snake_construct(&snake);
    char **board = create_board(get_terminal_width(), get_terminal_height());
    int free_space_count = fill_board(board, config[1], config[2]);
    free_space = malloc(sizeof(COORD)*free_space_count);
    fill_free_space_array(board, free_space);
    set_food(snake, board, free_space, free_space_count);
    system("cls");
    print_board(board);
    SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 0xF5);
    set_initial_snake_position(&snake);
    print_snake(snake);
    SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 0xF0);
    countdown(5);
    SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 0xF5);
    int game = 1;
    int direction, score = 0;
    time_t last_change = clock();
    int speed = config[0];
    gotoxy(0, get_terminal_height()-1);
    SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 0x8E);
    printf("Score: %d", score);
    while(game){
        SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 0xF5);
        direction = 0;
        while(clock() - last_change < speed){
            if (kbhit() && direction == 0){
                int c = getch();
                if (c==224) direction = getch();
            }
        }
        last_change = clock();
        switch(motion(&snake, board, direction)){
            case 0:
                game = 0;
                break;
            case 2: {
                set_food(snake, board, free_space, free_space_count);
                score++;
                gotoxy(0, get_terminal_height()-1);
                SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 0x8E);
                printf("Score: %d", score);
            }
            default:
                print_snake(snake);
        }
        gotoxy(0,0);
    }

    print_game_over(score);

}

void print_menu(){
    int width = get_terminal_width(), i, p = 0;
    if(width % 2) p = 1;
    system("cls");
    for(i = 0; i < width; i++) printf("#");
    printf("#");
    for(i = 0; i < width/2-3; i++) printf(" ");
    printf("MENU");
    for(i = 0; i < width/2-3+p; i++) printf(" ");
    printf("#");
    for(i = 0; i < width; i++) printf("#");
    gotoxy(width-28, get_terminal_height()-1);
    SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 0xF2);
    printf("Autor: Krzysztof Chroscinski");
}

void print_option(char *option, int current, int pos){
    if(current) SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 0x5F);
    else SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 0xF5);
    gotoxy(0, pos);
    printf("%s", option);
    gotoxy(0,0);
}

int menu(){
    int option = 1, c = 0, i;
    char options[3][20];
    strcpy(options[0], "Graj              ");
    strcpy(options[1], "Ustawienia        ");
    strcpy(options[2], "Wyjscie           ");
    print_menu();
    print_option(options[0], 1, 4);
    for(i = 1; i < sizeof(options)/sizeof(options[0]); i++) print_option(options[i], 0, i+4);
    while (c != 13) {
        if (kbhit()) {
            c = getch();
            if (c == 224) c = getch();
            switch (c) {
                case 72: {
                    if (option > 1) {
                        option--;
                        print_option(options[option - 1], 1, option + 3);
                        print_option(options[option], 0, option + 4);
                    }
                    break;
                }
                case 80: {
                    if (option < 3) {
                        option++;
                        print_option(options[option - 1], 1, option + 3);
                        print_option(options[option - 2], 0, option + 2);
                    }
                    break;
                }
            }
        }
    }
    return option;
}

int config_menu(){int option = 1, c = 0;
    char options[4][20];
    int i;
    strcpy(options[0], "Szybkosc weza     ");
    strcpy(options[1], "Typ planszy       ");
    strcpy(options[2], "Rozmiar planszy   ");
    strcpy(options[3], "Wyjscie           ");
    print_menu();
    print_option(options[0], 1, 4);
    for(i = 1; i < sizeof(options)/sizeof(options[0]); i++) print_option(options[i], 0, i+4);
    while (c != 13) {
        if (kbhit()) {
            c = getch();
            if (c == 224) c = getch();
            switch (c) {
                case 72: {
                    if (option > 1) {
                        option--;
                        print_option(options[option - 1], 1, option + 3);
                        print_option(options[option], 0, option + 4);
                    }
                    break;
                }
                case 80: {
                    if (option < 4) {
                        option++;
                        print_option(options[option - 1], 1, option + 3);
                        print_option(options[option - 2], 0, option + 2);
                    }
                    break;
                }
            }
        }
    }
    return option;
}

int main(){
    int option = 0, coption = 0;
    while(option != 3) {
        option = menu();
        switch (option) {
            case 1: {
                game();
                break;
            }
            case 2:{
                while(coption != 4){
                    coption = config_menu();
                }
                coption = 0;
                break;
            }
            case 3:
                exit(0);
        }
    }
    return 0;
}