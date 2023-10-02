# Task

Extend the mock sensor so that it:

* replies to the hardcoded address 0x333 in addition to the broadcast and the default/command line configured address

* implements a new command type (0xB0) which has 1 parameter with values from 0 to 0x80
	
	`cansend vcan0 666#B0xx (xx from 0x00 to 0x80)`

	The sensor node must reply with CAN frames the fizz buzz series (https://en.wikipedia.org/wiki/Fizz_buzz) from 0 to xx, each value being in it's own byte in the can frames.

	For Fizz the value written will be 0xFF, for Buzz 0xBB and for FizzBuzz 0xFB.

	Example reply for xx being 0x0D:

	0xB1 0x00 0x01 0x02 0xFF 0x04 0xBB 0xFF

	0xB1 0x07 0x08 0x09 0xBB 0x0B 0xFF 0x0D



# Setup vcan interface locally

sudo modprobe vcan

sudo ip link add dev vcan0 type vcan

sudo ifconfig vcan0 up

# See traffic on the bus 

candump vcan0

# Send a can message to a node

cansend vcan0 <can_id>#<byte0><byte1>...<byte7>

cansend vcan0 666#A0
