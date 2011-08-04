#include "global.h"
#include "slam.h"
#include "bot_ardrone.h"

#include <cv.hpp>

using namespace cv;


slam::slam():
	KF(12, 3, 0)
{
	running = false;
	KF_running = false;

	m_frame = NULL;
	m_sensor = NULL;
	m_ui = NULL;

	/*
	if (!running)
		run();

    IplImage* img1 = cvLoadImage("puzzel1.jpg");
    IplImage* img2 = cvLoadImage("puzzel2.jpg");
	IplImage* img3 = cvLoadImage("puzzel5.jpg");

	process_frame(img1);
	cvReleaseImage(&img1);
	process_frame(img2);
	cvReleaseImage(&img2);
	process_frame(img3);
	*/

	//run();
}


slam::~slam()
{
}


void slam::run()
{
	running = true;

	init_kf();


	/* modules */
	m_frame = new slam_module_frame((slam*) this);
	m_sensor = new slam_module_sensor((slam*) this);
	m_ui = new slam_module_ui((slam*) this);


	/* start threads */
	thread_process_frame = CreateThread(NULL, 0, start_process_frame, (void*) this, 0, NULL);
	thread_process_sensor = CreateThread(NULL, 0, start_process_sensor, (void*) this, 0, NULL);
	thread_ui = CreateThread(NULL, 0, start_ui, (void*) this, 0, NULL);
}


void slam::init_kf()
{
	// F vector
	setIdentity(KF.transitionMatrix); // completed (T added) when measurement received and T is known

	setIdentity(KF.processNoiseCov, Scalar::all(1e-5));
	setIdentity(KF.errorCovPost, Scalar::all(1));

	// random initial state
	//randn(KF.statePost, Scalar::all(0), Scalar::all(0.1));
	KF.statePost = 0.0f;
}


void slam::add_input_frame(bot_ardrone_frame *f)
{
	if (!running)
		run();

	// frame module not enable: drop frame
	if (m_frame == NULL)
		delete f;

	// drop frame is queue enabled but not empty
	if (SLAM_USE_QUEUE && queue_frame.empty())
		queue_frame.push(f);
	else if(!SLAM_USE_QUEUE)
		m_frame->process(f);
}


void slam::add_input_sensor(bot_ardrone_measurement *m)
{
	if (!running)
		run();

	if (SLAM_USE_QUEUE)
		queue_sensor.push(m);
	else if(!SLAM_USE_QUEUE)
		m_sensor->process(m);
}


void slam::get_world_position(float *pos)
{
	pos[0] = KF.statePost.at<float>(0);
	pos[1] = KF.statePost.at<float>(1);
	pos[2] = KF.statePost.at<float>(2);
	pos[3] = KF.statePost.at<float>(9);
	pos[4] = KF.statePost.at<float>(10);
	pos[5] = KF.statePost.at<float>(11);
}



/* threads */
static DWORD WINAPI start_process_frame(void* Param)
{
	slam* This = (slam*) Param; 
	slam_queue<bot_ardrone_frame*> *q = &This->queue_frame;
	bot_ardrone_frame *item;
	slam_module_frame *processor = This->m_frame;

	while (!exit_application)
	{
		if (q->empty())
			q->wait_until_filled(2000);

		// just in case
		if (q->empty())
			continue;

		item = q->front();

		processor->process(item);

		q->pop();

		/* free memory */
		delete item;
	}

	return 1;
}


static DWORD WINAPI start_process_sensor(void* Param)
{
	slam* This = (slam*) Param; 
	slam_queue<bot_ardrone_measurement*> *q = &This->queue_sensor;
	bot_ardrone_measurement *item;
	slam_module_sensor *processor = This->m_sensor;

	while (!exit_application)
	{
		if (q->empty())
			q->wait_until_filled(2000);

		// just in case
		if (q->empty())
			continue;

		item = q->front();

		processor->process(item);

		q->pop();

		/* free memory */
		delete item;
	}

	return 1;
}


static DWORD WINAPI start_ui(void* Param)
{
	slam* This = (slam*) Param; 
	slam_module_ui *processor = This->m_ui;

	while (!exit_application)
	{
		processor->update();
		Sleep(35);
	}

	return 1;
}