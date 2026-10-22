ਆਯਾਤ ctypes
ਤਃ enum ਆਯਾਤ Enum


# Note: this implementation directly follows the heatshrink_encoder.c code
# (it's neither expected to be very efficient, nor is it pythonic)

# Enum
ਵਰਗ HSE_state(ctypes.c_int):
    HSES_NOT_ਭਰਿਆ = 0
    HSES_FILLED = 1
    HSES_ਖੋਜ = 2
    HSES_YIELD_ਟੈਗ_ਦੋਆਧਾਰੀ_ਅੰਕ = 3
    HSES_YIELD_LITERAL = 4
    HSES_YIELD_BR_ਸੂਚਕਾਂਕ = 5
    HSES_YIELD_BR_ਲੰਮਾਈ = 6
    HSES_SAVE_BACKLOG = 7
    HSES_FLUSH_ਬਿੱਟ = 8
    HSES_ਪੂਰਾ = 9


# Constants
FLAG_IS_FINISHING = 0x01
ਮੇਲ_NOT_ਮਿਲਿਆ = 0xFFFF

HEATSHRINK_LITERAL_MARKER = 0x01
HEATSHRINK_BACKREF_MARKER = 0x00


# Structs
ਵਰਗ output_info(ctypes.Structure):
    _fields_ = [
        ("buf", ctypes.ਸੂਚਕ(ctypes.c_uint8)),
        ("buf_size", ctypes.c_ਮਾਪ_t),
        ("output_size", ctypes.ਸੂਚਕ(ctypes.c_ਮਾਪ_t))
    ]


# Functions
ਪਰਿਭਾਸ਼ਾ ਜੋੜੋ_ਟੈਗ_ਦੋਆਧਾਰੀ_ਅੰਕ(hse, oi, ਟੈਗ):
    ਧੱਕੋ_ਬਿੱਟ(hse, 1, ਟੈਗ, oi)


ਪਰਿਭਾਸ਼ਾ ਧੱਕੋ_ਬਿੱਟ(hse, ਗਿਣਤੀ_2, ਬਿੱਟ, oi):
    ਪ੍ਰਤਿਜ੍ਞਾ ਗਿਣਤੀ_2 <= 8
    current_byte = ctypes.c_uint8(hse.current_byte)
    ਦੋਆਧਾਰੀ_ਅੰਕ_ਸੂਚਕਾਂਕ = ctypes.c_uint8(hse.ਦੋਆਧਾਰੀ_ਅੰਕ_ਸੂਚਕਾਂਕ)
    ਲਈ i ਵਿੱਚ ਸੀਮਾ(ਗਿਣਤੀ_2 - 1, -1, -1):
        ਦੋਆਧਾਰੀ_ਅੰਕ = ਬਿੱਟ & (1 << i)
        ਜੇ ਦੋਆਧਾਰੀ_ਅੰਕ:
            current_byte.ਮੁੱਲ |= ਦੋਆਧਾਰੀ_ਅੰਕ_ਸੂਚਕਾਂਕ.ਮੁੱਲ
        ਦੋਆਧਾਰੀ_ਅੰਕ_ਸੂਚਕਾਂਕ.ਮੁੱਲ >>= 1
        ਜੇ ਦੋਆਧਾਰੀ_ਅੰਕ_ਸੂਚਕਾਂਕ.ਮੁੱਲ == 0:
            ਦੋਆਧਾਰੀ_ਅੰਕ_ਸੂਚਕਾਂਕ.ਮੁੱਲ = 0x80
            oi.buf[oi.output_ਮਾਪ[0]] = current_byte.ਮੁੱਲ
            oi.output_ਮਾਪ[0] += 1
            current_byte.ਮੁੱਲ = 0
    hse.current_byte = current_byte.ਮੁੱਲ
    hse.ਦੋਆਧਾਰੀ_ਅੰਕ_ਸੂਚਕਾਂਕ = ਦੋਆਧਾਰੀ_ਅੰਕ_ਸੂਚਕਾਂਕ.ਮੁੱਲ


