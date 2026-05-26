// ============================================================
// PROJECT: SMART OCCUPANCY MONITORING SYSTEM (FINAL VERSION)
// Hardware: UCA Education Board (868 MHz) + 2x PIR Sensors
// Authors: Mecherouh Mohamed Ilyes & Boulkra Ahmed
// Université Côte d'Azur - Campus Valrose
// ============================================================

#include <lorawan.h>

// 1. HARDWARE PIN CONFIGURATIONS
const int pir1Power = A2;
const int pir1Pin   = A3; // PIR1: Extérieur (Outside)
const int pir2Power = A0;
const int pir2Pin   = A1; // PIR2: Intérieur (Inside)

// 2. STATE TRACKING VARIABLES
int lastState1 = LOW;
int lastState2 = LOW;

bool pir1Triggered = false;
bool pir2Triggered = false;
unsigned long lastPIR1Time = 0;
unsigned long lastPIR2Time = 0;

// 3. OPTIMIZED TIMING FOR SIDE-BY-SIDE SENSORS
// Crucial: Narrowed down to 800ms because the physical distance is tiny!
const unsigned long SEQUENCE_WINDOW = 800;  
const unsigned long SYSTEM_COOLDOWN = 2000; // 2-second blinding window to allow physical PIR reset
unsigned long globalCooldownTimer  = 0;

int peopleCount = 0;
const int ROOM_ID = 101; // Your unique Room/Module ID

// 4. LORAWAN TTN V3 CREDENTIALS (OTAA MODE)
// 🔑 Your customized generated keys from The Things Network V3 Console
const char *devEui = "70B3D57ED0077B70"; 
const char *appEui = "0000000000000000"; 
const char *appKey = "B985AF0E8669C2D07C51988C66D2D562";

// 5. REGULATORY LORA TRANSMISSION CONTROL
// Limits radio updates to a safe window to avoid violating European Duty Cycles
const unsigned long TX_INTERVAL = 10000; 
unsigned long lastTxTime = 0;
bool pendingUplink = false;

void setup() {
  Serial.begin(115200);

  // Initialize PIR1 Hardware
  pinMode(pir1Power, OUTPUT);
  digitalWrite(pir1Power, HIGH);
  pinMode(pir1Pin, INPUT);

  // Initialize PIR2 Hardware
  pinMode(pir2Power, OUTPUT);
  digitalWrite(pir2Power, HIGH);
  pinMode(pir2Pin, INPUT);

  Serial.println(F("============================================="));
  Serial.println(F("      UCA SMART OCCUPANCY SYSTEM READY       "));
  Serial.println(F("============================================="));
  
  // Warmup Period: Crucial for PIR components to stabilize internal reference voltages
  Serial.println(F("[INFO] Stabilizing PIR pyroelectric sensors... Stand clear."));
  delay(15000); 

  // Initialize UCA Integrated LoRa Transceiver
  if (!LoraWANDevice.begin(EU868)) {
    Serial.println(F("[ERROR] Hardware Fault: LoRa radio initialization failed!"));
    while (1); // Halt execution if hardware is missing
  }
  
  // Feed parameters to the LoRaWAN Stack
  LoraWANDevice.setDevEUI(devEui);
  LoraWANDevice.setAppEUI(appEui);
  LoraWANDevice.setAppKey(appKey);
  
  Serial.println(F("[LORA] Handshaking... Joining TTN V3 Network via OTAA..."));
  while (!LoraWANDevice.join()) {
    Serial.println(F("[LORA] Gateway rejected or missed handshake. Retrying in 6s..."));
    delay(6000);
  }
  Serial.println(F("[LORA] Handshake Complete! Node successfully joined TTN V3."));
  
  Serial.print(F("Current Room Baseline Occupancy: "));
  Serial.println(peopleCount);
  Serial.println(F("---------------------------------------------"));
}

