// =======================================================
// Motion Gesture Classifier
// Part 1 - Initialization & Sensor Handling
// =======================================================

// --------------------
// HTML Elements
// --------------------
const toggleButton = document.getElementById("toggleSensorButton");

const sensorStatus = document.getElementById("sensorStatus");
const modelStatus = document.getElementById("modelStatus");

const prediction = document.getElementById("prediction");
const confidence = document.getElementById("confidence");

const sensorX = document.getElementById("sensorX");
const sensorY = document.getElementById("sensorY");
const sensorZ = document.getElementById("sensorZ");

const idleBar = document.getElementById("idleBar");
const leftBar = document.getElementById("leftRightBar");
const upBar = document.getElementById("upDownBar");

const idlePercent = document.getElementById("idlePercent");
const leftPercent = document.getElementById("leftRightPercent");
const upPercent = document.getElementById("upDownPercent");

const canvas = document.getElementById("waveform");
const ctx = canvas.getContext("2d");

// --------------------
// Constants
// --------------------
const SAMPLE_COUNT = 125;
const FEATURE_COUNT = 375;

// x,y,z repeated 125 times
let featureBuffer = [];

// waveform values
let waveform = [];

// classifier
let classifier = null;
let classifierReady = false;

// state
let sensorRunning = false;
let classifyTimer = null;

// latest sensor values
let latestX = 0;
let latestY = 0;
let latestZ = 0;

// --------------------
// Load Edge Impulse Model
// --------------------
async function loadModel() {

    try {

        classifier = new EdgeImpulseClassifier();

        await classifier.init();

        console.log(classifier.getProjectInfo());
        console.log(classifier.getProperties());

        classifierReady = true;

        modelStatus.textContent = "Loaded";
        modelStatus.className = "badge online";

    }
    catch(err){

        console.error(err);

        alert("Unable to load Edge Impulse model.");

    }

}

// --------------------
// Start Sensor
// --------------------
async function startSensor(){

    if(sensorRunning) return;

    if(!classifierReady){

        await loadModel();

    }

    // iPhone permission
    if(typeof DeviceMotionEvent !== "undefined" &&
       typeof DeviceMotionEvent.requestPermission === "function"){

        const response = await DeviceMotionEvent.requestPermission();

        if(response !== "granted"){

            alert("Motion permission denied.");

            return;

        }

    }

    window.addEventListener("devicemotion", motionHandler);

    sensorRunning = true;

    toggleButton.textContent = "Stop Sensor";

    sensorStatus.textContent = "Running";
    sensorStatus.className = "badge online";

    classifyTimer = setInterval(runClassification,300);

}

// --------------------
// Stop Sensor
// --------------------
function stopSensor(){

    sensorRunning = false;

    window.removeEventListener("devicemotion",motionHandler);

    clearInterval(classifyTimer);

    toggleButton.textContent = "Start Sensor";

    sensorStatus.textContent = "Stopped";
    sensorStatus.className = "badge offline";

    prediction.textContent = "Waiting...";
    confidence.textContent = "0%";

    featureBuffer=[];

}

// --------------------
// Motion Event
// --------------------
function motionHandler(event){

    const acc = event.accelerationIncludingGravity;

    if(!acc) return;

    latestX = acc.x ?? 0;
    latestY = acc.y ?? 0;
    latestZ = acc.z ?? 0;

    sensorX.textContent = latestX.toFixed(2);
    sensorY.textContent = latestY.toFixed(2);
    sensorZ.textContent = latestZ.toFixed(2);

    // Add newest sample
    featureBuffer.push(latestX);
    featureBuffer.push(latestY);
    featureBuffer.push(latestZ);

    // Keep exactly 375 floats
    while(featureBuffer.length > FEATURE_COUNT){

        featureBuffer.splice(0,3);

    }

    waveform.push(latestX);

    if(waveform.length>150){

        waveform.shift();

    }

}

// =======================================================
// Part 2 - Edge Impulse Classification
// =======================================================

function runClassification() {

    if (!classifierReady) return;

    if (featureBuffer.length < FEATURE_COUNT) return;

    try {

        // Run inference
        const result = classifier.classify(featureBuffer);

        if (!result || !result.results) return;

        let highest = result.results[0];

        // Find highest confidence prediction
        for (const item of result.results) {

            if (item.value > highest.value) {
                highest = item;
            }

        }

        // -------------------------
        // Update Prediction Text
        // -------------------------
        let displayName = highest.label;

        if (displayName === "nothing") {
            displayName = "IDLE";
        }
        else if (displayName === "left_right") {
            displayName = "LEFT RIGHT";
        }
        else if (displayName === "up_down") {
            displayName = "UP DOWN";
        }

        prediction.textContent = displayName;

        confidence.textContent =
            (highest.value * 100).toFixed(1) + "%";

        // -------------------------
        // Reset bars
        // -------------------------

        let idle = 0;
        let left = 0;
        let up = 0;

        // -------------------------
        // Read probabilities
        // -------------------------

        for (const item of result.results) {

            if (item.label === "nothing") {

                idle = item.value;

            }

            if (item.label === "left_right") {

                left = item.value;

            }

            if (item.label === "up_down") {

                up = item.value;

            }

        }

        // -------------------------
        // Update progress bars
        // -------------------------

        idleBar.style.width = (idle * 100) + "%";
        leftBar.style.width = (left * 100) + "%";
        upBar.style.width = (up * 100) + "%";

        idlePercent.textContent =
            (idle * 100).toFixed(1) + "%";

        leftPercent.textContent =
            (left * 100).toFixed(1) + "%";

        upPercent.textContent =
            (up * 100).toFixed(1) + "%";

    }
    catch (err) {

        console.error("Classification Error:", err);

    }

}

// =======================================================
// Part 3 - Waveform, Toggle Button & Initialization
// =======================================================

// --------------------
// Draw Live Waveform
// --------------------
function drawWaveform() {

    ctx.clearRect(0, 0, canvas.width, canvas.height);

    ctx.strokeStyle = "#00e5ff";
    ctx.lineWidth = 3;

    ctx.beginPath();

    const centerY = canvas.height / 2;

    const step = canvas.width / 150;

    for (let i = 0; i < waveform.length; i++) {

        const x = i * step;

        const y = centerY - waveform[i] * 10;

        if (i === 0) {

            ctx.moveTo(x, y);

        }
        else {

            ctx.lineTo(x, y);

        }

    }

    ctx.stroke();

    requestAnimationFrame(drawWaveform);

}

// --------------------
// Toggle Button
// --------------------
toggleButton.addEventListener("click", async () => {

    if (!sensorRunning) {

        await startSensor();

    }
    else {

        stopSensor();

    }

});

// --------------------
// Initialize UI
// --------------------
window.addEventListener("load", () => {

    prediction.textContent = "Waiting...";

    confidence.textContent = "0%";

    sensorStatus.textContent = "Stopped";
    sensorStatus.className = "badge offline";

    modelStatus.textContent = "Not Loaded";
    modelStatus.className = "badge offline";

    idleBar.style.width = "0%";
    leftBar.style.width = "0%";
    upBar.style.width = "0%";

    idlePercent.textContent = "0%";
    leftPercent.textContent = "0%";
    upPercent.textContent = "0%";

    drawWaveform();

    console.log("Motion Gesture Classifier Ready");

});