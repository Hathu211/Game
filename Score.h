#pragma once
namespace score {
	constexpr int scoreLevel1 = 20; 
	inline int getScoreLevel1(int level) {
		switch (level) {
		case 1: return scoreLevel1;
		default: return 0; 
		}
	}
}