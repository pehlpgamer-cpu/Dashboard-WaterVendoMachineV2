#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <time.h>
#include <Preferences.h>
#include <string.h>

#if __has_include("secrets.h")
#include "secrets.h"
#else
#error "Copy secrets.example.h to secrets.h and fill WiFi/Firebase values before compiling."
#endif

#ifndef WIFI_ENABLED
#define WIFI_ENABLED true
#endif

// -------------------- LCD --------------------
LiquidCrystal_I2C lcd(0x27, 16, 2);

// -------------------- DEBUG --------------------
#define COIN_DEBUG true
#define LCD_DEBUG_PULSES false

// -------------------- VENDING UX --------------------
// false = one cup per selection, keeps remaining balance
// true  = auto-dispense as many cups as balance allows
#define AUTO_MULTI_CUP false

// -------------------- ESP32 30-PIN SAFE PINS --------------------
#define SDA_PIN 21
#define SCL_PIN 22

#define coinSlotPin 34   // GPIO34 needs external 10k pull-up to 3.3V
#define pumpPin 26
#define coolerPin 27

#define btnCold 18
#define btnRegular 19
#define resetBtn 23

// -------------------- CONSTANTS --------------------
const int maxCups = 50;

const int regularPrice = 3;
const int coldPrice = 5;

const unsigned long pumpDuration = 11500;
const unsigned long pumpTimeout = 12000;
const unsigned long cupGapDuration = 700;

const int coinPulseTimeout = 300;
const unsigned long coinRejectedDisplayTime = 1500;

// Offline/pending log queue size.
// When full, oldest log is overwritten.
#define MAX_OFFLINE_LOGS 50
const uint32_t offlineQueueVersion = 2;

// -------------------- WIFI / NTP SETTINGS --------------------
const unsigned long WIFI_CHECK_INTERVAL_MS = 2000;
const unsigned long WIFI_CONNECT_TIMEOUT_MS = 20000;
const unsigned long WIFI_RECONNECT_INTERVAL_MS = 15000;
const unsigned long WIFI_STATUS_PRINT_INTERVAL_MS = 5000;
const unsigned long NTP_RETRY_INTERVAL_MS = 30000;

// -------------------- FIREBASE FLUSH SETTINGS --------------------
// Important:
// Firebase upload is allowed only when the machine is idle.
// This prevents upload failures from blocking Cold/Regular selection.
const unsigned long IDLE_BEFORE_FLUSH_MS = 15000;              // Wait 15s after customer activity
const unsigned long OFFLINE_FLUSH_MIN_INTERVAL_MS = 60000;    // 1 minute
const unsigned long OFFLINE_FLUSH_MAX_INTERVAL_MS = 300000;   // 5 minutes

// -------------------- COIN RULES --------------------
struct CoinRule {
  const char *label;
  int minPulse;
  int maxPulse;
  int value;
};

CoinRule coinRules[] = {
  {"1 PESO", 1, 10, 1},
  {"5 PESO", 20, 35, 5}
};

const int coinRuleCount = sizeof(coinRules) / sizeof(coinRules[0]);

// -------------------- FSM --------------------
enum State {
  IDLE,
  ACCEPTING_COINS,
  DISPENSING,
  CUP_GAP,
  COIN_REJECTED,
  OUT_OF_WATER,
  ERROR_STATE
};

State currentState = ERROR_STATE;
State lastDisplayedState = ERROR_STATE;
State resumeAfterMessage = IDLE;

// -------------------- BUTTON --------------------
struct ButtonState {
  uint8_t pin;
  int last;
  int current;
  bool pressed;
};

ButtonState coldButton = {btnCold, HIGH, HIGH, false};
ButtonState regularButton = {btnRegular, HIGH, HIGH, false};
ButtonState resetButton = {resetBtn, HIGH, HIGH, false};

// -------------------- OFFLINE TRANSACTION --------------------
struct TransactionLog {
  int amount;
  bool coldSelected;
  bool timeSynced;
  uint32_t uptimeMs;
  uint64_t timestampMs;
};

TransactionLog offlineQueue[MAX_OFFLINE_LOGS];
int offlineCount = 0;

// -------------------- GLOBAL VARIABLES --------------------
Preferences prefs;
bool prefsReady = false;

volatile int pulseBuffer = 0;
volatile bool coinInputEnabled = false;

int coinPulseCount = 0;
unsigned long coinLastPulseTime = 0;
unsigned long coinDebugTimer = 0;

int currentCoins = 0;
int cupsToDispense = 0;
int cupsSold = 0;

uint32_t totalSales = 0;
uint32_t lostLogCount = 0;

