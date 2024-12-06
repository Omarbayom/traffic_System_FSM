#include "Smarttrafficsystem.h"
#include <QGraphicsLineItem>
#include <QVBoxLayout>
#include <QPen>
#include <QComboBox>
#include <QGraphicsSceneMouseEvent>

// SmartTrafficLight Implementation
SmartTrafficLight::SmartTrafficLight(QGraphicsItem *parent)
    : QGraphicsRectItem(parent), currentState(State::Red), stateDuration(10), elapsedTime(0), remainingTime(stateDuration),
    pedestrianButtonPressed(false) {

    setRect(-10, -10, 20, 20); // Square traffic light
    setColor(Qt::red);

    timer = new QTimer(this);
    stateChangeTimer = new QTimer(this);

    // Connect the timer to update the light and the timer text
    connect(timer, &QTimer::timeout, this, &SmartTrafficLight::updateLight);
    connect(stateChangeTimer, &QTimer::timeout, this, &SmartTrafficLight::updateTimer);

    timer->start(1000);  // Update every second
    stateChangeTimer->start(1000);  // Update the remaining time display every second

    // Add a text item to display the remaining time
    timeDisplay = new QGraphicsTextItem(this);
    timeDisplay->setPos(0, -25);
    updateTimeDisplay();
}

void SmartTrafficLight::setColor(const QColor &color) {
    QBrush brush(color);
    setBrush(brush);
}

void SmartTrafficLight::updateLight() {
    elapsedTime++;

    if (elapsedTime >= stateDuration) {
        elapsedTime = 0;

        switch (currentState) {
        case State::Red:
            currentState = State::Yellow;
            setColor(Qt::yellow);
            stateDuration = 3; // Yellow light duration
            break;
        case State::Yellow:
            currentState = State::Green;
            setColor(Qt::green);
            stateDuration = 10; // Green light duration
            break;
        case State::Green:
            currentState = State::Red;
            setColor(Qt::red);
            stateDuration = 10; // Red light duration
            break;
        }

        resetPedestrianButton();  // Reset the pedestrian button status when the state changes
    }

    remainingTime = stateDuration - elapsedTime;
    updateTimeDisplay();
    emit pedestrianButtonEnabled(!pedestrianButtonPressed); // Enable button only if it hasn't been pressed
}

void SmartTrafficLight::updateTimer() {
    remainingTime = stateDuration - elapsedTime;
    updateTimeDisplay();
}

void SmartTrafficLight::updateTimeDisplay() {
    timeDisplay->setPlainText(QString::number(remainingTime) + "s");
}

QString SmartTrafficLight::getLightState() const {
    switch (currentState) {
    case State::Red: return "Red";
    case State::Yellow: return "Yellow";
    case State::Green: return "Green";
    }
    return "Unknown";
}

void SmartTrafficLight::setTrafficDensity(int density) {
    stateDuration = 10 + density; // Increase state duration based on traffic density
}

void SmartTrafficLight::triggerEmergency() {
    currentState = State::Green;
    setColor(Qt::green);
    elapsedTime = 0;
    stateDuration = 10; // Immediate green light for emergencies
    remainingTime = stateDuration;
    updateTimeDisplay();
    resetPedestrianButton();
}

void SmartTrafficLight::triggerPedestrianCrossing() {
    // Check the current state of the traffic light
    if (pedestrianButtonPressed) return; // If the pedestrian button was already pressed for this color, do nothing

    if (currentState == State::Green || currentState == State::Yellow) {
        // Reduce the timer to 3 seconds, but only if it's greater than 3
        if (remainingTime > 3) {
            stateDuration = 3;
        }
        elapsedTime = 0;
    } else if (currentState == State::Red) {
        // Add 5 seconds to the timer if it's less than 6
        if (stateDuration < 6) {
            stateDuration += 5;
        }
        currentState = State::Red;
        setColor(Qt::red);
        elapsedTime = 0;
    }

    // Set the light to red (pedestrian crossing state)

    remainingTime = stateDuration;
    updateTimeDisplay();

    pedestrianButtonPressed = true;  // Mark that the button has been pressed for this color
}

