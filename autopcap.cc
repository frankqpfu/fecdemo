#include <stdio.h>
#include <stdlib.h>
#include <pcap/pcap.h>
#include <list>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>

using namespace std;

#if 0
struct pcap_file_header {
	bpf_u_int32 magic;
	u_short version_major;
	u_short version_minor;
	bpf_int32 thiszone;	/* gmt to local correction */
	bpf_u_int32 sigfigs;	/* accuracy of timestamps */
	bpf_u_int32 snaplen;	/* max length saved portion of each pkt */
	bpf_u_int32 linktype;	/* data link type (LINKTYPE_*) */
};

struct pcap_pkthdr {
	struct timeval ts;	/* time stamp */
	bpf_u_int32 caplen;	/* length of portion present */
	bpf_u_int32 len;	/* length this packet (off wire) */
};

#endif

struct capture_record_elem
{
    struct pcap_pkthdr hdr;
    char * pkt_data;
    int pkt_len;
};

bool signal_captured = false;
list<capture_record_elem> capture_record_list;
int post_signal_num = 0;

void capture_list_push_elem(struct pcap_pkthdr &hdr, char * pkt_data)
{
    capture_record_elem elem;
    elem.hdr = hdr;
    elem.pkt_data = new char[hdr.caplen];
    memcpy(elem.pkt_data, pkt_data, hdr.caplen);
    elem.pkt_len = hdr.caplen;
    capture_record_list.push_back(elem);

    int list_size = capture_record_list.size();
    while(list_size -- > 2000)
    {
        //free the old data
        capture_record_elem e = capture_record_list.front();
        if (e.pkt_data != NULL)
        {
            delete [] e.pkt_data;
            e.pkt_data = NULL;
        }
        capture_record_list.pop_front();
    }
}

void construct_pcap_header(struct pcap_file_header &header)
{
    header.magic = 0xa1b2c3d4;
    header.version_major = 0x2;
    header.version_minor = 0x4;
    header.thiszone = 0;
    header.sigfigs = 0;
    header.snaplen = 0xffff;
    header.linktype = 1;
}


#define CHECK_RETURN(var, value, fp) \
    do { \
        if (var != value) \
        { \
            printf("line:%d var:%d is not expect value:%d\n", __LINE__, var, value); \
            fclose(fp); \
            return; \
        } \
    }while (0);

void capture_list_dump_file()
{
	printf("try to record the file!\n");
    time_t nowtime = time(NULL);
    struct tm * ptm = localtime(&nowtime);
    char filename[256];
    sprintf(filename, "%d%d%d_%d_%d_%d.pcap", ptm->tm_year, ptm->tm_mon, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
    
    FILE * fp = fopen(filename, "wb");
    if (fp == NULL)
    {
        printf("failed to open %s to record pcap\n", filename);
        return;
    }

    pcap_file_header header;
    construct_pcap_header(header);
    CHECK_RETURN(fwrite(&header, sizeof(header), 1, fp), 1, fp);
    
    list<capture_record_elem>::iterator it = capture_record_list.begin();
    for (; it != capture_record_list.end(); ++it)
    {
        capture_record_elem elem = *it;
		CHECK_RETURN(fwrite(&elem.hdr.ts.tv_sec, 4, 1, fp), 1, fp);
		CHECK_RETURN(fwrite(&elem.hdr.ts.tv_usec, 4, 1, fp), 1, fp);
		CHECK_RETURN(fwrite(&elem.hdr.caplen, 4, 1, fp), 1, fp);
		CHECK_RETURN(fwrite(&elem.hdr.len, 4, 1, fp), 1, fp);
        CHECK_RETURN(fwrite(elem.pkt_data, elem.pkt_len, 1, fp), 1, fp);
    }
    fclose(fp);
    return;
}


pcap_t * open_pcap_handle(char * dev, char * filter_exp)
{
    pcap_t *handle;					
    char errbuf[PCAP_ERRBUF_SIZE];	
    struct bpf_program fp;	
   
    bpf_u_int32 mask;		
    bpf_u_int32 net;		
    struct pcap_pkthdr header;
    const u_char *packet;	

	/*
    dev = pcap_lookupdev(errbuf);
    if (dev == NULL) 
    {
        fprintf(stderr, "Couldn't find default device: %s\n", errbuf);
        return NULL;
    }
	*/

    if (pcap_lookupnet(dev, &net, &mask, errbuf) == -1)
    {
        fprintf(stderr, "Couldn't get netmask for device %s: %s\n", dev, errbuf);
        net = 0;
        mask = 0;
    }

    handle = pcap_open_live(dev, BUFSIZ, 1, 1000, errbuf);
    if (handle == NULL) 
    {
        fprintf(stderr, "Couldn't open device %s: %s\n", dev, errbuf);
        return NULL;
    }

    if (pcap_compile(handle, &fp, filter_exp, 0, net) == -1) 
    {
        fprintf(stderr, "Couldn't parse filter %s: %s\n", filter_exp, pcap_geterr(handle));
        return NULL;
    }

    if (pcap_setfilter(handle, &fp) == -1) 
    {
        fprintf(stderr, "Couldn't install filter %s: %s\n", filter_exp, pcap_geterr(handle));
        return NULL;
    }
    return handle;
}

static void signal_handler(int signum)
{
    if (signum == SIGUSR1)
    {
        signal_captured = true;
    }
}

static void setup_signal_handler()
{
    struct sigaction profstat;
    profstat.sa_handler = signal_handler;
    profstat.sa_flags = 0;
    sigemptyset(&profstat.sa_mask);
    sigaddset(&profstat.sa_mask, SIGUSR1);

    if ( sigaction(SIGUSR1, &profstat, NULL) < 0 )
    {
        fprintf(stderr, "Fail to connect signal SIGUSR1 with start capturing");
    }
}

int main(int argc, char * argv[])
{
    if (argc <= 3)
    {
        printf("usage: %s [devname] [filter_exp1][trigger_exp2]\n", argv[0]);
        return -1;
    }

    char * dev_name = argv[1];
    char * filter1_exp = argv[2];
    char * filter2_exp = argv[3];

    int pid = fork();
	//int pid = 0;
    if (pid == 0) //parent, try to capture
    {
        pcap_t * handle = open_pcap_handle(dev_name, filter1_exp);
        setup_signal_handler();
		int pkt_count = 0;
        while (1)
        {
            struct pcap_pkthdr pkthdr;
            const u_char* pkt_data = pcap_next(handle, &pkthdr);
            if (pkt_data == NULL)
                continue;
	        
			printf("[%d] packet ts:%d, len:%d\n", pkt_count++, pkthdr.ts.tv_sec, pkthdr.len);
            capture_list_push_elem(pkthdr, (char*)pkt_data);
            
            if (signal_captured == true)
            {
                post_signal_num++;
                if (post_signal_num > 20)
                {
                    capture_list_dump_file();
                    signal_captured = false;
                    post_signal_num = 0;
                }
            }
        }
    }
    else
    {
        pcap_t * handle = open_pcap_handle(dev_name, filter2_exp);
        while (1)
        {
            struct pcap_pkthdr pkthdr;
            const u_char* pkt_data = pcap_next(handle, &pkthdr);
            if (pkt_data == NULL)
                continue;

			printf("trigger the point!\n");
            kill(pid, SIGUSR1);
        }
    }
}
