// implement fork from user space

#include <inc/string.h>
#include <inc/lib.h>

// PTE_COW marks copy-on-write page table entries.
// It is one of the bits explicitly allocated to user processes (PTE_AVAIL).
#define PTE_COW		0x800

//
// Custom page fault handler - if faulting page is copy-on-write,
// map in our own private writable copy.
//
static void
pgfault(struct UTrapframe *utf)
{
	void *addr = (void *) utf->utf_fault_va;
	uint32_t err = utf->utf_err;
	int r;

	// Check that the faulting access was (1) a write, and (2) to a
	// copy-on-write page.  If not, panic.
	// Hint:
	//   Use the read-only page table mappings at uvpt
	//   (see <inc/memlayout.h>).

	// LAB 4: Your code here.

	// Allocate a new page, map it at a temporary location (PFTEMP),
	// copy the data from the old page to the new page, then move the new
	// page to the old page's address.
	// Hint:
	//   You should make three system calls.

	// LAB 4: Your code here.
	// Check that the faulting access was 
	// (1). a write
	// (2). to a copy-on-write page. 
	if((err & FEC_WR) == 0 || (uvpt[PGNUM(addr)] & PTE_COW) == 0)
		panic("pgfault failed: this page is not writable or attempt to access a non-cow page!");
	envid_t envid = sys_getenvid();
	if((r = sys_page_alloc(envid, (void *)PFTEMP, PTE_P|PTE_W|PTE_U)) < 0)
		panic("pgfault failed: page allocation failed!");

	addr = ROUNDDOWN(addr, PGSIZE);
	memmove(PFTEMP, addr, PGSIZE);
	if ((r = sys_page_unmap(envid, addr)) < 0)
		panic("pgfault failed: page unmap failed: %e", r);
	if ((r = sys_page_map(envid, PFTEMP, envid, addr, PTE_P | PTE_W | PTE_U)) < 0)
		panic("pgfault failed: page map failed: %e", r);
	if ((r = sys_page_unmap(envid, PFTEMP)) < 0)
		panic("pgfault failed: page map failed: %e", r);
	//panic("pgfault not implemented");
	
}

//
// Map our virtual page pn (address pn*PGSIZE) into the target envid
// at the same virtual address.  If the page is writable or copy-on-write,
// the new mapping must be created copy-on-write, and then our mapping must be
// marked copy-on-write as well.  (Exercise: Why do we need to mark ours
// copy-on-write again if it was already copy-on-write at the beginning of
// this function?)
//
// Returns: 0 on success, < 0 on error.
// It is also OK to panic on error.
//
static int
duppage(envid_t envid, unsigned pn)
{
	int r;

	// LAB 4: Your code here.
//
//	void *addr;
//	pte_t pte;
//	int perm;
//	
//	addr = (void *)((uint32_t)pn * PGSIZE);
//	pte = uvpt[pn];
//	perm = PTE_P | PTE_U;
//	if ((pte & PTE_W) || (pte & PTE_COW))
//		perm |= PTE_COW;
//	if ((r = sys_page_map(thisenv->env_id, addr, envid, addr, perm)) < 0) {
//		panic("duppage failed: page remapping failed: %e", r);
//		return r;
//	}
//	if (perm & PTE_COW) {
//		if ((r = sys_page_map(thisenv->env_id, addr, thisenv->env_id, addr, perm)) < 0) {
//			panic("duppage failed: page remapping failed: %e", r);
//			return r;
//		}
//	}
//	//panic("duppage not implemented");
//	return 0;

	// LAB 5: For exercise 8. 
	void *addr;
	pte_t pte;
	int perm;

	addr = (void *)((uint32_t)pn * PGSIZE);
	pte = uvpt[pn];
	// If the page table entry has the PTE_SHARE bit set, 
	// just copy the mapping directly.  
	if (pte & PTE_SHARE) {
		if ((r = sys_page_map(sys_getenvid(), addr, envid, addr, pte & PTE_SYSCALL)) < 0) {
			panic("duppage failed: sys_page_map failed: %e", r);
			return r;
		}
	}
	else {
		perm = PTE_P | PTE_U;
        	if ((pte & PTE_W) || (pte & PTE_COW))
        		perm |= PTE_COW;
        	if ((r = sys_page_map(thisenv->env_id, addr, envid, addr, perm)) < 0) {
        		panic("duppage failed: page remapping failed: %e", r);
        		return r;
        	}
        	if (perm & PTE_COW) {
        		if ((r = sys_page_map(thisenv->env_id, addr, thisenv->env_id, addr, perm)) < 0) {
        			panic("duppage failed: page remapping failed: %e", r);
        			return r;
        		}
        	}
	}
	return 0;
}

//
// User-level fork with copy-on-write.
// Set up our page fault handler appropriately.
// Create a child.
// Copy our address space and page fault handler setup to the child.
// Then mark the child as runnable and return.
//
// Returns: child's envid to the parent, 0 to the child, < 0 on error.
// It is also OK to panic on error.
//
// Hint:
//   Use uvpd, uvpt, and duppage.
//   Remember to fix "thisenv" in the child process.
//   Neither user exception stack should ever be marked copy-on-write,
//   so you must allocate a new page for the child's user exception stack.
//
envid_t
fork(void)
{
	// LAB 4: Your code here.
	envid_t childenvid;
	uint32_t addr;
	int i, j, pn, r;
	extern void _pgfault_upcall(void);

	// (1). Install pgfault() as the C-level page fault handler. 
	set_pgfault_handler(pgfault);
	
	// (2). The parent calls sys_exofork() to create a child environment
	if ((childenvid = sys_exofork()) < 0) {
		panic("sys_exofrok failed: %e", childenvid);
		return childenvid;
	}
	if(childenvid == 0) {
		// child environment
		thisenv = &envs[ENVX(sys_getenvid())];
		return 0;
	}
	// (3). For each writable or copy-on-write page in its address space before UTOP, the parent calls duppage, which should
	//      (3.1) map the page copy-on-write into the address space of the child. 
	//	(3.2) then, remap the page copy-on-write into its own address space. Mind the order of (3.1) and (3.2)
	// Following is parent environment operation. 
	
	//	(3.1) Copy address space and page fault handler setup
	//	      to the child. 
	for (pn = PGNUM(UTEXT); pn < PGNUM(USTACKTOP); pn++) {
		if((uvpd[pn >> 10] & PTE_P) && (uvpt[pn] & PTE_P)) {
			if ((r = duppage(childenvid, pn)) < 0)
				return r;
		}
	}
	
	// allocate a page for child exception stack	
	if ((r = sys_page_alloc(childenvid, (void *)(UXSTACKTOP-PGSIZE), PTE_U | PTE_P | PTE_W)) < 0)
		return r;
	// (4) The parent sets the user page fault entrypoint for
	//      the child to look like its own. 
	if ((r = sys_env_set_pgfault_upcall(childenvid, _pgfault_upcall)) < 0)
		return r;
	
	// (5). The child is now ready to run, so the parent marks it runnable	
	if ((r = sys_env_set_status(childenvid, ENV_RUNNABLE)) < 0)
		panic("fork failed: sys_env_set_status failed: %e", r);
	return childenvid;
	
	//panic("fork not implemented");
}

// Challenge!
int
sfork(void)
{
	panic("sfork not implemented");
	return -E_INVAL;
}
