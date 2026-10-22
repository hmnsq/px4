استورد ctypes
من enum استورد Enum


# Note: this implementation directly follows the heatshrink_encoder.c code
# (it's neither expected to be very efficient, nor is it pythonic)

# Enum
صنف HSE_state(ctypes.c_int):
    HSES_NOT_ډک = 0
    HSES_FILLED = 1
    HSES_پلټنه = 2
    HSES_YIELD_نښان_بټ = 3
    HSES_YIELD_LITERAL = 4
    HSES_YIELD_BR_شاخص = 5
    HSES_YIELD_BR_اوږدتیا = 6
    HSES_SAVE_BACKLOG = 7
    HSES_FLUSH_بټونه = 8
    HSES_بشپړ = 9


# Constants
FLAG_IS_FINISHING = 0x01
سمون_NOT_وموندل = 0xFFFF

HEATSHRINK_LITERAL_MARKER = 0x01
HEATSHRINK_BACKREF_MARKER = 0x00


# Structs
صنف output_info(ctypes.Structure):
    _fields_ = [
        ("buf", ctypes.اشاره_ګر(ctypes.c_uint8)),
        ("buf_size", ctypes.c_اندازه_t),
        ("output_size", ctypes.اشاره_ګر(ctypes.c_اندازه_t))
    ]


# Functions
تعریف جمع_کړه_نښان_بټ(hse, oi, نښان):
    ټېل_بټونه(hse, 1, نښان, oi)


تعریف ټېل_بټونه(hse, شمېر, بټونه, oi):
    أكد شمېر <= 8
    current_byte = ctypes.c_uint8(hse.current_byte)
    بټ_شاخص = ctypes.c_uint8(hse.بټ_شاخص)
    لكل i في لړ(شمېر - 1, -1, -1):
        بټ = بټونه & (1 << i)
        إذا بټ:
            current_byte.ارزښت |= بټ_شاخص.ارزښت
        بټ_شاخص.ارزښت >>= 1
        إذا بټ_شاخص.ارزښت == 0:
            بټ_شاخص.ارزښت = 0x80
            oi.buf[oi.output_اندازه[0]] = current_byte.ارزښت
            oi.output_اندازه[0] += 1
            current_byte.ارزښت = 0
    hse.current_byte = current_byte.ارزښت
    hse.بټ_شاخص = بټ_شاخص.ارزښت


تعریف ټېل_literal_byte(hse, oi):
    processed_دننه_واټن = hse.سمون_لټون_شاخص - 1
    ننوت_دننه_واټن = get_ننوت_دننه_واټن(hse) + processed_دننه_واټن
    c = hse.بفر_2[ننوت_دننه_واټن]
    ټېل_بټونه(hse, 8, c, oi)


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

        ("buffer", ctypes.اشاره_ګر(ctypes.c_uint8)),
    ]

    تعریف __init__(self, کړکۍ_اندازه=8, lookahead_اندازه=4):
        أعلى().__init__()
        self.کړکۍ_اندازه = کړکۍ_اندازه
        self.lookahead_اندازه = lookahead_اندازه
        self.پلټنه_شاخص = hs_شاخص(کړکۍ_اندازه)
        self.بفر_2 = (ctypes.c_uint8 * (2 << کړکۍ_اندازه))()
        self.بټ_شاخص = 0x80


صنف hs_شاخص(ctypes.Structure):
    _fields_ = [("index", ctypes.اشاره_ګر(ctypes.c_int16))]

    تعریف __init__(self, کړکۍ_اندازه):
        أعلى().__init__()
        self.شاخص = (ctypes.c_int16 * (2 << کړکۍ_اندازه))()


صنف HSE_sink_res(Enum):
    HSER_SINK_OK = 0
    HSER_SINK_ERROR_NULL = -1
    HSER_SINK_ERROR_MISUSE = -2


صنف HSE_poll_res(Enum):
    HSER_POLL_تشه = 0
    HSER_POLL_نور = 1
    HSER_POLL_ERROR_NULL = -1
    HSER_POLL_ERROR_MISUSE = -2


صنف HSE_پای_res(Enum):
    HSER_پای_بشپړ = 0
    HSER_پای_نور = 1
    HSER_پای_ERROR_NULL = -1


تعریف is_finishing(hse):
    أرجع hse.flags & FLAG_IS_FINISHING


تعریف can_take_byte(oi):
    أرجع oi.output_اندازه[0] < oi.buf_اندازه


تعریف get_ننوت_بفر_اندازه(hse):
    أرجع 1 << hse.کړکۍ_اندازه


