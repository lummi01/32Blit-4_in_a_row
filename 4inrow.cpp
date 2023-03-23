#include "4inrow.hpp"
#include "assets.hpp"

#define SCREEN_WIDTH 160

using namespace blit;

Surface* gameboard = nullptr;

short tiefe = 6;

struct GAME
{
    short board[6][7];
    short status;
};

struct TOKEN
{
    short color;
    short x;
    short y;
    short dir;   
    float info; 
};

GAME game;
TOKEN token;

Tween winner;

float min(short resttiefe, float alpha, float beta);

void start()
{
    for (short y=0; y<6; y++)
        for (short x=0; x<7; x++)
            game.board[y][x] = 0;

    token.color = 1;
    token.x = 0;
    token.y = 0;

    game.status = 1;
}

int check_win()
{
    short pos0[8]{0,0,0,0,0,0,0,3};
    short posn[8]{4,4,7,4,6,3,3,6};
    short direction[8]{1,1,0,1,0,1,1,-1};

    for (short posxy=0; posxy<4; posxy++)
    {
        for (short posy=pos0[posxy + 4]; posy<posn[posxy + 4]; posy++)
        {
            for (short posx=pos0[posxy]; posx<posn[posxy]; posx++)
            {
                if(game.board[posy][posx] == token.color &&
                   game.board[posy + direction[posxy + 4]][posx + direction[posxy]] == token.color &&
                   game.board[posy + (direction[posxy + 4] * 2)][posx + (direction[posxy] * 2)] == token.color &&
                   game.board[posy + (direction[posxy + 4] * 3)][posx + (direction[posxy] * 3)] == token.color)
                {
                    token.x = posx;
                    token.y = posy;
                    token.dir = posxy;
                    game.status = 4;
                    return token.color;
                    break;
                }
            }
        }
    }
    for (short x=0; x<7; x++)
    {
        if (game.board[0][x] == 0)
        {
            return -1;
            break;
        }
    }
    game.status = 5;
    return 0;
}

int check()
{
    short pos0[8]{0,0,0,0,0,0,0,3};
    short posn[8]{4,4,7,4,6,3,3,6};
    short direction[8]{1,1,0,1,0,1,1,-1};

    for (short i=1; i<3; i++)
    {
        for (short posxy=0; posxy<4; posxy++)
        {
            for (short posy=pos0[posxy + 4]; posy<posn[posxy + 4]; posy++)
            {
                for (short posx=pos0[posxy]; posx<posn[posxy]; posx++)
                {
                    if(game.board[posy][posx] == i &&
                       game.board[posy + direction[posxy + 4]][posx + direction[posxy]] == i &&
                       game.board[posy + (direction[posxy + 4] * 2)][posx + (direction[posxy] * 2)] == i &&
                       game.board[posy + (direction[posxy + 4] * 3)][posx + (direction[posxy] * 3)] == i)
                    {
                        if (i == 1)
                            return 2;
                        else
                            return 0;
                        break;
                    }
                }
            }
        }
    }
    for (short x=0; x<7; x++)
    {
        if (game.board[0][x] == 0)
            return -1;
    }
    return 1;
}

float valency()
{
    float value = 1;
    float val[6][7]{
        { 3, 4, 5, 7, 5, 4, 3},
        { 4, 6, 8,10, 8, 6, 4},
        { 5, 8,11,13,11, 8, 5},
        { 5, 8,11,13,11, 8, 5},
        { 4, 6, 8,10, 8, 6, 4},
        { 3, 4, 5, 7, 5, 4, 3}};

    for (short y=0; y<6; y++)
    {
        for (short x=0; x<7; x++)
        {
            if (game.board[y][x] == 1)
                value+=(val[y][x] / 100);
            if (game.board[y][x] == 2)
                value-=(val[y][x] / 100);
        }
    }
    return value;
}

void add_token(short x, short p)
{
    for (short y=0; y<6; y++)
    {
        if (y == 5 || game.board[y + 1][x] > 0)
        {
            game.board[y][x] = p;
            break;
        }
    }
}

void del_token(short x)
{
    for (short y=0; y<6; y++)
    {
        if (game.board[y][x] > 0)
        {
            game.board[y][x] = 0;
            break;
        }
    }
}

float max(short resttiefe, float alpha, float beta)
{
    if (check() != -1)
        return check();
    if (resttiefe == 0)
        return valency();
    for (short x=0; x<7; x++)
    {
        if (game.board[0][x] == 0)
        {
            add_token(x, 1);
            float value = min(resttiefe -1, alpha, beta);
            if (value > alpha) 
                alpha = value;
            del_token(x);
            if (alpha >= beta)
                return beta;
        }
    }
    return alpha;
}

