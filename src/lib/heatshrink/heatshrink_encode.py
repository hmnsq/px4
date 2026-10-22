استورد ctypes
من enum استورد Enum


# Note: this implementation directly follows the heatshrink_encoder.c code
# (it's neither expected to be very efficient, nor is it pythonic)

# Enum
صنف HSE_state(ctypes.c_int):
    HSES_NOT_پر = 0
    HSES_FILLED = 1
    HSES_جستجو = 2
    HSES_YIELD_برچسب_بیت = 3
    HSES_YIELD_LITERAL = 4
    HSES_YIELD_BR_شاخص = 5
    HSES_YIELD_BR_درازی = 6
    HSES_SAVE_BACKLOG = 7
    HSES_FLUSH_بیتها = 8
    HSES_انجامشده = 9


# Constants
FLAG_IS_FINISHING = 0x01
تطابق_NOT_یافت = 0xFFFF

HEATSHRINK_LITERAL_MARKER = 0x01
HEATSHRINK_BACKREF_MARKER = 0x00


# Structs
صنف output_info(ctypes.Structure):
    _fields_ = [
        ("buf", ctypes.اشاره_گر(ctypes.c_uint8)),
        ("buf_size", ctypes.c_اندازه_t),
        ("output_size", ctypes.اشاره_گر(ctypes.c_اندازه_t))
    ]


# Functions
تعریف جمع_کن_برچسب_بیت(hse, oi, برچسب):
    افزودن_بیتها(hse, 1, برچسب, oi)


تعریف افزودن_بیتها(hse, شمار, بیتها, oi):
    أكد شمار <= 8
    current_byte = ctypes.c_uint8(hse.current_byte)
    بیت_شاخص = ctypes.c_uint8(hse.بیت_شاخص)
    لكل i في دامنه(شمار - 1, -1, -1):
        بیت = بیتها & (1 << i)
        إذا بیت:
            current_byte.مقدار |= بیت_شاخص.مقدار
        بیت_شاخص.مقدار >>= 1
        إذا بیت_شاخص.مقدار == 0:
            بیت_شاخص.مقدار = 0x80
            oi.buf[oi.output_اندازه[0]] = current_byte.مقدار
            oi.output_اندازه[0] += 1
            current_byte.مقدار = 0
    hse.current_byte = current_byte.مقدار
    hse.بیت_شاخص = بیت_شاخص.مقدار


تعریف افزودن_literal_byte(hse, oi):
    processed_جابه_جایی = hse.تطابق_پیمایش_شاخص - 1
    درون_داد_جابه_جایی = get_درون_داد_جابه_جایی(hse) + processed_جابه_جایی
    c = hse.میانگیر_2[درون_داد_جابه_جایی]
    افزودن_بیتها(hse, 8, c, oi)


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

        ("buffer", ctypes.اشاره_گر(ctypes.c_uint8)),
    ]

    تعریف __init__(self, پنجره_اندازه=8, lookahead_اندازه=4):
        أعلى().__init__()
        self.پنجره_اندازه = پنجره_اندازه
        self.lookahead_اندازه = lookahead_اندازه
        self.جستجو_شاخص = hs_شاخص(پنجره_اندازه)
        self.میانگیر_2 = (ctypes.c_uint8 * (2 << پنجره_اندازه))()
        self.بیت_شاخص = 0x80


صنف hs_شاخص(ctypes.Structure):
    _fields_ = [("index", ctypes.اشاره_گر(ctypes.c_int16))]

    تعریف __init__(self, پنجره_اندازه):
        أعلى().__init__()
        self.شاخص = (ctypes.c_int16 * (2 << پنجره_اندازه))()


صنف HSE_sink_res(Enum):
    HSER_SINK_OK = 0
    HSER_SINK_ERROR_NULL = -1
    HSER_SINK_ERROR_MISUSE = -2


صنف HSE_poll_res(Enum):
    HSER_POLL_خالی = 0
    HSER_POLL_بیشتر = 1
    HSER_POLL_ERROR_NULL = -1
    HSER_POLL_ERROR_MISUSE = -2


صنف HSE_پایان_res(Enum):
    HSER_پایان_انجامشده = 0
    HSER_پایان_بیشتر = 1
    HSER_پایان_ERROR_NULL = -1


تعریف is_finishing(hse):
    أرجع hse.flags & FLAG_IS_FINISHING


تعریف can_take_byte(oi):
    أرجع oi.output_اندازه[0] < oi.buf_اندازه


