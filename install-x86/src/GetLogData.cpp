#include <GetLogData.hpp>

LogEntry getLogData() {
    LogEntry logentry;
    logentry.attitudeObserverState = attitudeController.getStateEstimate();
    logentry.attitudeControlSignals = attitudeController.getControlSignal();
    logentry.referenceOrientation = attitudeController.getReferenceQuat();
    logentry.altitudeObserverState = altitudeController.getStateEstimate();
    logentry.altitudeControlSignal = altitudeController.getControlSignal().ut;
    logentry.referenceHeight = altitudeController.getReferenceHeight();
    logentry.navigationObserverState = positionController.getStateEstimate();
    logentry.positionControlSignal = positionController.getControlSignal();
    logentry.referenceLocation = positionController.(getReferencePosition)();
    logentry.frametime = getFrameTime();
    return logentry;
}