float min(short resttiefe, float alpha, float beta)
{
    if (check() != -1)
        return check();
    if (resttiefe == 0)
        return valency();
    for (short x=0; x<7; x++)
    {
        if (game.board[0][x] == 0)
        {
            add_token(x, 2);
            float value = max(resttiefe -1, alpha, beta);
            if (value < beta) 
                beta = value;
            del_token(x);
            if (beta <= alpha)
                return alpha;
        }
    }
    return beta;
}

float min_pos()
{
    if (check() != -1)
        return check();
    float alpha = -999;
    float beta = 999;
    for (short x=0; x<7; x++)
    {
        if (game.board[0][x] == 0)
        {
            add_token(x, 2);
            float value = max(8, alpha, beta);
            if (value < beta)
            {
                beta = value;
                token.info = beta;
                token.x = x;
            }
            del_token(x);
        }
    }
    return beta;
}


// init()
void init() 
{
    set_screen_mode(ScreenMode::lores);
    gameboard = Surface::load(asset_board);

    winner.init(tween_sine, 0, 200, 500, -1);
    winner.start();

    start();
}

// render(time)
void render(uint32_t time) 
{
    screen.pen = Pen(35, 35, 90);
    screen.clear();

    if (game.status == 1 || game.status == 2 || game.status == 3)
    {
        token.color == 1? screen.pen = Pen(255, 255, 0): screen.pen = Pen(255, 0, 0);
        screen.circle(Point(SCREEN_WIDTH / 2 - 49 + (token.x * 16), 8 + token.y), 7); 
    }
   
    for (short y=0; y<6; y++)
    {
        for (short x=0; x<7; x++)
        {
            if (game.board[y][x] > 0)
            {
                if (game.board[y][x] == 1)
                    screen.pen = Pen(255, 255, 0);
                else if (game.board[y][x] == 2)
                    screen.pen = Pen(255, 0, 0);
                screen.circle(Point(SCREEN_WIDTH / 2 - 49 + (x * 16), 28 + (y * 16)), 7); 
            }
        }
    }

    if (game.status == 4) // Winning tokens
    {
        short direction[8]{1,1,0,1,0,1,1,-1};
        short x = token.x;
        short y = token.y;

        screen.alpha = winner.value;
        screen.pen = Pen(35, 35, 90);  
        for (short i=0; i<4; i++)
        {
            screen.circle(Point(SCREEN_WIDTH / 2 - 49 + (x * 16), 28 + (y * 16)), 7); 
            x+=direction[token.dir];
            y+=direction[token.dir + 4];
        }
        screen.alpha = 255;
    }

    screen.blit(gameboard, Rect(0, 0, 113, 97), Point(SCREEN_WIDTH / 2 - 57, 20)); // board

    if (game.status == 4 || game.status == 5)
    {
        std::string winner = "no winner";
        if (game.status == 4)
            token.color == 1? winner = "yellow win": winner = "red win";
        screen.pen = Pen(255, 255, 255);
        screen.text(winner, minimal_font, Point(SCREEN_WIDTH / 2, 12), true, TextAlign::center_center);
        screen.text("press x", minimal_font, Point(SCREEN_WIDTH / 2, 109), true, TextAlign::center_center);
    }
}

// update(time)
void update(uint32_t time) 
{
    if (game.status == 1) // Select Player Colum
    {
        if (buttons.pressed & Button::DPAD_RIGHT && token.x < 6)
            token.x++;
        else if (buttons.pressed & Button::DPAD_LEFT && token.x > 0)
            token.x--;
        else if (buttons.pressed & Button::A && game.board[0][token.x] == 0)
            game.status = 3;
    }
    else if (game.status == 2) // Select CPU Colum
    {
        min_pos();
        game.status = 3;
    }
    else if (game.status == 3) // Token fall
    {
        token.y++;
        int y0 = (token.y - 22) / 16;
        if (y0 >= 5 || game.board[y0 + 1][token.x] > 0)
        {
            game.board[y0][token.x] = token.color;

            if (check_win() < 0)
            {
                if (token.color == 1)
                {
                    token.color = 2;
                    token.x = 6;
                    token.y = 0;
                    game.status = 2;
                }
                else
                {
                    token.color = 1;
                    token.x = 0;
                    token.y = 0;
                    game.status = 1;
                }
            }
        }
    }
    else if (game.status == 4 || game.status == 5)
    {
        if (buttons.pressed & Button::X)
            start();
    }
}
