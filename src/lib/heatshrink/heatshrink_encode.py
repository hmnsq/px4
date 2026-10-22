استورد ctypes
من enum استورد Enum


# Note: this implementation directly follows the heatshrink_encoder.c code
# (it's neither expected to be very efficient, nor is it pythonic)

# Enum
صنف HSE_state(ctypes.c_int):
    HSES_NOT_ڀريل = 0
    HSES_FILLED = 1
    HSES_ڳولا = 2
    HSES_YIELD_ٽيگ_ٻٽو_انگ = 3
    HSES_YIELD_LITERAL = 4
    HSES_YIELD_BR_اشاريو = 5
    HSES_YIELD_BR_ڊيگهائي = 6
    HSES_SAVE_BACKLOG = 7
    HSES_FLUSH_بٽ = 8
    HSES_پورو = 9


# Constants
FLAG_IS_FINISHING = 0x01
ميلاپ_NOT_مليو = 0xFFFF

HEATSHRINK_LITERAL_MARKER = 0x01
HEATSHRINK_BACKREF_MARKER = 0x00


# Structs
صنف output_info(ctypes.Structure):
    _fields_ = [
        ("buf", ctypes.اشارو(ctypes.c_uint8)),
        ("buf_size", ctypes.c_سائيز_t),
        ("output_size", ctypes.اشارو(ctypes.c_سائيز_t))
    ]


# Functions
وصف جوڙيو_ٽيگ_ٻٽو_انگ(hse, oi, ٽيگ):
    ڌڪو_بٽ(hse, 1, ٽيگ, oi)


وصف ڌڪو_بٽ(hse, ڳڻپ, بٽ, oi):
    أكد ڳڻپ <= 8
    current_byte = ctypes.c_uint8(hse.current_byte)
    ٻٽو_انگ_اشاريو = ctypes.c_uint8(hse.ٻٽو_انگ_اشاريو)
    لكل i في سلسلو(ڳڻپ - 1, -1, -1):
        ٻٽو_انگ = بٽ & (1 << i)
        إذا ٻٽو_انگ:
            current_byte.قدر |= ٻٽو_انگ_اشاريو.قدر
        ٻٽو_انگ_اشاريو.قدر >>= 1
        إذا ٻٽو_انگ_اشاريو.قدر == 0:
            ٻٽو_انگ_اشاريو.قدر = 0x80
            oi.buf[oi.output_سائيز[0]] = current_byte.قدر
            oi.output_سائيز[0] += 1
            current_byte.قدر = 0
    hse.current_byte = current_byte.قدر
    hse.ٻٽو_انگ_اشاريو = ٻٽو_انگ_اشاريو.قدر


وصف ڌڪو_literal_byte(hse, oi):
    processed_وٿي = hse.ميلاپ_جاچ_اشاريو - 1
    داخلا_وٿي = get_داخلا_وٿي(hse) + processed_وٿي
    c = hse.بفر_2[داخلا_وٿي]
    ڌڪو_بٽ(hse, 8, c, oi)


# Define necessary structures and enums
صنف heatshrink_encoder(ctypes.Structure):
    _fields_ = [
        ("input_size", ctypes.c_uint16),
        ("match_scan_index", ctypes.c_uint16),
        ("match_length", ctypes.c_uint16),
        ("match_pos", ctypes.c_uint16),
        ("outgoing_bits", ctypes.c_uint16),
        ("outgoing_bits_count", ctypes.c_uint8),
        ("flags", ctypes.c_uint8),
        ("state", ctypes.c_uint8),
        ("current_byte", ctypes.c_uint8),
        ("bit_index", ctypes.c_uint8),

        ("buffer", ctypes.اشارو(ctypes.c_uint8)),
    ]

    وصف __init__(self, دري_سائيز=8, lookahead_سائيز=4):
        أعلى().__init__()
        self.دري_سائيز = دري_سائيز
        self.lookahead_سائيز = lookahead_سائيز
        self.ڳولا_اشاريو = hs_اشاريو(دري_سائيز)
        self.بفر_2 = (ctypes.c_uint8 * (2 << دري_سائيز))()
        self.ٻٽو_انگ_اشاريو = 0x80


