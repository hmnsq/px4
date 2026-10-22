استورد ctypes
من enum استورد Enum


# Note: this implementation directly follows the heatshrink_encoder.c code
# (it's neither expected to be very efficient, nor is it pythonic)

# Enum
صنف HSE_state(ctypes.c_int):
    HSES_NOT_بھریا = 0
    HSES_FILLED = 1
    HSES_تلاش = 2
    HSES_YIELD_ٹیگ_ثنائی_ہندسہ = 3
    HSES_YIELD_LITERAL = 4
    HSES_YIELD_BR_اشاریہ = 5
    HSES_YIELD_BR_لمائی = 6
    HSES_SAVE_BACKLOG = 7
    HSES_FLUSH_بٹ = 8
    HSES_پورا = 9


# Constants
FLAG_IS_FINISHING = 0x01
میل_NOT_لبھیا = 0xFFFF

HEATSHRINK_LITERAL_MARKER = 0x01
HEATSHRINK_BACKREF_MARKER = 0x00


# Structs
صنف output_info(ctypes.Structure):
    _fields_ = [
        ("buf", ctypes.اشارہ_گر(ctypes.c_uint8)),
        ("buf_size", ctypes.c_ناپ_t),
        ("output_size", ctypes.اشارہ_گر(ctypes.c_ناپ_t))
    ]


# Functions
تعریف جوڑو_ٹیگ_ثنائی_ہندسہ(hse, oi, ٹیگ):
    ڈالو_بٹ(hse, 1, ٹیگ, oi)


تعریف ڈالو_بٹ(hse, گنتی, بٹ, oi):
    أكد گنتی <= 8
    current_byte = ctypes.c_uint8(hse.current_byte)
    ثنائی_ہندسہ_اشاریہ = ctypes.c_uint8(hse.ثنائی_ہندسہ_اشاریہ)
    لكل i في سلسلہ(گنتی - 1, -1, -1):
        ثنائی_ہندسہ = بٹ & (1 << i)
        إذا ثنائی_ہندسہ:
            current_byte.قدر |= ثنائی_ہندسہ_اشاریہ.قدر
        ثنائی_ہندسہ_اشاریہ.قدر >>= 1
        إذا ثنائی_ہندسہ_اشاریہ.قدر == 0:
            ثنائی_ہندسہ_اشاریہ.قدر = 0x80
            oi.buf[oi.output_ناپ[0]] = current_byte.قدر
            oi.output_ناپ[0] += 1
            current_byte.قدر = 0
    hse.current_byte = current_byte.قدر
    hse.ثنائی_ہندسہ_اشاریہ = ثنائی_ہندسہ_اشاریہ.قدر


تعریف ڈالو_literal_byte(hse, oi):
    processed_وتھ = hse.میل_جانچ_اشاریہ - 1
    داخلہ_وتھ = get_داخلہ_وتھ(hse) + processed_وتھ
    c = hse.بفر_2[داخلہ_وتھ]
    ڈالو_بٹ(hse, 8, c, oi)


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

        ("buffer", ctypes.اشارہ_گر(ctypes.c_uint8)),
    ]

    تعریف __init__(self, کھڑکی_ناپ=8, lookahead_ناپ=4):
        أعلى().__init__()
        self.کھڑکی_ناپ = کھڑکی_ناپ
        self.lookahead_ناپ = lookahead_ناپ
        self.تلاش_اشاریہ = hs_اشاریہ(کھڑکی_ناپ)
        self.بفر_2 = (ctypes.c_uint8 * (2 << کھڑکی_ناپ))()
        self.ثنائی_ہندسہ_اشاریہ = 0x80


صنف hs_اشاریہ(ctypes.Structure):
    _fields_ = [("index", ctypes.اشارہ_گر(ctypes.c_int16))]

    تعریف __init__(self, کھڑکی_ناپ):
        أعلى().__init__()
        self.اشاریہ = (ctypes.c_int16 * (2 << کھڑکی_ناپ))()


صنف HSE_sink_res(Enum):
    HSER_SINK_OK = 0
    HSER_SINK_ERROR_NULL = -1
    HSER_SINK_ERROR_MISUSE = -2


صنف HSE_poll_res(Enum):
    HSER_POLL_خالی_تھاواں = 0
    HSER_POLL_ہور = 1
    HSER_POLL_ERROR_NULL = -1
    HSER_POLL_ERROR_MISUSE = -2


صنف HSE_انت_res(Enum):
    HSER_انت_پورا = 0
    HSER_انت_ہور = 1
    HSER_انت_ERROR_NULL = -1


تعریف is_finishing(hse):
    أرجع hse.flags & FLAG_IS_FINISHING


