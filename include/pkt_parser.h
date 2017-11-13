/***************************************************************
* 名    称: pkt_parser.h
* 创 建 者: 付红竹
* 创建日期: 2017年11月03日
* 描    述:
* 修改记录:
***************************************************************/

#ifndef __PKT_PARSER_H__
#define __PKT_PATSER_H__
#include "http_proxy.h"


#define HTTP_HOST(http_info)    ((http_info)->headers[(http_info)->host_idx].value)
#define HTTP_USER_AGENT(http_info)    ((http_info)->headers[(http_info)->user_agent_idx].value)


typedef union {
    uint32_t flags;
    struct {
        uint32_t is_pc : 1;
        uint32_t is_mobile  : 1;
        uint32_t is_android : 1;
        uint32_t is_ios : 1;
        uint32_t is_html : 1;
        uint32_t is_js : 1;
        uint32_t is_app : 1;
    }flag;
} request_flag_t;


struct header_item_t {
    char key[MAX_HEADER_KEY_SIZE];
    char value[MAX_HEADER_VALUE_SIZE];
};


struct http_info_t {
    int8_t                  *http_raw;
    uint32_t                http_len;

    http_parser             *parser;
    char                    request_path[MAX_URL_SIZE];
    char                    request_query[MAX_URL_SIZE];
    char                    request_ori_url[MAX_URL_SIZE];
    char                    request_url[MAX_URL_SIZE];
    char                    request_extra_info[MAX_URL_SIZE];
    char                    redirect_extra_query[MAX_URL_SIZE];
    struct header_item_t    headers[MAX_HEADERS];
    int32_t                 header_idx;
    int32_t                 host_idx;
    int32_t                 user_agent_idx;
    int32_t                 cookie_idx;

    request_flag_t          request_flag;

    char                    request_url_key[33];
};


struct pkt_info_t {
//    uint32_t pkt_len;

//    uint8_t* raw;
//    uint32_t raw_len;
//    int32_t cs_hdl_idx;

//    struct   ether_header * eth;
//    uint32_t eth_len;
//    u_char   ether_dhost[ETHER_ADDR_LEN];
//    u_char   ether_shost[ETHER_ADDR_LEN];

//    uint16_t vlan_pri_cfi_vid;
//    uint16_t sub_vlan_pri_cfi_vid;      /* QinQ */
//    uint16_t session;                   /*pppoe session*/
//    char     user_name[USER_NAME_LEN];

//    struct   ip* ip;
//    uint32_t ip_len;
    struct   in_addr ip_src;
    struct   in_addr ip_dst;
//    struct   in_addr user_ip;

//    struct   tcphdr* tcp;
//    uint32_t tcp_len;
//    uint16_t th_sport;
//    uint16_t th_dport;

//    struct   udphdr* udp;
//    uint32_t udp_len;
//    uint16_t uh_sport;
//    uint16_t uh_dport;

//    pkt_type type;

    struct http_info_t http_info;
//    struct dns_info_t dns_info;
};



void grint_print_byte(const int8_t* begin, uint16_t len);
void grint_dump_pkt_info(struct pkt_info_t *pkt_info);

int32_t is_terminal_device_supported(struct pkt_info_t *pkt_info);

int32_t grint_handle_http(void* ctx, struct pkt_info_t *pkt_info);
int32_t grint_handle_http_init(http_parser_settings* settings);

int32_t grint_get_request_flag(struct http_info_t *http_info);




#endif /*__PKT_PATSER_H__*/
