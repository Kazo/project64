#include "stdafx.h"
#include "Project64Watch.h"
#include "Stadium2.h"
#include "N64System\N64Class.h"
#include "N64System\SystemGlobals.h"
#include <thread>

namespace Stadium2
{
	void Run();
	bool isGameRunning();
	unsigned char Read8(unsigned int Offset);
	unsigned short Read16(unsigned int Offset);
	unsigned int Read32(unsigned int Offset);
	void Read(char* Destination, unsigned int Offset, unsigned int Size);
	void Write8(unsigned int Offset, unsigned char value);
	void Write(char* Data, unsigned int Offset, unsigned int Size);
	void SetInput(unsigned int &buttons, char* Key);
	void SendInput(unsigned int Player, unsigned int buttons);
	void EditNames();
	void InjectTeams();
	void ReadBattleText();
	void ReadTeamData();
	void PickMove(unsigned int player, unsigned int &move);
	void PickAttack(unsigned int player, unsigned int &move);
	void PickPokemon(unsigned int player, unsigned int &move, bool forced);
	void DoMove(unsigned int player, unsigned int move, unsigned int &input);
	void OrderFlip();

	unsigned int FrameCounter;
	unsigned int FrameCounter2;
	bool TeamsReady;
	bool NewTurn;

	char* mBuffer;
	unsigned int aAddress;
	unsigned int aSize;

	//char Name1[0x0C];
	//char Name2[0x0C];
	char Team1[0x60 * 0x03];
	char Team2[0x60 * 0x03];
	char BattleTeams[0x58 * 0x06];
	char BattleText[0x180];
	char BattleText2[0x180];

	void Start()
	{
		std::thread t1(Run);
		t1.detach();
	}