bool isCold = false;
bool wifiConnected = false;
bool wifiConnectInProgress = false;
bool timeIsSynced = false;
bool flushPending = false;

unsigned long pumpStartTime = 0;
unsigned long cupGapStartTime = 0;
unsigned long lastButtonCheckTime = 0;
unsigned long lastWifiCheckTime = 0;
unsigned long messageStartTime = 0;

unsigned long wifiConnectStartTime = 0;
unsigned long lastWifiReconnectAttempt = 0;
unsigned long lastWifiStatusPrintTime = 0;
unsigned long lastNtpAttemptTime = 0;

unsigned long lastUserActivityTime = 0;
unsigned long lastOfflineFlushAttemptTime = 0;
int offlineFlushFailCount = 0;

int lastRejectedPulseCount = 0;
char errorMessage[17] = "System Error";

// LCD cache
int lastDisplayedCoins = -9999;
int lastDisplayedCupsToDispense = -9999;
int lastDisplayedCupsSold = -9999;
int lastDisplayedPulseCount = -9999;
int lastDisplayedRejectedPulse = -9999;
int lastDisplayedOfflineCount = -9999;

// -------------------- FUNCTION DECLARATIONS --------------------
void setState(State newState);
void resetCoinDetector();
void forceLCDRefresh();
void saveRuntimeState();
void persistOfflineQueue();
bool addOfflineTransaction(const TransactionLog &tx);
void flushOfflineQueue();
bool syncTime(unsigned long timeoutMs);
bool getEpochMillis(uint64_t &timestampMs);
void validatePayment();
void markUserActivity();
unsigned long getOfflineFlushInterval();

// -------------------- ISR --------------------
void IRAM_ATTR coinISR() {
  if (!coinInputEnabled) return;

  static unsigned long lastInterruptTime = 0;
  unsigned long now = millis();

  if (now - lastInterruptTime > 5) {
    pulseBuffer++;
  }

  lastInterruptTime = now;
}

// -------------------- STATE NAME --------------------
const char *stateName(State state) {
  switch (state) {
    case IDLE: return "IDLE";
    case ACCEPTING_COINS: return "ACCEPTING_COINS";
    case DISPENSING: return "DISPENSING";
    case CUP_GAP: return "CUP_GAP";
    case COIN_REJECTED: return "COIN_REJECTED";
    case OUT_OF_WATER: return "OUT_OF_WATER";
    case ERROR_STATE: return "ERROR_STATE";
    default: return "UNKNOWN";
  }
}

// -------------------- ACTIVITY TRACKER --------------------
void markUserActivity() {
  lastUserActivityTime = millis();
}

// -------------------- LCD HELPERS --------------------
void printLCDLine(int row, const char *text) {
  char buffer[17];

  strncpy(buffer, text, 16);
  buffer[16] = '\0';

  lcd.setCursor(0, row);
  lcd.print(buffer);

  int len = strlen(buffer);
  for (int i = len; i < 16; i++) {
    lcd.print(' ');
  }
}

void forceLCDRefresh() {
  lastDisplayedState = ERROR_STATE;
  lastDisplayedCoins = -9999;
  lastDisplayedCupsToDispense = -9999;
  lastDisplayedCupsSold = -9999;
  lastDisplayedPulseCount = -9999;
  lastDisplayedRejectedPulse = -9999;
  lastDisplayedOfflineCount = -9999;
}

// -------------------- COIN DETECTOR RESET --------------------
void resetCoinDetector() {
  noInterrupts();
  pulseBuffer = 0;
  interrupts();

  coinPulseCount = 0;
  coinLastPulseTime = 0;
}

// -------------------- CENTRALIZED STATE TRANSITION --------------------
void setState(State newState) {
  if (currentState == newState) {
    forceLCDRefresh();
    return;
  }

  State oldState = currentState;

  // Exit actions
  coinInputEnabled = false;
  resetCoinDetector();

  // Safety: pump/cooler off unless explicitly starting dispense
  if (newState != DISPENSING) {
    digitalWrite(pumpPin, LOW);
    digitalWrite(coolerPin, LOW);
  }

  currentState = newState;

  // Entry actions
  // Select-first behavior:
  // coin slot only works after Cold/Regular is selected.
  if (newState == ACCEPTING_COINS) {
    resetCoinDetector();
    coinInputEnabled = true;
  } else {
    coinInputEnabled = false;
  }

  if (COIN_DEBUG) {
    Serial.print("STATE: ");
    Serial.print(stateName(oldState));
    Serial.print(" -> ");
    Serial.println(stateName(newState));
  }

  forceLCDRefresh();
}

