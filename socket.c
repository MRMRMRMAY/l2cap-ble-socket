#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/l2cap.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

#define ATT_CID 4;

int main() {
	int s = socket(PF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP);
	if (s < 0) {
	  printf(" no socket\n");
	}

	struct sockaddr_l2 bind_addr = { 0 };
	bind_addr.l2_family = AF_BLUETOOTH;
	bind_addr.l2_cid = htobs(4); // ATT CID
	bacpy(&bind_addr.l2_bdaddr, BDADDR_ANY);
	bind_addr.l2_bdaddr_type = BDADDR_LE_PUBLIC;

	int err = bind(s, (struct sockaddr*)&bind_addr, sizeof(bind_addr));
	if (err) {
	  perror("L2CAP bind failed\n");
	}

	struct sockaddr_l2 conn_addr = { 0 };
	conn_addr.l2_family = AF_BLUETOOTH;
	conn_addr.l2_cid = htobs(4); // ATT CID
	str2ba("D0:39:72:C4:DC:A5", &conn_addr.l2_bdaddr );
	conn_addr.l2_bdaddr_type = BDADDR_LE_PUBLIC;

	err = connect(s, (struct sockaddr*)&conn_addr, sizeof(conn_addr));
	if (err) {
	    perror("L2CAP connect failed\n");
	}

	// switch the beetle leds on and off
	char on[] = { 0x12, 0x25, 0x00, 0x01 };
	char off[] = { 0x12, 0x25, 0x00, 0x00 };
	size_t size = sizeof(on);

	struct iovec iov;
	iov.iov_base = on;
	iov.iov_len = 4;

	ssize_t ret = writev(s, &iov, 1);
	printf("written %i\n", ret);

	usleep(10000000);

	iov.iov_base = off;
	writev(s, &iov, 1);

	usleep(10000000);
	close(s);
}
