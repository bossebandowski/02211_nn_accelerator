#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <machine/patmos.h>
#include <machine/spm.h>
#include "common.h"
#include "tteconfig.h"
#include "servo.h"

// Demo parameters
#define HYPERPERIOD			10000000
#define NO_TASKS			4
#define SYNC_WINDOW_HALF	10000		//ns
#define ASYNC2SYNC_THRES	10			//clusters
#define RECV_WINDOW_HALF	10000		//ns

#define SYNCTASK_GPIO_BIT	0
#define RECVTASK_GPIO_BIT	1
#define CALCTASK_GPIO_BIT	2
#define SENDTASK_GPIO_BIT	3

//Clock related variables
PTPPortInfo thisPtpPortInfo;
PTPv2Time hardTimestamp;
PTPv2Time softTimestamp;
static long long clkDiff = 0;
static long long clkDiffLast = 0;
static long long clkDiffSum = 0;
static unsigned short integration_cycle = 0;
static unsigned long long coldStartIntegrationTime = 0;
static unsigned long long initNanoseconds = 0;
static unsigned long long initSeconds = 0;

//Counters
static unsigned short rxPcfCount = 0;
static unsigned int stableCycles = 0;
static unsigned int unstableCycles = 0;
static unsigned int stableClusters = 0;
static unsigned int unstableClusters = 0;
static unsigned int totalScheduledReceptions = 0;
static unsigned int pulseDuration = DEAD_CALC(MAX_CYCLE);

//Flags
static unsigned char nodeIntegrated = 0;	//is used to indicate when the node has achieved sufficient syncrhonization
static unsigned char nodeSyncStable = 0;	//is used to enable task execution when the node is in a stable sync
static unsigned char nodeColdStart = 1;		//is used to indicate that a node has just booted and has not received a single PCF
static unsigned char nodeFirstSync = 1;
static unsigned char nodeRecvEnable = 0;

//Communication
static unsigned int txMsgCount = 0;
static unsigned int rxMsgCount = 0;

//Schedule
static SimpleTTETask schedule[NO_TASKS] = {
	{
		.period = 10000000,
		.release_times = (unsigned long long[]){0},
		.release_inst = 0,
		.nr_releases = 2,
		.last_time = 0,
		.delta_sum = 0,
		.exec_count = 0,
		.task_fp = (generic_task_fp)task_sync 
	},
	{
		.period = 5000000,
		.release_times = (unsigned long long[]){1200000, 6200000},
		.release_inst = 0,
		.nr_releases = 1,
		.last_time = 0,
		.delta_sum = 0,
		.exec_count = 0,
		.task_fp = (generic_task_fp)task_recv
	},
	{
		.period = 5000000,
		.release_times = (unsigned long long[]){1632912, 6632912},
		.release_inst = 0,
		.nr_releases = 2,
		.last_time = 0,
		.delta_sum = 0,
		.exec_count = 0,
		.task_fp = (generic_task_fp)task_calc
	},
	{
		.period = 5000000,
		.release_times = (unsigned long long[]){3600000, 8600000},
		.release_inst = 0,
		.nr_releases = 2,
		.last_time = 0,
		.delta_sum = 0,
		.exec_count = 0,
		.task_fp = (generic_task_fp)task_send
	}
};

__attribute__((noinline))
unsigned long long get_tte_aligned_time(unsigned long long current_time)
{
	long long clock_corr = (((TTE_SYNC_Kp*clkDiff)>>10) 
                        + ((TTE_SYNC_Ki*clkDiffSum)>>10));
	#ifdef TIME_CORRECTION_EN
	return (unsigned long long) ((long long) current_time + clock_corr);
	#else
	return (unsigned long long) current_time;
	#endif
}