// -------------------- PREFERENCES --------------------
void saveRuntimeState() {
  if (!prefsReady) return;

  prefs.putInt("balance", currentCoins);
  prefs.putInt("cupsSold", cupsSold);
  prefs.putUInt("totalSales", totalSales);
  prefs.putUInt("lostLogs", lostLogCount);
}

void persistOfflineQueue() {
  if (!prefsReady) return;

  prefs.putUInt("offVer", offlineQueueVersion);
  prefs.putInt("offCount", offlineCount);

  if (offlineCount > 0) {
    prefs.putBytes("offQueue", offlineQueue, sizeof(TransactionLog) * offlineCount);
  } else {
    prefs.remove("offQueue");
  }
}

void loadPersistentData() {
  prefsReady = prefs.begin("vendo", false);

  if (!prefsReady) {
    Serial.println("Preferences failed");
    return;
  }

  currentCoins = prefs.getInt("balance", 0);
  cupsSold = prefs.getInt("cupsSold", 0);
  totalSales = prefs.getUInt("totalSales", 0);
  lostLogCount = prefs.getUInt("lostLogs", 0);

  uint32_t storedQueueVersion = prefs.getUInt("offVer", 0);

  if (storedQueueVersion != offlineQueueVersion) {
    prefs.putUInt("offVer", offlineQueueVersion);
    prefs.putInt("offCount", 0);
    prefs.remove("offQueue");
    offlineCount = 0;
    Serial.println("Offline queue schema reset");
  } else {
    offlineCount = prefs.getInt("offCount", 0);
  }

  if (offlineCount < 0 || offlineCount > MAX_OFFLINE_LOGS) {
    offlineCount = 0;
  }

  size_t loadedBytes = prefs.getBytes("offQueue", offlineQueue, sizeof(offlineQueue));
  int loadedItems = loadedBytes / sizeof(TransactionLog);

  if (loadedItems < offlineCount) {
    offlineCount = loadedItems;
  }

  Serial.print("Loaded balance: ");
  Serial.println(currentCoins);

  Serial.print("Loaded cups sold: ");
  Serial.println(cupsSold);

  Serial.print("Loaded offline logs: ");
  Serial.println(offlineCount);

  Serial.print("Lost/overwritten logs: ");
  Serial.println(lostLogCount);
}

// -------------------- TIME --------------------
bool getEpochMillis(uint64_t &timestampMs) {
  time_t now;
  time(&now);

  // 1700000000 = around Nov 2023.
  // If less than this, ESP32 clock is probably not synced.
  if (now < 1700000000) {
    timestampMs = 0;
    return false;
  }

  timestampMs = (uint64_t)now * 1000ULL;
  return true;
}

bool syncTime(unsigned long timeoutMs) {
  if (!WIFI_ENABLED) {
    timeIsSynced = false;
    return false;
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("NTP skipped: WiFi not connected");
    timeIsSynced = false;
    return false;
  }

  lastNtpAttemptTime = millis();

  Serial.println("Syncing NTP time...");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("Gateway: ");
  Serial.println(WiFi.gatewayIP());
  Serial.print("DNS: ");
  Serial.println(WiFi.dnsIP());

  // UTC timestamp
  configTime(
    0,
    0,
    "pool.ntp.org",
    "time.google.com",
    "time.nist.gov"
  );

  unsigned long start = millis();

  while (millis() - start < timeoutMs) {
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("NTP failed: WiFi disconnected during sync");
      timeIsSynced = false;
      return false;
    }

    time_t now;
    time(&now);

    if (now > 1700000000) {
      timeIsSynced = true;

      Serial.print("Time synced. Epoch: ");
      Serial.println((unsigned long)now);

      return true;
    }

    delay(250);
  }

  timeIsSynced = false;
  Serial.println("Time sync failed");
  return false;
}

// -------------------- WIFI --------------------
const char *wifiStatusName(wl_status_t status) {
  switch (status) {
    case WL_IDLE_STATUS: return "WL_IDLE_STATUS";
    case WL_NO_SSID_AVAIL: return "WL_NO_SSID_AVAIL";
    case WL_SCAN_COMPLETED: return "WL_SCAN_COMPLETED";
    case WL_CONNECTED: return "WL_CONNECTED";
    case WL_CONNECT_FAILED: return "WL_CONNECT_FAILED";
    case WL_CONNECTION_LOST: return "WL_CONNECTION_LOST";
    case WL_DISCONNECTED: return "WL_DISCONNECTED";
    default: return "UNKNOWN";
  }
}

