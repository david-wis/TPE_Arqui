#include <stdint.h>
#include <syscalls.h>
#include <stdio.h>
#include <color.h>

#define ESC 0x01
#define SPACEBAR 0x39
#define KEY_W 0x11
#define KEY_A 0x1E
#define KEY_S 0x1F
#define KEY_D 0x20
#define KEY_Z 0x2C
#define KEY_ENTER 0x1C
#define KEY_UP 0x48
#define KEY_LEFT 0x4B
#define KEY_DOWN 0x50
#define KEY_RIGHT 0x4D
#define KEY_SLASH 0x35

#define PLAYER_SIZE 16
#define DELTA_TICK 1
#define SPEED 8
#define TURBO_TICKS 2
#define TURBO_COOLDOWN 20 

#define SCREEN_WIDTH 1024 

#define FIELD_WIDTH (1024/PLAYER_SIZE) 
#define FIELD_HEIGHT (768/PLAYER_SIZE)
#define FIELD_WIDTH_POSITIONS (FIELD_WIDTH/8 + 1) 

typedef struct {
    uint16_t x, y;              // Posiciones de la matriz que corresponden al jugador
    uint16_t vx, vy;            // Velocidad del jugador
    uint16_t oldVx, oldVy;            // Velocidad del jugador
    Color color, trailColor;    // Color del jugador y del rastro que deja
    uint64_t lastTurbo;         // Ticks en el turbo anterior
    uint8_t wins;
} Player;

static void loop();
static int checkCollision(Player * p);
static int update(Player * p);
static void initializeField();
static void setDirections(Player * p, uint8_t key, uint8_t up, uint8_t left, uint8_t down, uint8_t right, uint8_t turbo);
static void draw(Player * p, int * collided);
static void loopzen();
static void debug();
static void correctVelocity(Player * p);
static void endMusic();

Player _p1, _p2;
uint8_t _playing;
uint8_t _qtyPlayers;
//uint16_t _width, _height;
uint8_t _field[FIELD_WIDTH_POSITIONS][FIELD_HEIGHT]; // Optimizacion de espacio en bits

void startTron(int qtyPlayers) {
    //uint32_t res = getScreenResolution();
    //_width = res & 0x0000FFFF;
    //_height = res >> 16; 
    if (qtyPlayers < 1 || qtyPlayers > 2) {
        puts("Actualmente el juego solo soporta el modo individual o el de 2 jugadores, vuelva pronto!\n");
        return;
    }
    _qtyPlayers = qtyPlayers;
    char userInput;
    clear();
    setFontColor(0, 255, 0);
    printf("Bienvenido a %s\n", qtyPlayers == 2? "Tron" : "Tron-Zen");
    setFontColor(255, 255, 255);
    do {
        puts("El jugador 1 se mueve con W A S D y activa turbo con Z \n"
              "El jugador 2 se mueve con las flechas y activa turbo con \\\n");
        puts("Presione espacio para comenzar, y ESC para salir en el juego\n");
        initializeField();
        while (getScanCode() != SPACEBAR)
            ;
        clear();
        drawRect(0, 0, SCREEN_WIDTH, FIELD_HEIGHT - 15, (Color){173, 169, 170});
        setFontColor(0, 0, 0);
        if(qtyPlayers == 2){
            printf("                             %d : %d\n", _p1.wins, _p2.wins);
        }
        else
            printf("%d", _p1.wins);
        setFontColor(255, 255, 255);
        _playing = 1;
        uint64_t initialTicks = getTicks();
        _p1 = (Player){FIELD_WIDTH_POSITIONS * 8 * 0.2, FIELD_HEIGHT / 2, 1, 0, 1, 0, (Color){100, 255, 100}, (Color){100, 150, 100}, 0, _p1.wins};
        _p2 = (Player){FIELD_WIDTH_POSITIONS * 8 * 0.7, FIELD_HEIGHT / 2, -1, 0, -1, 0, (Color){255, 153, 184}, (Color){255, 0, 184}, 0, _p2.wins};
        uint64_t ticks = getTicks();
        uint64_t nextTicks;
        while (_playing)
        {
            //debug();
            nextTicks = getTicks();
            uint8_t key = getScanCode();
            if (key == ESC)
            {
                clear();
                puts("Fin del juego\n");
                return;
            }
            setDirections(&_p1, key, KEY_W, KEY_A, KEY_S, KEY_D, KEY_Z);
            setDirections((qtyPlayers == 2) ? &_p2 : &_p1, key, KEY_UP, KEY_LEFT, KEY_DOWN, KEY_RIGHT, KEY_SLASH);
            if (nextTicks - ticks > DELTA_TICK)
            {
                ticks = nextTicks;
                if (qtyPlayers == 2)
                    loop(_field);
                else
                    loopzen(_field);
            }

        }
        endMusic();
        puts("Fin del Juego hm?!!!!\n");
        printf("Desea volver a jugar? (espacio para confirmar, esc para salir)");
        while ((userInput = getScanCode()) != SPACEBAR && userInput != ESC)
            ;
        clear();
    } while (userInput == SPACEBAR);
}

