#include "bot_ardrone_recorder.h"
#include "bot_ardrone.h"
#include "yaml/include/yaml.h"
#include <io.h>


/* operators */
YAML::Emitter& operator << (YAML::Emitter& out, const double *d)
{
	int i;
	out << YAML::BeginSeq;
	for(i = 0; i < 3; i++)
		out << d[i];
	out << YAML::EndSeq;
	return out;
}


YAML::Emitter& operator << (YAML::Emitter& out, const float *f)
{
	int i;
	out << YAML::BeginSeq;
	for(i = 0; i < 3; i++)
		out << f[i];
	out << YAML::EndSeq;
	return out;
}


void operator >> (const YAML::Node& node, double *d)
{
	int i;
	for(i = 0; i < 3; i++)
		node[i] >> d[i];
}


void operator >> (const YAML::Node& node, float *f)
{
	int i;
	for(i = 0; i < 3; i++)
		node[i] >> f[i];
}
/* end operators */



bot_ardrone_recorder::bot_ardrone_recorder(bot_ardrone *bot)
{
	this->bot = bot;

	frame_counter = 1;
}


bot_ardrone_recorder::~bot_ardrone_recorder(void)
{
	// todo
}


void bot_ardrone_recorder::record_measurement(bot_ardrone_measurement *m)
{
	YAML::Emitter yaml;
	yaml << YAML::BeginMap;
	yaml << YAML::Key << "event" << YAML::Value << BOT_ARDRONE_EVENT_MEASUREMENT;
	yaml << YAML::Key << "time" << YAML::Value << m->time;
	yaml << YAML::Key << "type" << YAML::Value << m->type;
	yaml << YAML::Key << "battery" << YAML::Value << m->battery;
	yaml << YAML::Key << "sensor" << YAML::Value << m->sensor;
	yaml << YAML::Key << "gt_loc" << YAML::Value << m->gt_loc;
	yaml << YAML::Key << "gt_or" << YAML::Value << m->gt_or;
	yaml << YAML::Key << "ins_loc" << YAML::Value << m->ins_loc;
	yaml << YAML::Key << "ins_or" << YAML::Value << m->ins_or;
	yaml << YAML::Key << "sonar" << YAML::Value << m->sonar;
	yaml << YAML::EndMap;

	fout << yaml.c_str();
	fout << '\n';
}


void bot_ardrone_recorder::record_control(bot_ardrone_control *c)
{
	YAML::Emitter yaml;
	yaml << YAML::BeginMap;
	yaml << YAML::Key << "event" << YAML::Value << BOT_ARDRONE_EVENT_CONTROL;
	yaml << YAML::Key << "time" << YAML::Value << c->time;
	yaml << YAML::Key << "velocity" << YAML::Value << c->velocity;
	yaml << YAML::EndMap;

	fout << yaml.c_str();
	fout << '\n';
}


void bot_ardrone_recorder::record_frame(bot_ardrone_frame *f)
{
	char filename[25];

	sprintf_s(f->filename, 20, "%06d.jpg", frame_counter++);
	sprintf_s(filename, 25, "%s/%s", dataset_dir, f->filename);

	YAML::Emitter yaml;
	yaml << YAML::BeginMap;
	yaml << YAML::Key << "event" << YAML::Value << BOT_ARDRONE_EVENT_FRAME;
	yaml << YAML::Key << "time" << YAML::Value << f->time;
	yaml << YAML::Key << "data_size" << YAML::Value << f->data_size;
	yaml << YAML::Key << "filename" << YAML::Value << f->filename;
	yaml << YAML::EndMap;

	fout << yaml.c_str();
	fout << '\n';

	ofstream frame_out(filename, ios::out | ios::binary);
	frame_out.write(f->data, f->data_size);
	frame_out.close();
}


void bot_ardrone_recorder::playback(char *dataset)
{
	char filename[25];

	sprintf_s(dataset_dir, 25, "dataset/%s", dataset);
	sprintf_s(filename, 25, "%s/output.yaml", dataset_dir);

	// check
	if (fin.is_open())
		printf("ERROR: DATASET PLAYBACK FILE ALREADY OPEN!\n");

	int event_type;
	YAML::Node doc;

	fin.open(filename, ios::in);
	YAML::Parser parser(fin);

    while(parser.GetNextDocument(doc))
	{
		doc["event"] >> event_type;

		switch (event_type)
		{
			case BOT_ARDRONE_EVENT_MEASUREMENT:
			{
				bot_ardrone_measurement m;
				doc["time"] >> m.time;
				doc["type"] >> m.type;
				doc["sensor"] >> m.sensor;
				doc["battery"] >> m.battery;
				doc["gt_loc"] >> m.gt_loc;
				doc["gt_or"] >> m.gt_or;
				doc["ins_loc"] >> m.ins_loc;
				doc["ins_or"] >> m.ins_or;
				doc["sonar"] >> m.sonar;

				bot->measurement_received(&m);
				break;
			}
		
			case BOT_ARDRONE_EVENT_CONTROL:
			{
				bot_ardrone_control c;
				doc["time"] >> c.time;
				doc["velocity"] >> c.velocity;

				bot->control_update(&c);
				break;
			}

			case BOT_ARDRONE_EVENT_FRAME:
			{
				char filename[30];
				string tmpstring;
				bot_ardrone_frame f;
				doc["time"] >> f.time;
				doc["data_size"] >> f.data_size;
				doc["filename"] >> tmpstring;
				strcpy_s(f.filename, 30, tmpstring.c_str());

				sprintf_s(filename, 30, "%s/%s", dataset_dir, f.filename);
				ifstream frame_in(filename, ios::in | ios::binary);

				// data buffer
				frame_in.read(f.data, f.data_size);
				frame_in.close();

				bot->frame_received(&f);

				break;
			}
		}
	}
}


void bot_ardrone_recorder::prepare_dataset()
{
	if (fout.is_open())
		printf("ERROR: DATASET OUTPUT FILE ALREADY OPEN!\n");

	int i = 1;
	char filename[25];

	sprintf_s(filename, 25, "dataset/%03d", i);

	while ((_access(filename, 0)) == 0)
	{
		sprintf_s(filename, 25, "dataset/%03d", ++i);
	}

	sprintf_s(dataset_dir, 25, "%s", filename);
	CreateDirectory(dataset_dir, NULL);

	sprintf_s(filename, 25, "%s/output.yaml", dataset_dir);
	fout.open(filename, ios::out);

	printf("Created dataset %03d\n", i);
}