void beginWifiConnection(const char *reason) {
  if (!WIFI_ENABLED) return;

  unsigned long now = millis();

  // Do not call WiFi.begin() repeatedly while ESP32 is already connecting.
  if (
    wifiConnectInProgress &&
    now - wifiConnectStartTime < WIFI_CONNECT_TIMEOUT_MS
  ) {
    Serial.println("WiFi.begin skipped: connection already in progress");
    return;
  }

  Serial.print("Starting WiFi: ");
  Serial.println(reason);

  WiFi.mode(WIFI_STA);
  WiFi.persistent(false);
  WiFi.setAutoReconnect(true);
  WiFi.setSleep(false);

  if (wifiConnectInProgress) {
    Serial.println("Previous WiFi attempt timed out. Restarting WiFi connection...");
    WiFi.disconnect(false, false);
    delay(100);
  }

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  wifiConnectInProgress = true;
  wifiConnectStartTime = now;
  lastWifiReconnectAttempt = now;
}

void connectWiFi() {
  if (!WIFI_ENABLED) {
    Serial.println("WiFi disabled");
    wifiConnected = false;
    timeIsSynced = false;
    return;
  }

  Serial.println("Connecting to WiFi...");
  beginWifiConnection("initial connect");

  unsigned long start = millis();

  while (millis() - start < 12000) {
    wl_status_t status = WiFi.status();

    if (status == WL_CONNECTED) {
      wifiConnected = true;
      wifiConnectInProgress = false;

      Serial.println();
      Serial.println("WiFi connected");
      Serial.print("IP: ");
      Serial.println(WiFi.localIP());

      syncTime(15000);
      flushPending = true;
      return;
    }

    Serial.print(".");
    delay(500);
  }

  wifiConnected = false;
  timeIsSynced = false;

  Serial.println();
  Serial.print("WiFi initial connect not ready. Status: ");
  Serial.println(wifiStatusName(WiFi.status()));

  // checkWifiStatus() will continue reconnection safely.
}

void checkWifiStatus() {
  if (!WIFI_ENABLED) return;

  unsigned long now = millis();

  if (now - lastWifiCheckTime < WIFI_CHECK_INTERVAL_MS) {
    return;
  }

  lastWifiCheckTime = now;

  wl_status_t status = WiFi.status();
  bool currentlyConnected = (status == WL_CONNECTED);

  if (currentlyConnected) {
    if (!wifiConnected) {
      Serial.println("WiFi reconnected");
      Serial.print("IP: ");
      Serial.println(WiFi.localIP());

      flushPending = true;
    }

    wifiConnected = true;
    wifiConnectInProgress = false;

    // Retry NTP occasionally until it succeeds.
    if (
      !timeIsSynced &&
      now - lastNtpAttemptTime >= NTP_RETRY_INTERVAL_MS
    ) {
      syncTime(10000);
    }

    return;
  }

  if (wifiConnected) {
    Serial.print("WiFi lost. Status: ");
    Serial.println(wifiStatusName(status));
  }

  wifiConnected = false;
  timeIsSynced = false;

  // ESP32 is still connecting. Do not call WiFi.begin() again.
  if (
    wifiConnectInProgress &&
    now - wifiConnectStartTime < WIFI_CONNECT_TIMEOUT_MS
  ) {
    if (now - lastWifiStatusPrintTime >= WIFI_STATUS_PRINT_INTERVAL_MS) {
      lastWifiStatusPrintTime = now;

      Serial.print("WiFi still connecting. Status: ");
      Serial.println(wifiStatusName(status));
    }

    return;
  }

  if (now - lastWifiReconnectAttempt >= WIFI_RECONNECT_INTERVAL_MS) {
    beginWifiConnection("reconnect after timeout/failure");
  }
}

// -------------------- FIREBASE FLUSH CONTROL --------------------
unsigned long getOfflineFlushInterval() {
  unsigned long interval = OFFLINE_FLUSH_MIN_INTERVAL_MS;

  for (int i = 0; i < offlineFlushFailCount; i++) {
    if (interval >= OFFLINE_FLUSH_MAX_INTERVAL_MS / 2) {
      interval = OFFLINE_FLUSH_MAX_INTERVAL_MS;
      break;
    }

    interval *= 2;
  }

  if (interval > OFFLINE_FLUSH_MAX_INTERVAL_MS) {
    interval = OFFLINE_FLUSH_MAX_INTERVAL_MS;
  }

  return interval;
}

