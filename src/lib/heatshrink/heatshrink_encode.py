درآمد ctypes
سے enum درآمد Enum


# Note: this implementation directly follows the heatshrink_encoder.c code
# (it's neither expected to be very efficient, nor is it pythonic)

# Enum
جماعت HSE_state(ctypes.c_int):
    HSES_NOT_بھرا = 0
    HSES_FILLED = 1
    HSES_تلاش = 2
    HSES_YIELD_لیبل_ثنائی_ہندسہ = 3
    HSES_YIELD_LITERAL = 4
    HSES_YIELD_BR_اشاریہ = 5
    HSES_YIELD_BR_لمبائی = 6
    HSES_SAVE_BACKLOG = 7
    HSES_FLUSH_بٹ = 8
    HSES_مکمل = 9


# Constants
FLAG_IS_FINISHING = 0x01
مطابقت_NOT_ملا = 0xFFFF

HEATSHRINK_LITERAL_MARKER = 0x01
HEATSHRINK_BACKREF_MARKER = 0x00


# Structs
جماعت output_info(ctypes.Structure):
    _fields_ = [
        ("buf", ctypes.اشارہ_گر(ctypes.c_uint8)),
        ("buf_size", ctypes.c_سائز_t),
        ("output_size", ctypes.اشارہ_گر(ctypes.c_سائز_t))
    ]


# Functions
تعریف جوڑو_لیبل_ثنائی_ہندسہ(hse, oi, لیبل):
    ڈالو_بٹ(hse, 1, لیبل, oi)


تعریف ڈالو_بٹ(hse, گنتی, بٹ, oi):
    أكد گنتی <= 8
    current_byte = ctypes.c_uint8(hse.current_byte)
    ثنائی_ہندسہ_اشاریہ = ctypes.c_uint8(hse.ثنائی_ہندسہ_اشاریہ)
    برائے i میں سلسلہ(گنتی - 1, -1, -1):
        ثنائی_ہندسہ = بٹ & (1 << i)
        اگر ثنائی_ہندسہ:
            current_byte.قدر |= ثنائی_ہندسہ_اشاریہ.قدر
        ثنائی_ہندسہ_اشاریہ.قدر >>= 1
        اگر ثنائی_ہندسہ_اشاریہ.قدر == 0:
            ثنائی_ہندسہ_اشاریہ.قدر = 0x80
            oi.buf[oi.output_سائز[0]] = current_byte.قدر
            oi.output_سائز[0] += 1
            current_byte.قدر = 0
    hse.current_byte = current_byte.قدر
    hse.ثنائی_ہندسہ_اشاریہ = ثنائی_ہندسہ_اشاریہ.قدر


تعریف ڈالو_literal_byte(hse, oi):
    processed_انحراف = hse.مطابقت_جائزہ_اشاریہ - 1
    داخلہ_انحراف = get_داخلہ_انحراف(hse) + processed_انحراف
    c = hse.بفر_2[داخلہ_انحراف]
    ڈالو_بٹ(hse, 8, c, oi)


# Define necessary structures and enums
جماعت heatshrink_encoder(ctypes.Structure):
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

    تعریف __init__(self, کھڑکی_سائز=8, lookahead_سائز=4):
        بالا().__init__()
        self.کھڑکی_سائز = کھڑکی_سائز
        self.lookahead_سائز = lookahead_سائز
        self.تلاش_اشاریہ = hs_اشاریہ(کھڑکی_سائز)
        self.بفر_2 = (ctypes.c_uint8 * (2 << کھڑکی_سائز))()
        self.ثنائی_ہندسہ_اشاریہ = 0x80


جماعت hs_اشاریہ(ctypes.Structure):
    _fields_ = [("index", ctypes.اشارہ_گر(ctypes.c_int16))]

    تعریف __init__(self, کھڑکی_سائز):
        بالا().__init__()
        self.اشاریہ = (ctypes.c_int16 * (2 << کھڑکی_سائز))()


جماعت HSE_sink_res(Enum):
    HSER_SINK_OK = 0
    HSER_SINK_ERROR_NULL = -1
    HSER_SINK_ERROR_MISUSE = -2


جماعت HSE_poll_res(Enum):
    HSER_POLL_خالی_خانے = 0
    HSER_POLL_مزید = 1
    HSER_POLL_ERROR_NULL = -1
    HSER_POLL_ERROR_MISUSE = -2


جماعت HSE_اختتام_res(Enum):
    HSER_اختتام_مکمل = 0
    HSER_اختتام_مزید = 1
    HSER_اختتام_ERROR_NULL = -1


تعریف is_finishing(hse):
    واپس hse.flags & FLAG_IS_FINISHING


تعریف can_take_byte(oi):
    واپس oi.output_سائز[0] < oi.buf_سائز


