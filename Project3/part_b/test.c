/*
 * Kernel Module for logging Virtual Memory info for given process id
 * @author: Fuad Aghazada
 * @date: 30.11.2018
 */

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/stat.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/sched/task.h>
#include <linux/sched/signal.h>
#include <linux/mm.h>
#include <asm/page.h>
#include <linux/highmem.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Fuad Aghazada & Can Ozgurel");

#define DEBUG 0

/* Default global variables for module parameters */
static int processid = 1000;
static unsigned long virtaddr = 0;

/* Module paramaters (module_param) */
module_param(processid, int, 0);
module_param(virtaddr, long, 0);

/* Prototypes */
static void memory_info(struct task_struct *task);
static void page_table(struct task_struct *task);
static void trans_addr(struct task_struct *task);
static void print_entry(unsigned long addr);

/* ----------------------------------------------------- */

/* Initializing the module */
static int __init my_module_init(void)
{
    struct task_struct *task;
    printk(KERN_INFO "Passed Process id: %d\n", processid);

    // Traversing through the PCB list
    task = &init_task;      // current
    while((task = next_task(task)) != &init_task)
    {
        if(DEBUG == 1)
            printk(KERN_INFO "PID\t%d\n", task->pid);

        if(task->pid == processid)
        {
            printk(KERN_INFO "Process ID is found!\n");

            memory_info(task);
            page_table(task);
            trans_addr(task);

            break;
        }
    }

    return 0;
}

/* Cleaning the module */
static void __exit my_module_exit(void)
{
	printk(KERN_INFO "Module is destroyed!\n");
}