صنف hs_اشاريو(ctypes.Structure):
    _fields_ = [("index", ctypes.اشارو(ctypes.c_int16))]

    وصف __init__(self, دري_سائيز):
        أعلى().__init__()
        self.اشاريو = (ctypes.c_int16 * (2 << دري_سائيز))()


صنف HSE_sink_res(Enum):
    HSER_SINK_OK = 0
    HSER_SINK_ERROR_NULL = -1
    HSER_SINK_ERROR_MISUSE = -2


صنف HSE_poll_res(Enum):
    HSER_POLL_خالي_جايون = 0
    HSER_POLL_وڌيڪ = 1
    HSER_POLL_ERROR_NULL = -1
    HSER_POLL_ERROR_MISUSE = -2


صنف HSE_پڄاڻي_res(Enum):
    HSER_پڄاڻي_پورو = 0
    HSER_پڄاڻي_وڌيڪ = 1
    HSER_پڄاڻي_ERROR_NULL = -1


وصف is_finishing(hse):
    أرجع hse.flags & FLAG_IS_FINISHING


وصف can_take_byte(oi):
    أرجع oi.output_سائيز[0] < oi.buf_سائيز


وصف get_داخلا_بفر_سائيز(hse):
    أرجع 1 << hse.دري_سائيز


وصف get_lookahead_سائيز(hse):
    أرجع 1 << hse.lookahead_سائيز


وصف get_داخلا_وٿي(hse):
    أرجع get_داخلا_بفر_سائيز(hse)


وصف heatshrink_encoder_sink(hse, اندريون_buf, سائيز, داخلا_سائيز):
    إذا hse آهي ڪجهه_نه أو اندريون_buf آهي ڪجهه_نه أو داخلا_سائيز آهي ڪجهه_نه:
        أرجع HSE_sink_res.HSER_SINK_ERROR_NULL

    إذا is_finishing(hse):
        أرجع HSE_sink_res.HSER_SINK_ERROR_MISUSE

    إذا hse.state != HSE_state.HSES_NOT_ڀريل:
        أرجع HSE_sink_res.HSER_SINK_ERROR_MISUSE

    write_وٿي_2 = get_داخلا_وٿي(hse) + hse.داخلا_سائيز
    ibs = get_داخلا_بفر_سائيز(hse)
    rem = ibs - hse.داخلا_سائيز
    cp_sz = min(rem, سائيز)

    لكل i في سلسلو(cp_sz):
        hse.بفر_2[write_وٿي_2 + i] = اندريون_buf[i]
    داخلا_سائيز.قدر = cp_sz
    hse.داخلا_سائيز += cp_sz

    إذا cp_sz == rem:
        hse.state = HSE_state.HSES_FILLED

    أرجع HSE_sink_res.HSER_SINK_OK


وصف do_indexing(hse):
    # Build an index array I that contains flattened linked lists
    # for the previous instances of every byte in the buffer.

    hsi = hse.ڳولا_اشاريو
    آخري = [0xffff] * 256
    buf = hse.بفر_2
    اشاريو = hsi.اشاريو
    داخلا_وٿي = get_داخلا_وٿي(hse)
    پڇاڙي = داخلا_وٿي + hse.داخلا_سائيز

    لكل i في سلسلو(0, پڇاڙي):
        v = buf[i]
        lv = آخري[v]
        اشاريو[i] = lv
        آخري[v] = i


