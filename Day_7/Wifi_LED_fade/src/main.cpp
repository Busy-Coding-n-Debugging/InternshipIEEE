#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

// Replace with your network credentials
const char* ssid = "Redmi 13C";
const char* password = "A1b2C3d4E5";

// LED PWM Properties
const int ledPin = 4;       // GPIO pin for the LED
const int pwmChannel = 0;    // PWM channel (0-15)
const int freq = 5000;       // Frequency in Hz
const int resolution = 8;    // 8-bit resolution (0-255)

// Instantiate the web server on port 80
WebServer server(80);

// HTML page embedded using raw string literal stored in flash memory
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ESP32 Dimmer Control</title>
    <style>
        body {
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            text-align: center;
            background-color: #121212;
            color: #ffffff;
            margin-top: 50px;
        }
        h2 { color: #00adb5; }
        .container {
            max-width: 400px;
            margin: 0 auto;
            padding: 20px;
            background: #1e1e1e;
            border-radius: 10px;
            box-shadow: 0 4px 10px rgba(0,0,0,0.5);
        }
        .slider {
            -webkit-appearance: none;
            width: 100%;
            height: 15px;
            border-radius: 5px;
            background: #393e46;
            outline: none;
            margin: 20px 0;
        }
        .slider::-webkit-slider-thumb {
            -webkit-appearance: none;
            appearance: none;
            width: 25px;
            height: 25px;
            border-radius: 50%;
            background: #00adb5;
            cursor: pointer;
            box-shadow: 0 0 10px #00adb5;
        }
        .value-display {
            font-size: 24px;
            font-weight: bold;
            color: #00adb5;
        }
    </style>
</head>
<body>
    <div class="container">
        <h2>LED Dimmer Server</h2>
        <p>Drag the slider to adjust LED brightness</p>
        
        <input type="range" min="0" max="255" value="0" class="slider" id="brightnessSlider" oninput="updateBrightness(this.value)">
        
        <div class="value-display">Value: <span id="sliderValue">0</span></div>
    </div>

    <script>
        function updateBrightness(val) {
            // Update the text on the screen instantly
            document.getElementById('sliderValue').innerText = val;
            
            // Send the brightness value to the ESP32 server endpoint
            fetch('/dim?value=' + val)
                .then(response => response.text())
                .then(data => console.log(data))
                .catch(err => console.error("Error setting brightness:", err));
        }
    </script>
</body>
</html>
)rawliteral";

void handleRoot() {
    server.send(200, "text/html", index_html);
}

void handleDim() {
    // Check if the query parameter "value" exists in the URL (e.g., /dim?value=150)
    if (server.hasArg("value")) {
        String brightnessStr = server.arg("value");
        int brightnessValue = brightnessStr.toInt();
        
        // Constrain value between 0 and 255 to prevent unexpected PWM issues
        brightnessValue = constrain(brightnessValue, 0, 255);
        
        // Write the PWM duty cycle to the channel
        ledcWrite(pwmChannel, brightnessValue);
        
        Serial.print("Brightness set to: ");
        Serial.println(brightnessValue);
        
        server.send(200, "text/plain", "OK");
    } else {
        server.send(400, "text/plain", "Bad Request: Missing value parameter");
    }
}

void handleNotFound() {
    server.send(404, "text/plain", "404: Not Found");
}

void setup() {
    Serial.begin(115200);
    
    // Configure ESP32 PWM functionalites
    ledcSetup(pwmChannel, freq, resolution);
    
    // Attach the internal/external LED GPIO pin to the PWM channel
    ledcAttachPin(ledPin, pwmChannel);
    ledcWrite(pwmChannel, 0); // Start turned off

    // Connect to Wi-Fi
    WiFi.begin(ssid, password);
    Serial.print("Connecting to Wi-Fi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.print("Connected! IP Address: ");
    Serial.println(WiFi.localIP());

    // Setup routes
    server.on("/", handleRoot);
    server.on("/dim", handleDim); // Handles incoming /dim?value=X requests
    server.onNotFound(handleNotFound);

    server.begin();
    Serial.println("HTTP Dimmer server started");
}

void loop() {
    server.handleClient();
}