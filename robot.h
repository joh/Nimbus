// Battery
#define BAT_PIN 0
#define BAT_FULL 865    // 4.2V per cell
#define BAT_LOW 660     // 3.2V per cell

// Motors
#define RIGHT_PWM 3
#define RIGHT_DIR 12
#define LEFT_PWM 11
#define LEFT_DIR 13

#define FORWARD HIGH
#define BACKWARD LOW

#define SPEED_L 255
#define SPEED_R 255

// Sharp IR
#define SHARP_FRONT_PIN 2
#define SHARP_LEFT_PIN  3
#define SHARP_RIGHT_PIN 4

#define SHARP_SAMPLES 10
#define SHARP_MIN 0
#define SHARP_MAX 700
#define SHARP_THRESH 150
#define SHARP_FOLLOW 400

// Status LED
#define LED_RED_PIN   5
#define LED_GREEN_PIN 6
#define LED_BLUE_PIN  9

// Parameters
#define LOST_TIMEOUT 5000   // milliseconds
#define BUFSIZE 128
#define CMD_MAX_ARGS 8

// Mode of operation
typedef enum {
    SEARCH,
    FOLLOW,
    FOLLOW_L,
    FOLLOW_R,
    LOST,
    LOST_L,
    LOST_R
} State;

typedef enum {
    NONE = 0,
    FRONT,
    BACK,
    LEFT,
    RIGHT
} Side;