void loop() {
  // Keep the underlying LoRaWAN engine, background cycles, and downlinks alive
  LoraWANDevice.loop();

  int currentState1 = digitalRead(pir1Pin);
  int currentState2 = digitalRead(pir2Pin);
  unsigned long now = millis();

  // Step A: Auto-clear expired sequences if a person turns back halfway through the doorway
  if (pir1Triggered && (now - lastPIR1Time > SEQUENCE_WINDOW)) pir1Triggered = false;
  if (pir2Triggered && (now - lastPIR2Time > SEQUENCE_WINDOW)) pir2Triggered = false;

  // Step B: Evaluate inputs only if we aren't in the global blind cooldown window
  if (now - globalCooldownTimer > SYSTEM_COOLDOWN) {

    // ==================== PIR1 DETECTION (OUTSIDE) ====================
    if (currentState1 == HIGH && lastState1 == LOW) {
      lastState1 = HIGH;
      if (pir2Triggered) { 
        // Logic Pathway: PIR2 (Inside) -> PIR1 (Outside) = EXIT
        peopleCount = max(0, peopleCount - 1); // Floor lock at 0
        registerMovementEvent(now, "SORTIE (EXIT)");
      } else if (!pir1Triggered) {
        // First step of entering
        pir1Triggered = true;
        lastPIR1Time = now;
      }
    } 
    else if (currentState1 == LOW && lastState1 == HIGH) {
      lastState1 = LOW;
    }

    // ==================== PIR2 DETECTION (INSIDE) ====================
    if (currentState2 == HIGH && lastState2 == LOW) {
      lastState2 = HIGH;
      if (pir1Triggered) { 
        // Logic Pathway: PIR1 (Outside) -> PIR2 (Inside) = ENTRY
        peopleCount++;
        registerMovementEvent(now, "ENTREE (ENTRY)");
      } else if (!pir2Triggered) {
        // First step of leaving
        pir2Triggered = true;
        lastPIR2Time = now;
      }
    } 
    else if (currentState2 == LOW && lastState2 == HIGH) {
      lastState2 = LOW;
    }
  }

  // Step C: Deferred Uplink Handler (Adheres safely to European 1% Duty-Cycle Laws)
  if (pendingUplink && (now - lastTxTime >= TX_INTERVAL)) {
    sendDataPacket();
    lastTxTime = now;
    pendingUplink = false; // Reset request flag
  }
}

// 6. HELPER FUNCTIONS
void registerMovementEvent(unsigned long timestamp, const char* eventString) {
  Serial.print(F("[EVENT] "));
  Serial.print(eventString);
  Serial.print(F(" | Real-time Occupancy Total: "));
  Serial.println(peopleCount);
  
  // Clear directional tracking locks
  pir1Triggered = false;
  pir2Triggered = false;
  
  // Engage the blinding window timer to mask physical PIR trailing signals
  globalCooldownTimer = timestamp; 
  
  // Request a network broadcast
  pendingUplink = true; 
}

void sendDataPacket() {
  // LoRaWAN payloads must be packed raw bytes. 
  // We use 3 bytes to match the presentation specifications: [Room ID, Count, Type Flag]
  uint8_t uplinkBuffer[3];
  uplinkBuffer[0] = (uint8_t)ROOM_ID;       // Identifies your classroom
  uplinkBuffer[1] = (uint8_t)peopleCount;   // The tracking counter
  uplinkBuffer[2] = 0x0A;                   // Event status flag (0x0A = Counter update)

  Serial.println(F("[LORA] Pushing fresh data bytes onto TTN V3 network..."));
  
  // Parameters: send(buffer_size, buffer_ptr, port_num, request_acknowledgement)
  if (LoraWANDevice.send(sizeof(uplinkBuffer), uplinkBuffer, 1, false)) {
    Serial.println(F("[SUCCESS] LoRa packet successfully queued for transmission."));
  } else {
    Serial.println(F("[WARNING] Transmit skipped: Radio system busy or duty cycle limit reached."));
  }
}
