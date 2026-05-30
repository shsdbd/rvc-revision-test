#pragma once

#include <chrono>
#include <cstdint>

namespace rvc {

enum class MovementState : std::uint8_t {
    Off,
    Forward,
    StoppedForObstacle,
    TurningLeft,
    TurningRight,
    Backward
};

enum class CleaningState : std::uint8_t {
    Off,
    Normal,
    PowerUp
};

enum class MovementCommand : std::uint8_t {
    Stop,
    Forward,
    Backward,
    TurnLeft,
    TurnRight
};

enum class AvoidanceAction : std::uint8_t {
    TurnLeft,
    CheckRightPath
};

enum class Direction : std::uint8_t {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    STOP
};

enum class PowerLevel : std::uint8_t {
    OFF,
    NORMAL,
    POWER_UP
};

inline constexpr std::chrono::milliseconds kDefaultTurnDuration{4000};
inline constexpr std::chrono::milliseconds kDefaultPowerUpDuration{3000};

} // namespace rvc