وصف heatshrink_encoder_poll(hse, ٻاهريون_buf, ٻاهريون_buf_سائيز, output_سائيز):
    إذا hse آهي ڪجهه_نه أو ٻاهريون_buf آهي ڪجهه_نه أو output_سائيز آهي ڪجهه_نه:
        أرجع HSE_poll_res.HSER_POLL_ERROR_NULL

    إذا ٻاهريون_buf_سائيز == 0:
        أرجع HSE_poll_res.HSER_POLL_ERROR_MISUSE

    output_سائيز[0] = 0
    oi = output_info()
    oi.buf = ٻاهريون_buf
    oi.buf_سائيز = ٻاهريون_buf_سائيز
    oi.output_سائيز = output_سائيز

    طالما سچ:
        اندريون_state = hse.state
        إذا اندريون_state == HSE_state.HSES_NOT_ڀريل:
            أرجع HSE_poll_res.HSER_POLL_خالي_جايون
        وإلاإذا اندريون_state == HSE_state.HSES_پورو:
            أرجع HSE_poll_res.HSER_POLL_خالي_جايون
        وإلاإذا اندريون_state == HSE_state.HSES_FILLED:
            do_indexing(hse)
            hse.state = HSE_state.HSES_ڳولا
        وإلاإذا اندريون_state == HSE_state.HSES_ڳولا:
            hse.state = st_قدم_ڳولا(hse)
        وإلاإذا اندريون_state == HSE_state.HSES_YIELD_ٽيگ_ٻٽو_انگ:
            hse.state = st_yield_ٽيگ_ٻٽو_انگ(hse, oi)
        وإلاإذا اندريون_state == HSE_state.HSES_YIELD_LITERAL:
            hse.state = st_yield_literal(hse, oi)
        وإلاإذا اندريون_state == HSE_state.HSES_YIELD_BR_اشاريو:
            hse.state = st_yield_br_اشاريو(hse, oi)
        وإلاإذا اندريون_state == HSE_state.HSES_YIELD_BR_ڊيگهائي:
            hse.state = st_yield_br_ڊيگهائي(hse, oi)
        وإلاإذا اندريون_state == HSE_state.HSES_SAVE_BACKLOG:
            hse.state = st_save_backlog(hse)
        وإلاإذا اندريون_state == HSE_state.HSES_FLUSH_بٽ:
            hse.state = st_flush_ٻٽو_انگ_بفر(hse, oi)
        وإلا:
            أرجع HSE_poll_res.HSER_POLL_ERROR_MISUSE

        إذا hse.state == اندريون_state:
            إذا oi.output_سائيز == oi.buf_سائيز:
                أرجع HSE_poll_res.HSER_POLL_وڌيڪ


وصف heatshrink_encoder_پڄاڻي(hse):
    hse.flags |= FLAG_IS_FINISHING
    إذا hse.state == HSE_state.HSES_NOT_ڀريل:
        hse.state = HSE_state.HSES_FILLED
    إذا hse.state == HSE_state.HSES_پورو:
        أرجع HSE_پڄاڻي_res.HSER_پڄاڻي_پورو
    أرجع HSE_پڄاڻي_res.HSER_پڄاڻي_وڌيڪ


وصف st_قدم_ڳولا(hse):
    دري_ڊيگهائي = get_داخلا_بفر_سائيز(hse)
    lookahead_sz = get_lookahead_سائيز(hse)
    msi = hse.ميلاپ_جاچ_اشاريو

    fin = is_finishing(hse)
    إذا msi > hse.داخلا_سائيز - (1 إذا fin وإلا lookahead_sz):
        أرجع HSE_state.HSES_FLUSH_بٽ إذا fin وإلا HSE_state.HSES_SAVE_BACKLOG

    داخلا_وٿي = get_داخلا_وٿي(hse)
    پڇاڙي = داخلا_وٿي + msi
    شروعات = پڇاڙي - دري_ڊيگهائي

    max_possible = lookahead_sz إذا hse.داخلا_سائيز - msi >= lookahead_sz وإلا hse.داخلا_سائيز - msi

    ميلاپ_pos, ميلاپ_ڊيگهائي = ڳوليو_longest_ميلاپ(hse, شروعات, پڇاڙي, max_possible)

    إذا ميلاپ_pos == ميلاپ_NOT_مليو:
        hse.ميلاپ_جاچ_اشاريو += 1
        hse.ميلاپ_ڊيگهائي = 0
        أرجع HSE_state.HSES_YIELD_ٽيگ_ٻٽو_انگ
    وإلا:
        hse.ميلاپ_pos = ميلاپ_pos
        hse.ميلاپ_ڊيگهائي = ميلاپ_ڊيگهائي
        أرجع HSE_state.HSES_YIELD_ٽيگ_ٻٽو_انگ