ਪਰਿਭਾਸ਼ਾ ਧੱਕੋ_literal_byte(hse, oi):
    processed_ਵਿੱਥ = hse.ਮੇਲ_ਜਾਂਚ_ਸੂਚਕਾਂਕ - 1
    ਨਿਵੇਸ਼_ਵਿੱਥ = get_ਨਿਵੇਸ਼_ਵਿੱਥ(hse) + processed_ਵਿੱਥ
    c = hse.ਬਫਰ_2[ਨਿਵੇਸ਼_ਵਿੱਥ]
    ਧੱਕੋ_ਬਿੱਟ(hse, 8, c, oi)


# Define necessary structures and enums
ਵਰਗ heatshrink_encoder(ctypes.Structure):
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

        ("buffer", ctypes.ਸੂਚਕ(ctypes.c_uint8)),
    ]

    ਪਰਿਭਾਸ਼ਾ __init__(self, ਖਿੜਕੀ_ਮਾਪ=8, lookahead_ਮਾਪ=4):
        ਪਰਮ().__init__()
        self.ਖਿੜਕੀ_ਮਾਪ = ਖਿੜਕੀ_ਮਾਪ
        self.lookahead_ਮਾਪ = lookahead_ਮਾਪ
        self.ਖੋਜ_ਸੂਚਕਾਂਕ = hs_ਸੂਚਕਾਂਕ(ਖਿੜਕੀ_ਮਾਪ)
        self.ਬਫਰ_2 = (ctypes.c_uint8 * (2 << ਖਿੜਕੀ_ਮਾਪ))()
        self.ਦੋਆਧਾਰੀ_ਅੰਕ_ਸੂਚਕਾਂਕ = 0x80


ਵਰਗ hs_ਸੂਚਕਾਂਕ(ctypes.Structure):
    _fields_ = [("index", ctypes.ਸੂਚਕ(ctypes.c_int16))]

    ਪਰਿਭਾਸ਼ਾ __init__(self, ਖਿੜਕੀ_ਮਾਪ):
        ਪਰਮ().__init__()
        self.ਸੂਚਕਾਂਕ = (ctypes.c_int16 * (2 << ਖਿੜਕੀ_ਮਾਪ))()


ਵਰਗ HSE_sink_res(Enum):
    HSER_SINK_OK = 0
    HSER_SINK_ERROR_NULL = -1
    HSER_SINK_ERROR_MISUSE = -2


ਵਰਗ HSE_poll_res(Enum):
    HSER_POLL_ਖਾਲੀ_ਥਾਵਾਂ = 0
    HSER_POLL_ਹੋਰ = 1
    HSER_POLL_ERROR_NULL = -1
    HSER_POLL_ERROR_MISUSE = -2


ਵਰਗ HSE_ਅੰਤ_res(Enum):
    HSER_ਅੰਤ_ਪੂਰਾ = 0
    HSER_ਅੰਤ_ਹੋਰ = 1
    HSER_ਅੰਤ_ERROR_NULL = -1


ਪਰਿਭਾਸ਼ਾ is_finishing(hse):
    ਵਾਪਸ hse.flags & FLAG_IS_FINISHING


ਪਰਿਭਾਸ਼ਾ can_take_byte(oi):
    ਵਾਪਸ oi.output_ਮਾਪ[0] < oi.buf_ਮਾਪ


ਪਰਿਭਾਸ਼ਾ get_ਨਿਵੇਸ਼_ਬਫਰ_ਮਾਪ(hse):
    ਵਾਪਸ 1 << hse.ਖਿੜਕੀ_ਮਾਪ


ਪਰਿਭਾਸ਼ਾ get_lookahead_ਮਾਪ(hse):
    ਵਾਪਸ 1 << hse.lookahead_ਮਾਪ


ਪਰਿਭਾਸ਼ਾ get_ਨਿਵੇਸ਼_ਵਿੱਥ(hse):
    ਵਾਪਸ get_ਨਿਵੇਸ਼_ਬਫਰ_ਮਾਪ(hse)


