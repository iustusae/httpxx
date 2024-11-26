#pragma once
#include <asm-generic/socket.h>
#include <netinet/in.h>
#include <type_traits>

enum class AddressFamilies {
  af_unspec = AF_UNSPEC,       // Unspecified address family
  af_local = AF_LOCAL,         // Local communication
  af_unix = AF_UNIX,           // Unix domain sockets
  af_file = AF_FILE,           // File descriptor based communication
  af_inet = AF_INET,           // IPv4 internet protocols
  af_ax = AF_AX25,             // AX.25 packet radio protocol
  af_ipx = AF_IPX,             // IPX (Internetwork Packet Exchange)
  af_appletalk = AF_APPLETALK, // AppleTalk protocols
  af_netrom = AF_NETROM,       // NET/ROM packet radio protocol
  af_bridge = AF_BRIDGE,       // Ethernet bridging
  af_atmpvc = AF_ATMPVC,       // ATM PVCs (Permanent Virtual Circuits)
  af_x = AF_X25,               // X.25 (packet-switched network)
  af_inet6 = AF_INET6,         // IPv6 internet protocols
  af_rose = AF_ROSE,           // ROSE (Reliable On-Surface Emulation)
  af_dec = AF_DECnet,          // DECnet protocols
  af_netbeui = AF_NETBEUI,     // NetBIOS Extended User Interface
  af_security = AF_SECURITY,   // Security-aware communication
  af_key = AF_KEY,             // Key management protocol
  af_netlink = AF_NETLINK,     // Netlink communication protocol
  af_route = AF_ROUTE,         // Routing interface
  af_packet = AF_PACKET,       // Low-level packet interface
  af_ash = AF_ASH,             // Ash protocol
  af_econet = AF_ECONET,       // Econet protocol
  af_atmsvc = AF_ATMSVC,       // ATM service
  af_rds = AF_RDS,             // Reliable Datagram Sockets
  af_sna = AF_SNA,             // SNA (Systems Network Architecture)
  af_irda = AF_IRDA,           // IrDA (Infrared Data Association)
  af_pppox = AF_PPPOX,         // PPP over various media
  af_wanpipe = AF_WANPIPE,     // WAN (Wide Area Network) communication
  af_llc = AF_LLC,             // LLC (Logical Link Control)
  af_ib = AF_IB,               // InfiniBand
  af_mpls = AF_MPLS,           // Multiprotocol Label Switching
  af_can = AF_CAN,             // Controller Area Network
  af_tipc = AF_TIPC,           // TIPC (Transparent Inter-Process Communication)
  af_bluetooth = AF_BLUETOOTH, // Bluetooth protocols
  af_iucv = AF_IUCV,           // IUCV (Integrated User Communication Vehicle)
  af_rxrpc = AF_RXRPC,         // RXRPC (Reliable XRPC)
  af_isdn = AF_ISDN,           // ISDN (Integrated Services Digital Network)
  af_phonet = AF_PHONET,       // Phonet protocol
  af_ieee_802_15_4 =
      AF_IEEE802154,       // IEEE 802.15.4 (Wireless Personal Area Networks)
  af_caif = AF_CAIF,       // CAIF (Common Air Interface)
  af_alg = AF_ALG,         // Algorithm interface
  af_nfc = AF_NFC,         // NFC (Near Field Communication)
  af_vsock = AF_VSOCK,     // Virtio socket communication
  af_kcm = AF_KCM,         // Kernel Connection Multiplexer
  af_qipcrtr = AF_QIPCRTR, // QIPC router
  af_smc = AF_SMC,         // SMC (Shared Memory Communications)
  af_xdp = AF_XDP,         // Express Data Path
  af_mctp = AF_MCTP,       // MCTP (Management Component Transport Protocol)
  af_max = AF_MAX          // Maximum address family value
};

enum class SocketType {
  stream = SOCK_STREAM,              // Reliable, connection-based byte streams
  datagram = SOCK_DGRAM,             // Connectionless, unreliable datagrams
  raw = SOCK_RAW,                    // Raw protocol interface
  reliable_message = SOCK_RDM,       // Reliably-delivered messages
  sequenced_packet = SOCK_SEQPACKET, // Reliable, connection-based datagrams
  dccp = SOCK_DCCP,                  // Datagram Congestion Control Protocol
  packet = SOCK_PACKET,              // Low-level packet interface

  close_on_exec = SOCK_CLOEXEC, // Close-on-exec flag for new descriptors
  non_blocking = SOCK_NONBLOCK  // Non-blocking flag for descriptors
};

