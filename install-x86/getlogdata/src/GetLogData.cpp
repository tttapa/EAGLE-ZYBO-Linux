#include <GetLogData.hpp>

#include <ControllerInstances.hpp>
#include <MiscInstances.hpp>
#include <RCValues.hpp>
#include <Time.hpp>

LogEntry getLogData() {
    LogEntry logentry;
    logentry.altitudeControlSignal = altitudeController.getControlSignal();
    logentry.altitudeIntegralWindup = altitudeController.getIntegralWindup();
    logentry.altitudeMeasurement = altitudeController.getMeasurement();
    logentry.altitudeReference = altitudeController.getReference();
    logentry.altitudeStateEstimate = altitudeController.getStateEstimate();
    logentry.attitudeControlSignal = attitudeController.getControlSignal();
    logentry.attitudeIntegralWindup = attitudeController.getIntegralWindup();
    logentry.attitudeMeasurement = attitudeController.getMeasurement();
    logentry.attitudeReference = attitudeController.getReference();
    logentry.attitudeStateEstimate = attitudeController.getStateEstimate();
    logentry.positionControlSignal = positionController.getControlSignal();
    logentry.positionIntegralWindup = positionController.getIntegralWindup();
    logentry.positionMeasurement = positionController.getMeasurement();
    logentry.positionReference = positionController.getReference();
    logentry.positionStateEstimate = positionController.getStateEstimate();
    logentry.autonomousOutput = autonomousController.getOutput();

    logentry.rcInput = getRCInput();
    logentry.buzzerInstruction = buzzerManager.getCurrentInstruction();


    logentry.millis = getMillis();
    logentry.tickCount = getTickCount();
    logentry.pitchBias = biasManager.getPitchBias();
    logentry.rollBias = biasManager.getRollBias();
    logentry.thrustBias = biasManager.getThrustBias();
    logentry.autonomousHoveringThrust = biasManager.biasManager.getAutonomousHoveringThrust();
    logentry.time = getTime();
    return logentry;
}