تعریف can_take_byte(oi):
    أرجع oi.output_ناپ[0] < oi.buf_ناپ


تعریف get_داخلہ_بفر_ناپ(hse):
    أرجع 1 << hse.کھڑکی_ناپ


تعریف get_lookahead_ناپ(hse):
    أرجع 1 << hse.lookahead_ناپ


تعریف get_داخلہ_وتھ(hse):
    أرجع get_داخلہ_بفر_ناپ(hse)


تعریف heatshrink_encoder_sink(hse, وچ_buf, ناپ, داخلہ_ناپ):
    إذا hse اے کجھ_نئیں أو وچ_buf اے کجھ_نئیں أو داخلہ_ناپ اے کجھ_نئیں:
        أرجع HSE_sink_res.HSER_SINK_ERROR_NULL

    إذا is_finishing(hse):
        أرجع HSE_sink_res.HSER_SINK_ERROR_MISUSE

    إذا hse.state != HSE_state.HSES_NOT_بھریا:
        أرجع HSE_sink_res.HSER_SINK_ERROR_MISUSE

    write_وتھ_2 = get_داخلہ_وتھ(hse) + hse.داخلہ_ناپ
    ibs = get_داخلہ_بفر_ناپ(hse)
    rem = ibs - hse.داخلہ_ناپ
    cp_sz = min(rem, ناپ)

    لكل i في سلسلہ(cp_sz):
        hse.بفر_2[write_وتھ_2 + i] = وچ_buf[i]
    داخلہ_ناپ.قدر = cp_sz
    hse.داخلہ_ناپ += cp_sz

    إذا cp_sz == rem:
        hse.state = HSE_state.HSES_FILLED

    أرجع HSE_sink_res.HSER_SINK_OK


تعریف do_indexing(hse):
    # Build an index array I that contains flattened linked lists
    # for the previous instances of every byte in the buffer.

    hsi = hse.تلاش_اشاریہ
    آخری = [0xffff] * 256
    buf = hse.بفر_2
    اشاریہ = hsi.اشاریہ
    داخلہ_وتھ = get_داخلہ_وتھ(hse)
    سرا = داخلہ_وتھ + hse.داخلہ_ناپ

    لكل i في سلسلہ(0, سرا):
        v = buf[i]
        lv = آخری[v]
        اشاریہ[i] = lv
        آخری[v] = i


تعریف heatshrink_encoder_poll(hse, باہرلا_buf, باہرلا_buf_ناپ, output_ناپ):
    إذا hse اے کجھ_نئیں أو باہرلا_buf اے کجھ_نئیں أو output_ناپ اے کجھ_نئیں:
        أرجع HSE_poll_res.HSER_POLL_ERROR_NULL

    إذا باہرلا_buf_ناپ == 0:
        أرجع HSE_poll_res.HSER_POLL_ERROR_MISUSE

    output_ناپ[0] = 0
    oi = output_info()
    oi.buf = باہرلا_buf
    oi.buf_ناپ = باہرلا_buf_ناپ
    oi.output_ناپ = output_ناپ

    طالما سچ:
        وچ_state = hse.state
        إذا وچ_state == HSE_state.HSES_NOT_بھریا:
            أرجع HSE_poll_res.HSER_POLL_خالی_تھاواں
        وإلاإذا وچ_state == HSE_state.HSES_پورا:
            أرجع HSE_poll_res.HSER_POLL_خالی_تھاواں
        وإلاإذا وچ_state == HSE_state.HSES_FILLED:
            do_indexing(hse)
            hse.state = HSE_state.HSES_تلاش
        وإلاإذا وچ_state == HSE_state.HSES_تلاش:
            hse.state = st_قدم_تلاش(hse)
        وإلاإذا وچ_state == HSE_state.HSES_YIELD_ٹیگ_ثنائی_ہندسہ:
            hse.state = st_yield_ٹیگ_ثنائی_ہندسہ(hse, oi)
        وإلاإذا وچ_state == HSE_state.HSES_YIELD_LITERAL:
            hse.state = st_yield_literal(hse, oi)
        وإلاإذا وچ_state == HSE_state.HSES_YIELD_BR_اشاریہ:
            hse.state = st_yield_br_اشاریہ(hse, oi)
        وإلاإذا وچ_state == HSE_state.HSES_YIELD_BR_لمائی:
            hse.state = st_yield_br_لمائی(hse, oi)
        وإلاإذا وچ_state == HSE_state.HSES_SAVE_BACKLOG:
            hse.state = st_save_backlog(hse)
        وإلاإذا وچ_state == HSE_state.HSES_FLUSH_بٹ:
            hse.state = st_flush_ثنائی_ہندسہ_بفر(hse, oi)
        وإلا:
            أرجع HSE_poll_res.HSER_POLL_ERROR_MISUSE

        إذا hse.state == وچ_state:
            إذا oi.output_ناپ == oi.buf_ناپ:
                أرجع HSE_poll_res.HSER_POLL_ہور


