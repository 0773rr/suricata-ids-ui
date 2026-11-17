#include <ncurses.h>
#include <unistd.h>
#include <time.h>

typedef struct {

	uint64_t flow_id;

	char src_ip[46];
	char dest_ip[46];
	uint16_t src_port;
	uint16_t dest_port;

	char protocol[16];
	char app_protocol[32];

	time_t first_seen;
	time_t last_seen;

	uint64_t bytes_to_server;
	uint64_t bytes_to_client;

	char hostname[256];
	char country[64];
	char city[96];
	int abuse_score;
	char provider[128];

} Connection;

int main(){

	initscr();
	noecho();
	curs_set(0);

	while(1){
		int height, width;
		getmaxyx(stdscr, height, width);

		int left_side_width = width * 40 / 100;
		int right_side_width = width - left_side_width;

		WINDOW *alerts = newwin(height - 4,left_side_width,2,0);
		WINDOW *connections = newwin(height - 4,right_side_width,2,left_side_width);

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