وصف ڳوليو_longest_ميلاپ(hse, شروعات, پڇاڙي, maxlen):
    buf = hse.بفر_2

    ميلاپ_maxlen = 0
    ميلاپ_اشاريو = ميلاپ_NOT_مليو

    needlepoint = پڇاڙي

    pos = hse.ڳولا_اشاريو.اشاريو[پڇاڙي]
    buf_needlepoint_maxlen = buf[needlepoint + ميلاپ_maxlen]
    طالما pos >= شروعات:

        إذا buf[pos + ميلاپ_maxlen] != buf_needlepoint_maxlen:
            pos = hse.ڳولا_اشاريو.اشاريو[pos]
            استمر

        ڊيگهائي = 1
        لكل ڊيگهائي في سلسلو(1, maxlen):
            إذا buf[pos + ڊيگهائي] != buf[needlepoint + ڊيگهائي]:
                اكسر
        إذا ڊيگهائي > ميلاپ_maxlen:
            ميلاپ_maxlen = ڊيگهائي
            ميلاپ_اشاريو = pos
            buf_needlepoint_maxlen = buf[needlepoint + ميلاپ_maxlen]
            إذا ڊيگهائي == maxlen:
                اكسر  # won't find better

        pos = hse.ڳولا_اشاريو.اشاريو[pos]

    break_even_نقطو = 1 + hse.دري_سائيز + hse.lookahead_سائيز

    إذا ميلاپ_maxlen > (break_even_نقطو // 8):
        أرجع پڇاڙي - ميلاپ_اشاريو, ميلاپ_maxlen
    أرجع ميلاپ_NOT_مليو, 0


وصف ڌڪو_outgoing_بٽ(hse, oi):
    إذا hse.outgoing_بٽ_ڳڻپ > 8:
        ڳڻپ = 8
        بٽ = hse.outgoing_بٽ >> (hse.outgoing_بٽ_ڳڻپ - 8)
    وإلا:
        ڳڻپ = hse.outgoing_بٽ_ڳڻپ
        بٽ = hse.outgoing_بٽ

    إذا ڳڻپ > 0:
        ڌڪو_بٽ(hse, ڳڻپ, بٽ, oi)
        hse.outgoing_بٽ_ڳڻپ -= ڳڻپ
    أرجع ڳڻپ


وصف st_yield_ٽيگ_ٻٽو_انگ(hse, oi):
    إذا can_take_byte(oi):
        إذا hse.ميلاپ_ڊيگهائي == 0:
            جوڙيو_ٽيگ_ٻٽو_انگ(hse, oi, HEATSHRINK_LITERAL_MARKER)
            أرجع HSE_state.HSES_YIELD_LITERAL
        وإلا:
            جوڙيو_ٽيگ_ٻٽو_انگ(hse, oi, HEATSHRINK_BACKREF_MARKER)
            hse.outgoing_بٽ = hse.ميلاپ_pos - 1
            hse.outgoing_بٽ_ڳڻپ = hse.دري_سائيز
            أرجع HSE_state.HSES_YIELD_BR_اشاريو
    وإلا:
        أرجع HSE_state.HSES_YIELD_ٽيگ_ٻٽو_انگ


وصف st_yield_literal(hse, oi):
    إذا can_take_byte(oi):
        ڌڪو_literal_byte(hse, oi)
        أرجع HSE_state.HSES_ڳولا
    وإلا:
        أرجع HSE_state.HSES_YIELD_LITERAL


وصف st_yield_br_اشاريو(hse, oi):
    إذا can_take_byte(oi):
        إذا ڌڪو_outgoing_بٽ(hse, oi) > 0:
            أرجع HSE_state.HSES_YIELD_BR_اشاريو
        وإلا:
            hse.outgoing_بٽ = hse.ميلاپ_ڊيگهائي - 1
            hse.outgoing_بٽ_ڳڻپ = hse.lookahead_سائيز
            أرجع HSE_state.HSES_YIELD_BR_ڊيگهائي
    وإلا:
        أرجع HSE_state.HSES_YIELD_BR_اشاريو


وصف st_yield_br_ڊيگهائي(hse, oi):
    إذا can_take_byte(oi):
        إذا ڌڪو_outgoing_بٽ(hse, oi) > 0:
            أرجع HSE_state.HSES_YIELD_BR_ڊيگهائي
        وإلا:
            hse.ميلاپ_جاچ_اشاريو += hse.ميلاپ_ڊيگهائي
            hse.ميلاپ_ڊيگهائي = 0
            أرجع HSE_state.HSES_ڳولا
    وإلا:
        أرجع HSE_state.HSES_YIELD_BR_ڊيگهائي


وصف st_save_backlog(hse):
    save_backlog(hse)
    أرجع HSE_state.HSES_NOT_ڀريل


وصف st_flush_ٻٽو_انگ_بفر(hse, oi):
    إذا hse.ٻٽو_انگ_اشاريو == 0x80:
        أرجع HSE_state.HSES_پورو
    وإلاإذا can_take_byte(oi):
        oi.buf[oi.output_سائيز[0]] = hse.current_byte
        oi.output_سائيز[0] += 1
        أرجع HSE_state.HSES_پورو
    وإلا:
        أرجع HSE_state.HSES_FLUSH_بٽ


وصف save_backlog(hse):
    داخلا_buf_sz = get_داخلا_بفر_سائيز(hse)

    msi = hse.ميلاپ_جاچ_اشاريو

    rem = داخلا_buf_sz - msi  # unprocessed bytes
    سرڪايو_sz = داخلا_buf_sz + rem

    لكل i في سلسلو(سرڪايو_sz):
        hse.بفر_2[i] = hse.بفر_2[داخلا_buf_sz - rem + i]

    hse.ميلاپ_جاچ_اشاريو = 0
    hse.داخلا_سائيز -= داخلا_buf_sz - rem


وصف encode(معلومات, دري_سائيز, lookahead_سائيز):
    hse = heatshrink_encoder(دري_سائيز, lookahead_سائيز)
    داخلا_buf = (ctypes.c_uint8 * ڊيگهه(معلومات))()
    لكل i, d في enumerate(معلومات):
        داخلا_buf[i] = d
    اندريون_سائيز = ڊيگهه(داخلا_buf)

    ٻاهريون_buf_سائيز = 4 * اندريون_سائيز  # set output buffer size a bit larger
    ٻاهريون_buf = (ctypes.c_uint8 * ٻاهريون_buf_سائيز)()

    sunk = 0
    ret = []
    طالما sunk < اندريون_سائيز:
        داخلا_سائيز = ctypes.c_سائيز_t(اندريون_سائيز)
        heatshrink_encoder_sink(hse, داخلا_buf, اندريون_سائيز - sunk, داخلا_سائيز)
        داخلا_buf = داخلا_buf[داخلا_سائيز.قدر:]
        sunk += داخلا_سائيز.قدر
        إذا sunk == اندريون_سائيز:
            heatshrink_encoder_پڄاڻي(hse)

        poll_res = HSE_poll_res.HSER_POLL_وڌيڪ
        طالما poll_res == HSE_poll_res.HSER_POLL_وڌيڪ:
            output_سائيز = (ctypes.c_سائيز_t * 1)()
            poll_res = heatshrink_encoder_poll(hse, ٻاهريون_buf, ٻاهريون_buf_سائيز, output_سائيز)
            ret += list(ٻاهريون_buf)[0:output_سائيز[0]]

        إذا sunk == اندريون_سائيز:
            heatshrink_encoder_پڄاڻي(hse)
    أرجع ret