	void Run()
	{
		unsigned int input1;
		unsigned int input2;
		unsigned int cycle = 0;
		unsigned int player = 1;
		unsigned int move1 = 0; //0 undecided; 1-4 moves1-4; 5-7 pokemon1-3
		unsigned int move2 = 0;

		FrameCounter = 0;
		FrameCounter2 = 0;
		TeamsReady = false;

		srand(time(0));
		std::this_thread::sleep_for(std::chrono::milliseconds(3000));//Give it time to load the game.
		while (true)
		{
			if (isGameRunning())
			{
				if (Read32(0x949CC) != FrameCounter2)//Check an always increasing value in memory
				{
					FrameCounter2 = Read32(0x949CC);
					input1 = 0;
					input2 = 0;

					switch (Read8(0x90CC3))
					{
					case 0x00://Intro
					case 0x01://Title
					{
						player = 1;
						move1 = 0;
						move2 = 0;

						SetInput(input1, "A");
					}
					break;

					case 0x02://Menus
					{
						switch (Read8(0x9031B))
						{
						case 0x00://Game Pak Check
						case 0x33://Please Select
						{
							SetInput(input1, "A");
						}
						break;

						case 0x37://Hub World
						{
							switch (Read8(0xA9EF7))
							{
							case 0xA7://Stadium
							{
								SetInput(input1, "DPad R");
							}
							break;
							case 0xEE://Pokémon Academy
							{
								SetInput(input1, "DPad U");
							}
							break;
							case 0xC5://Free Battle
							{
								SetInput(input1, "A");
							}
							break;
							}
						}
						break;
						}
					}
					break;

					case 0x07://Free Battle Setup
					{
						switch (Read8(0xAA6F3))
						{
						case 0x0C://1P
						{
							SetInput(input1, "A");
						}
						break;

						case 0x7C:
						{
							switch (Read8(0xAA6A7))
							{
							case 0x76://COM
							{
								SetInput(input1, "DPad D");
							}
							break;

							case 0x94://2P
							{
								SetInput(input1, "A");
							}
							break;
							}
						}
						break;

						case 0xC5://Free Battle
						{
							SetInput(input1, "DPad U");
						}
						break;

						case 0xAB://Random
						{
							SetInput(input1, "A");
						}
						break;

						default:
						{
							switch (Read8(0xAA8DB))
							{
							case 0x00://Choose Entry
							case 0x10://Anything Goes
							{
								if (player == 1)
								{
									if (Read8(0x146240) == 0x00)//No 1P Team
									{
										SetInput(input1, "A");
									}
									else
									{
										player = 2;
									}
								}
								else if (Read8(0x146480) == 0x00)//No 2P Team
								{
									SetInput(input2, "A");
								}
								else
								{
									InjectTeams();
								}
							}
							break;

							case 0x02://OK
							{
								SetInput(input1, "A");
								SetInput(input2, "A");
							}
							break;

							case 0x26://Team 2
							{
								SetInput(input2, "A");
								while (!TeamsReady)
								{
									std::this_thread::sleep_for(std::chrono::milliseconds(1000));
								}
							}
							break;

							case 0x80://Team 1
							{
								if (player == 1)
								{
									SetInput(input1, "A");
								}
								else
								{
									SetInput(input2, "DPad D");
								}
							}
							break;
							}
						}
						break;
						}
					}
					break;

					case 0x08://In Battle
					{
						ReadBattleText();
						if (Read8(0x91B7C) == 0x01)//Waiting
						{
							if (NewTurn)
							{
								NewTurn = false;
								ReadTeamData();
							}

							if (move1 == 0x00)
							{
								PickMove(1, move1);
							}
							else
							{
								DoMove(1, move1, input1);
							}

							if (move2 == 0x00)
							{
								PickMove(2, move2);
							}
							else
							{
								DoMove(2, move2, input2);
							}
						}
						else
						{
							move1 = 0;
							move2 = 0;
							NewTurn = true;
						}
					}
					break;

					case 0x09://Battle Setup
					{
						if (Read8(0x147D30) == 0x31)//Names
						{
							EditNames();
						}

						if (Read8(0xAA8DB) == 0xE9)//Choose Pokemon
						{
							switch (Read8(0xAA407))
							{
							case 0x03://Pick Pokemon
							case 0xD1://Pick Pokemon
							{
								switch (cycle)
								{
								case 0x00:
								{
									SetInput(input1, "B");
									SetInput(input2, "B");
									cycle++;
								}
								break;

								case 0x01:
								{
									SetInput(input1, "C Left");
									SetInput(input2, "C Left");
									cycle++;
								}
								break;

								case 0x02:
								{
									SetInput(input1, "C Up");
									SetInput(input2, "C Up");
									cycle = 0;
								}
								break;
								}
							}
							break;

							case 0xB0://Yes
							{
								SetInput(input1, "A");
								SetInput(input2, "A");
							}
							break;
							}
						}
					}
					break;

					case 0x0A://Battle Result
					{
						SetInput(input1, "A");
						cycle = rand() % 3;
						if (Read8(0x147D61) != 0x00)//1P Wins!
						{
							Project64Watch::SendBattleResult(0x00);
							Write8(0x147D61, 0x00);
							TeamsReady = false;
							while (!TeamsReady)
							{
								std::this_thread::sleep_for(std::chrono::milliseconds(1000));
							}
							InjectTeams();
						}
						else if (Read8(0x147D63) != 0x00)//2P Wins!
						{
							Project64Watch::SendBattleResult(0x01);
							Write8(0x147D63, 0x00);
							TeamsReady = false;
							while (!TeamsReady)
							{
								std::this_thread::sleep_for(std::chrono::milliseconds(1000));
							}
							InjectTeams();
						}
					}
					break;
					}

					SendInput(0, input1);
					SendInput(1, input2);
					std::this_thread::sleep_for(std::chrono::milliseconds(250));
				}
				else
				{
					Project64Watch::Restart();
				}
			}
			else
			{
				Project64Watch::Restart();
			}
		}
	}

	bool isGameRunning()
	{
		if (Project64Watch::GetFrameCounter() != FrameCounter)
		{
			FrameCounter = Project64Watch::GetFrameCounter();
			return true;
		}
		else
		{
			return false;
		}
	}