تعریف get_lookahead_اندازه(hse):
    أرجع 1 << hse.lookahead_اندازه


تعریف get_ننوت_دننه_واټن(hse):
    أرجع get_ننوت_بفر_اندازه(hse)


تعریف heatshrink_encoder_sink(hse, دننه_buf, اندازه, ننوت_اندازه):
    إذا hse دی هیڅ أو دننه_buf دی هیڅ أو ننوت_اندازه دی هیڅ:
        أرجع HSE_sink_res.HSER_SINK_ERROR_NULL

    إذا is_finishing(hse):
        أرجع HSE_sink_res.HSER_SINK_ERROR_MISUSE

    إذا hse.state != HSE_state.HSES_NOT_ډک:
        أرجع HSE_sink_res.HSER_SINK_ERROR_MISUSE

    write_دننه_واټن_2 = get_ننوت_دننه_واټن(hse) + hse.ننوت_اندازه
    ibs = get_ننوت_بفر_اندازه(hse)
    rem = ibs - hse.ننوت_اندازه
    cp_sz = min(rem, اندازه)

    لكل i في لړ(cp_sz):
        hse.بفر_2[write_دننه_واټن_2 + i] = دننه_buf[i]
    ننوت_اندازه.ارزښت = cp_sz
    hse.ننوت_اندازه += cp_sz

    إذا cp_sz == rem:
        hse.state = HSE_state.HSES_FILLED

    أرجع HSE_sink_res.HSER_SINK_OK


تعریف do_indexing(hse):
    # Build an index array I that contains flattened linked lists
    # for the previous instances of every byte in the buffer.

    hsi = hse.پلټنه_شاخص
    وروستی = [0xffff] * 256
    buf = hse.بفر_2
    شاخص = hsi.شاخص
    ننوت_دننه_واټن = get_ننوت_دننه_واټن(hse)
    پای_ټکی = ننوت_دننه_واټن + hse.ننوت_اندازه

    لكل i في لړ(0, پای_ټکی):
        v = buf[i]
        lv = وروستی[v]
        شاخص[i] = lv
        وروستی[v] = i


تعریف heatshrink_encoder_poll(hse, بهر_buf, بهر_buf_اندازه, output_اندازه):
    إذا hse دی هیڅ أو بهر_buf دی هیڅ أو output_اندازه دی هیڅ:
        أرجع HSE_poll_res.HSER_POLL_ERROR_NULL

    إذا بهر_buf_اندازه == 0:
        أرجع HSE_poll_res.HSER_POLL_ERROR_MISUSE

    output_اندازه[0] = 0
    oi = output_info()
    oi.buf = بهر_buf
    oi.buf_اندازه = بهر_buf_اندازه
    oi.output_اندازه = output_اندازه

    طالما ریښتیا:
        دننه_state = hse.state
        إذا دننه_state == HSE_state.HSES_NOT_ډک:
            أرجع HSE_poll_res.HSER_POLL_تشه
        وإلاإذا دننه_state == HSE_state.HSES_بشپړ:
            أرجع HSE_poll_res.HSER_POLL_تشه
        وإلاإذا دننه_state == HSE_state.HSES_FILLED:
            do_indexing(hse)
            hse.state = HSE_state.HSES_پلټنه
        وإلاإذا دننه_state == HSE_state.HSES_پلټنه:
            hse.state = st_ګام_پلټنه(hse)
        وإلاإذا دننه_state == HSE_state.HSES_YIELD_نښان_بټ:
            hse.state = st_yield_نښان_بټ(hse, oi)
        وإلاإذا دننه_state == HSE_state.HSES_YIELD_LITERAL:
            hse.state = st_yield_literal(hse, oi)
        وإلاإذا دننه_state == HSE_state.HSES_YIELD_BR_شاخص:
            hse.state = st_yield_br_شاخص(hse, oi)
        وإلاإذا دننه_state == HSE_state.HSES_YIELD_BR_اوږدتیا:
            hse.state = st_yield_br_اوږدتیا(hse, oi)
        وإلاإذا دننه_state == HSE_state.HSES_SAVE_BACKLOG:
            hse.state = st_save_backlog(hse)
        وإلاإذا دننه_state == HSE_state.HSES_FLUSH_بټونه:
            hse.state = st_flush_بټ_بفر(hse, oi)
        وإلا:
            أرجع HSE_poll_res.HSER_POLL_ERROR_MISUSE

        إذا hse.state == دننه_state:
            إذا oi.output_اندازه == oi.buf_اندازه:
                أرجع HSE_poll_res.HSER_POLL_نور