تعریف get_داخلہ_بفر_سائز(hse):
    واپس 1 << hse.کھڑکی_سائز


تعریف get_lookahead_سائز(hse):
    واپس 1 << hse.lookahead_سائز


تعریف get_داخلہ_انحراف(hse):
    واپس get_داخلہ_بفر_سائز(hse)


تعریف heatshrink_encoder_sink(hse, اندر_buf, سائز, داخلہ_سائز):
    اگر hse ہے کچھ_نہیں یا اندر_buf ہے کچھ_نہیں یا داخلہ_سائز ہے کچھ_نہیں:
        واپس HSE_sink_res.HSER_SINK_ERROR_NULL

    اگر is_finishing(hse):
        واپس HSE_sink_res.HSER_SINK_ERROR_MISUSE

    اگر hse.state != HSE_state.HSES_NOT_بھرا:
        واپس HSE_sink_res.HSER_SINK_ERROR_MISUSE

    write_انحراف_2 = get_داخلہ_انحراف(hse) + hse.داخلہ_سائز
    ibs = get_داخلہ_بفر_سائز(hse)
    rem = ibs - hse.داخلہ_سائز
    cp_sz = min(rem, سائز)

    برائے i میں سلسلہ(cp_sz):
        hse.بفر_2[write_انحراف_2 + i] = اندر_buf[i]
    داخلہ_سائز.قدر = cp_sz
    hse.داخلہ_سائز += cp_sz

    اگر cp_sz == rem:
        hse.state = HSE_state.HSES_FILLED

    واپس HSE_sink_res.HSER_SINK_OK


تعریف do_indexing(hse):
    # Build an index array I that contains flattened linked lists
    # for the previous instances of every byte in the buffer.

    hsi = hse.تلاش_اشاریہ
    آخری = [0xffff] * 256
    buf = hse.بفر_2
    اشاریہ = hsi.اشاریہ
    داخلہ_انحراف = get_داخلہ_انحراف(hse)
    انتہا = داخلہ_انحراف + hse.داخلہ_سائز

    برائے i میں سلسلہ(0, انتہا):
        v = buf[i]
        lv = آخری[v]
        اشاریہ[i] = lv
        آخری[v] = i


تعریف heatshrink_encoder_poll(hse, برآمد_buf, برآمد_buf_سائز, output_سائز):
    اگر hse ہے کچھ_نہیں یا برآمد_buf ہے کچھ_نہیں یا output_سائز ہے کچھ_نہیں:
        واپس HSE_poll_res.HSER_POLL_ERROR_NULL

    اگر برآمد_buf_سائز == 0:
        واپس HSE_poll_res.HSER_POLL_ERROR_MISUSE

    output_سائز[0] = 0
    oi = output_info()
    oi.buf = برآمد_buf
    oi.buf_سائز = برآمد_buf_سائز
    oi.output_سائز = output_سائز

    جب سچ:
        اندر_state = hse.state
        اگر اندر_state == HSE_state.HSES_NOT_بھرا:
            واپس HSE_poll_res.HSER_POLL_خالی_خانے
        وگرنہ_اگر اندر_state == HSE_state.HSES_مکمل:
            واپس HSE_poll_res.HSER_POLL_خالی_خانے
        وگرنہ_اگر اندر_state == HSE_state.HSES_FILLED:
            do_indexing(hse)
            hse.state = HSE_state.HSES_تلاش
        وگرنہ_اگر اندر_state == HSE_state.HSES_تلاش:
            hse.state = st_قدم_تلاش(hse)
        وگرنہ_اگر اندر_state == HSE_state.HSES_YIELD_لیبل_ثنائی_ہندسہ:
            hse.state = st_yield_لیبل_ثنائی_ہندسہ(hse, oi)
        وگرنہ_اگر اندر_state == HSE_state.HSES_YIELD_LITERAL:
            hse.state = st_yield_literal(hse, oi)
        وگرنہ_اگر اندر_state == HSE_state.HSES_YIELD_BR_اشاریہ:
            hse.state = st_yield_br_اشاریہ(hse, oi)
        وگرنہ_اگر اندر_state == HSE_state.HSES_YIELD_BR_لمبائی:
            hse.state = st_yield_br_لمبائی(hse, oi)
        وگرنہ_اگر اندر_state == HSE_state.HSES_SAVE_BACKLOG:
            hse.state = st_save_backlog(hse)
        وگرنہ_اگر اندر_state == HSE_state.HSES_FLUSH_بٹ:
            hse.state = st_flush_ثنائی_ہندسہ_بفر(hse, oi)
        ورنہ:
            واپس HSE_poll_res.HSER_POLL_ERROR_MISUSE

        اگر hse.state == اندر_state:
            اگر oi.output_سائز == oi.buf_سائز:
                واپس HSE_poll_res.HSER_POLL_مزید