__attribute__((noinline))
int tte_pcf_handle(unsigned long long sched_rec_pit, unsigned long long schedule_start)
{
	unsigned long long trans_clock;
	unsigned long long permanence_pit;

	trans_clock = mem_iord_byte(rx_buff_addr + 34);
	trans_clock = (trans_clock << 8) | (mem_iord_byte(rx_buff_addr + 35));
	trans_clock = (trans_clock << 8) | (mem_iord_byte(rx_buff_addr + 36));
	trans_clock = (trans_clock << 8) | (mem_iord_byte(rx_buff_addr + 37));
	trans_clock = (trans_clock << 8) | (mem_iord_byte(rx_buff_addr + 38));
	trans_clock = (trans_clock << 8) | (mem_iord_byte(rx_buff_addr + 38));
	trans_clock = (trans_clock << 8) | (mem_iord_byte(rx_buff_addr + 40));
	trans_clock = (trans_clock << 8) | (mem_iord_byte(rx_buff_addr + 41));
	trans_clock = trans_clock / TTETIME_TO_NS;

	integration_cycle = mem_iord_byte(rx_buff_addr + 14);
	integration_cycle = (integration_cycle << 8) | (mem_iord_byte(rx_buff_addr + 15));
	integration_cycle = (integration_cycle << 8) | (mem_iord_byte(rx_buff_addr + 16));
	integration_cycle = (integration_cycle << 8) | (mem_iord_byte(rx_buff_addr + 17));

	#ifdef HW_TIMESTAMPING
	permanence_pit = PTP_TIME_TO_NS(hardTimestamp.seconds, hardTimestamp.nanoseconds) - schedule_start + ((unsigned long long) TTE_MAX_TRANS_DELAY - trans_clock);
	#else
	permanence_pit = PTP_TIME_TO_NS(softTimestamp.seconds, softTimestamp.nanoseconds) - schedule_start + ((unsigned long long) TTE_MAX_TRANS_DELAY - trans_clock);
	#endif

	if((nodeColdStart && integration_cycle==0) || !nodeColdStart)
	{
		nodeColdStart = 0;
		clkDiff = (long long) (permanence_pit - sched_rec_pit);
		clkDiffSum += clkDiff;
		//check scheduled receive window
		if(permanence_pit>(sched_rec_pit-TTE_PRECISION) && permanence_pit<(sched_rec_pit+TTE_PRECISION))
		{
			stableCycles++;
			if(integration_cycle == 0)
			{
				stableClusters++;
			}
		} else {
			unstableCycles++;
			if(integration_cycle == 0)
			{
				unstableClusters++;
			}
		}
	}
	return 1;
}

__attribute__((noinline))
void task_sync(unsigned long long start_time, unsigned long long schedule_time, SimpleTTETask* tasks)
{
	GPIO |= (1U << SYNCTASK_GPIO_BIT);
	int ethFrameType;
	PTP_RXCHAN_STATUS(thisPtpPortInfo.eth_base) = 0x1; //Clear timestampAvail flag
	unsigned long long listen_start = get_cpu_usecs(); //keep track when we started listening
	#pragma loopbound min 1 max 1
	do
	{
		if(eth_mac_receive_nb(rx_buff_addr))
		{
			if((unsigned short) ((mem_iord_byte(rx_buff_addr + 12) << 8) + (mem_iord_byte(rx_buff_addr + 13))) == 0x891D)
			{
				#ifdef HW_TIMESTAMPING
				hardTimestamp.nanoseconds = PTP_RXCHAN_TIMESTAMP_NS(thisPtpPortInfo.eth_base);
				hardTimestamp.seconds = PTP_RXCHAN_TIMESTAMP_SEC(thisPtpPortInfo.eth_base);
				#else
				softTimestamp.nanoseconds = (unsigned) RTC_TIME_NS(thisPtpPortInfo.eth_base);
				softTimestamp.seconds = (unsigned) RTC_TIME_SEC(thisPtpPortInfo.eth_base);
				#endif
				ethFrameType = TTE_PCF;
			}
			PTP_RXCHAN_STATUS(thisPtpPortInfo.eth_base) = 0x1; //Clear PTP timestampAvail flag
		}
	} while(ethFrameType != TTE_PCF && get_cpu_usecs() - listen_start < 2*SYNC_WINDOW_HALF*NS_TO_USEC);
	//work
	if(ethFrameType == TTE_PCF)
	{
		tte_pcf_handle(schedule_time, start_time);
		nodeIntegrated = !nodeColdStart && ethFrameType > 0 && (stableCycles - unstableCycles) > 0 && abs(clkDiff) < TTE_PRECISION;
		if((nodeFirstSync && integration_cycle == 0 && nodeIntegrated) || !nodeFirstSync)
        {
			nodeSyncStable = nodeIntegrated && (stableClusters - unstableClusters) > ASYNC2SYNC_THRES;	
			nodeFirstSync = 0;
		}
		rxPcfCount += 1;
	} 
	else if(ethFrameType == TIMEOUT)
	{
		nodeFirstSync = 1;
		nodeColdStart = 1;
		nodeIntegrated = 0;
		nodeSyncStable = 0;
		clkDiffLast = 0;
		clkDiffSum = 0;
		clkDiff = 0;
	}
	#pragma loopbound min 1 max 4
	for(int i=0; i<NO_TASKS; i++)
	{
		#pragma loopbound min 1 max 2
		for(int n=tasks[i].release_inst; n<tasks[i].nr_releases; n++)
			tasks[i].release_times[n] = get_tte_aligned_time(tasks[i].release_times[n]);
	}
	LEDS = (nodeSyncStable << 7) + (nodeIntegrated << 6) + (ethFrameType & 0xF);
	GPIO &= (0U << SYNCTASK_GPIO_BIT);
}

