// (c) Toposens GmbH 2022. All rights reserved.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


#include <getopt.h>
#include <libgen.h>

#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <net/if.h>

#define DEBUG

static struct option long_options[] = {
    {"canID",     required_argument, 0,  'c' },
    {"interface", required_argument, 0,  'i' },
    {"help",      no_argument,       0,  'h' },
    {0,           0,                 0,  0   }
};

void usage(char* name)
{
	printf("Usage:\n%s [FLAGS]\n", basename(name));
	printf("\t-c, --canID ID\n");
	printf("\t\t default ID is 666. The app will also answer to 000\n");
	printf("\t-i, --interface interface\n");
	printf("\t\t Interface to use. Default is vcan0, needs to exist. See README.md\n");

}

int main(int argc, char *argv[]) {
	int opt= 0;
	int long_index =0;
	int can_id = 0x666;
	char* can_interface = "vcan0";
	while ((opt = getopt_long(argc, argv,"hc:i:", long_options, &long_index )) != -1) {
		switch (opt) {
			case 'c' :
				can_id = strtoul(optarg, NULL, 16); 
				break;
			case 'i' :
				//not implemented
				can_interface = optarg;
				break;
			case 'h' :
				usage(argv[0]); 
				exit(EXIT_SUCCESS);
			default: 
				usage(argv[0]); 
				exit(EXIT_FAILURE);
		}
	}
	printf("Starting mock sensor on interface %s with address %X\n", can_interface, can_id);
	int s;
	if ((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
		perror("Socket call failed");
		return -1;
	}

	// Interface to be used. Currently hardcoded to vcan0
	struct ifreq ifr = { .ifr_name = "vcan0"};
	ioctl(s, SIOCGIFINDEX, &ifr);

	struct sockaddr_can addr;
	memset(&addr, 0, sizeof(addr));

	addr.can_family = AF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex;

	if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		perror("Socket bind failed");
		return -1;
	}

	//Addresses to respond to. 000 is used as broadcast
	struct can_filter rfilter[2];
	rfilter[0].can_id   = can_id;
	rfilter[0].can_mask = 0xFFF;
	rfilter[1].can_id   = 0x000;
	rfilter[1].can_mask = 0xFFF;

	setsockopt(s, SOL_CAN_RAW, CAN_RAW_FILTER, &rfilter, sizeof(rfilter));

	struct can_frame frame;

	useconds_t sleep_time = 1000;

	do {
		int i = 0;
		int nbytes;
		nbytes = read(s, &frame, sizeof(struct can_frame));
		if (nbytes < 0) {
			perror("Socket Read failed");
			return -1;
		}
		
#ifdef DEBUG
		printf("Received 0x%03X [%d] :", frame.can_id, frame.can_dlc);

		for (i = 0; i < frame.can_dlc; i++)
			printf("%02X ",frame.data[i]);

		printf("\n");
#endif

		// First byte indicates the command. 
		if((frame.data[0] == 0xA0)) {
			// Get CAN ID
			struct can_frame reply_frame = {.can_id = can_id, .can_dlc = 4};
			// We set the last bit to indicate it is a reply
			reply_frame.data[0] =  0xA1;
			reply_frame.data[1] =  can_id / 256;
			reply_frame.data[2] = (can_id /16) %16;
			reply_frame.data[3] =  can_id % 16;
			if (write(s, &reply_frame, sizeof(struct can_frame)) != sizeof(struct can_frame)) {
				perror("Socker write failed");
				return -1;
			}
			usleep(sleep_time);
		}

	} 
	while(1);
	
	return 0;
}