تعریف get_درون_داد_میانگیر_اندازه(hse):
    أرجع 1 << hse.پنجره_اندازه


تعریف get_lookahead_اندازه(hse):
    أرجع 1 << hse.lookahead_اندازه


تعریف get_درون_داد_جابه_جایی(hse):
    أرجع get_درون_داد_میانگیر_اندازه(hse)


تعریف heatshrink_encoder_sink(hse, درون_buf, اندازه, درون_داد_اندازه):
    إذا hse است هیچ أو درون_buf است هیچ أو درون_داد_اندازه است هیچ:
        أرجع HSE_sink_res.HSER_SINK_ERROR_NULL

    إذا is_finishing(hse):
        أرجع HSE_sink_res.HSER_SINK_ERROR_MISUSE

    إذا hse.state != HSE_state.HSES_NOT_پر:
        أرجع HSE_sink_res.HSER_SINK_ERROR_MISUSE

    write_جابه_جایی_2 = get_درون_داد_جابه_جایی(hse) + hse.درون_داد_اندازه
    ibs = get_درون_داد_میانگیر_اندازه(hse)
    rem = ibs - hse.درون_داد_اندازه
    cp_sz = min(rem, اندازه)

    لكل i في دامنه(cp_sz):
        hse.میانگیر_2[write_جابه_جایی_2 + i] = درون_buf[i]
    درون_داد_اندازه.مقدار = cp_sz
    hse.درون_داد_اندازه += cp_sz

    إذا cp_sz == rem:
        hse.state = HSE_state.HSES_FILLED

    أرجع HSE_sink_res.HSER_SINK_OK


تعریف do_indexing(hse):
    # Build an index array I that contains flattened linked lists
    # for the previous instances of every byte in the buffer.

    hsi = hse.جستجو_شاخص
    آخرین = [0xffff] * 256
    buf = hse.میانگیر_2
    شاخص = hsi.شاخص
    درون_داد_جابه_جایی = get_درون_داد_جابه_جایی(hse)
    انتها = درون_داد_جابه_جایی + hse.درون_داد_اندازه

    لكل i في دامنه(0, انتها):
        v = buf[i]
        lv = آخرین[v]
        شاخص[i] = lv
        آخرین[v] = i


تعریف heatshrink_encoder_poll(hse, بیرون_buf, بیرون_buf_اندازه, output_اندازه):
    إذا hse است هیچ أو بیرون_buf است هیچ أو output_اندازه است هیچ:
        أرجع HSE_poll_res.HSER_POLL_ERROR_NULL

    إذا بیرون_buf_اندازه == 0:
        أرجع HSE_poll_res.HSER_POLL_ERROR_MISUSE

    output_اندازه[0] = 0
    oi = output_info()
    oi.buf = بیرون_buf
    oi.buf_اندازه = بیرون_buf_اندازه
    oi.output_اندازه = output_اندازه

    طالما درست:
        درون_state = hse.state
        إذا درون_state == HSE_state.HSES_NOT_پر:
            أرجع HSE_poll_res.HSER_POLL_خالی
        وإلاإذا درون_state == HSE_state.HSES_انجامشده:
            أرجع HSE_poll_res.HSER_POLL_خالی
        وإلاإذا درون_state == HSE_state.HSES_FILLED:
            do_indexing(hse)
            hse.state = HSE_state.HSES_جستجو
        وإلاإذا درون_state == HSE_state.HSES_جستجو:
            hse.state = st_گام_جستجو(hse)
        وإلاإذا درون_state == HSE_state.HSES_YIELD_برچسب_بیت:
            hse.state = st_yield_برچسب_بیت(hse, oi)
        وإلاإذا درون_state == HSE_state.HSES_YIELD_LITERAL:
            hse.state = st_yield_literal(hse, oi)
        وإلاإذا درون_state == HSE_state.HSES_YIELD_BR_شاخص:
            hse.state = st_yield_br_شاخص(hse, oi)
        وإلاإذا درون_state == HSE_state.HSES_YIELD_BR_درازی:
            hse.state = st_yield_br_درازی(hse, oi)
        وإلاإذا درون_state == HSE_state.HSES_SAVE_BACKLOG:
            hse.state = st_save_backlog(hse)
        وإلاإذا درون_state == HSE_state.HSES_FLUSH_بیتها:
            hse.state = st_flush_بیت_میانگیر(hse, oi)
        وإلا:
            أرجع HSE_poll_res.HSER_POLL_ERROR_MISUSE

        إذا hse.state == درون_state:
            إذا oi.output_اندازه == oi.buf_اندازه:
                أرجع HSE_poll_res.HSER_POLL_بیشتر


