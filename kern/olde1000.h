#ifndef JOS_KERN_E1000_H
#define JOS_KERN_E1000_H

// LAB 6. Exercise 3
#define PCI_E1000_VENDOR 0x8086
#define PCI_E1000_DEVICE 0x100E

// LAB 6. Exercise 4
#define E1000_DEVICE_STATUS   0x00008  /* Device Status - RO */

// LAB 6. End of exercise 4
/* Register Set. (82543, 82544)
Registers are defined to be 32 bits and should be
accessed as 32 bit values. These registers are 
physically located on the NIC, but are mapped into
the host memory address space. 
RW: register is both readable and writable
RO: register is read only. 
WO: register is write only. 
R/clr: register is read only and is cleared when read. 
A: register array
*/
#define CTRL		0x00000 // RW: Device control
#define CTRL_DUP	0x00004	// RW: Device control duplicate
#define STATUS		0x00008 // RO: Device status
#define EECD		0x00010 // RW: EEPROM/Flash control
#define EERD		0x00014 // RW: EEPROM read

#define TDBAL	0x03800	// RW: TX descriptor base address low
#define TDBAH	0x03804	// RW: TX descriptor base address high 
#define TDLEN	0x03808	// RW: TX descriptor base address length
#define TDH	0x03810	// RW: TX descriptor head
#define TDT	0x03818	// RW: TX descriptor tail
#define TCTL	0x00400	// RW: TX control
#define TIPG	0x00410	// RW: TX inter-packet gap

// LAB 6. Exercise 5. Define certain length
#define TXRING_LEN	64
#define RXRING_LEN	128
#define TBUFFSIZE	2048
#define RBUFFSIZE	2048

// Transmit descriptor bit definitions
#define TXD_CMD_EOP	0x01	// End of packet
#define TXD_CMD_IFCS	0x02	// Insert FCS
#define TXD_CMD_IC	0x04	// Insert checksum
#define TXD_CMD_RS	0x08	// Report status
#define TXD_CMD_RPS	0x10	// Report packet sent
#define TXD_CMD_DEXT	0x20	// Descriptor extension
#define TXD_CMD_VLE	0x40	// Add vLAN tag
#define TXD_CMD_IDE	0x80	// Enable Tidv register

#define TXD_STAT_DD	0x01	// Descriptor Done
#define TXD_STAT_EC	0x02	// Excess collisions
#define TXD_STAT_LC	0x04	// Late collision
#define TXD_STAT_TU	0x08	// Transmit underrun

#define TXD_CMD_TCP	0x01	// TCP packet
#define TXD_CMD_IP	0x02	// IP packet
#define TXD_CMD_TSE	0x04	// TCP seg enable

#define TXD_STAT_TC 	0x04	// Tx underrun

/*
Transmit control
*/
#define TCTL_RST	0x00000001	// Software reset
#define TCTL_EN		0x00000001	// Enable tx
#define TCTL_BCE	0x00000004	// BUsy check enable
#define TCTL_PSP	0x00000008	// Pad shot packets
#define TCTL_CT		0x00000ff0	// Collision threshold
#define TCTL_COLD	0x003ff000	// Collision distance
#define TCTL_SWXOFF	0x00400000	// SW Xoff transmission
#define TCTL_PBE	0x00800000	// Packet burst enable
#define TCTL_RTLC	0x01000000	// Re-transmit on late collision
#define TCTL_NRTU	0x02000000	// No re-transmit on underrun
#define TCTL_MULR	0x10000000	// Multiple request support

// C structure
#include <inc/types.h>
struct tx_desc
{
	uint64_t addr;
	uint16_t length;
	uint8_t cso;
	uint8_t cmd;
	uint8_t status;
	uint8_t css;
	uint16_t special;	
} __attribute__((packed));

struct rx_desc
{
	uint64_t addr;
	uint16_t length;
	uint8_t cso;
	uint8_t cmd;
	uint8_t status;
	uint8_t css;
	uint16_t special;	
} __attribute__((packed));

// main body of packet
struct packet 
{
	char body[2048];
}; 

#endif  // SOL >= 6