	unsigned char Read8(unsigned int Offset)
	{
		aAddress = Offset & 0xFFFFFC;
		aSize = (((0x01 + (Offset & 0x03) + 0x04 - 0x01) / 0x04) * 0x04);
		mBuffer = (char*)realloc(mBuffer, aSize);
		memcpy(mBuffer, g_MMU->Rdram() + aAddress, aSize);
		OrderFlip();

		return mBuffer[(Offset & 0x03)];
	}

	unsigned short Read16(unsigned int Offset)
	{
		aAddress = Offset & 0xFFFFFC;
		aSize = (((0x02 + (Offset & 0x03) + 0x04 - 0x01) / 0x04) * 0x04);
		mBuffer = (char*)realloc(mBuffer, aSize);
		memcpy(mBuffer, g_MMU->Rdram() + aAddress, aSize);
		OrderFlip();

		return mBuffer[(Offset & 0x03) + 1] + (mBuffer[(Offset & 0x03)] << 0x08);

	}

	unsigned int Read32(unsigned int Offset)
	{
		aAddress = Offset & 0xFFFFFC;
		aSize = (((0x04 + (Offset & 0x03) + 0x04 - 0x01) / 0x04) * 0x04);
		mBuffer = (char*)realloc(mBuffer, aSize);
		memcpy(mBuffer, g_MMU->Rdram() + aAddress, aSize);
		OrderFlip();

		return mBuffer[(Offset & 0x03) + 3] + (mBuffer[(Offset & 0x03) + 2] << 0x08) + (mBuffer[(Offset & 0x03) + 1] << 0x10) + (mBuffer[(Offset & 0x03)] << 0x18);
	}

	void Read(char* Destination, unsigned int Offset, unsigned int Size)
	{
		aAddress = Offset & 0xFFFFFC;
		aSize = (((Size + (Offset & 0x03) + 0x04 - 0x01) / 0x04) * 0x04);
		mBuffer = (char*)realloc(mBuffer, aSize);
		memcpy(mBuffer, g_MMU->Rdram() + aAddress, aSize);
		OrderFlip();
		memcpy(Destination, mBuffer + (Offset & 0x03), aSize);
	}

	void Write8(unsigned int Offset, unsigned char Value)
	{
		char temp[4];
		temp[0] = Value;

		aAddress = Offset & 0xFFFFFC;
		aSize = (((0x01 + (Offset & 0x03) + 0x04 - 0x01) / 0x04) * 0x04);

		mBuffer = (char*)realloc(mBuffer, aSize);

		memcpy(mBuffer, g_MMU->Rdram() + aAddress, aSize);
		OrderFlip();

		memcpy(mBuffer + (Offset & 0x03), temp, 0x01);
		OrderFlip();

		memcpy(g_MMU->Rdram() + aAddress, mBuffer, aSize);
	}

	void Write(char* Data, unsigned int Offset, unsigned int Size)
	{
		aAddress = Offset & 0xFFFFFC;
		aSize = (((Size + (Offset & 0x03) + 0x04 - 0x01) / 0x04) * 0x04);

		mBuffer = (char*)realloc(mBuffer, aSize);

		memcpy(mBuffer, g_MMU->Rdram() + aAddress, aSize);
		OrderFlip();

		memcpy(mBuffer + (Offset & 0x03), Data, Size);
		OrderFlip();

		memcpy(g_MMU->Rdram() + aAddress, mBuffer, aSize);
	}