ਪਰਿਭਾਸ਼ਾ heatshrink_encoder_sink(hse, ਵਿੱਚ_buf, ਮਾਪ, ਨਿਵੇਸ਼_ਮਾਪ):
    ਜੇ hse ਹੈ ਕੁਝ_ਨਹੀਂ ਜਾਂ ਵਿੱਚ_buf ਹੈ ਕੁਝ_ਨਹੀਂ ਜਾਂ ਨਿਵੇਸ਼_ਮਾਪ ਹੈ ਕੁਝ_ਨਹੀਂ:
        ਵਾਪਸ HSE_sink_res.HSER_SINK_ERROR_NULL

    ਜੇ is_finishing(hse):
        ਵਾਪਸ HSE_sink_res.HSER_SINK_ERROR_MISUSE

    ਜੇ hse.state != HSE_state.HSES_NOT_ਭਰਿਆ:
        ਵਾਪਸ HSE_sink_res.HSER_SINK_ERROR_MISUSE

    write_ਵਿੱਥ_2 = get_ਨਿਵੇਸ਼_ਵਿੱਥ(hse) + hse.ਨਿਵੇਸ਼_ਮਾਪ
    ibs = get_ਨਿਵੇਸ਼_ਬਫਰ_ਮਾਪ(hse)
    rem = ibs - hse.ਨਿਵੇਸ਼_ਮਾਪ
    cp_sz = min(rem, ਮਾਪ)

    ਲਈ i ਵਿੱਚ ਸੀਮਾ(cp_sz):
        hse.ਬਫਰ_2[write_ਵਿੱਥ_2 + i] = ਵਿੱਚ_buf[i]
    ਨਿਵੇਸ਼_ਮਾਪ.ਮੁੱਲ = cp_sz
    hse.ਨਿਵੇਸ਼_ਮਾਪ += cp_sz

    ਜੇ cp_sz == rem:
        hse.state = HSE_state.HSES_FILLED

    ਵਾਪਸ HSE_sink_res.HSER_SINK_OK


ਪਰਿਭਾਸ਼ਾ do_indexing(hse):
    # Build an index array I that contains flattened linked lists
    # for the previous instances of every byte in the buffer.

    hsi = hse.ਖੋਜ_ਸੂਚਕਾਂਕ
    ਆਖਰੀ = [0xffff] * 256
    buf = hse.ਬਫਰ_2
    ਸੂਚਕਾਂਕ = hsi.ਸੂਚਕਾਂਕ
    ਨਿਵੇਸ਼_ਵਿੱਥ = get_ਨਿਵੇਸ਼_ਵਿੱਥ(hse)
    ਸਿਰਾ = ਨਿਵੇਸ਼_ਵਿੱਥ + hse.ਨਿਵੇਸ਼_ਮਾਪ

    ਲਈ i ਵਿੱਚ ਸੀਮਾ(0, ਸਿਰਾ):
        v = buf[i]
        lv = ਆਖਰੀ[v]
        ਸੂਚਕਾਂਕ[i] = lv
        ਆਖਰੀ[v] = i


