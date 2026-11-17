#include <ncurses.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <json-c/json.h>
#include <string.h>

typedef struct {

	uint64_t flow_id;

	char src_ip[46];
	char dst_ip[46];
	uint16_t src_port;
	uint16_t dst_port;

	char protocol[16];
	char app_protocol[32];

	time_t start_time;
	time_t end_time;

	uint64_t bytes_to_server;
	uint64_t bytes_to_client;

	char hostname[256];
	char country[64];
	char city[96];
	int abuse_score;
	char provider[128];

} Connection;

static void get_json_string(struct json_object *obj, const char *key, char *dest, size_t size){

	struct json_object *value;

	if (json_object_object_get_ex(obj, key, &value)){
		snprintf(dest, size, "%s", json_object_get_string(value));
	}

}

static bool parse_flow(const char *line, Connection *connection){

	struct json_object *root = json_tokener_parse(line);
	if (root == NULL){
		return false;
	}

	struct json_object *event_type;
	if (!json_object_object_get_ex(root, "event_type", &event_type)){
		json_object_put(root);
		return false;
	}
	if (strcmp(json_object_get_string(event_type), "flow") != 0) {
		json_object_put(root);
		return false;
	}

	Connection connection_data = {0};
	get_json_string(root, "src_ip", connection_data.src_ip, sizeof(connection_data.src_ip));
	get_json_string(root, "dest_ip", connection_data.dst_ip, sizeof(connection_data.dst_ip));
	get_json_string(root, "proto", connection_data.protocol, sizeof(connection_data.protocol));
	get_json_string(root, "app_proto", connection_data.app_protocol, sizeof(connection_data.app_protocol));

	struct json_object *value;
	struct json_object *flow;

	if (json_object_object_get_ex(root, "flow_id", &value)) {
		connection_data.flow_id = json_object_get_uint64(value);
	}
	if (json_object_object_get_ex(root, "src_port", &value)) {
		connection_data.src_port = json_object_get_int(value);
	}
	if (json_object_object_get_ex(root, "dest_port", &value)) {
		connection_data.dst_port = json_object_get_int(value);
	}
	if (json_object_object_get_ex(root, "flow", &flow) && flow != NULL) {

		/*
		 * add time extraction
		 */

		if (json_object_object_get_ex(flow, "bytes_toserver", &value)) {
			connection_data.bytes_to_server = json_object_get_uint64(value);
		}
		if (json_object_object_get_ex(flow, "bytes_toclient", &value)) {
			connection_data.bytes_to_client = json_object_get_uint64(value);
		}
	}

	*connection = connection_data;

	json_object_put(root);
	return true;

}

int main(){

	initscr();
	noecho();
	curs_set(0);

	while(true){
		int height, width;
		getmaxyx(stdscr, height, width);

		int left_display_width = width * 40 / 100;
		int right_display_width = width - left_display_width;

		WINDOW *alerts = newwin(height - 4,left_display_width,2,0);
		WINDOW *connections = newwin(height - 4,right_display_width,2,left_display_width);

		box(alerts,0,0);
		box(connections,0,0);

		wnoutrefresh(alerts);
		wnoutrefresh(connections);
		doupdate();

		delwin(alerts);
		delwin(connections);

		sleep(1);


	}

	endwin();
	return 0;
}