تعریف heatshrink_encoder_اختتام(hse):
    hse.flags |= FLAG_IS_FINISHING
    اگر hse.state == HSE_state.HSES_NOT_بھرا:
        hse.state = HSE_state.HSES_FILLED
    اگر hse.state == HSE_state.HSES_مکمل:
        واپس HSE_اختتام_res.HSER_اختتام_مکمل
    واپس HSE_اختتام_res.HSER_اختتام_مزید


تعریف st_قدم_تلاش(hse):
    کھڑکی_لمبائی = get_داخلہ_بفر_سائز(hse)
    lookahead_sz = get_lookahead_سائز(hse)
    msi = hse.مطابقت_جائزہ_اشاریہ

    fin = is_finishing(hse)
    اگر msi > hse.داخلہ_سائز - (1 اگر fin ورنہ lookahead_sz):
        واپس HSE_state.HSES_FLUSH_بٹ اگر fin ورنہ HSE_state.HSES_SAVE_BACKLOG

    داخلہ_انحراف = get_داخلہ_انحراف(hse)
    انتہا = داخلہ_انحراف + msi
    آغاز = انتہا - کھڑکی_لمبائی

    max_possible = lookahead_sz اگر hse.داخلہ_سائز - msi >= lookahead_sz ورنہ hse.داخلہ_سائز - msi

    مطابقت_pos, مطابقت_لمبائی = ڈھونڈو_longest_مطابقت(hse, آغاز, انتہا, max_possible)

    اگر مطابقت_pos == مطابقت_NOT_ملا:
        hse.مطابقت_جائزہ_اشاریہ += 1
        hse.مطابقت_لمبائی = 0
        واپس HSE_state.HSES_YIELD_لیبل_ثنائی_ہندسہ
    ورنہ:
        hse.مطابقت_pos = مطابقت_pos
        hse.مطابقت_لمبائی = مطابقت_لمبائی
        واپس HSE_state.HSES_YIELD_لیبل_ثنائی_ہندسہ