ਪਰਿਭਾਸ਼ਾ heatshrink_encoder_poll(hse, ਬਾਹਰਲਾ_buf, ਬਾਹਰਲਾ_buf_ਮਾਪ, output_ਮਾਪ):
    ਜੇ hse ਹੈ ਕੁਝ_ਨਹੀਂ ਜਾਂ ਬਾਹਰਲਾ_buf ਹੈ ਕੁਝ_ਨਹੀਂ ਜਾਂ output_ਮਾਪ ਹੈ ਕੁਝ_ਨਹੀਂ:
        ਵਾਪਸ HSE_poll_res.HSER_POLL_ERROR_NULL

    ਜੇ ਬਾਹਰਲਾ_buf_ਮਾਪ == 0:
        ਵਾਪਸ HSE_poll_res.HSER_POLL_ERROR_MISUSE

    output_ਮਾਪ[0] = 0
    oi = output_info()
    oi.buf = ਬਾਹਰਲਾ_buf
    oi.buf_ਮਾਪ = ਬਾਹਰਲਾ_buf_ਮਾਪ
    oi.output_ਮਾਪ = output_ਮਾਪ

    ਜਦੋਂ ਸੱਚ:
        ਵਿੱਚ_state = hse.state
        ਜੇ ਵਿੱਚ_state == HSE_state.HSES_NOT_ਭਰਿਆ:
            ਵਾਪਸ HSE_poll_res.HSER_POLL_ਖਾਲੀ_ਥਾਵਾਂ
        ਜਾਂਜੇ ਵਿੱਚ_state == HSE_state.HSES_ਪੂਰਾ:
            ਵਾਪਸ HSE_poll_res.HSER_POLL_ਖਾਲੀ_ਥਾਵਾਂ
        ਜਾਂਜੇ ਵਿੱਚ_state == HSE_state.HSES_FILLED:
            do_indexing(hse)
            hse.state = HSE_state.HSES_ਖੋਜ
        ਜਾਂਜੇ ਵਿੱਚ_state == HSE_state.HSES_ਖੋਜ:
            hse.state = st_ਕਦਮ_ਖੋਜ(hse)
        ਜਾਂਜੇ ਵਿੱਚ_state == HSE_state.HSES_YIELD_ਟੈਗ_ਦੋਆਧਾਰੀ_ਅੰਕ:
            hse.state = st_yield_ਟੈਗ_ਦੋਆਧਾਰੀ_ਅੰਕ(hse, oi)
        ਜਾਂਜੇ ਵਿੱਚ_state == HSE_state.HSES_YIELD_LITERAL:
            hse.state = st_yield_literal(hse, oi)
        ਜਾਂਜੇ ਵਿੱਚ_state == HSE_state.HSES_YIELD_BR_ਸੂਚਕਾਂਕ:
            hse.state = st_yield_br_ਸੂਚਕਾਂਕ(hse, oi)
        ਜਾਂਜੇ ਵਿੱਚ_state == HSE_state.HSES_YIELD_BR_ਲੰਮਾਈ:
            hse.state = st_yield_br_ਲੰਮਾਈ(hse, oi)
        ਜਾਂਜੇ ਵਿੱਚ_state == HSE_state.HSES_SAVE_BACKLOG:
            hse.state = st_save_backlog(hse)
        ਜਾਂਜੇ ਵਿੱਚ_state == HSE_state.HSES_FLUSH_ਬਿੱਟ:
            hse.state = st_flush_ਦੋਆਧਾਰੀ_ਅੰਕ_ਬਫਰ(hse, oi)
        ਨਹੀਂਤਾਂ:
            ਵਾਪਸ HSE_poll_res.HSER_POLL_ERROR_MISUSE

        ਜੇ hse.state == ਵਿੱਚ_state:
            ਜੇ oi.output_ਮਾਪ == oi.buf_ਮਾਪ:
                ਵਾਪਸ HSE_poll_res.HSER_POLL_ਹੋਰ


ਪਰਿਭਾਸ਼ਾ heatshrink_encoder_ਅੰਤ(hse):
    hse.flags |= FLAG_IS_FINISHING
    ਜੇ hse.state == HSE_state.HSES_NOT_ਭਰਿਆ:
        hse.state = HSE_state.HSES_FILLED
    ਜੇ hse.state == HSE_state.HSES_ਪੂਰਾ:
        ਵਾਪਸ HSE_ਅੰਤ_res.HSER_ਅੰਤ_ਪੂਰਾ
    ਵਾਪਸ HSE_ਅੰਤ_res.HSER_ਅੰਤ_ਹੋਰ


ਪਰਿਭਾਸ਼ਾ st_ਕਦਮ_ਖੋਜ(hse):
    ਖਿੜਕੀ_ਲੰਮਾਈ = get_ਨਿਵੇਸ਼_ਬਫਰ_ਮਾਪ(hse)
    lookahead_sz = get_lookahead_ਮਾਪ(hse)
    msi = hse.ਮੇਲ_ਜਾਂਚ_ਸੂਚਕਾਂਕ

    fin = is_finishing(hse)
    ਜੇ msi > hse.ਨਿਵੇਸ਼_ਮਾਪ - (1 ਜੇ fin ਨਹੀਂਤਾਂ lookahead_sz):
        ਵਾਪਸ HSE_state.HSES_FLUSH_ਬਿੱਟ ਜੇ fin ਨਹੀਂਤਾਂ HSE_state.HSES_SAVE_BACKLOG

    ਨਿਵੇਸ਼_ਵਿੱਥ = get_ਨਿਵੇਸ਼_ਵਿੱਥ(hse)
    ਸਿਰਾ = ਨਿਵੇਸ਼_ਵਿੱਥ + msi
    ਸ਼ੁਰੂ = ਸਿਰਾ - ਖਿੜਕੀ_ਲੰਮਾਈ

    max_possible = lookahead_sz ਜੇ hse.ਨਿਵੇਸ਼_ਮਾਪ - msi >= lookahead_sz ਨਹੀਂਤਾਂ hse.ਨਿਵੇਸ਼_ਮਾਪ - msi

    ਮੇਲ_pos, ਮੇਲ_ਲੰਮਾਈ = ਲੱਭੋ_longest_ਮੇਲ(hse, ਸ਼ੁਰੂ, ਸਿਰਾ, max_possible)

    ਜੇ ਮੇਲ_pos == ਮੇਲ_NOT_ਮਿਲਿਆ:
        hse.ਮੇਲ_ਜਾਂਚ_ਸੂਚਕਾਂਕ += 1
        hse.ਮੇਲ_ਲੰਮਾਈ = 0
        ਵਾਪਸ HSE_state.HSES_YIELD_ਟੈਗ_ਦੋਆਧਾਰੀ_ਅੰਕ
    ਨਹੀਂਤਾਂ:
        hse.ਮੇਲ_pos = ਮੇਲ_pos
        hse.ਮੇਲ_ਲੰਮਾਈ = ਮੇਲ_ਲੰਮਾਈ
        ਵਾਪਸ HSE_state.HSES_YIELD_ਟੈਗ_ਦੋਆਧਾਰੀ_ਅੰਕ


