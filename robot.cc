/**
 * Nimbus robot - XBee controlled
 * 
 * Accepts input commands in the form:
 *   
 *   CMD\n
 *   CMD ARG1 ARG2 ...\n
 *
 * Available commands:
 * 
 *   STOP
 *   SPEED L R
 *   COLOR R G B
 */

#include <WProgram.h>

#include "util.h"
#include "robot.h"

int i;
int speed;
int turn = 0;
int back = 0;

// Battery level
int bat;

// Current wheel speeds
int speed_l, speed_r;

// Current color
int color_r, color_g, color_b;

// Sensor readings
int sharp_front, sharp_left, sharp_right;

// Last dominant reading
Side sharp_last = NONE;

// Robot state
State state = SEARCH;

// Timer keeping track of how long we've been in LOST state
unsigned long lost_time;

// Input buffer
char buf[BUFSIZE];
char c;
unsigned int buf_pos = 0;

/**
 * Set wheel speeds
 */
void setSpeed(int left, int right) {
    digitalWrite(LEFT_DIR, (left < 0) ? BACKWARD : FORWARD);
    digitalWrite(RIGHT_DIR, (right < 0) ? BACKWARD : FORWARD);
    
    analogWrite(LEFT_PWM, (unsigned) abs(left));
    analogWrite(RIGHT_PWM, (unsigned) abs(right));
}

/**
 * Set LED color
 */
void setColor(int r, int g, int b) {
    analogWrite(LED_RED_PIN, r);
    analogWrite(LED_GREEN_PIN, g);
    analogWrite(LED_BLUE_PIN, b);
}

/**
 * Serial info
 *
 * Output has the following form:
 *
 * SHARP_FRONT <int>; SHARP_LEFT <int>; SHARP_RIGHT <int>; BAT <int>; \
 * SPEED <int> <int>; COLOR <uint> <uint> <uint>; \n
 */
void state_send()
{
    unsigned long t;
    
    // Sharp sensors
    Serial.print("SHARP_FRONT ");
    Serial.print(sharp_front);
    Serial.print("; ");
    
    Serial.print("SHARP_LEFT ");
    Serial.print(sharp_left);
    Serial.print("; ");
    
    Serial.print("SHARP_RIGHT ");
    Serial.print(sharp_right);
    Serial.print("; ");
    
    // Battery
    t = (100 * (bat - BAT_LOW)) / (BAT_FULL - BAT_LOW);
    
    Serial.print("BAT ");
    Serial.print(t);
    Serial.print("; ");
    
    // Speed
    Serial.print("SPEED ");
    Serial.print(speed_l);
    Serial.print(" ");
    Serial.print(speed_r);
    Serial.print("; ");
    
    // Color
    Serial.print("COLOR ");
    Serial.print(color_r);
    Serial.print(" ");
    Serial.print(color_g);
    Serial.print(" ");
    Serial.print(color_b);
    Serial.print("; ");
    
    // End of info
    Serial.print("\n");
}

/**
 * Check voltage of battery
 */
void battery_check() {
    bat = analogRead(BAT_PIN);
    
    if (bat <= BAT_LOW) {
        // Low battery, stop motors and signal with LED
        setSpeed(0, 0);
        
        while (1) {
            Serial.println("LOW BAT!");
            setColor(255, 0, 0);
            delay(500);
            setColor(0, 0, 0);
            delay(500);
        }
    }
}

/**
 * Parse command
 * 
 * Accepts input commands in the form:
 *   
 *   CMD\n
 *   CMD ARG1 ARG2 ...\n
 *
 * Available commands:
 * 
 *   STOP
 *   SPEED L R
 *   COLOR R G B
 *   STATUS
 */
void command_parse()
{
    char *cmd, *str, *args[CMD_MAX_ARGS];
    unsigned int i, n_args = 0;
    
    //Serial.print("Got command string: ");
    //Serial.println(buf);
    
    cmd = strtok(buf, " ");
    //Serial.print("Command: ");
    //Serial.println(cmd);
    
    for (i = 0; i < CMD_MAX_ARGS && NULL != (str = strtok(NULL, " ")); i++) {
        args[i] = str;
    }
    
    n_args = i;
    /*
    Serial.print("Arguments: ");
    Serial.println(n_args);
    
    for (i = 0; i < n_args; i++) {
        Serial.print("  ");
        Serial.println(args[i]);
    }
    */
    if (strcasecmp(cmd, "STOP") == 0) {
        speed_l = 0;
        speed_r = 0;
        
        //setSpeed(speed_l, speed_r);
    }
    
    if (strcasecmp(cmd, "SPEED") == 0) {
        if (n_args < 2) {
            Serial.println("Usage: SPEED L R");
            return;
        }
        
        speed_l = atoi(args[0]);
        speed_r = atoi(args[1]);
        
        //setSpeed(speed_l, speed_r);
    }
    
    if (strcasecmp(cmd, "COLOR") == 0) {
        if (n_args < 3) {
            Serial.println("Usage: COLOR R G B");
            return;
        }
        
        color_r = atoi(args[0]);
        color_g = atoi(args[1]);
        color_b = atoi(args[2]);
        
        //setColor(color_r, color_g, color_b);
    }
}

/**
 * Read command
 */
void command_read()
{
    if (Serial.available() > 0) {
        // Read the incoming byte
        buf[buf_pos] = Serial.read();
        
        if (buf[buf_pos] == '\n') {
            // End of command
            buf[buf_pos] = '\0';
            buf_pos = 0;
            
            // Parse it
            command_parse();
            
        } else {
            buf_pos++;
        }
	}
}


void setup() {
    Serial.begin(9600);
    
    pinMode(RIGHT_PWM, OUTPUT);  //Set control pins to be outputs
    pinMode(RIGHT_DIR, OUTPUT);
    pinMode(LEFT_PWM, OUTPUT);
    pinMode(LEFT_DIR, OUTPUT);
    
    pinMode(LED_RED_PIN, OUTPUT);
    pinMode(LED_GREEN_PIN, OUTPUT);
    pinMode(LED_BLUE_PIN, OUTPUT);
    
    // Stop
    speed_l = 0;
    speed_r = 0;
    setSpeed(0, 0);
    
    // Initialize state
    state = SEARCH;
}

void loop() {
    // Check battery
    battery_check();
    
    // Read sensors
    sharp_front = analogReadMedian(SHARP_FRONT_PIN, SHARP_SAMPLES);
    sharp_left = analogReadMedian(SHARP_LEFT_PIN, SHARP_SAMPLES);
    sharp_right = analogReadMedian(SHARP_RIGHT_PIN, SHARP_SAMPLES);
    
    // Read incoming command
    command_read();
    
    // Set speed
    setSpeed(speed_l, speed_r);
    
    // Set color
    setColor(color_r, color_g, color_b);
    
    // Send state
    //state_send();
    
    //delay(10);
}

