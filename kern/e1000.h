#ifndef JOS_KERN_E1000_H
#define JOS_KERN_E1000_H

// LAB 6. Exercise 3
#define E1000_VENDOR_ID	0x8086
#define E1000_DEVICE_ID	0x100E

#define E1000_STATUS	0x00008	// LAB 6: Exercise 4

#include <kern/pci.h>
int e100_attach_func(struct pci_func *pcif);

// LAB 6: Exercise 5
#define TXDESCS	32
#define TX_PKT_SIZE	1518

#define E1000_TCTL	0x00400	// RO: TX Control
#define E1000_TIPG	0x00410	// RW: TX Inter-packet gap
#define E1000_TDBAL	0x03800 // RW: TX descriptor base address low
#define E1000_TDBAH	0x03804	// RW: TX descriptor base address high 
#define E1000_TDLEN	0x03808	// RW: TX descriptor length
#define E1000_TDH	0x03810	// RW: TX descriptor head
#define E1000_TDT	0x03818	// RW: TX descriptor tail
#define E1000_TXD_STAT_DD	0x00000001	// Descriptor Done
#define E1000_TXD_CMD_EOP	0x00000001	// End of packet
#define E1000_TXD_CMD_RS	0x00000008	// Report status

// transmit descriptor 
struct e1000_tx_desc
{
	uint64_t addr;
	uint16_t length;
	uint8_t	cso;
	uint8_t cmd;
	uint8_t status;
	uint8_t css;
	uint8_t special;
}__attribute__((packed));

struct e1000_tctl 
{
	uint32_t rsv1:	1;
	uint32_t en:	1;
	uint32_t rsv2:	1;
	uint32_t psp:	1;
	uint32_t ct:	8;
	uint32_t cold:	10;
	uint32_t swxoff:	1;
	uint32_t rsv3:	1;
	uint32_t rtlc:	1;
	uint32_t nrtu:	1;
	uint32_t rsv4:	6;
};

struct e1000_tdh 
{
	uint16_t tdh;
	uint16_t rsv;
};

struct e1000_tdt 
{
	uint16_t tdt;
	uint16_t rsv;
};

struct e1000_tdlen
{
	uint32_t zero:	7;
	uint32_t len:	13;
	uint32_t rsv:	12;
};

struct e1000_tipg 
{
	uint32_t ipgt:	10;
	uint32_t ipgr1:	10;
	uint32_t ipgr2:	10;
	uint32_t rsv: 	2;
};

int e1000_attach_func(struct pci_func *pcif);
static void e1000_transmit_init();

#endif  // SOL >= 6