bool SmartTrafficLight::canCross() const {
    return currentState == State::Green || currentState == State::Yellow;
}

void SmartTrafficLight::resetPedestrianButton() {
    pedestrianButtonPressed = false;  // Reset the pedestrian button when the light changes color
}

void SmartTrafficLight::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    // Emit a signal when the traffic light is clicked
    //emit trafficLightClicked(this);
    //QGraphicsRectItem::mousePressEvent(event); // Call the base class implementation
}

// SmartTrafficSystem Implementation
SmartTrafficSystem::SmartTrafficSystem(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *layout = new QVBoxLayout(this);

    view = new QGraphicsView(this);
    scene = new QGraphicsScene(this);
    view->setScene(scene);

    layout->addWidget(view);

    emergencyButton = new QPushButton("Simulate Emergency", this);
    pedestrianButton = new QPushButton("Simulate Pedestrian Crossing", this);
    trafficSelector = new QComboBox(this);

    layout->addWidget(trafficSelector);
    layout->addWidget(emergencyButton);
    layout->addWidget(pedestrianButton);

    connect(emergencyButton, &QPushButton::clicked, [this]() {
        int selectedIndex = trafficSelector->currentIndex();
        if (selectedIndex >= 0 && selectedIndex < trafficLights.size()) {
            trafficLights[selectedIndex]->triggerEmergency();  // Trigger emergency for the selected traffic light
        }
    });

    connect(pedestrianButton, &QPushButton::clicked, [=]() {
        int selectedIndex = trafficSelector->currentIndex();
        if (selectedIndex >= 0 && selectedIndex < trafficLights.size()) {
            trafficLights[selectedIndex]->triggerPedestrianCrossing(); // Trigger pedestrian crossing for the selected traffic light
        }
    });

    // Create roads and traffic lights
    createRoadsAndLights();

    // Connect the signal from SmartTrafficLight to handle traffic light click
    for (SmartTrafficLight *light : trafficLights) {
        connect(light, &SmartTrafficLight::trafficLightClicked, this, &SmartTrafficSystem::simulatePedestrianCrossing);
        connect(light, &SmartTrafficLight::pedestrianButtonEnabled, this, &SmartTrafficSystem::updatePedestrianButton);
    }
}

void SmartTrafficSystem::createRoadsAndLights() {
    // Set scene dimensions
    scene->setSceneRect(0, 0, 600, 600);

    // Draw horizontal roads (for 2x2 grid)
    for (int i = 200; i <= 400; i += 200) {
        QGraphicsLineItem *road = new QGraphicsLineItem(0, i, 600, i);
        road->setPen(QPen(Qt::gray, 20));
        scene->addItem(road);
    }

    // Draw vertical roads (for 2x2 grid)
    for (int i = 200; i <= 400; i += 200) {
        QGraphicsLineItem *road = new QGraphicsLineItem(i, 0, i, 600);
        road->setPen(QPen(Qt::gray, 20));
        scene->addItem(road);
    }

    // Place traffic lights in a 2x2 grid format
    for (int x = 200; x <= 400; x += 200) {
        for (int y = 200; y <= 400; y += 200) {
            SmartTrafficLight *light = new SmartTrafficLight();
            light->setPos(x, y);
            scene->addItem(light);
            trafficLights.push_back(light);

            // Add light to traffic selector combobox
            trafficSelector->addItem(QString("Light at (%1, %2)").arg(x).arg(y));
        }
    }
}

void SmartTrafficSystem::updatePedestrianButton(bool enabled) {
    pedestrianButton->setEnabled(enabled);
}

void SmartTrafficSystem::simulatePedestrianCrossing(SmartTrafficLight* selectedLight) {
    selectedLight->triggerPedestrianCrossing();
}

void SmartTrafficSystem::synchronizeTraffic() {
    // Implement synchronization logic between traffic lights here
}
