#include "WbFrametimeLogger.hpp"

#include <cassert>
#include <ctime>
#include <fstream>
#include <iostream>

WbFrametimeLogger::WbFrametimeLogger(size_t samplesPerMeasurement, size_t totalMeasurements, std::string name, std::string filename, size_t maxPartialSamples) 
: mSamplesPerMeasurement(samplesPerMeasurement), 
  mTotalMeasurements(totalMeasurements),
  mSamplesTaken(0), 
  mMeasurementsTaken(0),
  mMaxPartialSamples(maxPartialSamples), 
  mPartialSamplesTaken(0), 
  mSamplingStarted(false),
  mClocksPerMilliSecond(1e-3 * static_cast<double>(CLOCKS_PER_SEC)),
  mName(name), 
  mFilename(filename) {
	mSamples.reserve(mSamplesPerMeasurement);
	mPartialSamples.reserve(mMaxPartialSamples);
	mSampleTimes.reserve(mSamplesPerMeasurement);
	mMeasurements.reserve(mTotalMeasurements);
}

void WbFrametimeLogger::startSample() {
	// if maxPartialSamples == 0, we will only sample between startFrame and endFrame
	if (! mMaxPartialSamples)
		mSamples.push_back(WbFrametimeSample(clock()));
	else
		mSamplingStarted = true;
}

void WbFrametimeLogger::startPartialSample() {
	if (mSamplingStarted)
		mPartialSamples.push_back(WbFrametimeSample(clock()));
}

void WbFrametimeLogger::endPartialSample() {
	if (mSamplingStarted) {
		mPartialSamples.back().mEnd = clock();

		++mPartialSamplesTaken;

		assert(mPartialSamplesTaken <= mMaxPartialSamples);
	}
}

void WbFrametimeLogger::endSample() {
	if (! mMaxPartialSamples)
		mSamples.back().mEnd = clock();
	else {
		assert(mPartialSamplesTaken > 0);

		mSamples.push_back(WbFrametimeSample(mPartialSamples.front().mStart));

		for (size_t i = 0; i < mPartialSamplesTaken; ++i)
			mSamples.back().mEnd += mPartialSamples[i].mEnd - mPartialSamples[i].mStart;

		mPartialSamples.clear();
		mPartialSamplesTaken = 0;
	}

	mSamplingStarted = false;
	++mSamplesTaken;

	// std::cout << "Taken sample " << mSamplesTaken << " of " << mSamplesPerMeasurement << std::endl;

	if (mSamplesTaken == mSamplesPerMeasurement) {
		// std::cout << "Producing measurement from samples" << std::endl;
		double mean = 0.0, variance = 0.0;

		// compute mean
		for (size_t i = 0; i < mSamplesPerMeasurement; ++i) {
			mSampleTimes.push_back(static_cast<double>((mSamples[i].mEnd - mSamples[i].mStart) / mClocksPerMilliSecond));
			// std::cout << "mSampleTime[" << i << "]: " << mSampleTimes[i] << std::endl;
			mean += mSampleTimes[i];
		}
		mean /= static_cast<double>(mSamplesPerMeasurement);

		// compute variance
		for (size_t i = 0; i < mSamplesPerMeasurement; ++i) {
			variance += mSampleTimes[i] * mSampleTimes[i];
		}
		variance = (variance / static_cast<double>(mSamplesPerMeasurement)) - (mean * mean);

		mMeasurements.push_back(WbFrametimeMeasurement(mean, variance));

		// std::cout << "mMeasurements[" << mMeasurementsTaken << "]: mean=" << mMeasurements.back().mMean << ", variance=" << mMeasurements.back().mVariance << std::endl;

		++mMeasurementsTaken;

		mSamples.clear();
		mSampleTimes.clear();
		mSamplesTaken = 0;

		// only write to disk every 'mTotalMeasurements' measurements
		if (mMeasurementsTaken == mTotalMeasurements) {
			// std::cout << "Writing measurements to file" << std::endl;
			writeToFile();

			mMeasurements.clear();
			mMeasurementsTaken = 0;
		}
	}
}

void WbFrametimeLogger::writeToFile() {
	std::ofstream ofs;

	ofs.open(mFilename.c_str(), std::ofstream::out | std::ofstream::app);
	ofs << mName << ": took " << mTotalMeasurements << " measurements of " << mSamplesPerMeasurement << " samples" << std::endl;
	for (size_t i=0; i < mTotalMeasurements; ++i) {
		ofs << "Measurement " << i << ": mean is " << mMeasurements[i].mMean << "ms, variance is " << mMeasurements[i].mVariance << "ms" << std::endl;
	}
	ofs.close();
}
