#pragma once

class Player{
	private:
		int m_Shields;
		int m_Score;
	public:
		Player(int Shields);
		void restoreShields(int amount);
		void damageShields(int amount);
		void addScore(int multiplier);
		void reduceScore(int multiplier);
		int getShield();
		int getScore();
		bool ifDead();
};