void maybeFlushOfflineQueue() {
  if (!WIFI_ENABLED) return;
  if (!wifiConnected) return;
  if (WiFi.status() != WL_CONNECTED) return;
  if (offlineCount == 0) return;
  if (currentState != IDLE) return;

  unsigned long now = millis();

  // Important:
  // Do not upload while a customer may still be interacting with the machine.
  if (now - lastUserActivityTime < IDLE_BEFORE_FLUSH_MS) {
    return;
  }

  unsigned long retryInterval = getOfflineFlushInterval();

  // flushPending allows a reconnect/sale to request an upload,
  // but repeated failures still use backoff.
  if (!flushPending && now - lastOfflineFlushAttemptTime < retryInterval) {
    return;
  }

  // Even if flushPending is true, avoid immediate repeated calls.
  if (flushPending && now - lastOfflineFlushAttemptTime < 5000) {
    return;
  }

  flushPending = false;
  lastOfflineFlushAttemptTime = now;

  flushOfflineQueue();
}

// -------------------- FIREBASE --------------------
TransactionLog makeTransaction(int amount, bool coldSelected) {
  TransactionLog tx;

  tx.amount = amount;
  tx.coldSelected = coldSelected;
  tx.uptimeMs = millis();
  tx.timeSynced = getEpochMillis(tx.timestampMs);

  return tx;
}

bool postTransactionToFirebase(const TransactionLog &tx) {
  if (!WIFI_ENABLED || !wifiConnected) return false;

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Firebase POST skipped: WiFi not connected");
    return false;
  }

  uint64_t timestampMs = tx.timestampMs;
  bool effectiveTimeSynced = tx.timeSynced;

  if (timestampMs == 0) {
    if (!getEpochMillis(timestampMs)) {
      Serial.println("Firebase POST skipped: no synced clock for timestamp");
      return false;
    }

    effectiveTimeSynced = true;
  }

  char url[300];

  snprintf(
    url,
    sizeof(url),
    "https://firestore.googleapis.com/v1/projects/%s/databases/(default)/documents/waterLogs?key=%s",
    FIREBASE_PROJECT_ID,
    FIREBASE_API_KEY
  );

  char json[700];

  snprintf(
    json,
    sizeof(json),
    "{"
      "\"fields\":{"
        "\"amount\":{\"integerValue\":\"%d\"},"
        "\"isCold\":{\"booleanValue\":%s},"
        "\"timestamp\":{\"integerValue\":\"%llu\"},"
        "\"timeSynced\":{\"booleanValue\":%s},"
        "\"clientUptimeMs\":{\"integerValue\":\"%lu\"}"
      "}"
    "}",
    tx.amount,
    tx.coldSelected ? "true" : "false",
    (unsigned long long)timestampMs,
    effectiveTimeSynced ? "true" : "false",
    (unsigned long)tx.uptimeMs
  );

  Serial.println("Posting transaction to Firebase...");
  Serial.print("Project ID: ");
  Serial.println(FIREBASE_PROJECT_ID);

  WiFiClientSecure client;

  // Testing mode:
  // This skips certificate validation.
  // For production, use the correct Google root certificate instead.
  client.setInsecure();
  client.setTimeout(3000);

  HTTPClient http;
  http.setTimeout(3000);
  http.setReuse(false);

  bool beginOk = http.begin(client, url);

  if (!beginOk) {
    Serial.println("Firebase HTTP begin failed");
    return false;
  }

  http.addHeader("Content-Type", "application/json");

  int code = http.POST((uint8_t *)json, strlen(json));

  Serial.print("Firebase HTTP code: ");
  Serial.println(code);

  bool posted = (code >= 200 && code < 300);

  if (code <= 0) {
    Serial.print("Firebase transport error: ");
    Serial.println(http.errorToString(code));
  } else if (!posted) {
    Serial.println("Firebase rejected payload:");
    Serial.println(http.getString());
  } else {
    Serial.println("Firebase POST success");
  }

  http.end();

  return posted;
}

// -------------------- OFFLINE QUEUE WITH OVERWRITE OLDEST --------------------
bool addOfflineTransaction(const TransactionLog &tx) {
  if (offlineCount >= MAX_OFFLINE_LOGS) {
    // Remove oldest transaction by shifting everything left.
    for (int i = 1; i < offlineCount; i++) {
      offlineQueue[i - 1] = offlineQueue[i];
    }

    offlineCount--;
    lostLogCount++;

    Serial.println("Offline queue full. Oldest log overwritten.");
    Serial.print("Lost/overwritten logs: ");
    Serial.println(lostLogCount);
  }

  offlineQueue[offlineCount] = tx;
  offlineCount++;

  persistOfflineQueue();
  saveRuntimeState();

  Serial.print("Saved pending transaction. Queue: ");
  Serial.print(offlineCount);
  Serial.print("/");
  Serial.println(MAX_OFFLINE_LOGS);

  return true;
}