	void SetInput(unsigned int &buttons, char* Key)
	{
		if (strcmp(Key, "DPad R") == 0)
		{
			buttons |= 0x01;
		}
		else if (strcmp(Key, "DPad L") == 0)
		{
			buttons |= (0x01 << 0x01);
		}
		else if (strcmp(Key, "DPad D") == 0)
		{
			buttons |= (0x01 << 0x02);
		}
		else if (strcmp(Key, "DPad U") == 0)
		{
			buttons |= (0x01 << 0x03);
		}
		else if (strcmp(Key, "Start") == 0)
		{
			buttons |= (0x01 << 0x04);
		}
		else if (strcmp(Key, "Z") == 0)
		{
			buttons |= (0x01 << 0x05);
		}
		else if (strcmp(Key, "B") == 0)
		{
			buttons |= (0x01 << 0x06);
		}
		else if (strcmp(Key, "A") == 0)
		{
			buttons |= (0x01 << 0x07);
		}
		else if (strcmp(Key, "C Right") == 0)
		{
			buttons |= (0x01 << 0x08);
		}
		else if (strcmp(Key, "C Left") == 0)
		{
			buttons |= (0x01 << 0x09);
		}
		else if (strcmp(Key, "C Down") == 0)
		{
			buttons |= (0x01 << 0x0A);
		}
		else if (strcmp(Key, "C Up") == 0)
		{
			buttons |= (0x01 << 0x0B);
		}
		else if (strcmp(Key, "R") == 0)
		{
			buttons |= (0x01 << 0x0C);
		}
		else if (strcmp(Key, "L") == 0)
		{
			buttons |= (0x01 << 0x0D);
		}
	}

	void SendInput(unsigned int player, unsigned int buttons)
	{
		Project64Watch::SetVKeys(player, buttons);
	}

	void PickMove(unsigned int player, unsigned int &move)
	{
		unsigned int Offset = 0;
		if (player == 1)
		{
			Offset = 0x1DD265;
		}
		else if (player == 2)
		{
			Offset = 0x1DD27D;
		}

		switch (Read16(Offset))
		{
		case 0x0101://Attack / Switch / Run
		{
			if ((rand() % 10) == 0x00)//Switch Chance
			{
				PickPokemon(player, move, false);
			}
			else
			{
				PickAttack(player, move);
			}
		}
		break;

		case 0x0505://Replace fainted Pokémon
		{
			PickPokemon(player, move, true);
		}
		break;
		}
	}

	void EditNames()
	{
		Write("RED TEAM", 0x147D30, 0x09);
		Write("BLUE TEAM", 0x147D3C, 0x0A);
	}

	void InjectTeams()
	{
		Write(Team1, 0x146240, 0x60 * 3);
		Write(Team2, 0x146480, 0x60 * 3);
	}

	void ReadBattleText()
	{
		Read(BattleText, 0x1E1DF8, 0x180);
		if (BattleText[0] != 0x00)
		{
			if (strcmp(BattleText, BattleText2) != 0x00)
			{
				memcpy(BattleText2, BattleText, 0x180);
				Project64Watch::SendBattleText(BattleText);
			}
		}
	}

	void ReadTeamData()
	{
		Read(BattleTeams, 0xD1DC0, 0x58 * 0x03);
		Read(BattleTeams + (0x58 * 0x03), 0xD22A0, 0x58 * 0x03);
		Project64Watch::SendTeamUpdate(BattleTeams);
	}

	void PickAttack(unsigned int player, unsigned int &move)
	{
		unsigned int randMove = 0x00;
		unsigned int Offset = 0x00;

		if (player == 1)
		{
			Offset = 0xD1CF8;
		}
		else if (player == 2)
		{
			Offset = 0xD1D4C;
		}

		for (unsigned int i = 0; i < 4; i++)
		{
			if (Read8(Offset + i + 0x09) != 0x00)
			{
				randMove++;
			}
		}

		if (randMove != 00)//Checks if all moves are 0 PP
		{
			if ((Read8(Offset + 0x11) & 0x10) == 0x00)//Encore Check
			{
				while (true)
				{
					randMove = rand() % 4;
					if ((Read8(Offset + randMove + 0x09) != 0x00) && (Read8(Offset + randMove + 0x05) != Read8(Offset + 0x25)))//if PP not 0 and move not disabled
					{
						move = randMove + 1;
						break;
					}
				}
			}
			else
			{
				for (unsigned int i = 0; i < 4; i++)
				{
					if (Read8(Offset + 0x03) == Read8(Offset + i + 0x05))//Encore Check
					{
						move = i + 1;
					}
				}
			}
		}
		else//All moves had 0 PP
		{
			move = 1 + (rand() % 4);
		}
	}

