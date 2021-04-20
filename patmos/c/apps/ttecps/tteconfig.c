#include "tteconfig.h"

const char* eth_protocol_names[] = {"UFF", "IP", "ICMP", "UDP", "TCP", "PTP", "ARP", "LLDP", "MDNS", "TTE_PCF", "TTE"};
const unsigned int rx_buff_addr = 0x000;
const unsigned int tx_buff_addr = 0x800;
const unsigned char multicastip[4] = {224, 0, 0, 255};
const unsigned char TTE_MAC[] = {0x02, 0x89, 0x1D, 0x00, 0x04, 0x00};
const unsigned char TTE_CT[] = { 0xAB, 0xAD, 0xBA, 0xBE };
const unsigned char TTE_SENSE_VL[] = { 0x0F, 0xA2 };
const unsigned char TTE_CONTROL_VL[] = { 0x0F, 0xA1 };


// __attribute__((noinline))
// unsigned long long get_tte_aligned_time(unsigned long long current_time, unsigned long long clkDiff, unsigned long long clkDiffSum)
// {
// 	long long clock_corr = (((TTE_SYNC_Kp*clkDiff)>>10) 
//                          + ((TTE_SYNC_Ki*clkDiffSum)>>10));
// 	#ifdef TIME_CORRECTION_EN
// 	return (unsigned long long) ((long long) current_time + clock_corr);
// 	#else
// 	return (unsigned long long) current_time;
// 	#endif
// }