تعریف heatshrink_encoder_پای(hse):
    hse.flags |= FLAG_IS_FINISHING
    إذا hse.state == HSE_state.HSES_NOT_ډک:
        hse.state = HSE_state.HSES_FILLED
    إذا hse.state == HSE_state.HSES_بشپړ:
        أرجع HSE_پای_res.HSER_پای_بشپړ
    أرجع HSE_پای_res.HSER_پای_نور


تعریف st_ګام_پلټنه(hse):
    کړکۍ_اوږدتیا = get_ننوت_بفر_اندازه(hse)
    lookahead_sz = get_lookahead_اندازه(hse)
    msi = hse.سمون_لټون_شاخص

    fin = is_finishing(hse)
    إذا msi > hse.ننوت_اندازه - (1 إذا fin وإلا lookahead_sz):
        أرجع HSE_state.HSES_FLUSH_بټونه إذا fin وإلا HSE_state.HSES_SAVE_BACKLOG

    ننوت_دننه_واټن = get_ننوت_دننه_واټن(hse)
    پای_ټکی = ننوت_دننه_واټن + msi
    پیل = پای_ټکی - کړکۍ_اوږدتیا

    max_possible = lookahead_sz إذا hse.ننوت_اندازه - msi >= lookahead_sz وإلا hse.ننوت_اندازه - msi

    سمون_pos, سمون_اوږدتیا = پیدا_کړه_longest_سمون(hse, پیل, پای_ټکی, max_possible)

    إذا سمون_pos == سمون_NOT_وموندل:
        hse.سمون_لټون_شاخص += 1
        hse.سمون_اوږدتیا = 0
        أرجع HSE_state.HSES_YIELD_نښان_بټ
    وإلا:
        hse.سمون_pos = سمون_pos
        hse.سمون_اوږدتیا = سمون_اوږدتیا
        أرجع HSE_state.HSES_YIELD_نښان_بټ


