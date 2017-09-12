/* server.c */ 
#include <stdio.h> 
#include <string.h> 
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <ctype.h>
#include "wrap.h" 
#define MAXLINE 80 
#define IOT_DEV_FIND_SERV_PORT 1025 

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;

const char *device_find_request = "Are You Espressif IOT Smart Device?";

#define MAC2STR(a) (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5]
#define MACSTR "%02x:%02x:%02x:%02x:%02x:%02x"

#define ip4_addr1(ipaddr) (((uint8*)(ipaddr))[0])
#define ip4_addr2(ipaddr) (((uint8*)(ipaddr))[1])
#define ip4_addr3(ipaddr) (((uint8*)(ipaddr))[2])
#define ip4_addr4(ipaddr) (((uint8*)(ipaddr))[3])

#define ip4_addr1_16(ipaddr) ((uint16)ip4_addr1(ipaddr))
#define ip4_addr2_16(ipaddr) ((uint16)ip4_addr2(ipaddr))
#define ip4_addr3_16(ipaddr) ((uint16)ip4_addr3(ipaddr))
#define ip4_addr4_16(ipaddr) ((uint16)ip4_addr4(ipaddr))

#define IP2STR(ipaddr) ip4_addr1_16(ipaddr), \
    ip4_addr2_16(ipaddr), \
    ip4_addr3_16(ipaddr), \
    ip4_addr4_16(ipaddr)

#define IPSTR "%d.%d.%d.%d"

#define IP4_ADDR(ipaddr, a,b,c,d) \
        ipaddr = ((uint32)((d) & 0xff) << 24) | \
                         ((uint32)((c) & 0xff) << 16) | \
                         ((uint32)((b) & 0xff) << 8)  | \
                          (uint32)((a) & 0xff)


#define DEVICE_FIND_RESPONSE_HEADER "I'm %s." // %s ===> device_find_response_type
const char *device_find_response_type = "IOTServer";

void iot_device_find_response(char *prespBuff, void *pIP);


int main(void) 
{ 
	struct sockaddr_in servaddr, cliaddr; 
	socklen_t cliaddr_len;
	int sockfd; 
	char buf[MAXLINE]; 
	char str[INET_ADDRSTRLEN]; 
	int i, n; 
	
	char DeviceBuffer[80] = "I'm Light.1a:fe:34:12:34:56 192.168.1.230";
	unsigned short length;
	unsigned int dummy_ip = 0;
	
	IP4_ADDR(dummy_ip, 192,168,1,230);
	
	sockfd = Socket(AF_INET, SOCK_DGRAM, 0); 
	bzero(&servaddr, sizeof(servaddr)); 
	servaddr.sin_family = AF_INET; 
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
	servaddr.sin_port = htons(IOT_DEV_FIND_SERV_PORT); 
	Bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)); 
	printf("Accepting connections ...\n"); 
	while (1) 
	{ 
		cliaddr_len = sizeof(cliaddr); 
		n = recvfrom(sockfd, buf, MAXLINE, 0, (struct sockaddr *)&cliaddr, &cliaddr_len); 
		if (n == -1) perr_exit("recvfrom error"); 
		printf("received from %s at PORT %d\n", 	\
			inet_ntop(AF_INET, &cliaddr.sin_addr, str, sizeof(str)), \
			ntohs(cliaddr.sin_port));
		
		//查看接收内容
		for (i = 0; i < n; i++) 
			putchar(buf[i]); 
		printf("\n\r"); 
		iot_device_find_response(DeviceBuffer, &dummy_ip);
		length = strlen(DeviceBuffer);
		n = sendto(sockfd, DeviceBuffer, length, 0, (struct sockaddr *)&cliaddr, sizeof(cliaddr)); 
		if (n == -1) 
			perr_exit("sendto error");
		
		printf("Server response:%s\n\r", DeviceBuffer); 

	} 

}
#if 1

//station: 00:9a:cd:23:93:ce join, AID = 1
//I'm Light.18:fe:34:fe:a8:64 0.0.0.0
//I'm Light.1a:fe:34:fe:a8:64 192.168.4.1
//I'm Light.1a:fe:34:fe:a8:64 192.168.4.1

void iot_device_find_response(char *prespBuff, void *pIP)
{
    unsigned char hwaddr[6] = {0x11,0x22,0x33,0x99,0x88,0x77};


/*     if (wifi_get_opmode() != STATION_MODE) {  // get  MAC ADDR
        wifi_get_ip_info(SOFTAP_IF, &ipconfig);
        wifi_get_macaddr(SOFTAP_IF, hwaddr);

        if (!ip_addr_netcmp((struct ip_addr *)ptrespconn.proto.udp->remote_ip, &ipconfig.ip, &ipconfig.netmask)) {
            wifi_get_ip_info(STATION_IF, &ipconfig);
            wifi_get_macaddr(STATION_IF, hwaddr);
        }
    } else {
        wifi_get_ip_info(STATION_IF, &ipconfig);
        wifi_get_macaddr(STATION_IF, hwaddr);
    } */

    if (prespBuff == NULL) {
        return;
    }

	sprintf(prespBuff, DEVICE_FIND_RESPONSE_HEADER MACSTR " " IPSTR, device_find_response_type,
        MAC2STR(hwaddr), IP2STR(pIP));

/*     if (length == os_strlen(device_find_request) &&
            os_strncmp(pusrdata, device_find_request, os_strlen(device_find_request)) == 0) {
        os_sprintf(DeviceBuffer, "%s" MACSTR " " IPSTR, device_find_response_ok,
                   MAC2STR(hwaddr), IP2STR(&ipconfig.ip));

        os_printf("%s\n", DeviceBuffer);
        length = os_strlen(DeviceBuffer);
        if (espconn_get_connection_info(&ptrespconn, &premot, 0) != ESPCONN_OK)
        	return;
        os_memcpy(ptrespconn.proto.udp->remote_ip, premot->remote_ip, 4);
        ptrespconn.proto.udp->remote_port = premot->remote_port;
        espconn_sent(&ptrespconn, DeviceBuffer, length);
    } else if (length == (os_strlen(device_find_request) + 18)) {
        os_sprintf(Device_mac_buffer, "%s " MACSTR , device_find_request, MAC2STR(hwaddr));
        os_printf("%s", Device_mac_buffer);

        if (os_strncmp(Device_mac_buffer, pusrdata, os_strlen(device_find_request) + 18) == 0) {
            //os_printf("%s\n", Device_mac_buffer);
            length = os_strlen(DeviceBuffer);
            os_sprintf(DeviceBuffer, "%s" MACSTR " " IPSTR, device_find_response_ok,
                       MAC2STR(hwaddr), IP2STR(&ipconfig.ip));

            os_printf("%s\n", DeviceBuffer);
            length = os_strlen(DeviceBuffer);
			if (espconn_get_connection_info(&ptrespconn, &premot, 0) != ESPCONN_OK)
				return;
			os_memcpy(ptrespconn.proto.udp->remote_ip, premot->remote_ip, 4);
			ptrespconn.proto.udp->remote_port = premot->remote_port;
            espconn_sent(&ptrespconn, DeviceBuffer, length);
        } else {
            return;
        }
    } */
}
#endif


