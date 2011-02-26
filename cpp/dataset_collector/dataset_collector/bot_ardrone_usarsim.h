#pragma once
#include "botinterface.h"
#include "mysocket.h"
#include <string>

#define BOT_ARDRONE_USARSIM_SOCKET_CONTROL 1
#define BOT_ARDRONE_USARSIM_SOCKET_CAM 2
#define BOT_ARDRONE_USARSIM_FRAME_REQDELAY 100

class bot_ardrone;
struct bot_ardrone_control;
struct bot_ardrone_frame;

using namespace std;

class bot_ardrone_usarsim : public botinterface
{
public:
	bot_ardrone_usarsim(bot_ardrone *bot);
	~bot_ardrone_usarsim(void);

	void init();
	void control_update(bot_ardrone_control &control);
	void control_send(char *message);
	void socket_callback(int id, char *message, int bytes);
	void process_measurement(char *message, int bytes);
	void process_frame(char *message, int bytes);

	bot_ardrone_frame *frame;

private:
	bot_ardrone *bot;
	mysocket *control_socket;
	mysocket *frame_socket;
};