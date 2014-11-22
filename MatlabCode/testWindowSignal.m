% This script tests the windowSignal function
close all
figure
plotAll(allSignals, allSignalNames);
suptitle('Signals Before Normalizing');

newSignals = normalizeSignal(allSignals);

figure
plotAll(newSignals, allSignalNames);
suptitle('Signals After Normalizing');