ਪਰਿਭਾਸ਼ਾ ਲੱਭੋ_longest_ਮੇਲ(hse, ਸ਼ੁਰੂ, ਸਿਰਾ, maxlen):
    buf = hse.ਬਫਰ_2

    ਮੇਲ_maxlen = 0
    ਮੇਲ_ਸੂਚਕਾਂਕ = ਮੇਲ_NOT_ਮਿਲਿਆ

    needlepoint = ਸਿਰਾ

    pos = hse.ਖੋਜ_ਸੂਚਕਾਂਕ.ਸੂਚਕਾਂਕ[ਸਿਰਾ]
    buf_needlepoint_maxlen = buf[needlepoint + ਮੇਲ_maxlen]
    ਜਦੋਂ pos >= ਸ਼ੁਰੂ:

        ਜੇ buf[pos + ਮੇਲ_maxlen] != buf_needlepoint_maxlen:
            pos = hse.ਖੋਜ_ਸੂਚਕਾਂਕ.ਸੂਚਕਾਂਕ[pos]
            ਜਾਰੀ

        ਲੰਮਾਈ = 1
        ਲਈ ਲੰਮਾਈ ਵਿੱਚ ਸੀਮਾ(1, maxlen):
            ਜੇ buf[pos + ਲੰਮਾਈ] != buf[needlepoint + ਲੰਮਾਈ]:
                ਰੋਕੋ
        ਜੇ ਲੰਮਾਈ > ਮੇਲ_maxlen:
            ਮੇਲ_maxlen = ਲੰਮਾਈ
            ਮੇਲ_ਸੂਚਕਾਂਕ = pos
            buf_needlepoint_maxlen = buf[needlepoint + ਮੇਲ_maxlen]
            ਜੇ ਲੰਮਾਈ == maxlen:
                ਰੋਕੋ  # won't find better

        pos = hse.ਖੋਜ_ਸੂਚਕਾਂਕ.ਸੂਚਕਾਂਕ[pos]

    break_even_ਬਿੰਦੂ = 1 + hse.ਖਿੜਕੀ_ਮਾਪ + hse.lookahead_ਮਾਪ

    ਜੇ ਮੇਲ_maxlen > (break_even_ਬਿੰਦੂ // 8):
        ਵਾਪਸ ਸਿਰਾ - ਮੇਲ_ਸੂਚਕਾਂਕ, ਮੇਲ_maxlen
    ਵਾਪਸ ਮੇਲ_NOT_ਮਿਲਿਆ, 0


ਪਰਿਭਾਸ਼ਾ ਧੱਕੋ_outgoing_ਬਿੱਟ(hse, oi):
    ਜੇ hse.outgoing_ਬਿੱਟ_ਗਿਣਤੀ > 8:
        ਗਿਣਤੀ_2 = 8
        ਬਿੱਟ = hse.outgoing_ਬਿੱਟ >> (hse.outgoing_ਬਿੱਟ_ਗਿਣਤੀ - 8)
    ਨਹੀਂਤਾਂ:
        ਗਿਣਤੀ_2 = hse.outgoing_ਬਿੱਟ_ਗਿਣਤੀ
        ਬਿੱਟ = hse.outgoing_ਬਿੱਟ

    ਜੇ ਗਿਣਤੀ_2 > 0:
        ਧੱਕੋ_ਬਿੱਟ(hse, ਗਿਣਤੀ_2, ਬਿੱਟ, oi)
        hse.outgoing_ਬਿੱਟ_ਗਿਣਤੀ -= ਗਿਣਤੀ_2
    ਵਾਪਸ ਗਿਣਤੀ_2


ਪਰਿਭਾਸ਼ਾ st_yield_ਟੈਗ_ਦੋਆਧਾਰੀ_ਅੰਕ(hse, oi):
    ਜੇ can_take_byte(oi):
        ਜੇ hse.ਮੇਲ_ਲੰਮਾਈ == 0:
            ਜੋੜੋ_ਟੈਗ_ਦੋਆਧਾਰੀ_ਅੰਕ(hse, oi, HEATSHRINK_LITERAL_MARKER)
            ਵਾਪਸ HSE_state.HSES_YIELD_LITERAL
        ਨਹੀਂਤਾਂ:
            ਜੋੜੋ_ਟੈਗ_ਦੋਆਧਾਰੀ_ਅੰਕ(hse, oi, HEATSHRINK_BACKREF_MARKER)
            hse.outgoing_ਬਿੱਟ = hse.ਮੇਲ_pos - 1
            hse.outgoing_ਬਿੱਟ_ਗਿਣਤੀ = hse.ਖਿੜਕੀ_ਮਾਪ
            ਵਾਪਸ HSE_state.HSES_YIELD_BR_ਸੂਚਕਾਂਕ
    ਨਹੀਂਤਾਂ:
        ਵਾਪਸ HSE_state.HSES_YIELD_ਟੈਗ_ਦੋਆਧਾਰੀ_ਅੰਕ


ਪਰਿਭਾਸ਼ਾ st_yield_literal(hse, oi):
    ਜੇ can_take_byte(oi):
        ਧੱਕੋ_literal_byte(hse, oi)
        ਵਾਪਸ HSE_state.HSES_ਖੋਜ
    ਨਹੀਂਤਾਂ:
        ਵਾਪਸ HSE_state.HSES_YIELD_LITERAL


ਪਰਿਭਾਸ਼ਾ st_yield_br_ਸੂਚਕਾਂਕ(hse, oi):
    ਜੇ can_take_byte(oi):
        ਜੇ ਧੱਕੋ_outgoing_ਬਿੱਟ(hse, oi) > 0:
            ਵਾਪਸ HSE_state.HSES_YIELD_BR_ਸੂਚਕਾਂਕ
        ਨਹੀਂਤਾਂ:
            hse.outgoing_ਬਿੱਟ = hse.ਮੇਲ_ਲੰਮਾਈ - 1
            hse.outgoing_ਬਿੱਟ_ਗਿਣਤੀ = hse.lookahead_ਮਾਪ
            ਵਾਪਸ HSE_state.HSES_YIELD_BR_ਲੰਮਾਈ
    ਨਹੀਂਤਾਂ:
        ਵਾਪਸ HSE_state.HSES_YIELD_BR_ਸੂਚਕਾਂਕ


ਪਰਿਭਾਸ਼ਾ st_yield_br_ਲੰਮਾਈ(hse, oi):
    ਜੇ can_take_byte(oi):
        ਜੇ ਧੱਕੋ_outgoing_ਬਿੱਟ(hse, oi) > 0:
            ਵਾਪਸ HSE_state.HSES_YIELD_BR_ਲੰਮਾਈ
        ਨਹੀਂਤਾਂ:
            hse.ਮੇਲ_ਜਾਂਚ_ਸੂਚਕਾਂਕ += hse.ਮੇਲ_ਲੰਮਾਈ
            hse.ਮੇਲ_ਲੰਮਾਈ = 0
            ਵਾਪਸ HSE_state.HSES_ਖੋਜ
    ਨਹੀਂਤਾਂ:
        ਵਾਪਸ HSE_state.HSES_YIELD_BR_ਲੰਮਾਈ


ਪਰਿਭਾਸ਼ਾ st_save_backlog(hse):
    save_backlog(hse)
    ਵਾਪਸ HSE_state.HSES_NOT_ਭਰਿਆ


ਪਰਿਭਾਸ਼ਾ st_flush_ਦੋਆਧਾਰੀ_ਅੰਕ_ਬਫਰ(hse, oi):
    ਜੇ hse.ਦੋਆਧਾਰੀ_ਅੰਕ_ਸੂਚਕਾਂਕ == 0x80:
        ਵਾਪਸ HSE_state.HSES_ਪੂਰਾ
    ਜਾਂਜੇ can_take_byte(oi):
        oi.buf[oi.output_ਮਾਪ[0]] = hse.current_byte
        oi.output_ਮਾਪ[0] += 1
        ਵਾਪਸ HSE_state.HSES_ਪੂਰਾ
    ਨਹੀਂਤਾਂ:
        ਵਾਪਸ HSE_state.HSES_FLUSH_ਬਿੱਟ


ਪਰਿਭਾਸ਼ਾ save_backlog(hse):
    ਨਿਵੇਸ਼_buf_sz = get_ਨਿਵੇਸ਼_ਬਫਰ_ਮਾਪ(hse)

    msi = hse.ਮੇਲ_ਜਾਂਚ_ਸੂਚਕਾਂਕ

    rem = ਨਿਵੇਸ਼_buf_sz - msi  # unprocessed bytes
    ਖਿਸਕਾਓ_sz = ਨਿਵੇਸ਼_buf_sz + rem

    ਲਈ i ਵਿੱਚ ਸੀਮਾ(ਖਿਸਕਾਓ_sz):
        hse.ਬਫਰ_2[i] = hse.ਬਫਰ_2[ਨਿਵੇਸ਼_buf_sz - rem + i]

    hse.ਮੇਲ_ਜਾਂਚ_ਸੂਚਕਾਂਕ = 0
    hse.ਨਿਵੇਸ਼_ਮਾਪ -= ਨਿਵੇਸ਼_buf_sz - rem


ਪਰਿਭਾਸ਼ਾ encode(ਅੰਕੜੇ, ਖਿੜਕੀ_ਮਾਪ, lookahead_ਮਾਪ):
    hse = heatshrink_encoder(ਖਿੜਕੀ_ਮਾਪ, lookahead_ਮਾਪ)
    ਨਿਵੇਸ਼_buf = (ctypes.c_uint8 * ਲੰਬਾਈ(ਅੰਕੜੇ))()
    ਲਈ i, d ਵਿੱਚ enumerate(ਅੰਕੜੇ):
        ਨਿਵੇਸ਼_buf[i] = d
    ਵਿੱਚ_ਮਾਪ = ਲੰਬਾਈ(ਨਿਵੇਸ਼_buf)

    ਬਾਹਰਲਾ_buf_ਮਾਪ = 4 * ਵਿੱਚ_ਮਾਪ  # set output buffer size a bit larger
    ਬਾਹਰਲਾ_buf = (ctypes.c_uint8 * ਬਾਹਰਲਾ_buf_ਮਾਪ)()

    sunk = 0
    ret = []
    ਜਦੋਂ sunk < ਵਿੱਚ_ਮਾਪ:
        ਨਿਵੇਸ਼_ਮਾਪ = ctypes.c_ਮਾਪ_t(ਵਿੱਚ_ਮਾਪ)
        heatshrink_encoder_sink(hse, ਨਿਵੇਸ਼_buf, ਵਿੱਚ_ਮਾਪ - sunk, ਨਿਵੇਸ਼_ਮਾਪ)
        ਨਿਵੇਸ਼_buf = ਨਿਵੇਸ਼_buf[ਨਿਵੇਸ਼_ਮਾਪ.ਮੁੱਲ:]
        sunk += ਨਿਵੇਸ਼_ਮਾਪ.ਮੁੱਲ
        ਜੇ sunk == ਵਿੱਚ_ਮਾਪ:
            heatshrink_encoder_ਅੰਤ(hse)

        poll_res = HSE_poll_res.HSER_POLL_ਹੋਰ
        ਜਦੋਂ poll_res == HSE_poll_res.HSER_POLL_ਹੋਰ:
            output_ਮਾਪ = (ctypes.c_ਮਾਪ_t * 1)()
            poll_res = heatshrink_encoder_poll(hse, ਬਾਹਰਲਾ_buf, ਬਾਹਰਲਾ_buf_ਮਾਪ, output_ਮਾਪ)
            ret += list(ਬਾਹਰਲਾ_buf)[0:output_ਮਾਪ[0]]

        ਜੇ sunk == ਵਿੱਚ_ਮਾਪ:
            heatshrink_encoder_ਅੰਤ(hse)
    ਵਾਪਸ ret