__attribute__((noinline))
void task_recv(SimpleTTMessage* message, int length)
{
	GPIO |= (1U << RECVTASK_GPIO_BIT);
	if(nodeSyncStable){
		unsigned short ethType = UNSUPPORTED;
		unsigned long long listen_start = get_cpu_usecs();
		#pragma loopbound min 1 max 1
		do{
			if(eth_mac_receive_nb(rx_buff_addr))
			{
				ethType = (unsigned short) ((mem_iord_byte(rx_buff_addr + 12) << 8) + (mem_iord_byte(rx_buff_addr + 13)));
				LEDS &= 0xF0;
				LEDS |= (ethType & 0xF);
				if(ethType != 0x891D)
				{
					break;
				}
			}
		}while(get_cpu_usecs() - listen_start < 2*SYNC_WINDOW_HALF*NS_TO_USEC);
		if(ethType != UNSUPPORTED)
		{
			//Data
			#pragma loopbound min 24 max 24
			for(int i=0; i<length; i++)
			{
				*((unsigned char*)message + i) = mem_iord_byte(rx_buff_addr + 18 + i);
			}
			rxMsgCount++;
			LEDS |= (3U << 4);
		} 
		else 
		{
			LEDS |= (1U << 5);
		}
	}
	GPIO &= (0U << RECVTASK_GPIO_BIT);
}

__attribute__((noinline))
void task_calc(const SimpleTTMessage *incoming_message, SimpleTTMessage *outgoing_message)
{
    outgoing_message->seq = incoming_message->seq;
    outgoing_message->accel.x = incoming_message->accel.x;
    outgoing_message->accel.y = incoming_message->accel.y;
    outgoing_message->accel.z = incoming_message->accel.z;
    outgoing_message->gyro.x = incoming_message->gyro.x;
    outgoing_message->gyro.y = incoming_message->gyro.y;
    outgoing_message->gyro.z = incoming_message->gyro.z;
    outgoing_message->theta = 0.93f * incoming_message->gyro.x + 0.07 * atan2_approximation2(incoming_message->accel.y, incoming_message->accel.z);
    outgoing_message->duty_cycle = (outgoing_message->theta * (MAX_CYCLE - MIN_CYCLE) / 180) + MIN_CYCLE;
}

__attribute__((noinline))
void task_send(const SimpleTTMessage* message, int length, const unsigned char VL[2])
{
	GPIO |= (1U << SENDTASK_GPIO_BIT);
	if(nodeSyncStable)
	{
		unsigned char ethType[2];
		ethType[0]=((4+length)>>8) & 0xFF;
		ethType[1]=(4+length) & 0xFF;
		//Header
		#pragma loopbound min 4 max 4
		for(int i=0; i<4; i++)
		{
			mem_iowr_byte(tx_buff_addr + i, TTE_CT[i]);
		}
		mem_iowr_byte(tx_buff_addr + 4, VL[0]);
		mem_iowr_byte(tx_buff_addr + 5, VL[1]);

		#pragma loopbound min 6 max 6
		for(int i=6; i<12; i++)
		{
			mem_iowr_byte(tx_buff_addr + i, TTE_MAC[i-6]);
		}
		mem_iowr_byte(tx_buff_addr + 12, ethType[0]);
		mem_iowr_byte(tx_buff_addr + 13, ethType[1]);
		//Data
		#pragma loopbound min 24 max 24
		for(int i=0; i<length; i++)
		{
			mem_iowr_byte(tx_buff_addr + 18 + i, *((unsigned char*) message + i));
		}
		eth_mac_send_nb(tx_buff_addr, 18+4+length);
		txMsgCount++;
	}
	GPIO &= (0U << SENDTASK_GPIO_BIT);
}

