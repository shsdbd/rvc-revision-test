#include "rvc/RvcController.hpp"

namespace rvc {
namespace {

inline constexpr int kMaxSynchronousEscapeChecks{16};

class OffState final : public IRvcState {
public:
    void startCleaning(RvcController& context) override {
        context.frontSensor().initialize();
        context.sideSensor().initialize();
        context.dustSensor().initialize();
        context.frontSensor().registerInterruptHandler([&context]() {
            context.onFrontObstacleDetected();
        });
        context.startForwardCleaning();
        context.changeState(makeForwardState());
    }

    [[nodiscard]] MovementState movementState() const override {
        return MovementState::Off;
    }
};

class ForwardState final : public IRvcState {
public:
    void onEnter(RvcController& context) override {
        context.cleaningManager().onMovementStateChanged(MovementState::Forward);
    }

    void onFrontObstacleDetected(RvcController& context) override {
        context.stopMovementAndCleaning();
        context.changeState(makeStoppedForObstacleState());
    }

    [[nodiscard]] MovementState movementState() const override {
        return MovementState::Forward;
    }
};

class StoppedForObstacleState final : public IRvcState {
public:
    void onEnter(RvcController& context) override {
        const bool leftDetected = context.sideSensor().readLeft();
        const AvoidanceAction action =
            context.avoidanceStrategy().decideAfterFrontObstacle(leftDetected);

        if (action == AvoidanceAction::CheckRightPath) {
            context.movementManager().turnRight();
        } else {
            context.movementManager().turnLeft();
        }

        bool frontBlocked = context.frontSensor().isObstacleDetected();
        if (frontBlocked) {
            if (action != AvoidanceAction::TurnLeft) {
                context.movementManager().turnLeft();
            }
            context.movementManager().moveBackward();

            for (int i = 0; i < kMaxSynchronousEscapeChecks; ++i) {
                if (!context.sideSensor().readLeft()) {
                    context.movementManager().turnLeft();
                    break;
                }

                context.movementManager().turnRight();
                frontBlocked = context.frontSensor().isObstacleDetected();
                if (frontBlocked) {
                    context.movementManager().turnLeft();
                    continue;
                }

                break;
            }
        }

        context.startForwardCleaning();
        context.changeState(makeForwardState());
    }

    [[nodiscard]] MovementState movementState() const override {
        return MovementState::StoppedForObstacle;
    }
};

class TurningState final : public IRvcState {
public:
    void onEnter(RvcController& context) override {
        context.cleaningManager().onMovementStateChanged(movementState());
        context.movementManager().turnLeft();
    }

    void tick(RvcController& context) override {
        if (!context.movementManager().isTurnComplete()) {
            return;
        }

        context.startForwardCleaning();
        context.changeState(makeForwardState());
    }

    [[nodiscard]] MovementState movementState() const override {
        return MovementState::TurningLeft;
    }
};

class RightPathCheckState final : public IRvcState {
public:
    void onEnter(RvcController& context) override {
        context.cleaningManager().onMovementStateChanged(MovementState::TurningRight);
        context.movementManager().turnRight();
    }

    void tick(RvcController& context) override {
        if (!context.movementManager().isTurnComplete()) {
            return;
        }

        if (context.frontSensor().isObstacleDetected()) {
            context.changeState(makeReturnFromRightCheckState());
            return;
        }

        context.startForwardCleaning();
        context.changeState(makeForwardState());
    }

    [[nodiscard]] MovementState movementState() const override {
        return MovementState::TurningRight;
    }
};

class ReturnFromRightCheckState final : public IRvcState {
public:
    void onEnter(RvcController& context) override {
        context.cleaningManager().onMovementStateChanged(MovementState::TurningLeft);
        context.movementManager().turnLeft();
    }

    void tick(RvcController& context) override {
        if (!context.movementManager().isTurnComplete()) {
            return;
        }

        context.changeState(makeBackwardState());
    }

    [[nodiscard]] MovementState movementState() const override {
        return MovementState::TurningLeft;
    }
};

class BackwardState final : public IRvcState {
public:
    void onEnter(RvcController& context) override {
        context.cleaningManager().onMovementStateChanged(MovementState::Backward);
        context.movementManager().moveBackward();
    }

    void tick(RvcController& context) override {
        context.stopMovementAndCleaning();
        const bool leftDetected = context.sideSensor().readLeft();
        const AvoidanceAction action =
            context.avoidanceStrategy().decideAfterBackwardTick(leftDetected);

        if (action == AvoidanceAction::CheckRightPath) {
            context.changeState(makeRightPathCheckState());
            return;
        }

        context.changeState(makeTurningState(AvoidanceAction::TurnLeft));
    }

    [[nodiscard]] MovementState movementState() const override {
        return MovementState::Backward;
    }
};

} // namespace

std::unique_ptr<IRvcState> makeOffState() {
    return std::make_unique<OffState>();
}

std::unique_ptr<IRvcState> makeForwardState() {
    return std::make_unique<ForwardState>();
}

std::unique_ptr<IRvcState> makeStoppedForObstacleState() {
    return std::make_unique<StoppedForObstacleState>();
}

std::unique_ptr<IRvcState> makeTurningState(AvoidanceAction) {
    return std::make_unique<TurningState>();
}

std::unique_ptr<IRvcState> makeRightPathCheckState() {
    return std::make_unique<RightPathCheckState>();
}

std::unique_ptr<IRvcState> makeReturnFromRightCheckState() {
    return std::make_unique<ReturnFromRightCheckState>();
}

std::unique_ptr<IRvcState> makeBackwardState() {
    return std::make_unique<BackwardState>();
}

} // namespace rvc
