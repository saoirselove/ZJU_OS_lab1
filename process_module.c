#include<linux/module.h> //Need by all modules, modules struct and verison control
#include<linux/kernel.h> //Need for kern info
#include<linux/sched.h> //Need for task_struct

//Sign up the new funciton for kernel
int init_module(void)
{
	//Linux use the task_struct to manage the processes. The definition of task_struct are in the include/linux/sched.h
	//Every task_struct will have a long state. And the state will have two separate sets of flags.
	//State begin with task is about runnability and start with exit is about exiting
	//----------------------------------------------------------------------------------------------------------
	//State can be field can take 5 mutually exclusion valus, says these five state can not be use together. It just like
	//a people can not be both man and woman. Every process in the system must be in one of there state. 
	// TASK_RUNNING          	executing or ready to execute(wait for the cpu time)
	// TASK_INTERRUPTIBLE		wait for some conditon satisfied, such as interrupt, resources, signal
	// TASK_UNINTERRUPTIBLE		only can be wake by the resources get
	// TASK_STOPPED				stop execute, after getting SIGSTOP,SIGTSTP,SIGTTOU sinal will  enter
	// TASK_TRACED				surveillance by debbuger
	//----------------------------------------------------------------------------------------------------------
	int processCount=0;                   	//total process
	int processCountRunning=0;				//total running process
	int processCountInterruptible=0;		//interruptible process count
	int processCountUninterruptible=0;		//uninteruptible process count
	int processCountStopped=0;				//stopped process count
	int processCountTraced=0;				//traced process count
	int processCountZombie=0;				//zombie process count (ps: process is terminated by the father process not get it's terminated info)
	int processCountDead=0;					//terminate process count (ps:the last state of a process)
	int processCountUnknow=0;				//none of the above, we put the process into the unknow set
	long processState;						//a temporary long variable for record the running-state in task_struct
	long processExitState;					//a temporary long variable for record the exit-state in task_struct
	
	//For this version of linux there be 3 special process: process idle(pid=0), process systemd(pid=1), process kthread(pid=2). the idle's predecessor
	//is the first process created by the system and the only one not generated by fork() or kernel_thread. For SMP system, each processor will have a 
	//idle process. The free time of the system actually is the running time of the idle process.
	//Kernel static construct a task_struct named init_task during the initialization. Then kenerl use the rest_init() to build kernel init thread(pid=1) and 
	//kthread kernel thread(pid=2). After the initialization finished the kernel will use the init_task as its descriptor, the kernel will be the idle process.
	//init_task decide the gene of all the process and thread and it will turn to the process idle(pid=0) and run in the kernel mode after initialization.
	//----------------------------------------------------------------------------------------------------------
	
	//Briefly, the init_task in kernel is the descrptor of process 0.
	struct task_struct *p=&init_task;		//a struct_task pointer ponit to the init_task
	
	printk("@@@ModuleFunctionStart\n");		//output message to the "/var/log/kern.log". Use some remark to get the location in the kern.log
	//We could use the marco for for_each_process(), which is start the loop from the init_task, and travese from the first process to the end process and back to 
	// the init_task. Then we could print the infomation of each task_struct to finish our target.(remark: the process 0 will not be printed!)

	for_each_process(p)//for(p=&init_task;(p=next_task(p))!=&init_task;)
	{
		printk("@PID:%d\n",p->pid);						//pid
		printk("@NAME:%s\n",p->comm);					//descrptor
		printk("@FATHER_PID:%d\n",p->parent->pid);		//father's pid
		printk("@FATHER_NAME:%s\n",p->parent->comm);	//father's name

		++processCount;

		processState=p->state;							//get the process state
		processExitState=p->exit_state;					//get the exit_state

		if(processExitState==EXIT_ZOMBIE)++processCountZombie;	//check the exitprocess state and change the count
		else if(processExitState==EXIT_DEAD)++processCountDead;	//change the count
		//else break;

		if(processExitState)		//output the state information of the process
		{
			printk("@STATE:%ld\n",processExitState);
			printk("-------------------------------------------------------------------------\n");
			continue;
		}

		//statistic the 5 mutual exclusive states
		if(processState==TASK_RUNNING)++processCountRunning;
		else if(processState==TASK_INTERRUPTIBLE)++processCountInterruptible;
		else if(processState==TASK_UNINTERRUPTIBLE)++processCountUninterruptible;
		else if(processState==TASK_STOPPED)++processCountStopped;
		else if(processState==TASK_TRACED)++processCountTraced;
		else ++processCountUnknow;

		printk("@STATE:%ld\n",processState);  //output the state information of the process
		printk("-------------------------------------------------------------------------\n");
	}

	//Eutput the statistic information
	printk("@TASK_TOTAL           = %d\n",processCount);
	printk("@TASK_RUNNING         = %d\n",processCountRunning);
	printk("@TASK_INTERRUPTIBLE   = %d\n",processCountInterruptible);
	printk("@TASK_UNINTERRUPTIBLE = %d\n",processCountUninterruptible);
	printk("@TASK_STOPPED         = %d\n",processCountStopped);
	printk("@TASK_TRACED          = %d\n",processCountTraced);
	printk("@EXIT_ZOMBIE          = %d\n",processCountZombie);
	printk("@EXIT_DEAD            = %d\n",processCountDead);
	printk("@UNKNOW               = %d\n",processCountUnknow);

	//End the out put
	printk("!!!ModuleFunctionFinished\n");

	return 0;
}

void cleanup_module(void)//Logout the function the module provide
{
	printk("@Byb~Dude!\n");//Output the end information
}

MODULE_LICENSE("GPL");//licens


		