void flushOfflineQueue() {
  if (!WIFI_ENABLED || !wifiConnected || offlineCount == 0) return;

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Offline flush skipped: WiFi not connected");
    return;
  }

  bool queueNeedsTimestamp = false;

  for (int i = 0; i < offlineCount; i++) {
    if (offlineQueue[i].timestampMs == 0) {
      queueNeedsTimestamp = true;
      break;
    }
  }

  if (queueNeedsTimestamp && !timeIsSynced && !syncTime(5000)) {
    Serial.println("Offline queue waiting for NTP timestamp");
    offlineFlushFailCount++;
    return;
  }

  Serial.println("Flushing pending/offline queue...");

  int sent = 0;

  while (sent < offlineCount) {
    if (!postTransactionToFirebase(offlineQueue[sent])) {
      break;
    }

    sent++;
  }

  if (sent > 0) {
    for (int i = sent; i < offlineCount; i++) {
      offlineQueue[i - sent] = offlineQueue[i];
    }

    offlineCount -= sent;
    persistOfflineQueue();

    offlineFlushFailCount = 0;

    Serial.print("Pending logs sent: ");
    Serial.println(sent);
  } else {
    offlineFlushFailCount++;

    Serial.print("Offline flush failed. Fail count: ");
    Serial.println(offlineFlushFailCount);

    Serial.print("Next retry interval ms: ");
    Serial.println(getOfflineFlushInterval());
  }

  Serial.print("Offline queue remaining: ");
  Serial.println(offlineCount);
}

bool recordTransaction(int price, bool coldSelected) {
  // Important design change:
  // Do NOT post directly to Firebase here.
  // Direct HTTP upload can block buttons and coin handling.
  // Instead, save locally first, then upload later while IDLE.
  TransactionLog tx = makeTransaction(price, coldSelected);

  totalSales += price;
  saveRuntimeState();

  addOfflineTransaction(tx);

  // Request background flush later.
  flushPending = true;

  // Always return true so vending does not stop because of logging.
  return true;
}

// -------------------- LCD UPDATE --------------------
void updateLCD() {
  if (
    currentState == lastDisplayedState &&
    currentCoins == lastDisplayedCoins &&
    cupsToDispense == lastDisplayedCupsToDispense &&
    cupsSold == lastDisplayedCupsSold &&
    coinPulseCount == lastDisplayedPulseCount &&
    lastRejectedPulseCount == lastDisplayedRejectedPulse &&
    offlineCount == lastDisplayedOfflineCount
  ) {
    return;
  }

  lastDisplayedState = currentState;
  lastDisplayedCoins = currentCoins;
  lastDisplayedCupsToDispense = cupsToDispense;
  lastDisplayedCupsSold = cupsSold;
  lastDisplayedPulseCount = coinPulseCount;
  lastDisplayedRejectedPulse = lastRejectedPulseCount;
  lastDisplayedOfflineCount = offlineCount;

  char line[17];

  if (currentState == IDLE) {
    if (currentCoins > 0) {
      snprintf(line, sizeof(line), "Balance: P%d", currentCoins);
      printLCDLine(0, line);
      printLCDLine(1, "Select Water");
    } else {
      printLCDLine(0, "Select Water");
      printLCDLine(1, "Cold | Regular");
    }
  }

  else if (currentState == ACCEPTING_COINS) {
    int price = isCold ? coldPrice : regularPrice;

    snprintf(line, sizeof(line), "%s P%d", isCold ? "Cold" : "Regular", price);
    printLCDLine(0, line);

    if (LCD_DEBUG_PULSES && coinPulseCount > 0) {
      snprintf(line, sizeof(line), "Pulse: %d", coinPulseCount);
    } else {
      snprintf(line, sizeof(line), "Paid: %d/%d", currentCoins, price);
    }

    printLCDLine(1, line);
  }

  else if (currentState == DISPENSING) {
    printLCDLine(0, "Dispensing...");
    snprintf(line, sizeof(line), "Left:%d Bal:%d", cupsToDispense, currentCoins);
    printLCDLine(1, line);
  }

  else if (currentState == CUP_GAP) {
    printLCDLine(0, "Next cup...");
    snprintf(line, sizeof(line), "Left: %d", cupsToDispense);
    printLCDLine(1, line);
  }

  else if (currentState == COIN_REJECTED) {
    printLCDLine(0, "Invalid Coin");
    snprintf(line, sizeof(line), "Pulse: %d", lastRejectedPulseCount);
    printLCDLine(1, line);
  }

  else if (currentState == OUT_OF_WATER) {
    printLCDLine(0, "OUT OF WATER");
    printLCDLine(1, "Press RESET");
  }

  else if (currentState == ERROR_STATE) {
    printLCDLine(0, "SYSTEM ERROR");
    printLCDLine(1, errorMessage);
  }
}

