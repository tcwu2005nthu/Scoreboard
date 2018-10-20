/*
    Responsible for managing the display itself
*/

#include "globals.h"
#include "constants.h"
#include "displayController.h"

displayController::displayController() {
}

/**
 *  Displays score include sets and servers if enabled
 *  @params score1 - score for the team 1
 *          score2 - score for the team 2
 *          set1   - number of sets won by first team (if enabled)
 *          set2   - number of sets won by second team (if enabled) 
 *          player - player on service, supports value from 1- 4 (if enabled)
 **/
void displayController::score(uint8_t score1, uint8_t score2, uint8_t set1, uint8_t set2, uint8_t player) {
    
    showScore(score1, score2);
    showWhoServe(player);
    showSets(set1, set2);

}

void displayController::showScore(uint8_t score1, uint8_t score2) {

    // We are going from left (team1) to right (team2) 
    // so the first index of the array will be the very left segment on the scoreboard
    // the the last index will be the very right segment on the scoreboard
    //   __   __    __   __
    //  |__| |__|  |__| |__|
    //  |__| |__|  |__| |__|
    //   [0] [1]   [2]  [3]
    //  
    // All segments off by default
    uint8_t digits[4] = {digitOne[10], digitOne[10], digitOne[10], digitOne[10]};
        
    uint8_t remind = 0;
    uint8_t digit = 0;
    if (score1 > 9) {        
        remind = score1/10;
        digit = score1 % 10;
        digits[0] = digitOne[digit];
        digits[1] = digitOne[remind];
    }
    else {
        // the second segment from left will be set to the represent number from 0-9
        digits[1] = digitOne[score1];
    }

    if (score2 > 9) {
        remind = score2/10;
        digit = score2 % 10;
        digits[2] = digitOne[digit];
        digits[3] = digitOne[remind];
    }
    else {
        // the from segment from left will be set to the represent number from 0-9
        digits[3] = digitOne[score2];
    }

    data.firstBigSegment = digits[0];
    data.secondBigSegment = digits[1];
    data.thirdBigSegment = digits[2];
    data.fourthdBigSegment = digits[3];

    // Write the bytes to the score output
    digitalWrite(SCORE_LATCH_PIN, LOW);
    for (uint8_t i = 0; i < 4; i++) {
         shiftOut(SCORE_DATA_PIN, SCORE_CLK_PIN, LSBFIRST,digits[i]);
    }        
    digitalWrite(SCORE_LATCH_PIN, HIGH);
}

/**
 * Blinks the whole display whatever was supplied as last information
 **/
void displayController::blink() {

    // It assumes the information is already displayed 
    // turn everything off and then display again for 3 times
    for(uint8_t i = 0; i < 4; i++) {

        // turn everything off
        digitalWrite(SCORE_LATCH_PIN, LOW);
        digitalWrite(PLAYER_LATCH_PIN, LOW);
        digitalWrite(SETS_LATCH_PIN, LOW);
        
        for (uint8_t i = 0; i < 4; i++) {
            shiftOut(SCORE_DATA_PIN, SCORE_CLK_PIN, LSBFIRST, SEGMENT_OFF);
        }        
        
        shiftOut(PLAYER_DATA_PIN, PLAYER_CLK_PIN, LSBFIRST, SEGMENT_OFF);
        shiftOut(SETS_DATA_PIN, SETS_CLK_PIN, LSBFIRST, SEGMENT_OFF);
        shiftOut(SETS_DATA_PIN, SETS_CLK_PIN, LSBFIRST, SEGMENT_OFF);

        digitalWrite(SCORE_LATCH_PIN, HIGH);
        digitalWrite(PLAYER_LATCH_PIN, HIGH);
        digitalWrite(SETS_LATCH_PIN, HIGH);
        delay(300);

        // and now everything on
        digitalWrite(SCORE_LATCH_PIN, LOW);
        digitalWrite(PLAYER_LATCH_PIN, LOW);
        digitalWrite(SETS_LATCH_PIN, LOW);
                
        shiftOut(SCORE_DATA_PIN, SCORE_CLK_PIN, LSBFIRST, data.firstBigSegment);
        shiftOut(SCORE_DATA_PIN, SCORE_CLK_PIN, LSBFIRST, data.secondBigSegment);
        shiftOut(SCORE_DATA_PIN, SCORE_CLK_PIN, LSBFIRST, data.thirdBigSegment);
        shiftOut(SCORE_DATA_PIN, SCORE_CLK_PIN, LSBFIRST, data.fourthdBigSegment);
        shiftOut(PLAYER_DATA_PIN, PLAYER_CLK_PIN, LSBFIRST, data.flags);
        shiftOut(SETS_DATA_PIN, SETS_CLK_PIN, LSBFIRST, data.firstSmallSegment);
        shiftOut(SETS_DATA_PIN, SETS_CLK_PIN, LSBFIRST, data.secondSmallSegment);

        digitalWrite(SCORE_LATCH_PIN, HIGH);
        digitalWrite(PLAYER_LATCH_PIN, HIGH);
        digitalWrite(SETS_LATCH_PIN, HIGH);
        if (i < 3) delay(700); // wait just first two cycles, last one no need to wait
    }
}