تعریف heatshrink_encoder_انت(hse):
    hse.flags |= FLAG_IS_FINISHING
    إذا hse.state == HSE_state.HSES_NOT_بھریا:
        hse.state = HSE_state.HSES_FILLED
    إذا hse.state == HSE_state.HSES_پورا:
        أرجع HSE_انت_res.HSER_انت_پورا
    أرجع HSE_انت_res.HSER_انت_ہور


تعریف st_قدم_تلاش(hse):
    کھڑکی_لمائی = get_داخلہ_بفر_ناپ(hse)
    lookahead_sz = get_lookahead_ناپ(hse)
    msi = hse.میل_جانچ_اشاریہ

    fin = is_finishing(hse)
    إذا msi > hse.داخلہ_ناپ - (1 إذا fin وإلا lookahead_sz):
        أرجع HSE_state.HSES_FLUSH_بٹ إذا fin وإلا HSE_state.HSES_SAVE_BACKLOG

    داخلہ_وتھ = get_داخلہ_وتھ(hse)
    سرا = داخلہ_وتھ + msi
    شروع = سرا - کھڑکی_لمائی

    max_possible = lookahead_sz إذا hse.داخلہ_ناپ - msi >= lookahead_sz وإلا hse.داخلہ_ناپ - msi

    میل_pos, میل_لمائی = لبھو_longest_میل(hse, شروع, سرا, max_possible)

    إذا میل_pos == میل_NOT_لبھیا:
        hse.میل_جانچ_اشاریہ += 1
        hse.میل_لمائی = 0
        أرجع HSE_state.HSES_YIELD_ٹیگ_ثنائی_ہندسہ
    وإلا:
        hse.میل_pos = میل_pos
        hse.میل_لمائی = میل_لمائی
        أرجع HSE_state.HSES_YIELD_ٹیگ_ثنائی_ہندسہ