// -------------------- COIN CLASSIFIER --------------------
bool classifyCoin(int pulseCount, int &coinValue, const char *&coinLabel) {
  for (int i = 0; i < coinRuleCount; i++) {
    if (
      pulseCount >= coinRules[i].minPulse &&
      pulseCount <= coinRules[i].maxPulse
    ) {
      coinValue = coinRules[i].value;
      coinLabel = coinRules[i].label;
      return true;
    }
  }

  coinValue = 0;
  coinLabel = "UNKNOWN";
  return false;
}

void enterCoinRejected(int pulseCount) {
  lastRejectedPulseCount = pulseCount;
  messageStartTime = millis();
  resumeAfterMessage = ACCEPTING_COINS;

  setState(COIN_REJECTED);
}

void processCoins() {
  if (currentState != ACCEPTING_COINS) {
    resetCoinDetector();
    return;
  }

  unsigned long now = millis();

  noInterrupts();
  int detected = pulseBuffer;
  pulseBuffer = 0;
  interrupts();

  if (detected > 0) {
    markUserActivity();

    coinPulseCount += detected;
    coinLastPulseTime = now;

    if (COIN_DEBUG) {
      Serial.print("RAW PULSE: ");
      Serial.print(detected);
      Serial.print(" | TOTAL: ");
      Serial.println(coinPulseCount);
    }
  }

  if (coinPulseCount > 0 && (now - coinLastPulseTime > coinPulseTimeout)) {
    int coinValue = 0;
    const char *coinLabel = "UNKNOWN";

    bool validCoin = classifyCoin(coinPulseCount, coinValue, coinLabel);

    if (COIN_DEBUG) {
      Serial.println("------ COIN ANALYSIS ------");
      Serial.print("Final Pulse Count: ");
      Serial.println(coinPulseCount);
    }

    if (validCoin) {
      currentCoins += coinValue;
      saveRuntimeState();

      if (COIN_DEBUG) {
        Serial.print("VALID: ");
        Serial.println(coinLabel);
        Serial.print("ADDED: ");
        Serial.println(coinValue);
        Serial.print("BALANCE: ");
        Serial.println(currentCoins);
      }

      resetCoinDetector();
    } else {
      if (COIN_DEBUG) {
        Serial.println("INVALID COIN / UNKNOWN RANGE");
      }

      int rejectedPulse = coinPulseCount;
      resetCoinDetector();
      enterCoinRejected(rejectedPulse);
    }

    if (COIN_DEBUG) {
      Serial.println("---------------------------");
    }
  }

  if (COIN_DEBUG && millis() - coinDebugTimer > 5000) {
    coinDebugTimer = millis();

    Serial.print("STATE: ");
    Serial.print(stateName(currentState));
    Serial.print(" | BALANCE: ");
    Serial.print(currentCoins);
    Serial.print(" | QUEUE: ");
    Serial.print(offlineCount);
    Serial.print("/");
    Serial.print(MAX_OFFLINE_LOGS);
    Serial.print(" | LOST LOGS: ");
    Serial.println(lostLogCount);
  }
}

// -------------------- PAYMENT --------------------
void beginDispensing() {
  markUserActivity();

  setState(DISPENSING);

  pumpStartTime = millis();

  digitalWrite(pumpPin, HIGH);
  digitalWrite(coolerPin, isCold ? HIGH : LOW);
}

void validatePayment() {
  if (currentState != ACCEPTING_COINS) return;

  if (cupsSold >= maxCups) {
    setState(OUT_OF_WATER);
    return;
  }

  int price = isCold ? coldPrice : regularPrice;

  if (currentCoins < price) return;

  int capacityRemaining = maxCups - cupsSold;

  if (capacityRemaining <= 0) {
    setState(OUT_OF_WATER);
    return;
  }

  if (AUTO_MULTI_CUP) {
    int affordableCups = currentCoins / price;

    if (affordableCups > capacityRemaining) {
      affordableCups = capacityRemaining;
    }

    cupsToDispense = affordableCups;
    currentCoins -= cupsToDispense * price;
  } else {
    cupsToDispense = 1;
    currentCoins -= price;
  }

  saveRuntimeState();

  Serial.print("Payment accepted. Cups queued: ");
  Serial.println(cupsToDispense);

  beginDispensing();
}

// -------------------- DISPENSING --------------------
void enterError(const char *message) {
  strncpy(errorMessage, message, 16);
  errorMessage[16] = '\0';

  setState(ERROR_STATE);
}

