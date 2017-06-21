namespace Project64Watch
{
	void Start(int port);
	void SendTeamUpdate(char* TeamData);
	void SendBattleText(char* TextData);
	void SendBattleResult(unsigned int player);
	void UpdateFrameCounter();
	unsigned int GetFrameCounter();
	void Restart();
	uint32_t GetVKeys(int32_t Control, bool timer);
	void SetVKeys(unsigned int player, unsigned int keys);
}