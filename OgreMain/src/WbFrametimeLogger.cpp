#include "WbFrametimeLogger.hpp"

#include <cassert>
#include <ctime>
#include <fstream>
#include <iostream>

WbFrametimeLogger::WbFrametimeLogger(size_t framesToLog, size_t maxPartials, std::string name, std::string outputPath) 
: framesToLog(framesToLog), framesLogged(0), maxPartials(maxPartials), partialsLogged(0), frameStarted(false), name(name), outputPath(outputPath) {
	frametimes.reserve(framesToLog);
	partialtimes.reserve(maxPartials);
}

void WbFrametimeLogger::startFrame() {
	// if maxPartials == 0, the user indicates that he only uses startFrame and endFrame
	if (! maxPartials)
		frametimes.push_back(WbFrameTime(clock()));
	else
		frameStarted = true;
}

void WbFrametimeLogger::startPartial() {
	if (frameStarted)
		partialtimes.push_back(WbFrameTime(clock()));
}

void WbFrametimeLogger::endPartial() {
	if (frameStarted) {
		partialtimes.back().end = clock();

		++partialsLogged;

		assert(partialsLogged <= maxPartials);
	}
}

void WbFrametimeLogger::endFrame() {
	if (! maxPartials)
		frametimes.back().end = clock();
	else {
		assert(partialsLogged > 0);

		frametimes.push_back(WbFrameTime(partialtimes.front().start));

		for (size_t i = 0; i < partialsLogged; ++i)
			frametimes.back().end += partialtimes[i].end - partialtimes[i].start;

		partialtimes.clear();
		partialsLogged = 0;
	}

	frameStarted = false;
	++framesLogged;

	if (framesLogged == framesToLog) {
		writeToFile();
		frametimes.clear();
		framesLogged = 0;
	}
}

void WbFrametimeLogger::writeToFile() {
	double avg = 0.0;

	for (size_t i = 0; i < framesToLog; ++i)
		avg += static_cast<double>(frametimes[i].end - frametimes[i].start) / CLOCKS_PER_SEC;

	avg /= 1e-3 * framesToLog;

	std::ofstream ofs;
  	ofs.open(outputPath.c_str(), std::ofstream::out | std::ofstream::app);
  	ofs << name << ": " << framesToLog << " frames average: " << avg << " ms" << std::endl;
  	ofs.close();
}
