
#include <bluetooth/bluetooth.h>
#include <bluetooth/sdp.h>

#ifdef SDP_DEBUG
#include <syslog.h>
#define SDPDBG(fmt, arg...) syslog(LOG_DEBUG, "%s: " fmt "\n", __func__ , ## arg)
#else
#define SDPDBG(fmt...)
#endif

typedef struct request {
	bdaddr_t device;
	bdaddr_t bdaddr;
	int      local;
	int      sock;
	int      mtu;
	int      flags;
	uint8_t  *buf;
	int      len;
} sdp_req_t;

void handle_request(int sk, uint8_t *data, int len);

int service_register_req(sdp_req_t *req, sdp_buf_t *rsp);
int service_update_req(sdp_req_t *req, sdp_buf_t *rsp);
int service_remove_req(sdp_req_t *req, sdp_buf_t *rsp);

void register_public_browse_group(void);
void register_server_service(void);
void register_device_id(const uint16_t vendor, const uint16_t product,
						const uint16_t version);

typedef struct {
	uint32_t timestamp;
	union {
		uint16_t maxBytesSent;
		uint16_t lastIndexSent;
	} cStateValue;
} sdp_cont_state_t;

#define SDP_CONT_STATE_SIZE (sizeof(uint8_t) + sizeof(sdp_cont_state_t))

sdp_buf_t *sdp_get_cached_rsp(sdp_cont_state_t *cstate);
void sdp_cstate_cache_init(void);
void sdp_cstate_clean_buf(void);

int record_sort(const void *r1, const void *r2);
void sdp_svcdb_reset(void);
void sdp_svcdb_collect_all(int sock);
void sdp_svcdb_set_collectable(sdp_record_t *rec, int sock);
void sdp_svcdb_collect(sdp_record_t *rec);
sdp_record_t *sdp_record_find(uint32_t handle);
void sdp_record_add(const bdaddr_t *device, sdp_record_t *rec);
int sdp_record_remove(uint32_t handle);
sdp_list_t *sdp_get_record_list(void);
sdp_list_t *sdp_get_access_list(void);
int sdp_check_access(uint32_t handle, bdaddr_t *device);
uint32_t sdp_next_handle(void);

uint32_t sdp_get_time();

#define SDP_SERVER_COMPAT (1 << 0)
#define SDP_SERVER_MASTER (1 << 1)

int start_sdp_server(uint16_t mtu, const char *did, uint32_t flags);
void stop_sdp_server(void);

int add_record_to_server(const bdaddr_t *src, sdp_record_t *rec);
int remove_record_from_server(uint32_t handle);

void create_ext_inquiry_response(const char *name,
					int8_t tx_power, sdp_list_t *services,
					uint8_t *data);
static inline int android_get_control_socket(const char *name);
