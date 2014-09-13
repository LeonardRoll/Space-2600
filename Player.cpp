#include "Player.h"

Player::Player(int Shields){
	m_Shields = Shields;
	m_Score = 0;
}

void Player::restoreShields(int amount){
	m_Shields += amount;
}
void Player::damageShields(int amount){
	m_Shields -= amount;
}

void Player::addScore(int multiplier){
	m_Score += 1 * multiplier;
}

void Player::reduceScore(int multiplier){
	m_Score -= 1 * multiplier;
}

int Player::getShield(){
	return m_Shields;
}

int Player::getScore(){
	return m_Score;
}

bool Player::ifDead(){
	if(m_Shields <= 0)
		return true;
	else
		return false;
}