static void endMusic() {
    playSound(100);
    for (int i = 0; i < 10000000; i++)
        for (int i = 0; i < 10; i++)
            ;
    stopSound();
    playSound(100);
    for (int i = 0; i < 10000000; i++)
        for (int i = 0; i < 10; i++)
            ;
    stopSound();
    playSound(130);
    for (int i = 0; i < 10000000; i++)
        for (int i = 0; i < 10; i++)
            ;
    stopSound();
    playSound(150);
    for (int i = 0; i < 10000000; i++)
        for (int i = 0; i < 20; i++)
            ;
    stopSound();
    playSound(130);
    for (int i = 0; i < 10000000; i++)
        for (int i = 0; i < 10; i++)
            ;
    stopSound();
    playSound(150);
    for (int i = 0; i < 10000000; i++)
        for (int i = 0; i < 20; i++)
            ;
    stopSound();
}

static void debug() {
    for (int i = 0; i < FIELD_WIDTH; i++)
        for (int j = 0; j < FIELD_HEIGHT; j++)
            if (_field[i / 8][j] & (1 << (i % 8)))
                drawRect(i * PLAYER_SIZE, j * PLAYER_SIZE, PLAYER_SIZE, PLAYER_SIZE, (Color){0, 0, 255});
}


static void initializeField() {
    
    for (int i = 0; i < FIELD_WIDTH_POSITIONS; i++)
        for (int j = 1; j < FIELD_HEIGHT-1; j++)
            _field[i][j] = 0x00;
    
    for(int i = 0; i < FIELD_WIDTH_POSITIONS; i++){
        _field[i][2] |= 0xFF;
        _field[i][FIELD_HEIGHT-1] |= 0xFF;
    }

    for(int i = 1; i < FIELD_HEIGHT - 1; i++){
        _field[0][i] |= 0x01;
        _field[FIELD_WIDTH_POSITIONS-2][i] |= 0x80; // Para que quede mejor en pantalla
    }
}


static void setDirections(Player * p, uint8_t key, uint8_t up, uint8_t left, uint8_t down, uint8_t right, uint8_t turbo) {
    uint64_t ticks = getTicks();
    if (key == up && p->oldVy == 0) {
        p->vx = 0;     
        p->vy = -1;
    } else if (key == left && p->oldVx == 0) {
        p->vx = -1;
        p->vy = 0;
    } else if (key == down && p->oldVy == 0) {
        p->vx = 0;
        p->vy = 1;
    } else if (key == right && p->oldVx == 0) {
        p->vx = 1;
        p->vy = 0;
    } else if (key == turbo && ticks - p->lastTurbo - TURBO_TICKS >= TURBO_COOLDOWN) { // Si ya paso el cooldown y se toca se activa
        p->lastTurbo = ticks;
    } 
}

static void loop() {
    drawRect(_p1.x*PLAYER_SIZE, _p1.y*PLAYER_SIZE, PLAYER_SIZE, PLAYER_SIZE, _p1.trailColor);
    drawRect(_p2.x*PLAYER_SIZE, _p2.y*PLAYER_SIZE, PLAYER_SIZE, PLAYER_SIZE, _p2.trailColor);
    int col1 = update(&_p1);
    int col2 = update(&_p2);
    if (col1 && col2 || (_p1.x == _p2.x && _p1.y == _p2.y))
        puts("Empate!\n");
    else if (col1){
        puts("Gano el jugador 2!\n");
        _p2.wins += 1;
    }
    else if (col2){
        puts("Gano el jugador 1!\n");
        _p1.wins += 1;
    }
}

static void loopzen() {
    drawRect(_p1.x*PLAYER_SIZE, _p1.y*PLAYER_SIZE, PLAYER_SIZE, PLAYER_SIZE, _p1.trailColor);
    update(&_p1);
}

static int checkCollision(Player * p){
    char pos = _field[p->x/8][p->y];
    if((pos & (1 << (p->x % 8)))) {
        _playing = 0;
        return 1;
    }
    return 0;
}

// Evita que un jugador pueda suicidarse por apretar una tecla invalida
static void correctVelocity(Player * p) {
    if (p->oldVx == -p->vx)
        p->vx = p->oldVx;
    if (p->oldVy == -p->vy)
        p->vy = p->oldVy;
    p->oldVx = p->vx;
    p->oldVy = p->vy;
}

static int update(Player * p) {
    uint64_t ticks = getTicks();
    uint8_t realV = 1;
    int collided = 0;
    if (ticks - p->lastTurbo <= TURBO_TICKS)
        realV = SPEED;

    correctVelocity(p);
    for (int i = 0; i < realV && !collided; i++) {
        p->x += p->vx;
        p->y += p->vy;
        draw(p, &collided);
    }
    return collided;
}

static void draw(Player * p, int * collided) {
    *collided = checkCollision(p);
    _field[p->x / 8][p->y] |= (1 << (p->x % 8));
    drawRect(p->x * PLAYER_SIZE, p->y * PLAYER_SIZE, PLAYER_SIZE, PLAYER_SIZE, p->color);
}