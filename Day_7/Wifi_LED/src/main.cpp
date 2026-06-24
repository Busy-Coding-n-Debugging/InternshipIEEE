#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

// Replace with your network credentials
const char* ssid = "Redmi 13C";
const char* password = "A1b2C3d4E5";

// LED Pin (GPIO 2 is the built-in blue LED on most ESP32 boards)
const int ledPin = 2;
bool ledState = LOW;

// Instantiate the web server on port 80
WebServer server(80);

// HTML page embedded using raw string literal stored in flash memory
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ESP32 Dynamic LED Control</title>
    <style>
        body {
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            text-align: center;
            background-color: #121212;
            color: #ffffff;
            margin-top: 50px;
        }
        h2 { color: #00adb5; }
        .btn {
            background-color: #ff414d;
            border: none;
            color: white;
            padding: 15px 40px;
            font-size: 20px;
            font-weight: bold;
            cursor: pointer;
            border-radius: 30px;
            transition: 0.3s ease;
            box-shadow: 0 4px 15px rgba(255, 65, 77, 0.4);
        }
        /* Style class for when the LED is active */
        .btn.on {
            background-color: #00adb5;
            box-shadow: 0 4px 15px rgba(0, 173, 181, 0.6);
        }
    </style>
</head>
<body>
    <h2>ESP32 Automation Server</h2>
    <p>Click below to control the physical LED state.</p>
    
    <button id="ledBtn" class="btn" onclick="toggleLED()">LED: OFF</button>

    <script>
        function toggleLED() {
            // Fetch the toggle route from the ESP32
            fetch('/toggle')
                .then(response => response.text())
                .then(state => {
                    const btn = document.getElementById('ledBtn');
                    // ESP32 returns "1" if the LED is now ON
                    if (state === "1") {
                        btn.innerText = "LED: ON";
                        btn.classList.add('on');
                    } else {
                        btn.innerText = "LED: OFF";
                        btn.classList.remove('on');
                    }
                })
                .catch(err => console.error("Error communicating with ESP32:", err));
        }
    </script>
</body>
</html>
)rawliteral";

void handleRoot() {
    server.send(200, "text/html", index_html);
}

void handleToggle() {
    ledState = !ledState;
    digitalWrite(ledPin, ledState);
    
    // Send back the current state as a string response ("1" or "0")
    if(ledState == HIGH) {
        server.send(200, "text/plain", "1");
    } else {
        server.send(200, "text/plain", "0");
    }
}

void handleNotFound() {
    server.send(404, "text/plain", "404: Not Found");
}

void setup() {
    Serial.begin(115200);
    
    pinMode(ledPin, OUTPUT);
    digitalWrite(ledPin, ledState);

    // Connect to your local Wi-Fi network
    WiFi.begin(ssid, password);
    Serial.print("Connecting to Wi-Fi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.print("Connected successfully! IP: ");
    Serial.println(WiFi.localIP());

    // Link URLs to their respective handler functions
    server.on("/", handleRoot);
    server.on("/toggle", handleToggle);
    server.onNotFound(handleNotFound);

    // Fire up the server
    server.begin();
    Serial.println("HTTP server started");
}

void loop() {
    server.handleClient();
}