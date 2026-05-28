// ============================================================
// PROJECT: SMART OCCUPANCY MONITORING SYSTEM (FINAL VERSION)
// Hardware: UCA Education Board (868 MHz) + 2x PIR Sensors
// Authors: Mecherouh Mohamed Ilyes & Boulkra Ahmed
// Université Côte d'Azur - Campus Valrose
// ============================================================

#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>

// ===== YOUR LoRaWAN CREDENTIALS =====
static const u4_t DEVADDR = 0x260B211F;

static const PROGMEM u1_t NWKSKEY[16] = {
    0x71, 0x06, 0x1B, 0x1E, 0x72, 0x15, 0x25, 0xB3,
    0x24, 0x88, 0xEB, 0xC1, 0x8D, 0x84, 0xC5, 0x2A
};

static const u1_t PROGMEM APPSKEY[16] = {
    0x4A, 0xC2, 0x92, 0x23, 0x4D, 0xAB, 0x15, 0x66,
    0x3E, 0xF4, 0x5B, 0xB1, 0x54, 0xC4, 0x54, 0x9B
};

// ===== PIR PINS =====
const int pir1Power = A2;
const int pir1Pin = A3;
const int pir2Power = A0;
const int pir2Pin = A1;

// ===== PIR VARIABLES =====
int lastState1 = LOW;
int lastState2 = LOW;
unsigned long lastChange1 = 0;
unsigned long lastChange2 = 0;
unsigned long lastDirectionTime = 0;
int peopleCount = 0;

// TIMINGS (more forgiving)
const unsigned long DEBOUNCE = 80;        // 80ms debounce
const unsigned long SEQUENCE_WINDOW = 3000; // 3 seconds to complete
const unsigned long COOLDOWN = 1000;       // 1 second cooldown

// ===== LoRa =====
static osjob_t sendjob;
char payloadBuffer[32];
const unsigned TX_INTERVAL = 30;

const lmic_pinmap lmic_pins = {
    .nss = 10,
    .rxtx = LMIC_UNUSED_PIN,
    .rst = 8,
    .dio = {6, 6, 6},
};

void os_getArtEui(u1_t* buf) { }
void os_getDevEui(u1_t* buf) { }
void os_getDevKey(u1_t* buf) { }

void sendLoRa() {
    if (LMIC.opmode & OP_TXRXPEND) return;
    snprintf(payloadBuffer, sizeof(payloadBuffer), "101:%d", peopleCount);
    LMIC_setTxData2(1, (uint8_t*)payloadBuffer, strlen(payloadBuffer), 0);
    Serial.print(F("Sent: "));
    Serial.println(payloadBuffer);
}

void onEvent(ev_t ev) {
    if (ev == EV_TXCOMPLETE) {
        os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(TX_INTERVAL), sendLoRa);
    }
}

void setup() {
    Serial.begin(115200);
    
    pinMode(pir1Power, OUTPUT);
    digitalWrite(pir1Power, HIGH);
    pinMode(pir1Pin, INPUT);
    
    pinMode(pir2Power, OUTPUT);
    digitalWrite(pir2Power, HIGH);
    pinMode(pir2Pin, INPUT);
    
    Serial.println(F("=== SMART OCCUPANCY COUNTER ==="));
    delay(10000);
    
    os_init();
    LMIC_reset();
    
    uint8_t appskey[sizeof(APPSKEY)];
    uint8_t nwkskey[sizeof(NWKSKEY)];
    memcpy_P(appskey, APPSKEY, sizeof(APPSKEY));
    memcpy_P(nwkskey, NWKSKEY, sizeof(NWKSKEY));
    LMIC_setSession(0x1, DEVADDR, nwkskey, appskey);
    
    LMIC_setClockError(MAX_CLOCK_ERROR * 2 / 100);
    LMIC_setLinkCheckMode(0);
    LMIC_setDrTxpow(DR_SF9, 14);
    
    Serial.println(F("Ready! Count: 0"));
    sendLoRa();
}

void loop() {
    os_runloop_once();
    
    int s1 = digitalRead(pir1Pin);
    int s2 = digitalRead(pir2Pin);
    unsigned long now = millis();
    
    // Check if system is in cooldown
    bool inCooldown = (now - lastDirectionTime) < COOLDOWN;
    
    // ---- PIR1 (OUTSIDE) ----
    if (s1 == HIGH && lastState1 == LOW && !inCooldown) {
        lastChange1 = now;
        delay(DEBOUNCE);
        if (digitalRead(pir1Pin) == HIGH) {
            lastState1 = HIGH;
            
            // Check if PIR2 was triggered recently (EXIT)
            if ((now - lastChange2) < SEQUENCE_WINDOW && lastChange2 > 0) {
                if (peopleCount > 0) {
                    peopleCount--;
                    Serial.print(F("<<< EXIT  | Count: "));
                    Serial.println(peopleCount);
                    lastDirectionTime = now;
                    sendLoRa();
                }
                lastChange2 = 0;
            }
        }
    } 
    else if (s1 == LOW && lastState1 == HIGH) {
        lastState1 = LOW;
    }
    
    // ---- PIR2 (INSIDE) ----
    if (s2 == HIGH && lastState2 == LOW && !inCooldown) {
        lastChange2 = now;
        delay(DEBOUNCE);
        if (digitalRead(pir2Pin) == HIGH) {
            lastState2 = HIGH;
            
            // Check if PIR1 was triggered recently (ENTRY)
            if ((now - lastChange1) < SEQUENCE_WINDOW && lastChange1 > 0) {
                peopleCount++;
                Serial.print(F(">>> ENTRY | Count: "));
                Serial.println(peopleCount);
                lastDirectionTime = now;
                sendLoRa();
                lastChange1 = 0;
            }
        }
    } 
    else if (s2 == LOW && lastState2 == HIGH) {
        lastState2 = LOW;
    }
    
    // Auto-reset old triggers (cleanup)
    if (lastChange1 > 0 && (now - lastChange1) > SEQUENCE_WINDOW) {
        lastChange1 = 0;
    }
    if (lastChange2 > 0 && (now - lastChange2) > SEQUENCE_WINDOW) {
        lastChange2 = 0;
    }
}