تعریف ڈھونڈو_longest_مطابقت(hse, آغاز, انتہا, maxlen):
    buf = hse.بفر_2

    مطابقت_maxlen = 0
    مطابقت_اشاریہ = مطابقت_NOT_ملا

    needlepoint = انتہا

    pos = hse.تلاش_اشاریہ.اشاریہ[انتہا]
    buf_needlepoint_maxlen = buf[needlepoint + مطابقت_maxlen]
    جب pos >= آغاز:

        اگر buf[pos + مطابقت_maxlen] != buf_needlepoint_maxlen:
            pos = hse.تلاش_اشاریہ.اشاریہ[pos]
            جاری

        لمبائی_2 = 1
        برائے لمبائی_2 میں سلسلہ(1, maxlen):
            اگر buf[pos + لمبائی_2] != buf[needlepoint + لمبائی_2]:
                توڑو
        اگر لمبائی_2 > مطابقت_maxlen:
            مطابقت_maxlen = لمبائی_2
            مطابقت_اشاریہ = pos
            buf_needlepoint_maxlen = buf[needlepoint + مطابقت_maxlen]
            اگر لمبائی_2 == maxlen:
                توڑو  # won't find better

        pos = hse.تلاش_اشاریہ.اشاریہ[pos]

    break_even_نقطہ = 1 + hse.کھڑکی_سائز + hse.lookahead_سائز

    اگر مطابقت_maxlen > (break_even_نقطہ // 8):
        واپس انتہا - مطابقت_اشاریہ, مطابقت_maxlen
    واپس مطابقت_NOT_ملا, 0


تعریف ڈالو_outgoing_بٹ(hse, oi):
    اگر hse.outgoing_بٹ_گنتی > 8:
        گنتی = 8
        بٹ = hse.outgoing_بٹ >> (hse.outgoing_بٹ_گنتی - 8)
    ورنہ:
        گنتی = hse.outgoing_بٹ_گنتی
        بٹ = hse.outgoing_بٹ

    اگر گنتی > 0:
        ڈالو_بٹ(hse, گنتی, بٹ, oi)
        hse.outgoing_بٹ_گنتی -= گنتی
    واپس گنتی


تعریف st_yield_لیبل_ثنائی_ہندسہ(hse, oi):
    اگر can_take_byte(oi):
        اگر hse.مطابقت_لمبائی == 0:
            جوڑو_لیبل_ثنائی_ہندسہ(hse, oi, HEATSHRINK_LITERAL_MARKER)
            واپس HSE_state.HSES_YIELD_LITERAL
        ورنہ:
            جوڑو_لیبل_ثنائی_ہندسہ(hse, oi, HEATSHRINK_BACKREF_MARKER)
            hse.outgoing_بٹ = hse.مطابقت_pos - 1
            hse.outgoing_بٹ_گنتی = hse.کھڑکی_سائز
            واپس HSE_state.HSES_YIELD_BR_اشاریہ
    ورنہ:
        واپس HSE_state.HSES_YIELD_لیبل_ثنائی_ہندسہ


تعریف st_yield_literal(hse, oi):
    اگر can_take_byte(oi):
        ڈالو_literal_byte(hse, oi)
        واپس HSE_state.HSES_تلاش
    ورنہ:
        واپس HSE_state.HSES_YIELD_LITERAL


تعریف st_yield_br_اشاریہ(hse, oi):
    اگر can_take_byte(oi):
        اگر ڈالو_outgoing_بٹ(hse, oi) > 0:
            واپس HSE_state.HSES_YIELD_BR_اشاریہ
        ورنہ:
            hse.outgoing_بٹ = hse.مطابقت_لمبائی - 1
            hse.outgoing_بٹ_گنتی = hse.lookahead_سائز
            واپس HSE_state.HSES_YIELD_BR_لمبائی
    ورنہ:
        واپس HSE_state.HSES_YIELD_BR_اشاریہ


تعریف st_yield_br_لمبائی(hse, oi):
    اگر can_take_byte(oi):
        اگر ڈالو_outgoing_بٹ(hse, oi) > 0:
            واپس HSE_state.HSES_YIELD_BR_لمبائی
        ورنہ:
            hse.مطابقت_جائزہ_اشاریہ += hse.مطابقت_لمبائی
            hse.مطابقت_لمبائی = 0
            واپس HSE_state.HSES_تلاش
    ورنہ:
        واپس HSE_state.HSES_YIELD_BR_لمبائی


تعریف st_save_backlog(hse):
    save_backlog(hse)
    واپس HSE_state.HSES_NOT_بھرا


تعریف st_flush_ثنائی_ہندسہ_بفر(hse, oi):
    اگر hse.ثنائی_ہندسہ_اشاریہ == 0x80:
        واپس HSE_state.HSES_مکمل
    وگرنہ_اگر can_take_byte(oi):
        oi.buf[oi.output_سائز[0]] = hse.current_byte
        oi.output_سائز[0] += 1
        واپس HSE_state.HSES_مکمل
    ورنہ:
        واپس HSE_state.HSES_FLUSH_بٹ


تعریف save_backlog(hse):
    داخلہ_buf_sz = get_داخلہ_بفر_سائز(hse)

    msi = hse.مطابقت_جائزہ_اشاریہ

    rem = داخلہ_buf_sz - msi  # unprocessed bytes
    سرکاؤ_sz = داخلہ_buf_sz + rem

    برائے i میں سلسلہ(سرکاؤ_sz):
        hse.بفر_2[i] = hse.بفر_2[داخلہ_buf_sz - rem + i]

    hse.مطابقت_جائزہ_اشاریہ = 0
    hse.داخلہ_سائز -= داخلہ_buf_sz - rem


تعریف encode(ڈیٹا, کھڑکی_سائز, lookahead_سائز):
    hse = heatshrink_encoder(کھڑکی_سائز, lookahead_سائز)
    داخلہ_buf = (ctypes.c_uint8 * لمبائی(ڈیٹا))()
    برائے i, d میں enumerate(ڈیٹا):
        داخلہ_buf[i] = d
    اندر_سائز = لمبائی(داخلہ_buf)

    برآمد_buf_سائز = 4 * اندر_سائز  # set output buffer size a bit larger
    برآمد_buf = (ctypes.c_uint8 * برآمد_buf_سائز)()

    sunk = 0
    ret = []
    جب sunk < اندر_سائز:
        داخلہ_سائز = ctypes.c_سائز_t(اندر_سائز)
        heatshrink_encoder_sink(hse, داخلہ_buf, اندر_سائز - sunk, داخلہ_سائز)
        داخلہ_buf = داخلہ_buf[داخلہ_سائز.قدر:]
        sunk += داخلہ_سائز.قدر
        اگر sunk == اندر_سائز:
            heatshrink_encoder_اختتام(hse)

        poll_res = HSE_poll_res.HSER_POLL_مزید
        جب poll_res == HSE_poll_res.HSER_POLL_مزید:
            output_سائز = (ctypes.c_سائز_t * 1)()
            poll_res = heatshrink_encoder_poll(hse, برآمد_buf, برآمد_buf_سائز, output_سائز)
            ret += list(برآمد_buf)[0:output_سائز[0]]

        اگر sunk == اندر_سائز:
            heatshrink_encoder_اختتام(hse)
    واپس ret
