#include "global.h"
#include "bot_ardrone.h"
#include "bot_ardrone_usarsim.h"
#include "bot_ardrone_recorder.h"

using namespace std;

clock_t bot_ardrone::start_clock = 0;

#undef memset

bot_ardronBOT_EVENT_CONTROL::bot_ardronBOT_EVENT_CONTROL()
{
	memset(this, 0, sizeof(bot_ardronBOT_EVENT_CONTROL));
	state = BOT_STATE_LANDED;
}


bot_ardronBOT_EVENT_MEASUREMENT::bot_ardronBOT_EVENT_MEASUREMENT()
{
	memset(this, 0, sizeof(bot_ardronBOT_EVENT_MEASUREMENT));
	time = bot_ardrone::get_clock();
	usarsim = false;
}


bot_ardronBOT_EVENT_FRAME::bot_ardronBOT_EVENT_FRAME()
{
	memset(this, 0, sizeof(bot_ardronBOT_EVENT_FRAME));
	this->data = new char[BOT_ARDRONBOT_EVENT_FRAME_BUFSIZE];
	data_start = this->data;
}


bot_ardrone::bot_ardrone(int botinterface)
{
	start_clock = clock();
	i = NULL;
	recorder = NULL;
	record = playback = false;
	battery = NULL;
	enable_stitching = false;

	control_reset();

	/* INTERFACE */
	switch (botinterface)
	{
		case BOT_ARDRONE_INTERFACE_USARSIM:
			i = new bot_ardrone_usarsim((bot_ardrone*) this);
			break;

		case BOT_ARDRONE_INTERFACE_ARDRONELIB:
			i = new bot_ardrone_ardronelib((bot_ardrone*) this);
			break;

		default:
			printf("ARDRONE: NO INTERFACE USED\n");
	}

	if (i != NULL)
		i->init();

	/* SLAM */
	slamcontroller = new slam();
	if (SLAM_ENABLED)
		slamcontroller->run();
}


bot_ardrone::~bot_ardrone(void)
{
}


void bot_ardrone::control_set(int type, int opt, float val)
{
	switch(type)
	{
		case BOT_ARDRONE_Velocity:
			control.velocity[opt] = val;
			control.state = BOT_STATE_FLY;
			break;

		default:
			break;
	}
}


float bot_ardrone::control_get(int type, int opt)
{
	switch(type)
	{
		case BOT_ARDRONE_Velocity:
			return control.velocity[opt];

		default:
			return 0.0f;
	}
}


void bot_ardrone::control_update()
{
	control.time = get_clock();

	control_update(&control);
}


void bot_ardrone::control_update(bot_ardronBOT_EVENT_CONTROL *c)
{
	if (PRINT_DEBUG)
		printf("%f - ARDRONE: control update!\n", c->time);

	if(record)
		recorder->record_control(&control);

	if (i != NULL)
		i->control_update((void*) &control);

	// if (SLAM_USE_QUEUE)
}


void bot_ardrone::control_reset()
{
	control.velocity[BOT_ARDRONE_AltitudeVelocity] = 0.0;
	control.velocity[BOT_ARDRONE_LinearVelocity] = 0.0;
	control.velocity[BOT_ARDRONE_LateralVelocity] = 0.0;
	control.velocity[BOT_ARDRONE_RotationalVelocity] = 0.0;

	if (control.state == BOT_STATE_FLY)
		control.state = BOT_STATE_HOVER;
}


void bot_ardrone::take_off()
{
	i->take_off();
	control.state = BOT_STATE_HOVER;
}


void bot_ardrone::land()
{
	i->land();
	control.state = BOT_STATE_LANDED;
}


void bot_ardrone::measurement_received(bot_ardronBOT_EVENT_MEASUREMENT *m)
{
	if (exit_dataset_collector)
		return;

	if (PRINT_DEBUG)
		printf("%f - ARDRONE: measurement received!\n", m->time);

	if (record)
		recorder->record_measurement(m);

	// if(SLAM_USE_QUEUE)
}


void bot_ardrone::frame_received(bot_ardronBOT_EVENT_FRAME *f)
{
	if (exit_dataset_collector)
		return;

	if (PRINT_DEBUG)
		printf("%f - ARDRONE: frame received: %s!\n", f->time, f->filename);

	// time since last frame
	/*double diffticks = ((double)clock() - lastframe_time) / CLOCKS_PER_SEC;
	if (diffticks < BOT_ARDRONE_MIN_FRAME_INTERVAL)
	{
		printf("Skipping frame\n");
		//return;
	}*/

	lastframe_time = clock();

	if (record && BOT_ARDRONE_RECORD_FRAMES)
		recorder->record_frame(f);

	if (SLAM_ENABLED && enable_stitching)
	{
		if (SLAM_USE_QUEUE)
		{
			slam_queue_item queue_item = {FRAME, f};
			slamcontroller->slam_queue.push(queue_item);
			//printf("sending slam_queue_pushed, queue size: %i\n", slamcontroller->slam_queue.size());
			SetEvent(slamcontroller->slam_queue_pushed);
		}
		else
		{
			slamcontroller->process_frame(f);
		}
	}
}


double bot_ardrone::get_clock()
{
	return ((double)clock() - start_clock) / CLOCKS_PER_SEC;
}


void bot_ardrone::set_record()
{
	if (recorder == NULL)
	{
		recorder = new bot_ardrone_recorder((bot_ardrone*) this);
		recorder->prepare_dataset();
		record = true;
	}
}


void bot_ardrone::set_playback(char *dataset)
{
	if (recorder == NULL)
	{
		recorder = new bot_ardrone_recorder((bot_ardrone*) this);
		recorder->playback(dataset);
		playback = true;
	}
}