void displayController::blinkScore() {
    // It assumes the information is already displayed 
    // turn everything off and then display again for 3 times
    for(uint8_t i = 0; i < 4; i++) {

        // turn everything off
        digitalWrite(SCORE_LATCH_PIN, LOW);
        for (uint8_t i = 0; i < 4; i++) {
            shiftOut(SCORE_DATA_PIN, SCORE_CLK_PIN, LSBFIRST, SEGMENT_OFF);
        }        
        digitalWrite(SCORE_LATCH_PIN, HIGH);        
        delay(300);

        // and now everything on
        digitalWrite(SCORE_LATCH_PIN, LOW);
        shiftOut(SCORE_DATA_PIN, SCORE_CLK_PIN, LSBFIRST, data.firstBigSegment);
        shiftOut(SCORE_DATA_PIN, SCORE_CLK_PIN, LSBFIRST, data.secondBigSegment);
        shiftOut(SCORE_DATA_PIN, SCORE_CLK_PIN, LSBFIRST, data.thirdBigSegment);
        shiftOut(SCORE_DATA_PIN, SCORE_CLK_PIN, LSBFIRST, data.fourthdBigSegment);
        digitalWrite(SCORE_LATCH_PIN, HIGH);        
        if (i < 3) delay(700); // wait just first two cycles, last one no need to wait
    }
}

void displayController::showWhoServe(uint8_t player) {
    // Checks if displaying who should serve is enabled
    if (enableServers) {
        if (player >= 1 && player <= 4 ) {
            // here we don't use 7-segment display, just move the the voltage to correct pins
            if (player == 3) player = 4; //0b00100000 we use LSBFIRST so going from left
            if (player == 4) player = 8; //0b00010000 we use LSBFIRST so going from left
            data.flags = player;
            digitalWrite(PLAYER_LATCH_PIN, LOW);
            shiftOut(PLAYER_DATA_PIN, PLAYER_CLK_PIN, LSBFIRST, player);
            digitalWrite(PLAYER_LATCH_PIN, HIGH);
        }    
    }
}

void displayController::showSets(uint8_t set1, uint8_t set2) {
    // Checks if displaying sets is enabled
    if (enableSets) {

        data.firstSmallSegment = set1;
        data.secondSmallSegment = set2;

        digitalWrite(SETS_LATCH_PIN, LOW);
        shiftOut(SETS_DATA_PIN, SETS_CLK_PIN, LSBFIRST, data.firstSmallSegment);
        shiftOut(SETS_DATA_PIN, SETS_CLK_PIN, LSBFIRST, data.secondSmallSegment);
        digitalWrite(SETS_LATCH_PIN, HIGH);
    }
}

/**
 * Displays greetings on the display and blinks with it
 **/
void displayController::greetings() {

    data.firstBigSegment = AHOJ[0];
    data.secondBigSegment = AHOJ[1];
    data.thirdBigSegment = AHOJ[2];
    data.fourthdBigSegment = AHOJ[3];

    for(uint8_t i = 0; i < 4; i++) {
        digitalWrite(SCORE_LATCH_PIN, LOW);
        for (uint8_t i = 0; i < 4; i++) {
            shiftOut(SCORE_DATA_PIN, SCORE_CLK_PIN, LSBFIRST, AHOJ[i]);
        }        
        digitalWrite(SCORE_LATCH_PIN, HIGH);       
    }
    delay(700);
    blink();
}