enum class Protocol {
  ip = 0,            // Internet Protocol
  hopopt = 0,        // Hop-by-hop options for IPv6
  icmp = 1,          // Internet Control Message Protocol
  igmp = 2,          // Internet Group Management Protocol
  ggp = 3,           // Gateway-Gateway Protocol
  ipv4 = 4,          // IPv4 encapsulation
  st = 5,            // ST datagram mode
  tcp = 6,           // Transmission Control Protocol
  cbt = 7,           // CBT
  egp = 8,           // Exterior Gateway Protocol
  igp = 9,           // Any private interior gateway
  bbn_rcc = 10,      // BBN RCC Monitoring
  nvp = 11,          // Network Voice Protocol
  pup = 12,          // PARC Universal Packet Protocol
  argus = 13,        // ARGUS (deprecated)
  emcon = 14,        // EMCON
  xnet = 15,         // Cross Net Debugger
  chaos = 16,        // Chaos
  udp = 17,          // User Datagram Protocol
  mux = 18,          // Multiplexing protocol
  dcn = 19,          // DCN Measurement Subsystems
  hmp = 20,          // Host Monitoring Protocol
  prm = 21,          // Packet Radio Measurement Protocol
  xns_idp = 22,      // Xerox NS IDP
  trunk_1 = 23,      // Trunk-1
  trunk_2 = 24,      // Trunk-2
  leaf_1 = 25,       // Leaf-1
  leaf_2 = 26,       // Leaf-2
  rdp = 27,          // Reliable Datagram Protocol
  irtp = 28,         // Internet Reliable Transaction Protocol
  iso_tp4 = 29,      // ISO Transport Protocol Class 4
  netblt = 30,       // Bulk Data Transfer Protocol
  mfe_nsp = 31,      // MFE Network Services Protocol
  merit_inp = 32,    // MERIT Internodal Protocol
  dccp = 33,         // Datagram Congestion Control Protocol
  three_pc = 34,     // Third Party Connect Protocol
  idpr = 35,         // Inter-Domain Policy Routing Protocol
  xtp = 36,          // Xpress Transfer Protocol
  ddp = 37,          // Datagram Delivery Protocol
  idpr_cmtp = 38,    // IDPR Control Message Transport Protocol
  tp_plus_plus = 39, // TP++ Transport Protocol
  il = 40,           // IL Transport Protocol
  ipv6 = 41,         // IPv6 encapsulation
  sdrp = 42,         // Source Demand Routing Protocol
  ipv6_route = 43,   // Routing Header for IPv6
  ipv6_frag = 44,    // Fragment Header for IPv6
  idrp = 45,         // Inter-Domain Routing Protocol
  rsvp = 46,         // Resource Reservation Protocol
  gre = 47,          // Generic Routing Encapsulation
  dsr = 48,          // Dynamic Source Routing Protocol
  bna = 49,          // BNA
  esp = 50,          // Encapsulating Security Payload
  ipv6_crypt = 50,   // Encryption Header for IPv6 (not in official list)
  ah = 51,           // Authentication Header
  ipv6_auth = 51,    // Authentication Header for IPv6 (not in official list)
  i_nlsp = 52,       // Integrated Net Layer Security TUBA
  swipe = 53,        // IP with Encryption
  narp = 54,         // NBMA Address Resolution Protocol
  mobile = 55,       // IP Mobility
  tlsp = 56,         // Transport Layer Security Protocol
  skip = 57,         // SKIP
  ipv6_icmp = 58,    // ICMP for IPv6
  ipv6_nonxt = 59,   // No Next Header for IPv6
  ipv6_opts = 60,    // Destination Options for IPv6
  cftp = 62,         // CFTP
  sat_expak = 64,    // SATNET and Backroom EXPAK
  kryptolan = 65,    // Kryptolan
  rvd = 66,          // MIT Remote Virtual Disk Protocol
  ippc = 67,         // Internet Pluribus Packet Core
  sat_mon = 69,      // SATNET Monitoring
  visa = 70,         // VISA Protocol
  ipcv = 71,         // Internet Packet Core Utility
  cpnx = 72,         // Computer Protocol Network Executive
  cphb = 73,         // Computer Protocol Heart Beat
  wsn = 74,          // Wang Span Network
  pvp = 75,          // Packet Video Protocol
  br_sat_mon = 76,   // Backroom SATNET Monitoring
  sun_nd = 77,       // SUN ND Protocol
  wb_mon = 78,       // Wideband Monitoring
  wb_expak = 79,     // Wideband EXPAK
  iso_ip = 80,       // ISO Internet Protocol
  vmtp = 81,         // Versatile Message Transport
  secure_vmtp = 82,  // Secure VMTP
  vines = 83,        // VINES
  ttp = 84,          // TTP
  nsfnet_igp = 85,   // NSFNET-IGP
  dgp = 86,          // Dissimilar Gateway Protocol
  tcf = 87,          // TCF
  eigrp = 88,        // Enhanced Interior Routing Protocol (Cisco)
  ospf = 89,         // Open Shortest Path First IGP
  sprite_rpc = 90,   // Sprite RPC Protocol
  larp = 91,         // Locus Address Resolution Protocol
  mtp = 92,          // Multicast Transport Protocol
  ax_25 = 93,        // AX.25 Frames
  ipip = 94,         // Yet Another IP encapsulation
  micp = 95,         // Mobile Internetworking Control Protocol
  scc_sp = 96,       // Semaphore Communications Security Protocol
  etherip = 97,      // Ethernet-within-IP Encapsulation
  encap = 98,        // Yet Another IP encapsulation
  gmtp = 100,        // GMTP
  ifmp = 101,        // Ipsilon Flow Management Protocol
  pnni = 102,        // PNNI over IP
  pim = 103,         // Protocol Independent Multicast
  aris = 104,        // ARIS
  scps = 105,        // SCPS
  qnx = 106,         // QNX
  a_n = 107,         // Active Networks
  ipcomp = 108,      // IP Payload Compression Protocol
  snp = 109,         // Sitara Networks Protocol
  compaq_peer = 110, // Compaq Peer Protocol
  ipx_in_ip = 111,   // IPX in IP
  vrrp = 112,        // Virtual Router Redundancy Protocol
  pgm = 113,         // PGM Reliable Transport Protocol
  l2tp = 115,        // Layer Two Tunneling Protocol
  ddx = 116,         // D-II Data Exchange
  iatp = 117,        // Interactive Agent Transfer Protocol
  stp = 118,         // Schedule Transfer
  srp = 119,         // SpectraLink Radio Protocol
  uti = 120,         // UTI
  smp = 121,         // Simple Message Protocol
  sm = 122,          // SM (deprecated)
  ptp = 123,         // Performance Transparency Protocol
  isis = 124,        // ISIS over IPv4
  fire = 125,        // Fire
  crtp = 126,        // Combat Radio Transport Protocol
  crudp = 127,       // Combat Radio User Datagram
  sscopmce = 128,    // SSCOPMCE
  iplt = 129,        // IPLT
  sps = 130,         // Secure Packet Shield
  pipe = 131,        // Private IP Encapsulation within IP
  sctp = 132,        // Stream Control Transmission Protocol
  fc = 133,          // Fibre Channel
  rsvp_e2e_ignore = 134, // RSVP-E2E-IGNORE
  mobility_header = 135, // Mobility Header
  udplite = 136,         // UDPLite
  mpls_in_ip = 137,      // MPLS-in-IP
  manet = 138,           // MANET Protocols
  hip = 139,             // Host Identity Protocol
  shim6 = 140,           // Shim6 Protocol
  wesp = 141,            // Wrapped Encapsulating Security Payload
  rohc = 142,            // Robust Header Compression
  mptcp = 262,           // Multipath TCP connection
};