void handleDispensing() {
  unsigned long elapsed = millis() - pumpStartTime;

  if (elapsed >= pumpTimeout) {
    digitalWrite(pumpPin, LOW);
    digitalWrite(coolerPin, LOW);

    enterError("Pump Timeout");
    return;
  }

  if (elapsed >= pumpDuration) {
    digitalWrite(pumpPin, LOW);
    digitalWrite(coolerPin, LOW);

    markUserActivity();

    int price = isCold ? coldPrice : regularPrice;

    cupsSold++;
    cupsToDispense--;

    saveRuntimeState();

    Serial.print("Cup dispensed. Cups sold: ");
    Serial.println(cupsSold);

    // Queue the transaction only.
    // Firebase upload will happen later while IDLE.
    recordTransaction(price, isCold);

    if (cupsSold >= maxCups) {
      setState(OUT_OF_WATER);
      return;
    }

    if (AUTO_MULTI_CUP && cupsToDispense > 0) {
      cupGapStartTime = millis();
      setState(CUP_GAP);
    } else {
      cupsToDispense = 0;
      setState(IDLE);
    }
  }
}

void handleCupGap() {
  if (millis() - cupGapStartTime >= cupGapDuration) {
    beginDispensing();
  }
}

void handleCoinRejected() {
  if (millis() - messageStartTime >= coinRejectedDisplayTime) {
    setState(resumeAfterMessage);
  }
}

// -------------------- BUTTONS --------------------
void updateButton(ButtonState &button) {
  button.current = digitalRead(button.pin);
  button.pressed = (button.last == HIGH && button.current == LOW);
  button.last = button.current;
}

void selectWater(bool coldSelected) {
  markUserActivity();

  isCold = coldSelected;

  if (currentState != ACCEPTING_COINS) {
    setState(ACCEPTING_COINS);
  } else {
    forceLCDRefresh();
  }

  Serial.println(coldSelected ? "Cold selected" : "Regular selected");

  validatePayment();
}

void handleResetButton() {
  markUserActivity();

  Serial.println("Reset pressed");

  digitalWrite(pumpPin, LOW);
  digitalWrite(coolerPin, LOW);

  currentCoins = 0;
  cupsToDispense = 0;
  cupsSold = 0;

  resetCoinDetector();
  saveRuntimeState();

  setState(IDLE);
}

void readButtons() {
  if (millis() - lastButtonCheckTime < 50) return;
  lastButtonCheckTime = millis();

  updateButton(coldButton);
  updateButton(regularButton);
  updateButton(resetButton);

  if (resetButton.pressed) {
    handleResetButton();
    return;
  }

  if (currentState == IDLE || currentState == ACCEPTING_COINS) {
    if (coldButton.pressed) {
      selectWater(true);
      return;
    }

    if (regularButton.pressed) {
      selectWater(false);
      return;
    }
  }
}

// -------------------- SETUP --------------------
void setup() {
  Serial.begin(115200);
  Serial.println("=== WATER VENDO SYSTEM START ===");

  Wire.begin(SDA_PIN, SCL_PIN);

  lcd.init();
  lcd.backlight();

  printLCDLine(0, "System Starting");
  printLCDLine(1, "Please wait...");

  pinMode(coinSlotPin, INPUT); // GPIO34 requires external 10k pull-up

  pinMode(pumpPin, OUTPUT);
  pinMode(coolerPin, OUTPUT);

  pinMode(btnCold, INPUT_PULLUP);
  pinMode(btnRegular, INPUT_PULLUP);
  pinMode(resetBtn, INPUT_PULLUP);

  digitalWrite(pumpPin, LOW);
  digitalWrite(coolerPin, LOW);

  loadPersistentData();

  attachInterrupt(digitalPinToInterrupt(coinSlotPin), coinISR, FALLING);

  connectWiFi();

  if (cupsSold >= maxCups) {
    setState(OUT_OF_WATER);
  } else {
    setState(IDLE);
  }

  lastUserActivityTime = millis();
  lastOfflineFlushAttemptTime = millis();

  updateLCD();
}

// -------------------- LOOP --------------------
void loop() {
  // Highest priority: physical controls
  readButtons();

  // WiFi status check is lightweight and non-spammy
  checkWifiStatus();

  // Main vending FSM
  switch (currentState) {
    case IDLE:
      break;

    case ACCEPTING_COINS:
      processCoins();
      validatePayment();
      break;

    case DISPENSING:
      handleDispensing();
      break;

    case CUP_GAP:
      handleCupGap();
      break;

    case COIN_REJECTED:
      handleCoinRejected();
      break;

    case OUT_OF_WATER:
      break;

    case ERROR_STATE:
      break;
  }

  // Lowest priority:
  // Firebase upload only runs when idle and after no user activity.
  maybeFlushOfflineQueue();

  updateLCD();
}
