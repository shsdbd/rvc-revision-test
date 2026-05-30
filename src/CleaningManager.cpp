#include "rvc/CleaningManager.hpp"

#include <utility>

namespace rvc {

CleaningManager::CleaningManager(ICleaningMotor& cleaningMotor, IDustSensor& dustSensor,
                                 ITimer& powerUpTimer, std::chrono::milliseconds powerUpDuration)
    : cleaningMotor_{&cleaningMotor}, dustSensor_{&dustSensor}, powerUpTimer_{&powerUpTimer},
      powerUpDuration_{powerUpDuration} {
}

CleaningManager::CleaningManager(ICleaner& cleaner, Timer::ClockFn clockFn)
    : cleaner_{&cleaner}, ownedPowerUpTimer_{std::make_unique<Timer>(std::move(clockFn))},
      powerUpTimer_{ownedPowerUpTimer_.get()}, powerUpDuration_{kDefaultPowerUpDuration} {
}

void CleaningManager::start() {
    enterNormal();
}

void CleaningManager::startCleaning() {
    start();
}

void CleaningManager::stop() {
    enterOff();
}

void CleaningManager::stopCleaning() {
    stop();
}

void CleaningManager::powerUp() {
    enterPowerUp();
}

void CleaningManager::handleDustDetected(bool detected) {
    latestDustDetected_ = detected;
}

void CleaningManager::update() {
    tick();
}

void CleaningManager::onDustDetected(MovementState movementState) {
    if (currentState_ == CleaningState::Off) {
        return;
    }

    if (movementState == MovementState::Forward) {
        enterPowerUp();
        return;
    }

    enterOff();
}

void CleaningManager::onMovementStateChanged(MovementState movementState) {
    if (movementState != MovementState::Forward) {
        enterOff();
        return;
    }

    if (currentState_ == CleaningState::Off) {
        enterNormal();
    }
}

void CleaningManager::tick() {
    tick(MovementState::Forward);
}

void CleaningManager::tick(MovementState movementState) {
    if (movementState != MovementState::Forward) {
        enterOff();
        return;
    }

    const bool dustDetected =
        dustSensor_ != nullptr ? dustSensor_->isDustDetected() : latestDustDetected_;
    if (dustDetected && currentState_ != CleaningState::PowerUp) {
        enterPowerUp();
        return;
    }

    if (currentState_ != CleaningState::PowerUp || !powerUpTimer_->expired()) {
        return;
    }

    if (dustDetected) {
        powerUpTimer_->start(powerUpDuration_);
        return;
    }

    enterNormal();
}

CleaningState CleaningManager::currentState() const {
    return currentState_;
}

PowerLevel CleaningManager::getPowerLevel() const {
    return powerLevel_;
}

bool CleaningManager::getLatestDustDetected() const {
    return latestDustDetected_;
}

void CleaningManager::enterOff() {
    powerUpTimer_->reset();
    currentState_ = CleaningState::Off;
    powerLevel_ = PowerLevel::OFF;
    if (cleaningMotor_ != nullptr) {
        cleaningMotor_->off();
        return;
    }
    cleaner_->setPower(PowerLevel::OFF);
}

void CleaningManager::enterNormal() {
    powerUpTimer_->reset();
    currentState_ = CleaningState::Normal;
    powerLevel_ = PowerLevel::NORMAL;
    if (cleaningMotor_ != nullptr) {
        cleaningMotor_->normal();
        return;
    }
    cleaner_->setPower(PowerLevel::NORMAL);
}

void CleaningManager::enterPowerUp() {
    currentState_ = CleaningState::PowerUp;
    powerLevel_ = PowerLevel::POWER_UP;
    powerUpTimer_->start(powerUpDuration_);
    if (cleaningMotor_ != nullptr) {
        cleaningMotor_->powerUp();
        return;
    }
    cleaner_->setPower(PowerLevel::POWER_UP);
}

} // namespace rvc
