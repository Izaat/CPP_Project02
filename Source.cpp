#include "stdio.h"
#include <string>
using namespace std;

#include "consoleGameEngine.h"


class Flappy_FlappyBird : public consoleGameEngine
{
public:
	Flappy_FlappyBird() {
		m_sAppName = L"Flappy Bird";
	}

private:

	float fBirdPosition = 0.0f;
	float fBirdVelocity = 0.0f;
	float fBirdAcceleration = 0.0f;
	float fGravity = 100.0f;
	float fLevelPosition = 0.0f;                                                       //speed offset of obstacles

	float fSectionWidth;                                                               //screen width
	list<int> listSection;                                                             //list obstacle heights in integers

	bool bHasCollided = false;
	bool bResetGame = false;

    //score variables
	int nAttemptCount = 0;
	int nFlapCount = 0;
	int nMaxFlapCount = 0;


protected:
	// Called by consoleGameEngine
	virtual bool OnUserCreate()	{
		listSection = { 0, 0, 0, 0 };                                                //obstacle height ilst set to 0
		bResetGame = true;
		fSectionWidth = (float)ScreenWidth() / (float)(listSection.size() - 1);     //density of obstacles
		return true;
	}

	// Called by consoleGameEngine
	virtual bool OnUserUpdate(float fElapsedTime)	{
		if (bResetGame)	{
			bHasCollided = false;
			bResetGame = false;
			listSection = { 0, 0, 0, 0 };                                         //density of obstacles, add more obstacle within screen with more 0s
			fBirdAcceleration = 0.0f;
			fBirdVelocity = 0.0f;
			fBirdPosition = ScreenHeight() / 2.0f;                                //starting position of bird
			nFlapCount = 0;
			nAttemptCount++;                                                       //every reset = another attempt
		}

		// Game
		if (bHasCollided)	{	                                                 // Pause everything until user releases space to reset
			if (m_keys[VK_SPACE].bReleased)
				bResetGame = true;
		}
		else	{

			if (m_keys[VK_SPACE].bPressed && fBirdVelocity >= fGravity / 10.0f) 	{   //The space key on keyboard and velocity of bird falling
				fBirdAcceleration = 0.0f;                                           //When the bird flaps, acceleration = 0
				fBirdVelocity = -fGravity / 4.0f;                                   //When bird flaps, Velocity is proportional to gravity, negative result for the bird to go up screen
				nFlapCount++;
				if (nFlapCount > nMaxFlapCount)                                      //if flapcount  reach maxflap, then maxflap becomes the current flapcount
					nMaxFlapCount = nFlapCount;
			}
			else	fBirdAcceleration += fGravity * fElapsedTime;                        //bird fall due to gravity

			if (fBirdAcceleration >= fGravity)                                           //Capped to gravity because it is accumulating
				fBirdAcceleration = fGravity;                                                                       

			fBirdVelocity += fBirdAcceleration * fElapsedTime;  
			fBirdPosition += fBirdVelocity * fElapsedTime;
			fLevelPosition += 14.0f * fElapsedTime;                                      //the higher the value, the faster the obstacles

			if (fLevelPosition > fSectionWidth)		{                             //when the bird is going offscreen
				fLevelPosition -= fSectionWidth;
				listSection.pop_front();
				int i = rand() % (ScreenHeight() - 20);                               //size of obstacles, -20 so there will always be obstacle on the screen
				if (i <= 10) i = 0;                                                   //empty space for player to rest (random)
				listSection.push_back(i);                                             //pushing empty space to the back of the obstacle list
			}

			// Display
			Fill(0, 0, ScreenWidth(), ScreenHeight(), L' ');                               //Clear screen before drawing

			// Draw Sections
			int nSection = 0;                                                              //position of the list
			for (auto s : listSection)	{                                              //automatically iterate through the list
				if (s != 0)	{                                                     //if not equal to 0 then draw
					Fill(
                        nSection * fSectionWidth + 10 - fLevelPosition, ScreenHeight() - s,            //top-left coordinate of bottom obstacle
                        nSection * fSectionWidth + 15 - fLevelPosition, ScreenHeight(),                //bottom-right coordinate of bottom obstacle
                        PIXEL_SOLID, FG_GREEN
                    );  
                    
					Fill(
                        nSection * fSectionWidth + 10 - fLevelPosition, 0,                             //from top of scrren
                        nSection * fSectionWidth + 15 - fLevelPosition, ScreenHeight() - s - 15,       //gaps between obstacles is 15
                        PIXEL_SOLID, FG_GREEN
                    );
				}
				nSection++;
			}

			int nBirdX = (int)(ScreenWidth() / 3.0f);                                          //make bird stay at same location, width of console divide by 3

			// Collision Detection
			bHasCollided = fBirdPosition < 2 || fBirdPosition > ScreenHeight() - 2 ||           //whether bird hits top of screen or bottom of screen
				m_bufScreen[(int)(fBirdPosition + 0) * ScreenWidth() + nBirdX].Char.UnicodeChar != L' ' ||
				m_bufScreen[(int)(fBirdPosition + 1) * ScreenWidth() + nBirdX].Char.UnicodeChar != L' ' ||
				m_bufScreen[(int)(fBirdPosition + 0) * ScreenWidth() + nBirdX + 6].Char.UnicodeChar != L' ' ||          //front of the bird
				m_bufScreen[(int)(fBirdPosition + 1) * ScreenWidth() + nBirdX + 6].Char.UnicodeChar != L' ';

			// Draw Bird
			if (fBirdVelocity > 0) {        							//if bird is falling
				DrawString(nBirdX, fBirdPosition + 0, L"IZAAT");
				DrawString(nBirdX, fBirdPosition + 1, L"<TAMRIN=Q");
			}
			else	{   
                //if bird velocity is negative
				DrawString(nBirdX, fBirdPosition + 0, L"<TAMRIN=Q");
				DrawString(nBirdX, fBirdPosition + 1, L"IZAAT");
			}

			DrawString(1, 1, L"Attempt: " + to_wstring(nAttemptCount) + L" Score: " + to_wstring(nFlapCount) + L" High Score: " + to_wstring(nMaxFlapCount));           //display scores
		}

		return true;
	}
};


int main()  {
	// Use consoleGameEngine derived app
	Flappy_FlappyBird game;
	game.ConstructConsole(80, 48, 16, 16);
	game.Start();

	return 0;
}