تعریف لبھو_longest_میل(hse, شروع, سرا, maxlen):
    buf = hse.بفر_2

    میل_maxlen = 0
    میل_اشاریہ = میل_NOT_لبھیا

    needlepoint = سرا

    pos = hse.تلاش_اشاریہ.اشاریہ[سرا]
    buf_needlepoint_maxlen = buf[needlepoint + میل_maxlen]
    طالما pos >= شروع:

        إذا buf[pos + میل_maxlen] != buf_needlepoint_maxlen:
            pos = hse.تلاش_اشاریہ.اشاریہ[pos]
            استمر

        لمائی = 1
        لكل لمائی في سلسلہ(1, maxlen):
            إذا buf[pos + لمائی] != buf[needlepoint + لمائی]:
                اكسر
        إذا لمائی > میل_maxlen:
            میل_maxlen = لمائی
            میل_اشاریہ = pos
            buf_needlepoint_maxlen = buf[needlepoint + میل_maxlen]
            إذا لمائی == maxlen:
                اكسر  # won't find better

        pos = hse.تلاش_اشاریہ.اشاریہ[pos]

    break_even_نقطہ = 1 + hse.کھڑکی_ناپ + hse.lookahead_ناپ

    إذا میل_maxlen > (break_even_نقطہ // 8):
        أرجع سرا - میل_اشاریہ, میل_maxlen
    أرجع میل_NOT_لبھیا, 0


تعریف ڈالو_outgoing_بٹ(hse, oi):
    إذا hse.outgoing_بٹ_گنتی > 8:
        گنتی = 8
        بٹ = hse.outgoing_بٹ >> (hse.outgoing_بٹ_گنتی - 8)
    وإلا:
        گنتی = hse.outgoing_بٹ_گنتی
        بٹ = hse.outgoing_بٹ

    إذا گنتی > 0:
        ڈالو_بٹ(hse, گنتی, بٹ, oi)
        hse.outgoing_بٹ_گنتی -= گنتی
    أرجع گنتی


تعریف st_yield_ٹیگ_ثنائی_ہندسہ(hse, oi):
    إذا can_take_byte(oi):
        إذا hse.میل_لمائی == 0:
            جوڑو_ٹیگ_ثنائی_ہندسہ(hse, oi, HEATSHRINK_LITERAL_MARKER)
            أرجع HSE_state.HSES_YIELD_LITERAL
        وإلا:
            جوڑو_ٹیگ_ثنائی_ہندسہ(hse, oi, HEATSHRINK_BACKREF_MARKER)
            hse.outgoing_بٹ = hse.میل_pos - 1
            hse.outgoing_بٹ_گنتی = hse.کھڑکی_ناپ
            أرجع HSE_state.HSES_YIELD_BR_اشاریہ
    وإلا:
        أرجع HSE_state.HSES_YIELD_ٹیگ_ثنائی_ہندسہ


تعریف st_yield_literal(hse, oi):
    إذا can_take_byte(oi):
        ڈالو_literal_byte(hse, oi)
        أرجع HSE_state.HSES_تلاش
    وإلا:
        أرجع HSE_state.HSES_YIELD_LITERAL


تعریف st_yield_br_اشاریہ(hse, oi):
    إذا can_take_byte(oi):
        إذا ڈالو_outgoing_بٹ(hse, oi) > 0:
            أرجع HSE_state.HSES_YIELD_BR_اشاریہ
        وإلا:
            hse.outgoing_بٹ = hse.میل_لمائی - 1
            hse.outgoing_بٹ_گنتی = hse.lookahead_ناپ
            أرجع HSE_state.HSES_YIELD_BR_لمائی
    وإلا:
        أرجع HSE_state.HSES_YIELD_BR_اشاریہ


تعریف st_yield_br_لمائی(hse, oi):
    إذا can_take_byte(oi):
        إذا ڈالو_outgoing_بٹ(hse, oi) > 0:
            أرجع HSE_state.HSES_YIELD_BR_لمائی
        وإلا:
            hse.میل_جانچ_اشاریہ += hse.میل_لمائی
            hse.میل_لمائی = 0
            أرجع HSE_state.HSES_تلاش
    وإلا:
        أرجع HSE_state.HSES_YIELD_BR_لمائی


تعریف st_save_backlog(hse):
    save_backlog(hse)
    أرجع HSE_state.HSES_NOT_بھریا


تعریف st_flush_ثنائی_ہندسہ_بفر(hse, oi):
    إذا hse.ثنائی_ہندسہ_اشاریہ == 0x80:
        أرجع HSE_state.HSES_پورا
    وإلاإذا can_take_byte(oi):
        oi.buf[oi.output_ناپ[0]] = hse.current_byte
        oi.output_ناپ[0] += 1
        أرجع HSE_state.HSES_پورا
    وإلا:
        أرجع HSE_state.HSES_FLUSH_بٹ


تعریف save_backlog(hse):
    داخلہ_buf_sz = get_داخلہ_بفر_ناپ(hse)

    msi = hse.میل_جانچ_اشاریہ

    rem = داخلہ_buf_sz - msi  # unprocessed bytes
    کھسکاؤ_sz = داخلہ_buf_sz + rem

    لكل i في سلسلہ(کھسکاؤ_sz):
        hse.بفر_2[i] = hse.بفر_2[داخلہ_buf_sz - rem + i]

    hse.میل_جانچ_اشاریہ = 0
    hse.داخلہ_ناپ -= داخلہ_buf_sz - rem


تعریف encode(انکڑے, کھڑکی_ناپ, lookahead_ناپ):
    hse = heatshrink_encoder(کھڑکی_ناپ, lookahead_ناپ)
    داخلہ_buf = (ctypes.c_uint8 * لمبائی(انکڑے))()
    لكل i, d في enumerate(انکڑے):
        داخلہ_buf[i] = d
    وچ_ناپ = لمبائی(داخلہ_buf)

    باہرلا_buf_ناپ = 4 * وچ_ناپ  # set output buffer size a bit larger
    باہرلا_buf = (ctypes.c_uint8 * باہرلا_buf_ناپ)()

    sunk = 0
    ret = []
    طالما sunk < وچ_ناپ:
        داخلہ_ناپ = ctypes.c_ناپ_t(وچ_ناپ)
        heatshrink_encoder_sink(hse, داخلہ_buf, وچ_ناپ - sunk, داخلہ_ناپ)
        داخلہ_buf = داخلہ_buf[داخلہ_ناپ.قدر:]
        sunk += داخلہ_ناپ.قدر
        إذا sunk == وچ_ناپ:
            heatshrink_encoder_انت(hse)

        poll_res = HSE_poll_res.HSER_POLL_ہور
        طالما poll_res == HSE_poll_res.HSER_POLL_ہور:
            output_ناپ = (ctypes.c_ناپ_t * 1)()
            poll_res = heatshrink_encoder_poll(hse, باہرلا_buf, باہرلا_buf_ناپ, output_ناپ)
            ret += list(باہرلا_buf)[0:output_ناپ[0]]

        إذا sunk == وچ_ناپ:
            heatshrink_encoder_انت(hse)
    أرجع ret