__attribute__((noinline))
void cyclic_executive_loop(SimpleTTETask* schedule){
	SimpleTTMessage incoming_message, outgoing_message;
	unsigned long long start_time = get_ptp_nanos(thisPtpPortInfo.eth_base);
	#pragma loopbound min 1 max 1
	while(KEYS != 0xE && KEYS != 0xC){
    	register unsigned long long schedule_time = get_ptp_nanos(thisPtpPortInfo.eth_base);
		schedule_time = get_tte_aligned_time(schedule_time - start_time);
		#pragma loopbound min 1 max 1
		for(int task=0; task < NO_TASKS; task++){
			if(schedule_time >= schedule[task].release_times[schedule[task].release_inst])
			{
				switch (task)
				{
				case 0:
					((task_sync_fp)(schedule[task].task_fp))(start_time, schedule_time, schedule);
					break;
				case 1:
					((task_recv_fp)(schedule[task].task_fp))(&incoming_message, sizeof(SimpleTTMessage));
					break;
				case 2:
					// ((task_calc_fp)(schedule[task].task_fp))(&incoming_message, &outgoing_message);
                    task_calc(&incoming_message, &outgoing_message);    //TODO: check casting error?
					break;
				case 3:
					((task_send_fp)(schedule[task].task_fp))(&outgoing_message, sizeof(SimpleTTMessage), TTE_CONTROL_VL);
					break;
				}
				schedule[task].release_times[schedule[task].release_inst] += HYPERPERIOD;
				schedule[task].release_inst = (schedule[task].release_inst + 1) % schedule[task].nr_releases;
				schedule[task].delta_sum += schedule_time - get_tte_aligned_time(schedule[task].last_time);
				schedule[task].last_time = schedule_time;
				schedule[task].exec_count += 1;
				break;
			}
		}
	}
}

int main(int argc, char **argv){
	LEDS = GPIO = 0x1FF;
	puts("\nTTECPS Control Node Started");

	//MAC controller settings
	set_mac_address(0x1D000400, 0x00000289);
	eth_iowr(MODER, (RECSMALL_BIT | CRCEN_BIT | FULLD_BIT | PRO_BIT | TXEN_BIT | RXEN_BIT));
	eth_iowr(INT_SOURCE, INT_SOURCE_RXB_BIT);
    eth_iowr(RX_BD_ADDR_BASE(eth_iord(TX_BD_NUM)), RX_BD_EMPTY_BIT | RX_BD_IRQEN_BIT | RX_BD_WRAP_BIT);
	eth_iowr(INT_SOURCE, INT_SOURCE_TXB_BIT);
	eth_iowr(TX_BD_ADDR_BASE, TX_BD_READY_BIT | TX_BD_IRQEN_BIT | TX_BD_PAD_EN_BIT);
	thisPtpPortInfo = ptpv2_intialize_local_port(PATMOS_IO_ETH, PTP_SLAVE, my_mac, my_ip, 1, 0);
	LEDS = GPIO = 0x0;

	sort_asc_ttetasks(schedule, NO_TASKS);

	// Executive Loop
	do{
		nodeFirstSync = 1;
		nodeColdStart = 1;
		nodeIntegrated = 0;
		nodeSyncStable = 0;
		clkDiffLast = 0;
		clkDiffSum = 0;
		clkDiff = 0;
		cyclic_executive_loop(schedule);
	}while(KEYS !=  0xE);

	//Report
	puts("\nTTECPS Control Node Exiting and Reporting...");
	puts("------------------------------------------");
	puts("Clock Sync Quality Log:");
	printf("--Avg. clock offset = %lld ns\n", clkDiffSum / rxPcfCount);
	puts("---------------------------------------------------------------------------");
	puts("Communication Log:");
	printf("--Received No. of Frames = %u\n", rxPcfCount+rxMsgCount);
	printf("--In-Sched No. of PCF = %u (loss = %.3f %%) \n", stableCycles, 100-((float)stableCycles/(float)rxPcfCount)*100);
	printf("--No-Sched No. of PCF = %u (success = %.3f %%) \n", unstableCycles, 100-((float)unstableCycles/(float)rxPcfCount)*100);
	printf("--Received No. of Message Frames = %u\n", rxMsgCount);
	printf("--Transmit No. of Message Frames = %u\n", txMsgCount);
	puts("---------------------------------------------------------------------------");
	puts("Task log:");
	for(int i=0; i<NO_TASKS; i++){
		unsigned long long avgDelta = schedule[i].delta_sum/schedule[i].exec_count;
		printf("--task[%d]   avg. dt = %llu\t(avg. jitter = %d ns) from a total of %lu executions\n", i, avgDelta, abs(schedule[i].period - avgDelta), schedule[i].exec_count);
	}
	puts("---------------------------------------------------------------------------");
	
	return 0;
}