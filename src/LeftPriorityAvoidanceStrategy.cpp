#include "rvc/IObstacleAvoidanceStrategy.hpp"

namespace rvc {

AvoidanceAction LeftPriorityAvoidanceStrategy::decideAfterFrontObstacle(bool leftDetected) const {
    return leftDetected ? AvoidanceAction::CheckRightPath : AvoidanceAction::TurnLeft;
}

AvoidanceAction LeftPriorityAvoidanceStrategy::decideAfterBackwardTick(bool leftDetected) const {
    return leftDetected ? AvoidanceAction::CheckRightPath : AvoidanceAction::TurnLeft;
}

} // namespace rvc