	void PickPokemon(unsigned int player, unsigned int &move, bool forced)
	{
		unsigned int mon = 0;
		unsigned int moncount = 0;
		unsigned int activeteam = 0;
		unsigned int activespecies = 0;
		unsigned int activemon1 = 0;
		unsigned int activemon2 = 0;

		if (player == 1)
		{
			activeteam = 0xD1DC0;
			activespecies = 0x1DD237;
			activemon1 = 0xD1CF8;
			activemon2 = 0xD1D4C;
		}
		else if (player == 2)
		{
			activeteam = 0xD22A0;
			activespecies = 0x1DD24F;
			activemon1 = 0xD1D4C;
			activemon2 = 0xD1CF8;
		}

		if ((Read8(activemon1 + 0x1B) == 0x00) && ((Read8(activemon2 + 0x11) & 0x80) == 0x00) || forced)//not trapped from Wrap, Mean Look, etc.
		{
			for (unsigned int i = 0; i < 3; i++)//check to see if we have more than 1 pokemon alive, otherwise can't switch. if forced switch then 1 pokemon left is possible
			{
				if (Read16(activeteam + (0x58 * i) + 0x26) != 0x00)
				{
					moncount++;
				}
			}

			while ((moncount > 1) || forced)
			{
				mon = rand() % 3;
				if ((Read16(activeteam + (0x58 * mon) + 0x26) != 0x00) && (Read8(activeteam + (0x58 * mon)) != Read8(activespecies)))//if HP not 0 and not already active
				{
					break;
				}
			}

			if ((moncount > 1) || forced)
			{
				move = 5 + mon;
			}
		}
	}

	void DoMove(unsigned int player, unsigned int move, unsigned int &input)
	{
		unsigned int Offset = 0x00;

		if (player == 1)
		{
			Offset = 0x1DD265;
		}
		else if (player == 2)
		{
			Offset = 0x1DD27D;
		}

		switch (Read16(Offset))
		{
		case 0x0101://Attack / Switch / Run
		{
			switch (move)
			{
			case 1:
			case 2:
			case 3:
			case 4:
			{
				SetInput(input, "A");
			}
			break;

			case 5:
			case 6:
			case 7:
			{
				SetInput(input, "B");
			}
			break;
			}
		}
		break;

		case 0x0505://Pick Pokemon Forced
		case 0x0501://Pick Pokemon
		{
			switch (move)
			{
			case 5:
			{
				SetInput(input, "C Left");
			}
			break;

			case 6:
			{
				SetInput(input, "C Up");
			}
			break;

			case 7:
			{
				SetInput(input, "C Right");
			}
			break;
			}
		}
		break;

		case 0x0201://Pick Move
		{
			switch (move)
			{
			case 1:
			{
				SetInput(input, "C Up");
			}
			break;

			case 2:
			{
				SetInput(input, "C Right");
			}
			break;

			case 3:
			{
				SetInput(input, "C Down");
			}
			break;

			case 4:
			{
				SetInput(input, "C Left");
			}
			break;
			}
		}
		break;
		}
	}

	void SetTeamsReady()
	{
		TeamsReady = true;
	}

	void SetTeam1(char* data, unsigned int size)
	{
		memcpy(Team1, data, size);
	}

	void SetTeam2(char* data, unsigned int size)
	{
		memcpy(Team2, data, size);
	}

	void OrderFlip()
	{
		char temp = 0;
		for (int i = 0; i < aSize; i += 4)
		{
			temp = mBuffer[i];
			mBuffer[i] = mBuffer[i + 3];
			mBuffer[i + 3] = temp;
			temp = mBuffer[i + 1];
			mBuffer[i + 1] = mBuffer[i + 2];
			mBuffer[i + 2] = temp;
		}
	}
}