/**
    Function memory_info():
    Printing basic memory information for a specific process/task in the following format:

    @Reference: http://venkateshabbarapu.blogspot.com/2012/09/process-segments-and-vma.html

    vm-area-start vm-area-size  # 0

    * start(virtual address), end(virtual address) and size of the code(segment)    # 1
    * start, end and size of data   # 2
    * start, end and size of stack  # 3
    * start, end and size of heap   # 4
    * start, end and size of main arguments   # 5
    * start, end and size of environment variables   # 6
    * number of frames used by the process    # 7
    * total virtual memory used by the process (total_vm)   # 8

    @param task: given task
*/
static void memory_info(struct task_struct *task)
{
    // Declarations (For handling Warning ISO C90)

    struct mm_struct *task_mm;   // Memory Management of the given task
    struct vm_area_struct *mmap;
    struct vm_area_struct *vm_cur;  // for iterating

    unsigned long vm_start, vm_end;
    unsigned long start_code, end_code, size_code;
    unsigned long start_data, end_data, size_data;
    unsigned long start_stack, end_stack, size_stack;
    unsigned long start_heap, end_heap, size_heap;
    unsigned long start_arg, end_arg, size_arg;
    unsigned long start_env, end_env, size_env;
    unsigned long number_of_frames;
    unsigned long total_number_of_pages, total_v_size;

    // Title
    printk(KERN_INFO "*** Memory info for Process ID: %d ***\n\n", task->pid);

    // # 0 - Virtual Memory area
    task_mm = task->mm;
    mmap = task_mm->mmap;
    vm_start = mmap->vm_start;
    vm_end = mmap->vm_end;

    printk(KERN_INFO "--- Virtual Memory Area ---\n");
    printk(KERN_INFO "Start: 0x%lx\n", vm_start);
    printk(KERN_INFO "End: 0x%lx\n", vm_end);
    printk(KERN_INFO "---------------------------\n");

    // # 1 - Code (Text) segment
    start_code = task_mm->start_code;
    end_code = task_mm->end_code;
    size_code = end_code - start_code;

    printk(KERN_INFO "--- Code (Text) segment ---\n");
    printk(KERN_INFO "Start: 0x%lx\n", start_code);
    printk(KERN_INFO "End: 0x%lx\n", end_code);
    printk(KERN_INFO "Size: %lu\n", size_code);
    printk(KERN_INFO "---------------------------\n");

    // # 2 - Data segment
    start_data = task_mm->start_data;
    end_data = task_mm->end_data;
    size_data = end_data - start_data;

    printk(KERN_INFO "--- Data ---\n");
    printk(KERN_INFO "Start: 0x%lx\n", start_data);
    printk(KERN_INFO "End: 0x%lx\n", end_data);
    printk(KERN_INFO "Size: %lu\n", size_data);
    printk(KERN_INFO "---------------------------\n");

    // # 3 - Stack segment

    // From figure in the reference link:
    // We need to iterate through until we get last (stack) segment
    vm_cur = task_mm->mmap;
    while(vm_cur != NULL)
    {
        if(vm_cur->vm_end >= vm_cur->vm_mm->start_stack && vm_cur->vm_start <= vm_cur->vm_mm->start_stack)
            break;

        vm_cur = vm_cur->vm_next;
    }

    start_stack = vm_cur->vm_start;
    end_stack = vm_cur->vm_end;
    size_stack = end_stack - start_stack;

    printk(KERN_INFO "--- Stack ---\n");
    printk(KERN_INFO "Start: 0x%lx\n", start_stack);
    printk(KERN_INFO "End: 0x%lx\n", end_stack);
    printk(KERN_INFO "Size: %lu\n", size_stack);
    printk(KERN_INFO "-------------\n");

    // # 4 - Heap segment
    start_heap = task_mm->start_brk;
    end_heap = task_mm->brk;
    size_heap = end_heap - start_heap;

    printk(KERN_INFO "--- Heap ---\n");
    printk(KERN_INFO "Start: 0x%lx\n", start_heap);
    printk(KERN_INFO "End: 0x%lx\n", end_heap);
    printk(KERN_INFO "Size: %lu\n", size_heap);
    printk(KERN_INFO "------------\n");

    // # 5 - Main Arguments
    start_arg = task_mm->arg_start;
    end_arg = task_mm->arg_end;
    size_arg = end_arg - start_arg;

    printk(KERN_INFO "--- Arguments ---\n");
    printk(KERN_INFO "Start: 0x%lx\n", start_arg);
    printk(KERN_INFO "End: 0x%lx\n", end_arg);
    printk(KERN_INFO "Size: %lu\n", size_arg);
    printk(KERN_INFO "-----------------\n");

    // # 6 - Environmnet Variables
    start_env = task_mm->env_start;
    end_env = task_mm->env_end;
    size_env = end_env - start_env;

    printk(KERN_INFO "--- Environment Variables ---\n");
    printk(KERN_INFO "Start: 0x%lx\n", start_env);
    printk(KERN_INFO "End: 0x%lx\n", end_env);
    printk(KERN_INFO "Size: %lu\n", size_env);
    printk(KERN_INFO "-----------------------------\n");

    // # 7 - Number of frames
    number_of_frames = get_mm_rss(task_mm);

    printk(KERN_INFO "--- Number of Frames used by this process ---\n");
    printk(KERN_INFO "Number of frames: %lu\n", number_of_frames * 4);
    printk(KERN_INFO "---------------------------------------------\n");

    // # 8 - Total Virtual Memory
    total_number_of_pages = task_mm->total_vm;
    total_v_size = total_number_of_pages * 4;   // Each page 4 bytes

    printk(KERN_INFO "--- Total Virtual Memory used by this process ---\n");
    printk(KERN_INFO "Size: %lu\n", total_v_size);
    printk(KERN_INFO "-------------------------------------------------\n");
}