تعریف heatshrink_encoder_پایان(hse):
    hse.flags |= FLAG_IS_FINISHING
    إذا hse.state == HSE_state.HSES_NOT_پر:
        hse.state = HSE_state.HSES_FILLED
    إذا hse.state == HSE_state.HSES_انجامشده:
        أرجع HSE_پایان_res.HSER_پایان_انجامشده
    أرجع HSE_پایان_res.HSER_پایان_بیشتر


تعریف st_گام_جستجو(hse):
    پنجره_درازی = get_درون_داد_میانگیر_اندازه(hse)
    lookahead_sz = get_lookahead_اندازه(hse)
    msi = hse.تطابق_پیمایش_شاخص

    fin = is_finishing(hse)
    إذا msi > hse.درون_داد_اندازه - (1 إذا fin وإلا lookahead_sz):
        أرجع HSE_state.HSES_FLUSH_بیتها إذا fin وإلا HSE_state.HSES_SAVE_BACKLOG

    درون_داد_جابه_جایی = get_درون_داد_جابه_جایی(hse)
    انتها = درون_داد_جابه_جایی + msi
    شروع = انتها - پنجره_درازی

    max_possible = lookahead_sz إذا hse.درون_داد_اندازه - msi >= lookahead_sz وإلا hse.درون_داد_اندازه - msi

    تطابق_pos, تطابق_درازی = پیدا_کن_longest_تطابق(hse, شروع, انتها, max_possible)

    إذا تطابق_pos == تطابق_NOT_یافت:
        hse.تطابق_پیمایش_شاخص += 1
        hse.تطابق_درازی = 0
        أرجع HSE_state.HSES_YIELD_برچسب_بیت
    وإلا:
        hse.تطابق_pos = تطابق_pos
        hse.تطابق_درازی = تطابق_درازی
        أرجع HSE_state.HSES_YIELD_برچسب_بیت


