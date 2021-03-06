
#ifndef WPA_COMMON_H
#define WPA_COMMON_H

#define WPA_REPLAY_COUNTER_LEN 8
#define WPA_NONCE_LEN 32
#define WPA_KEY_RSC_LEN 8


/* IEEE Std 802.1X-2004 */

#ifdef _MSC_VER
#pragma pack(push, 1)
#endif /* _MSC_VER */

struct ieee802_1x_hdr {
	u8 version;
	u8 type;
	u16 length;
	/* followed by length octets of data */
} STRUCT_PACKED;

#ifdef _MSC_VER
#pragma pack(pop)
#endif /* _MSC_VER */

#define EAPOL_VERSION 2

enum { IEEE802_1X_TYPE_EAP_PACKET = 0,
       IEEE802_1X_TYPE_EAPOL_START = 1,
       IEEE802_1X_TYPE_EAPOL_LOGOFF = 2,
       IEEE802_1X_TYPE_EAPOL_KEY = 3,
       IEEE802_1X_TYPE_EAPOL_ENCAPSULATED_ASF_ALERT = 4
};

enum { EAPOL_KEY_TYPE_RC4 = 1, EAPOL_KEY_TYPE_RSN = 2,
       EAPOL_KEY_TYPE_WPA = 254 };

#ifdef CONFIG_IEEE80211W
#define WPA_DGTK_LEN 16
#define WPA_DHV_LEN 16
#define WPA_IGTK_LEN 16
#endif /* CONFIG_IEEE80211W */

#endif /* WPA_COMMON_H */