/**
    Multi-Level Page Table Content

    @param task: given task
*/
static void page_table(struct task_struct *task)
{
    // Declarations
    struct mm_struct *task_mm;
    struct vm_area_struct *vma;
    struct page *page = NULL;

    unsigned long virtual_page, phy_addr;

    pgd_t *pgd;
    p4d_t *p4d;
    pud_t *pud;
    pmd_t *pmd;
    pte_t *pte;

    // Title
    printk(KERN_INFO "\n*** Page Table for Process ID: %d ***\n\n", task->pid);

    task_mm = task->mm;   // Memory Management of the given task
    vma = task_mm->mmap;

    while(vma != NULL)
    {
        virtual_page = vma->vm_start;
        while(virtual_page < vma->vm_end)
        {
            pgd = pgd_offset(task_mm, virtual_page);
            if(!pgd_none(*pgd) && !pgd_bad(*pgd))
            {
                printk(KERN_INFO "+++++PGD+++++\n");
                print_entry(pgd_val(*pgd));
                p4d = p4d_offset(pgd, virtual_page);
                if(!p4d_none(*p4d) && !p4d_bad(*p4d))
                {
                    printk(KERN_INFO "+++++P4D+++++\n");
                    print_entry(p4d_val(*p4d));
                    pud = pud_offset(p4d, virtual_page);
                    if(!pud_none(*pud) && !pud_bad(*pud))
                    {
                        printk(KERN_INFO "+++++PUD+++++\n");
                        print_entry(pud_val(*pud));
                        pmd = pmd_offset(pud, virtual_page);
                        if(!pmd_none(*pmd) && !pmd_bad(*pmd))
                        {
                            printk(KERN_INFO "+++++PMD+++++\n");
                            print_entry(pmd_val(*pmd));
                            if(pte_offset_map(pmd, virtual_page))
                            {
                                pte = pte_offset_map(pmd, virtual_page);
                                if(pte_page(*pte))
                                {
                                    printk(KERN_INFO "+++++PTE+++++\n");
                                    page = pte_page(*pte);
                                    phy_addr = page_to_phys(page);

                                    // Printing entry fields
                                    printk(KERN_INFO "*** Physical address: 0x%lx ***\n", phy_addr);
                                    print_entry(pte_val(*pte));

                                    pte_unmap(pte);
                                }
                                else return;
                            }
                            else return;
                        }
                        else return;
                    }
                    else return;
                }
                else return;
            }
            else return;

            virtual_page += PAGE_SIZE;
        }
        vma = vma->vm_next;
    }
}

/**
    Translating the given logical address to physical address

    @param task: given task
*/
static void trans_addr(struct task_struct *task)
{
    // Declarations
    struct mm_struct *task_mm;
    struct page *page = NULL;

    unsigned long phy_addr;

    pgd_t *pgd;
    p4d_t *p4d;
    pud_t *pud;
    pmd_t *pmd;
    pte_t *pte;


    task_mm = task->mm;   // Memory Management of the given task

    // Title
    printk(KERN_INFO "\n*** Address translation for Process ID: %d ***\n", task->pid);
    printk(KERN_INFO "*** Virtual (logical) address: %lx ***\n", virtaddr);

    pgd = pgd_offset(task_mm, virtaddr);
    if(!pgd_none(*pgd) && !pgd_bad(*pgd))
    {
        p4d = p4d_offset(pgd, virtaddr);
        if(!p4d_none(*p4d) && !p4d_bad(*p4d))
        {
            pud = pud_offset(p4d, virtaddr);
            if(!pud_none(*pud) && !pud_bad(*pud))
            {
                pmd = pmd_offset(pud, virtaddr);
                if(!pmd_none(*pmd) && !pmd_bad(*pmd))
                {
                    if(pte_offset_map(pmd, virtaddr))
                    {
                        pte = pte_offset_map(pmd, virtaddr);
                        if(pte_page(*pte))
                        {
                            page = pte_page(*pte);
                            phy_addr = page_to_phys(page);

                            printk(KERN_INFO "*** Physical address: 0x%lx ***\n", phy_addr);

                            pte_unmap(pte);
                        }
                        else
                        {
                            printk(KERN_INFO "ERROR: No such page exists!\n");
                            return;
                        }
                    }
                    else
                    {
                        printk(KERN_INFO "ERROR: No such PTE!\n");
                        return;
                    }
                }
                else
                {
                    printk(KERN_INFO "ERROR: No such PMD!\n");
                    return;
                }
            }
            else
            {
                printk(KERN_INFO "ERROR: No such PUD!\n");
                return;
            }
        }
        else
        {
            printk(KERN_INFO "ERROR: No such P4D!\n");
            return;
        }
    }
    else
    {
        printk(KERN_INFO "ERROR: No such PGD!\n");
        return;
    }
}

/**
*/
static void print_entry(unsigned long addr)
{
    printk(KERN_INFO "----------------------------------\n");
    printk(KERN_INFO "Present: %lu\n", (addr & 1));
    printk(KERN_INFO "R/W: %lu\n", (addr << 62) >> 63);
    printk(KERN_INFO "U/S: %lu\n", (addr & 4) % 3);
    printk(KERN_INFO "PWT: %lu\n", (addr & 8) % 7);
    printk(KERN_INFO "PCD: %lu\n", (addr & 16) % 15);
    printk(KERN_INFO "A: %lu\n", (addr & 32) % 31);
    printk(KERN_INFO "Rsvd: %lu\n", (addr & 128) % 127);
    printk(KERN_INFO "----------------------------------\n");
}


module_init(my_module_init);
module_exit(my_module_exit);
