import time

ATTR_FLAGS = 1
ATTR_APPEARANCE = 25
ATTR_SHORTNAME = 8
ATTR_FULLNAME = 9
ATTR_MFG_DATA = 255

BLE_APPEARANCE_DEFCON = b'\xDC\x19'
BLE_PEER_TIMEOUT = 30

BADGELIFE_MFGS = {
    0x49E: 'Bender',    
    0x4D3: 'Queercon',
    0x503: 'DC503',
    0x801: 'DC801',
    0xC97: 'CPV',
    0x5050: 'DCZIA',
    0xFFFE: 'B&D',
}

def gap_msg_attrs(msg):
    i = 0
    attrs = {}
    while i < len(msg):
        # Parse each attribute in a try/except to avoid issues with malformed packets
        try:
            length = msg[i]
            type = msg[i + 1]
            attrs[type] = msg[i + 2:i + 1 + length]
            i += length + 1
        except Exception:
            break
    return attrs        

def is_badgelife_msg(attrs):
    return ATTR_APPEARANCE in attrs and attrs[ATTR_APPEARANCE] == BLE_APPEARANCE_DEFCON and (ATTR_FULLNAME in attrs or ATTR_SHORTNAME in attrs)

def is_cpv_badge(attrs):
    return is_badgelife_msg(attrs) and get_mfg(attrs) == 0xC97

def get_mfg(attrs):
    mfg_data = attrs[ATTR_MFG_DATA]
    return mfg_data[0] | (mfg_data[1] << 8)

def get_mfg_str(attrs):
    mfg = get_mfg(attrs)
    if mfg in BADGELIFE_MFGS:
        return BADGELIFE_MFGS[mfg]
    return 'Unknown'

def get_name(attrs):
    if ATTR_FULLNAME in attrs:
        return attrs[ATTR_FULLNAME].decode('utf-8')
    if ATTR_SHORTNAME in attrs:
        return attrs[ATTR_SHORTNAME].decode('utf-8') + '...'
    return 'Unknown'

class BLEPeerList:
    def __init__(self):
        self.peers = {}
        
    def add_peer(self, msg):
        attrs = gap_msg_attrs(msg)
        if is_badgelife_msg(attrs):
            self.peers[msg] = (time.time(), attrs)
            self.prune_peer_list()
            
    def prune_peer_list(self):
        peerhashes = self.peers.keys()
        for hashvalue in peerhashes:
            if self.peers[hashvalue][0] + BLE_PEER_TIMEOUT < time.time():
                self.peers.pop(hashvalue, None)
    
    def get_peers(self):
        peerlist = []
        for hashvalue, peertuple in self.peers.items():
            peerlist.append(peertuple[1])
        return peerlist