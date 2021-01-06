#include "../../../../fxobjects/fxobjects.h"

class WDFButterLPF3 : public IAudioSignalProcessor {
public:
	WDFButterLPF3() { createWDF(); }
	~WDFButterLPF3() {}
protected:
	WdfSeriesAdaptor seriesAdaptor_L1;
	WdfParallelAdaptor parallelAdaptor_C1;
	WdfSeriesTerminatedAdaptor seriesTerminatedAdaptor_L2;

	void createWDF() {
		seriesAdaptor_L1.setComponent(wdfComponent::L, 95.49e-3);
		parallelAdaptor_C1.setComponent(wdfComponent::C, 0.5305e-6);
		seriesTerminatedAdaptor_L2.setComponent(wdfComponent::L, 95.49e-3);
		
		/*connections*/
		WdfAdaptorBase::connectAdaptors(&seriesAdaptor_L1, &parallelAdaptor_C1);
		WdfAdaptorBase::connectAdaptors(&parallelAdaptor_C1, &seriesTerminatedAdaptor_L2);

		/*set resistances*/
		seriesAdaptor_L1.setSourceResistance(600.0);
		seriesTerminatedAdaptor_L2.setTerminalResistance(600.0);
	}

	virtual bool reset(double sampleRate) {
		seriesAdaptor_L1.reset(sampleRate);
		parallelAdaptor_C1.reset(sampleRate);
		seriesTerminatedAdaptor_L2.reset(sampleRate);

		seriesAdaptor_L1.initializeAdaptorChain();
		return true;
	}

	virtual double processAudioSample(double xn) {
		seriesAdaptor_L1.setInput1(xn);
		return seriesTerminatedAdaptor_L2.getOutput2();
	}
};