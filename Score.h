#pragma once
#include <string>

namespace score {
	constexpr int scoreLevel1 = 20; 
	constexpr int scoreLevel2 = 50; 
	inline int getScoreLevel1(int level) {
		switch (level) {
		case 1: return scoreLevel1;
		default: return 0; 
		}
	}
	inline int getScoreLevel2(int level) {
		switch (level) {
		case 2:
			return scoreLevel2;
		default: return 0;
		} 
	}
	inline int getScoreFishAI(const std::string& imagePath, int level) {
		if (level == 2) {
			if (imagePath == "assets/bachtuoc50.png") {
				return scoreLevel2; 
			}
			return scoreLevel1; 
		}
		return scoreLevel1; 
	}
}