تعریف پیدا_کن_longest_تطابق(hse, شروع, انتها, maxlen):
    buf = hse.میانگیر_2

    تطابق_maxlen = 0
    تطابق_شاخص = تطابق_NOT_یافت

    needlepoint = انتها

    pos = hse.جستجو_شاخص.شاخص[انتها]
    buf_needlepoint_maxlen = buf[needlepoint + تطابق_maxlen]
    طالما pos >= شروع:

        إذا buf[pos + تطابق_maxlen] != buf_needlepoint_maxlen:
            pos = hse.جستجو_شاخص.شاخص[pos]
            استمر

        درازی = 1
        لكل درازی في دامنه(1, maxlen):
            إذا buf[pos + درازی] != buf[needlepoint + درازی]:
                اكسر
        إذا درازی > تطابق_maxlen:
            تطابق_maxlen = درازی
            تطابق_شاخص = pos
            buf_needlepoint_maxlen = buf[needlepoint + تطابق_maxlen]
            إذا درازی == maxlen:
                اكسر  # won't find better

        pos = hse.جستجو_شاخص.شاخص[pos]

    break_even_نقطه = 1 + hse.پنجره_اندازه + hse.lookahead_اندازه

    إذا تطابق_maxlen > (break_even_نقطه // 8):
        أرجع انتها - تطابق_شاخص, تطابق_maxlen
    أرجع تطابق_NOT_یافت, 0


تعریف افزودن_outgoing_بیتها(hse, oi):
    إذا hse.outgoing_بیتها_شمار > 8:
        شمار = 8
        بیتها = hse.outgoing_بیتها >> (hse.outgoing_بیتها_شمار - 8)
    وإلا:
        شمار = hse.outgoing_بیتها_شمار
        بیتها = hse.outgoing_بیتها

    إذا شمار > 0:
        افزودن_بیتها(hse, شمار, بیتها, oi)
        hse.outgoing_بیتها_شمار -= شمار
    أرجع شمار


تعریف st_yield_برچسب_بیت(hse, oi):
    إذا can_take_byte(oi):
        إذا hse.تطابق_درازی == 0:
            جمع_کن_برچسب_بیت(hse, oi, HEATSHRINK_LITERAL_MARKER)
            أرجع HSE_state.HSES_YIELD_LITERAL
        وإلا:
            جمع_کن_برچسب_بیت(hse, oi, HEATSHRINK_BACKREF_MARKER)
            hse.outgoing_بیتها = hse.تطابق_pos - 1
            hse.outgoing_بیتها_شمار = hse.پنجره_اندازه
            أرجع HSE_state.HSES_YIELD_BR_شاخص
    وإلا:
        أرجع HSE_state.HSES_YIELD_برچسب_بیت


تعریف st_yield_literal(hse, oi):
    إذا can_take_byte(oi):
        افزودن_literal_byte(hse, oi)
        أرجع HSE_state.HSES_جستجو
    وإلا:
        أرجع HSE_state.HSES_YIELD_LITERAL


تعریف st_yield_br_شاخص(hse, oi):
    إذا can_take_byte(oi):
        إذا افزودن_outgoing_بیتها(hse, oi) > 0:
            أرجع HSE_state.HSES_YIELD_BR_شاخص
        وإلا:
            hse.outgoing_بیتها = hse.تطابق_درازی - 1
            hse.outgoing_بیتها_شمار = hse.lookahead_اندازه
            أرجع HSE_state.HSES_YIELD_BR_درازی
    وإلا:
        أرجع HSE_state.HSES_YIELD_BR_شاخص


تعریف st_yield_br_درازی(hse, oi):
    إذا can_take_byte(oi):
        إذا افزودن_outgoing_بیتها(hse, oi) > 0:
            أرجع HSE_state.HSES_YIELD_BR_درازی
        وإلا:
            hse.تطابق_پیمایش_شاخص += hse.تطابق_درازی
            hse.تطابق_درازی = 0
            أرجع HSE_state.HSES_جستجو
    وإلا:
        أرجع HSE_state.HSES_YIELD_BR_درازی


تعریف st_save_backlog(hse):
    save_backlog(hse)
    أرجع HSE_state.HSES_NOT_پر


تعریف st_flush_بیت_میانگیر(hse, oi):
    إذا hse.بیت_شاخص == 0x80:
        أرجع HSE_state.HSES_انجامشده
    وإلاإذا can_take_byte(oi):
        oi.buf[oi.output_اندازه[0]] = hse.current_byte
        oi.output_اندازه[0] += 1
        أرجع HSE_state.HSES_انجامشده
    وإلا:
        أرجع HSE_state.HSES_FLUSH_بیتها


تعریف save_backlog(hse):
    درون_داد_buf_sz = get_درون_داد_میانگیر_اندازه(hse)

    msi = hse.تطابق_پیمایش_شاخص

    rem = درون_داد_buf_sz - msi  # unprocessed bytes
    لغزش_sz = درون_داد_buf_sz + rem

    لكل i في دامنه(لغزش_sz):
        hse.میانگیر_2[i] = hse.میانگیر_2[درون_داد_buf_sz - rem + i]

    hse.تطابق_پیمایش_شاخص = 0
    hse.درون_داد_اندازه -= درون_داد_buf_sz - rem


تعریف encode(داده, پنجره_اندازه, lookahead_اندازه):
    hse = heatshrink_encoder(پنجره_اندازه, lookahead_اندازه)
    درون_داد_buf = (ctypes.c_uint8 * طول(داده))()
    لكل i, d في enumerate(داده):
        درون_داد_buf[i] = d
    درون_اندازه = طول(درون_داد_buf)

    بیرون_buf_اندازه = 4 * درون_اندازه  # set output buffer size a bit larger
    بیرون_buf = (ctypes.c_uint8 * بیرون_buf_اندازه)()

    sunk = 0
    ret = []
    طالما sunk < درون_اندازه:
        درون_داد_اندازه = ctypes.c_اندازه_t(درون_اندازه)
        heatshrink_encoder_sink(hse, درون_داد_buf, درون_اندازه - sunk, درون_داد_اندازه)
        درون_داد_buf = درون_داد_buf[درون_داد_اندازه.مقدار:]
        sunk += درون_داد_اندازه.مقدار
        إذا sunk == درون_اندازه:
            heatshrink_encoder_پایان(hse)

        poll_res = HSE_poll_res.HSER_POLL_بیشتر
        طالما poll_res == HSE_poll_res.HSER_POLL_بیشتر:
            output_اندازه = (ctypes.c_اندازه_t * 1)()
            poll_res = heatshrink_encoder_poll(hse, بیرون_buf, بیرون_buf_اندازه, output_اندازه)
            ret += list(بیرون_buf)[0:output_اندازه[0]]

        إذا sunk == درون_اندازه:
            heatshrink_encoder_پایان(hse)
    أرجع ret
