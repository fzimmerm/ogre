#ifndef WB_FRAMETIME_LOGGER
#define WB_FRAMETIME_LOGGER

#include <string>
#include <vector>

struct WbFrameTime {
	WbFrameTime(clock_t start) 
	: start(start), end(start) {}

	clock_t start;
	clock_t end;
};

class WbFrametimeLogger {
	public:
		WbFrametimeLogger(size_t framesToLog, size_t maxPartials, std::string name, std::string outputPath);

		void startFrame();
		void startPartial();
		void endPartial();
		void endFrame();

		void writeToFile();

	private:
		size_t framesToLog;
		size_t framesLogged;
		size_t maxPartials;
		size_t partialsLogged;

		bool frameStarted;

		std::vector<WbFrameTime> frametimes;
		std::vector<WbFrameTime> partialtimes;

		std::string name;
		std::string outputPath;
};

#endif