تعریف پیدا_کړه_longest_سمون(hse, پیل, پای_ټکی, maxlen):
    buf = hse.بفر_2

    سمون_maxlen = 0
    سمون_شاخص = سمون_NOT_وموندل

    needlepoint = پای_ټکی

    pos = hse.پلټنه_شاخص.شاخص[پای_ټکی]
    buf_needlepoint_maxlen = buf[needlepoint + سمون_maxlen]
    طالما pos >= پیل:

        إذا buf[pos + سمون_maxlen] != buf_needlepoint_maxlen:
            pos = hse.پلټنه_شاخص.شاخص[pos]
            استمر

        اوږدتیا = 1
        لكل اوږدتیا في لړ(1, maxlen):
            إذا buf[pos + اوږدتیا] != buf[needlepoint + اوږدتیا]:
                اكسر
        إذا اوږدتیا > سمون_maxlen:
            سمون_maxlen = اوږدتیا
            سمون_شاخص = pos
            buf_needlepoint_maxlen = buf[needlepoint + سمون_maxlen]
            إذا اوږدتیا == maxlen:
                اكسر  # won't find better

        pos = hse.پلټنه_شاخص.شاخص[pos]

    break_even_ټکی = 1 + hse.کړکۍ_اندازه + hse.lookahead_اندازه

    إذا سمون_maxlen > (break_even_ټکی // 8):
        أرجع پای_ټکی - سمون_شاخص, سمون_maxlen
    أرجع سمون_NOT_وموندل, 0


تعریف ټېل_outgoing_بټونه(hse, oi):
    إذا hse.outgoing_بټونه_شمېر > 8:
        شمېر = 8
        بټونه = hse.outgoing_بټونه >> (hse.outgoing_بټونه_شمېر - 8)
    وإلا:
        شمېر = hse.outgoing_بټونه_شمېر
        بټونه = hse.outgoing_بټونه

    إذا شمېر > 0:
        ټېل_بټونه(hse, شمېر, بټونه, oi)
        hse.outgoing_بټونه_شمېر -= شمېر
    أرجع شمېر


تعریف st_yield_نښان_بټ(hse, oi):
    إذا can_take_byte(oi):
        إذا hse.سمون_اوږدتیا == 0:
            جمع_کړه_نښان_بټ(hse, oi, HEATSHRINK_LITERAL_MARKER)
            أرجع HSE_state.HSES_YIELD_LITERAL
        وإلا:
            جمع_کړه_نښان_بټ(hse, oi, HEATSHRINK_BACKREF_MARKER)
            hse.outgoing_بټونه = hse.سمون_pos - 1
            hse.outgoing_بټونه_شمېر = hse.کړکۍ_اندازه
            أرجع HSE_state.HSES_YIELD_BR_شاخص
    وإلا:
        أرجع HSE_state.HSES_YIELD_نښان_بټ


تعریف st_yield_literal(hse, oi):
    إذا can_take_byte(oi):
        ټېل_literal_byte(hse, oi)
        أرجع HSE_state.HSES_پلټنه
    وإلا:
        أرجع HSE_state.HSES_YIELD_LITERAL


تعریف st_yield_br_شاخص(hse, oi):
    إذا can_take_byte(oi):
        إذا ټېل_outgoing_بټونه(hse, oi) > 0:
            أرجع HSE_state.HSES_YIELD_BR_شاخص
        وإلا:
            hse.outgoing_بټونه = hse.سمون_اوږدتیا - 1
            hse.outgoing_بټونه_شمېر = hse.lookahead_اندازه
            أرجع HSE_state.HSES_YIELD_BR_اوږدتیا
    وإلا:
        أرجع HSE_state.HSES_YIELD_BR_شاخص


تعریف st_yield_br_اوږدتیا(hse, oi):
    إذا can_take_byte(oi):
        إذا ټېل_outgoing_بټونه(hse, oi) > 0:
            أرجع HSE_state.HSES_YIELD_BR_اوږدتیا
        وإلا:
            hse.سمون_لټون_شاخص += hse.سمون_اوږدتیا
            hse.سمون_اوږدتیا = 0
            أرجع HSE_state.HSES_پلټنه
    وإلا:
        أرجع HSE_state.HSES_YIELD_BR_اوږدتیا


تعریف st_save_backlog(hse):
    save_backlog(hse)
    أرجع HSE_state.HSES_NOT_ډک


تعریف st_flush_بټ_بفر(hse, oi):
    إذا hse.بټ_شاخص == 0x80:
        أرجع HSE_state.HSES_بشپړ
    وإلاإذا can_take_byte(oi):
        oi.buf[oi.output_اندازه[0]] = hse.current_byte
        oi.output_اندازه[0] += 1
        أرجع HSE_state.HSES_بشپړ
    وإلا:
        أرجع HSE_state.HSES_FLUSH_بټونه


تعریف save_backlog(hse):
    ننوت_buf_sz = get_ننوت_بفر_اندازه(hse)

    msi = hse.سمون_لټون_شاخص

    rem = ننوت_buf_sz - msi  # unprocessed bytes
    ښويدنه_sz = ننوت_buf_sz + rem

    لكل i في لړ(ښويدنه_sz):
        hse.بفر_2[i] = hse.بفر_2[ننوت_buf_sz - rem + i]

    hse.سمون_لټون_شاخص = 0
    hse.ننوت_اندازه -= ننوت_buf_sz - rem


تعریف encode(معلومات, کړکۍ_اندازه, lookahead_اندازه):
    hse = heatshrink_encoder(کړکۍ_اندازه, lookahead_اندازه)
    ننوت_buf = (ctypes.c_uint8 * اوږدوالی(معلومات))()
    لكل i, d في enumerate(معلومات):
        ننوت_buf[i] = d
    دننه_اندازه = اوږدوالی(ننوت_buf)

    بهر_buf_اندازه = 4 * دننه_اندازه  # set output buffer size a bit larger
    بهر_buf = (ctypes.c_uint8 * بهر_buf_اندازه)()

    sunk = 0
    ret = []
    طالما sunk < دننه_اندازه:
        ننوت_اندازه = ctypes.c_اندازه_t(دننه_اندازه)
        heatshrink_encoder_sink(hse, ننوت_buf, دننه_اندازه - sunk, ننوت_اندازه)
        ننوت_buf = ننوت_buf[ننوت_اندازه.ارزښت:]
        sunk += ننوت_اندازه.ارزښت
        إذا sunk == دننه_اندازه:
            heatshrink_encoder_پای(hse)

        poll_res = HSE_poll_res.HSER_POLL_نور
        طالما poll_res == HSE_poll_res.HSER_POLL_نور:
            output_اندازه = (ctypes.c_اندازه_t * 1)()
            poll_res = heatshrink_encoder_poll(hse, بهر_buf, بهر_buf_اندازه, output_اندازه)
            ret += list(بهر_buf)[0:output_اندازه[0]]

        إذا sunk == دننه_اندازه:
            heatshrink_encoder_پای(hse)
    أرجع ret
