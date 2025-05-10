#pragma once
#include <string>

namespace score {
	constexpr int scoreCavang = 20;
	constexpr int scoreBachtuoc = 50;
	inline int getRandomSharkScore() {
		return 130 + (rand() % 671);
	} 
	inline int getScoreLevel1(int level) {
		switch (level) {
		case 1: return scoreCavang;
		default: return 0;
		}
	}
	inline int getScoreLevel2(int level) {
		switch (level) {
		case 2: return scoreBachtuoc;
		default: return 0;
		}
	} 
	inline int getScoreFishAI(const std::string& imagePathAI, int level) {
		if (level == 2) {
			if (imagePathAI == "assets/cavang20.png") return scoreCavang;
			else if (imagePathAI == "assets/bachtuoc50.png") return scoreBachtuoc;
			else if (imagePathAI == "assets/camapbig.png" || imagePathAI == "assets/camapsmall.png") {
				return getRandomSharkScore();
			}
		}
	}
}