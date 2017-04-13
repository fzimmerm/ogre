#ifndef WB_FRAMETIME_LOGGER
#define WB_FRAMETIME_LOGGER

#include <string>
#include <vector>
#include <ctime>

struct WbFrametimeSample {
	WbFrametimeSample(clock_t start)
	: mStart(start), mEnd(start) {}

	clock_t mStart;
	clock_t mEnd;
};

struct WbFrametimeMeasurement {
	WbFrametimeMeasurement(double mean, double variance)
	: mMean(mean), mVariance(variance) {}

	double mMean;
	double mVariance;
};

class WbFrametimeLogger {
	public:
		WbFrametimeLogger(size_t samplesPerMeasurement, size_t totalMeasurements, std::string name, std::string filename, size_t maxPartialSamples = 0);

		void startSample();
		void startPartialSample();
		void endPartialSample();
		void endSample();

		void writeToFile();

	private:
		size_t mSamplesPerMeasurement;
		size_t mTotalMeasurements;
		size_t mSamplesTaken;
		size_t mMeasurementsTaken;
		size_t mMaxPartialSamples;
		size_t mPartialSamplesTaken;

		bool mSamplingStarted;
		double mClocksPerMilliSecond;

		std::vector<WbFrametimeSample> 			mSamples;
		std::vector<WbFrametimeSample> 			mPartialSamples;
		std::vector<double> 								mSampleTimes;
		std::vector<WbFrametimeMeasurement> mMeasurements;

		std::string mName;
		std::string mFilename;
};

#endif
