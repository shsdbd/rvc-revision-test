#pragma once

#include "rvc/Types.hpp"

namespace rvc {

class IObstacleAvoidanceStrategy {
public:
    virtual ~IObstacleAvoidanceStrategy() = default;

    virtual AvoidanceAction decideAfterFrontObstacle(bool leftDetected) const = 0;
    virtual AvoidanceAction decideAfterBackwardTick(bool leftDetected) const = 0;
};

class LeftPriorityAvoidanceStrategy final : public IObstacleAvoidanceStrategy {
public:
    AvoidanceAction decideAfterFrontObstacle(bool leftDetected) const override;
    AvoidanceAction decideAfterBackwardTick(bool leftDetected) const override;
};

} // namespace rvc