enum class SocketLevel { sol_socket = SOL_SOCKET };

enum class SocketOptions {
  so_debug = SO_DEBUG,
  so_reuseaddr = SO_REUSEADDR,
  so_type = SO_TYPE,
  so_error = SO_ERROR,
  so_dontroute = SO_DONTROUTE,
  so_broadcast = SO_BROADCAST,
  so_sndbuf = SO_SNDBUF,
  so_rcvbuf = SO_RCVBUF,
  so_sndbufforce = SO_SNDBUFFORCE,
  so_rcvbufforce = SO_RCVBUFFORCE,
  so_keepalive = SO_KEEPALIVE,
  so_oobinline = SO_OOBINLINE,
  so_no_check = SO_NO_CHECK,
  so_priority = SO_PRIORITY,
  so_linger = SO_LINGER,
  so_bsdcompat = SO_BSDCOMPAT,
  so_reuseport = SO_REUSEPORT,
  so_passcred = SO_PASSCRED,
  so_peercred = SO_PEERCRED,
  so_rcvlowat = SO_RCVLOWAT,
  so_sndlowat = SO_SNDLOWAT,
  so_rcvtimeo_old = SO_RCVTIMEO_OLD,
  so_sndtimeo_old = SO_SNDTIMEO_OLD,
  so_security_authentication = SO_SECURITY_AUTHENTICATION,
  so_security_encryption_transport = SO_SECURITY_ENCRYPTION_TRANSPORT,
  so_security_encryption_network = SO_SECURITY_ENCRYPTION_NETWORK,
  so_bindtodevice = SO_BINDTODEVICE,
  /* s =S,ocket filtering */ so_attach_filter = SO_ATTACH_FILTER,
  so_detach_filter = SO_DETACH_FILTER,
  so_get_filter = SO_GET_FILTER,
  so_peername = SO_PEERNAME,
  so_acceptconn = SO_ACCEPTCONN,
  so_peersec = SO_PEERSEC,
  so_passsec = SO_PASSSEC,
  so_mark = SO_MARK,
  so_protocol = SO_PROTOCOL,
  so_domain = SO_DOMAIN,
  so_rxq_ovfl = SO_RXQ_OVFL,
  so_wifi_status = SO_WIFI_STATUS,
  scm_wifi_status = SCM_WIFI_STATUS,
  so_peek_off = SO_PEEK_OFF,
  so_nofcs = SO_NOFCS,
  so_lock_filter = SO_LOCK_FILTER,
  so_select_err_queue = SO_SELECT_ERR_QUEUE,
  so_busy_poll = SO_BUSY_POLL,
  so_max_pacing_rate = SO_MAX_PACING_RATE,
  so_bpf_extensions = SO_BPF_EXTENSIONS,
  so_incoming_cpu = SO_INCOMING_CPU,
  so_attach_bpf = SO_ATTACH_BPF,
  so_detach_bpf = SO_DETACH_BPF,
  so_attach_reuseport_cbpf = SO_ATTACH_REUSEPORT_CBPF,
  so_attach_reuseport_ebpf = SO_ATTACH_REUSEPORT_EBPF,
  so_cnx_advice = SO_CNX_ADVICE,
  scm_timestamping_opt_stats = SCM_TIMESTAMPING_OPT_STATS,
  so_meminfo = SO_MEMINFO,
  so_incoming_napi_id = SO_INCOMING_NAPI_ID,
  so_cookie = SO_COOKIE,
  scm_timestamping_pktinfo = SCM_TIMESTAMPING_PKTINFO,
  so_peergroups = SO_PEERGROUPS,
  so_zerocopy = SO_ZEROCOPY,
  so_txtime = SO_TXTIME,
  scm_txtime = SCM_TXTIME,
  so_bindtoifindex = SO_BINDTOIFINDEX,
  so_timestamp_old = SO_TIMESTAMP_OLD,
  so_timestampns_old = SO_TIMESTAMPNS_OLD,
  so_timestamping_old = SO_TIMESTAMPING_OLD,
  so_timestamp_new = SO_TIMESTAMP_NEW,
  so_timestampns_new = SO_TIMESTAMPNS_NEW,
  so_timestamping_new = SO_TIMESTAMPING_NEW,
  so_rcvtimeo_new = SO_RCVTIMEO_NEW,
  so_sndtimeo_new = SO_SNDTIMEO_NEW,
  so_detach_reuseport_bpf = SO_DETACH_REUSEPORT_BPF,
  so_prefer_busy_poll = SO_PREFER_BUSY_POLL,
  so_busy_poll_budget = SO_BUSY_POLL_BUDGET,
  so_netns_cookie = SO_NETNS_COOKIE,
  so_buf_lock = SO_BUF_LOCK,
  so_reserve_mem = SO_RESERVE_MEM,
  so_txrehash = SO_TXREHASH,
  so_rcvmark = SO_RCVMARK,
  so_passpidfd = SO_PASSPIDFD,
  so_peerpidfd = SO_PEERPIDFD,
};

template <typename Enumeration>
auto as_integer(Enumeration const value) ->
    typename std::underlying_type<Enumeration>::type {
  return static_cast<typename std::underlying_type<Enumeration>::type>(value);
}

// template <typename Enumerand,
//           std::enable_if_t<std::is_enum<Enumerand>::value, bool> = true>
// inline auto operator|(Enumerand rhs, Enumerand lhs) {
//   return as_integer(lhs) | as_integer(rhs);
// }

template <typename Enumerand,
          std::enable_if_t<std::is_enum<Enumerand>::value, bool> = true>
inline auto operator^(Enumerand rhs, Enumerand lhs) {
  return as_integer(lhs) ^ as_integer(rhs);
}

template <typename Enumerand,
          std::enable_if_t<std::is_enum<Enumerand>::value, bool> = true>
inline auto operator&(Enumerand rhs, Enumerand lhs) {
  return as_integer(lhs) & as_integer(rhs);
}
