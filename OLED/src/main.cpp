#include <WiFi.h>
#include <HTTPClient.h>
#include <ESP32Servo.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <ArduinoJson.h>

U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

const char *ssid = "chuan";
const char *password = "12345678";
const char *serverIP = "http://192.168.1.5:5000";

Servo myServo;
Servo myServo2;
const int servoPin = 5;
const int servoPin2 = 2;

unsigned long lastCheckTime = 0;
const unsigned long checkInterval = 500;

int buttonPin = 4;
int buttonPin2 = 16;
int buttonPin3 = 17;
bool selectedCocaCola = true;
bool lastButtonState = HIGH;
bool lastButtonState2 = HIGH;
unsigned long lastDebounceTime = 0;
unsigned long lastDebounceTime2 = 0;
const unsigned long debounceDelay = 50;

bool isPaymentScreen = false;
String selectedProduct = "";
int paymentAmount = 10000;
bool paymentCompleted = false;

void setup()
{
  Serial.begin(115200);
  u8g2.begin();
  u8g2.setFont(u8g2_font_ncenB14_tr);
  updateDisplay();

  pinMode(buttonPin, INPUT);
  pinMode(buttonPin2, INPUT);
  pinMode(buttonPin3, INPUT);

  WiFi.begin(ssid, password);
  Serial.print("Đang kết nối WiFi...");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ Đã kết nối WiFi");
  Serial.printf("📶 WiFi RSSI: %d dBm\n", WiFi.RSSI());

  myServo.write(0); // Đảm bảo giá trị đầu tiên là 0
  myServo.attach(servoPin);
  delay(500);
  myServo2.write(0);
  myServo2.attach(servoPin2);
  delay(500);
}

void updateDisplay()
{
  u8g2.clearBuffer();
  u8g2.drawStr(20, 19, "Coca");
  if (selectedCocaCola)
  {
    u8g2.drawFrame(15, 5, 80, 20);
  }

  u8g2.drawStr(20, 39, "Pepsi");
  if (!selectedCocaCola)
  {
    u8g2.drawFrame(15, 25, 80, 20);
  }

  u8g2.sendBuffer();
}

void showPaymentInfoScreen(const String &product)
{
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x12_tf);
  u8g2.drawStr(0, 15, "Thanh toan thanh cong!");
  u8g2.drawStr(0, 30, ("San pham: " + product).c_str());
  u8g2.drawStr(0, 45, "Nhan nut de quay lai");
  u8g2.sendBuffer();
}

void handlePaymentScreenButtons()
{
  bool reading2 = digitalRead(buttonPin2);
  bool reading3 = digitalRead(buttonPin3);

  // Nút D16 - chuyển sang màn hình thanh toán nếu đã thanh toán đúng sản phẩm
  if (reading2 == LOW && lastButtonState2 == HIGH)
  {
    String currentSelection = selectedCocaCola ? "coca" : "pepsi";

    if (paymentCompleted && selectedProduct == currentSelection)
    {
      isPaymentScreen = true;
      showPaymentInfoScreen(selectedProduct);
    }
    else
    {
      isPaymentScreen = true;
      u8g2.clearBuffer();
      u8g2.setFont(u8g2_font_6x12_tf);
      u8g2.drawStr(0, 20, "Chua thanh toan!");
      u8g2.drawStr(0, 40, "Nhan nut de quay lai");
      u8g2.sendBuffer();
    }

    delay(200);
  }

  // Nút D17 - quay lại màn hình chọn sản phẩm + reset trạng thái thanh toán
  if (reading3 == LOW && isPaymentScreen)
  {
    isPaymentScreen = false;
    paymentCompleted = false; // Reset trạng thái thanh toán
    selectedProduct = "";     // Xóa sản phẩm đã lưu
    updateDisplay();
    delay(200);
  }

  lastButtonState2 = reading2;
}

// chống nhiễu nút nhấn
void handleButton()
{
  bool reading = digitalRead(buttonPin);

  if (reading != lastButtonState)
  {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay)
  {
    if (reading == LOW)
    {
      selectedCocaCola = !selectedCocaCola;
      updateDisplay();
    }
  }

  lastButtonState = reading;
}

// gửi request dến máy chủ
void confirmPayment()
{
  WiFiClient client;
  HTTPClient http;

  String url = String(serverIP) + "/confirm-payment";
  Serial.println("📡 Gửi xác nhận đến: " + url);

  http.begin(client, url);
  http.addHeader("Content-Type", "application/json");
  int httpCode = http.POST("{}");

  if (httpCode == HTTP_CODE_OK)
  {
    Serial.println("[📨 Confirm Response]: " + http.getString());
  }
  else
  {
    Serial.printf("❌ Lỗi xác nhận HTTP: %d\n", httpCode);
  }

  http.end();
}

void loop()
{
  handleButton();
  handlePaymentScreenButtons();

  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("⚠️ Mất kết nối WiFi. Đang thử lại...");
    WiFi.reconnect();
    delay(5000);
    return;
  }

  if (millis() - lastCheckTime > checkInterval)
  {
    lastCheckTime = millis();

    WiFiClient client;
    HTTPClient http;
    String url = String(serverIP) + "/check-payment";
    Serial.println("📡 Gửi request đến: " + url);

    http.begin(client, url);
    int httpCode = http.GET();

    if (httpCode == HTTP_CODE_OK)
    {
      String payload = http.getString();
      Serial.println("[📨 Server Response]: " + payload);

      DynamicJsonDocument doc(256);
      DeserializationError error = deserializeJson(doc, payload);

      if (error)
      {
        Serial.println("❌ Lỗi phân tích JSON: " + String(error.c_str()));
        http.end();
        return;
      }

      const char *status = doc["status"];
      if (strcmp(status, "success") == 0)
      {
        const char *product = doc["product"];
        if (product == nullptr)
        {
          Serial.println("❌ Không tìm thấy trường 'product'");
          http.end();
          return;
        }

        Serial.println("💰 Thanh toán hợp lệ cho: " + String(product));
        selectedProduct = String(product); // lưu tên sản phẩm

        if (strcmp(product, "coca") == 0)
        {
          myServo.write(90);
          delay(2000);
          myServo.write(0);
          delay(500);
        }
        else if (strcmp(product, "pepsi") == 0)
        {
          myServo2.write(90);
          delay(2000);
          myServo2.write(0);
          delay(500);
        }

        paymentCompleted = true; // ✅ đánh dấu đã thanh toán
        confirmPayment();
      }
      else
      {
        Serial.println("📥 Trạng thái thanh toán: " + String(status));
      }
    }
    else
    {
      Serial.printf("❌ Lỗi HTTP: %d\n", httpCode);
    }

    http.end();
  }
}