/* server.c */ 
#include <stdio.h> 
#include <string.h> 
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <ctype.h>
#include "wrap.h" 
#define MAXLINE 80 
#define IOT_DEV_FIND_SERV_PORT 1025 

int main(void) 
{ 
	struct sockaddr_in servaddr, cliaddr; 
	socklen_t cliaddr_len;
	int sockfd; 
	char buf[MAXLINE]; 
	char str[INET_ADDRSTRLEN]; 
	int i, n; 
	
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
		
		//n = sendto(sockfd, buf, n, 0, (struct sockaddr *)&cliaddr, sizeof(cliaddr)); 
		//if (n == -1) 
			//perr_exit("sendto error");
	} 

}
#if 0
void user_devicefind_recv(void *arg, char *pusrdata, unsigned short length)
{
    char DeviceBuffer[40] = {0};
    char Device_mac_buffer[60] = {0};
    char hwaddr[6];
    remot_info *premot = NULL;
    struct ip_info ipconfig;

    if (wifi_get_opmode() != STATION_MODE) {
        wifi_get_ip_info(SOFTAP_IF, &ipconfig);
        wifi_get_macaddr(SOFTAP_IF, hwaddr);

        if (!ip_addr_netcmp((struct ip_addr *)ptrespconn.proto.udp->remote_ip, &ipconfig.ip, &ipconfig.netmask)) {
            wifi_get_ip_info(STATION_IF, &ipconfig);
            wifi_get_macaddr(STATION_IF, hwaddr);
        }
    } else {
        wifi_get_ip_info(STATION_IF, &ipconfig);
        wifi_get_macaddr(STATION_IF, hwaddr);
    }

    if (pusrdata == NULL) {
        return;
    }

    if (length == os_strlen(device_find_request) &&
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
    }
}
#endif


