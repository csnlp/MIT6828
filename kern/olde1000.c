#include <kern/e1000.h>
#include <inc/x86.h>
#include <kern/pmap.h>
#include <inc/string.h>
#include <kern/pci.h>

// LAB 6: Your driver code here

// declare arraies of desc and packet: exercise 5
struct tx_desc tx_d[TXRING_LEN] __attribute__((aligned(PGSIZE)))
		= {{0, 0, 0, 0, 0, 0, 0}};
struct rx_desc rx_d[RXRING_LEN] __attribute__((aligned(PGSIZE)))
		= {{0, 0, 0, 0, 0, 0, 0}};

struct packet ptxbuf[TXRING_LEN] __attribute__((aligned(PGSIZE))) = {{{0}}};

struct packet prxbuf[RXRING_LEN] __attribute__((aligned(PGSIZE))) = {{{0}}};
//: End: LAB 6: exercise 3.

// Start: LAB 6: exercise 4
#define E1000_LOCATE(offset) (offset >> 2)
volatile uint32_t *e1000;

// End: LAB 6: exercise 4

// Start: LAB 6: exercise 5. 
// packets initialization

// Descriptor initialization

static void 
init_desc() {
	int i;
	for (i = 0; i < TXRING_LEN; i++) {
		memset(&tx_d[i], 0, sizeof(tx_d[i]));
		tx_d[i].addr = PADDR(&ptxbuf[i]);
		tx_d[i].status = TXD_STAT_DD;
		tx_d[i].cmd = TXD_CMD_RS | TXD_CMD_EOP;
	}
	
}

int
e1000_transmit(void *addr, size_t len)
{
	uint32_t tail = e1000[TDT/4];
	struct tx_desc *nxt = &tx_d[tail];
	if ((nxt->status & TXD_STAT_DD) != TXD_STAT_DD)
		return -1;
	if (len > TBUFFSIZE)
		len = TBUFFSIZE;
	memmove(&ptxbuf[tail], addr, len);
	nxt->length = (uint16_t)len;
	nxt->status &= TXD_STAT_DD;
	e1000[TDT/4] = (tail + 1) % TXRING_LEN;
	return 0;
}

// Start: LAB 6: exercise 3

int
pci_e1000_attach(struct pci_func *pcif)
{
	pci_func_enable(pcif);
	init_desc();
	// Add from exercise 4: 
	e1000 = mmio_map_region(pcif->reg_base[0], pcif->reg_size[0]);
	cprintf("device status:[%08x]\n", e1000[E1000_LOCATE(E1000_DEVICE_STATUS)]);
	// End of exercise 4

	// Add from exercise 5: start
	e1000[TDBAL/4] = PADDR(tx_d);
	e1000[TDBAH/4] = 0;
	e1000[TDLEN/4] = TXRING_LEN * sizeof(struct tx_desc);
	e1000[TDH/4] = 0;
	e1000[TDT/4] = 0;
	e1000[TCTL/4] = TCTL_EN | TCTL_PSP | (TCTL_CT & (0x10 << 4)) | (TCTL_COLD & (0x40 << 12));
	e1000[TIPG/4] = 10 | (8 << 10) | (12 << 20);
	cprintf("e1000: status %x\n", e1000[STATUS/4]);
	
	return 1;
}
