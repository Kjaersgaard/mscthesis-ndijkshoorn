#pragma once
#include <Windows.h>

#define BOT_ARDRONE_BEHAVIOR_SLEEP 100

enum bot_behavior { BOT_BEHAVIOR_NONE, BOT_BEHAVIOR_ACCEL, BOT_BEHAVIOR_CRUISE, BOT_BEHAVIOR_DEACCEL, BOT_BEHAVIOR_APPROACH };

class bot_ardrone;

static DWORD WINAPI start_behavior_thread(void* Param);


class bot_ardrone_behavior
{
public:
	bot_ardrone_behavior(bot_ardrone *b);
	~bot_ardrone_behavior();
	void map();
	void waypoint();
	void fixed_height();
	void forcefield();
	void stop();
	void circle();

	bool flyto(float x, float y, float speed = 700.0f);
	bool heightto(float z);

private:
	bot_ardrone *bot;
	HANDLE thread;
};