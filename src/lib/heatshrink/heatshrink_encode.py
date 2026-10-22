ಆಮದು ctypes
ತಃ enum ಆಮದು Enum


# Note: this implementation directly follows the heatshrink_encoder.c code
# (it's neither expected to be very efficient, nor is it pythonic)

# Enum
ವರ್ಗ HSE_state(ctypes.c_int):
    HSES_NOT_ತುಂಬಿದ = 0
    HSES_FILLED = 1
    HSES_ಹುಡುಕು = 2
    HSES_YIELD_ಟ್ಯಾಗ್_ಬಿಟ್ = 3
    HSES_YIELD_LITERAL = 4
    HSES_YIELD_BR_ಸೂಚ್ಯಂಕ = 5
    HSES_YIELD_BR_ಉದ್ದಳತೆ = 6
    HSES_SAVE_BACKLOG = 7
    HSES_FLUSH_ಬಿಟ್ಟುಗಳು = 8
    HSES_ಮುಗಿದ = 9


# Constants
FLAG_IS_FINISHING = 0x01
ಹೊಂದಿಕೆ_NOT_ಸಿಕ್ಕಿತು = 0xFFFF

HEATSHRINK_LITERAL_MARKER = 0x01
HEATSHRINK_BACKREF_MARKER = 0x00


# Structs
ವರ್ಗ output_info(ctypes.Structure):
    _fields_ = [
        ("buf", ctypes.ಸೂಚಕ(ctypes.c_uint8)),
        ("buf_size", ctypes.c_ಗಾತ್ರ_t),
        ("output_size", ctypes.ಸೂಚಕ(ctypes.c_ಗಾತ್ರ_t))
    ]


# Functions
ವ್ಯಾಖ್ಯಾನ ಕೂಡಿಸು_ಟ್ಯಾಗ್_ಬಿಟ್(hse, oi, ಟ್ಯಾಗ್):
    ತಳ್ಳು_ಬಿಟ್ಟುಗಳು(hse, 1, ಟ್ಯಾಗ್, oi)


ವ್ಯಾಖ್ಯಾನ ತಳ್ಳು_ಬಿಟ್ಟುಗಳು(hse, ಎಣಿಕೆ_2, ಬಿಟ್ಟುಗಳು, oi):
    ಪ್ರತಿಜ್ಞಾ ಎಣಿಕೆ_2 <= 8
    current_byte = ctypes.c_uint8(hse.current_byte)
    ಬಿಟ್_ಸೂಚ್ಯಂಕ = ctypes.c_uint8(hse.ಬಿಟ್_ಸೂಚ್ಯಂಕ)
    ಗಾಗಿ i ಒಳಗೆ ಶ್ರೇಣಿ(ಎಣಿಕೆ_2 - 1, -1, -1):
        ಬಿಟ್ = ಬಿಟ್ಟುಗಳು & (1 << i)
        ಆದರೆ ಬಿಟ್:
            current_byte.ಮೌಲ್ಯ |= ಬಿಟ್_ಸೂಚ್ಯಂಕ.ಮೌಲ್ಯ
        ಬಿಟ್_ಸೂಚ್ಯಂಕ.ಮೌಲ್ಯ >>= 1
        ಆದರೆ ಬಿಟ್_ಸೂಚ್ಯಂಕ.ಮೌಲ್ಯ == 0:
            ಬಿಟ್_ಸೂಚ್ಯಂಕ.ಮೌಲ್ಯ = 0x80
            oi.buf[oi.output_ಗಾತ್ರ[0]] = current_byte.ಮೌಲ್ಯ
            oi.output_ಗಾತ್ರ[0] += 1
            current_byte.ಮೌಲ್ಯ = 0
    hse.current_byte = current_byte.ಮೌಲ್ಯ
    hse.ಬಿಟ್_ಸೂಚ್ಯಂಕ = ಬಿಟ್_ಸೂಚ್ಯಂಕ.ಮೌಲ್ಯ


ವ್ಯಾಖ್ಯಾನ ತಳ್ಳು_literal_byte(hse, oi):
    processed_ಸ್ಥಾನಾಂತರ = hse.ಹೊಂದಿಕೆ_ಪರಿಶೀಲನೆ_ಸೂಚ್ಯಂಕ - 1
    ಒಳಹರಿವು_ಸ್ಥಾನಾಂತರ = get_ಒಳಹರಿವು_ಸ್ಥಾನಾಂತರ(hse) + processed_ಸ್ಥಾನಾಂತರ
    c = hse.ಬಫರ್_2[ಒಳಹರಿವು_ಸ್ಥಾನಾಂತರ]
    ತಳ್ಳು_ಬಿಟ್ಟುಗಳು(hse, 8, c, oi)


# Define necessary structures and enums
ವರ್ಗ heatshrink_encoder(ctypes.Structure):
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

        ("buffer", ctypes.ಸೂಚಕ(ctypes.c_uint8)),
    ]

    ವ್ಯಾಖ್ಯಾನ __init__(self, ಕಿಟಕಿ_ಗಾತ್ರ=8, lookahead_ಗಾತ್ರ=4):
        ಪರ().__init__()
        self.ಕಿಟಕಿ_ಗಾತ್ರ = ಕಿಟಕಿ_ಗಾತ್ರ
        self.lookahead_ಗಾತ್ರ = lookahead_ಗಾತ್ರ
        self.ಹುಡುಕು_ಸೂಚ್ಯಂಕ = hs_ಸೂಚ್ಯಂಕ(ಕಿಟಕಿ_ಗಾತ್ರ)
        self.ಬಫರ್_2 = (ctypes.c_uint8 * (2 << ಕಿಟಕಿ_ಗಾತ್ರ))()
        self.ಬಿಟ್_ಸೂಚ್ಯಂಕ = 0x80


ವರ್ಗ hs_ಸೂಚ್ಯಂಕ(ctypes.Structure):
    _fields_ = [("index", ctypes.ಸೂಚಕ(ctypes.c_int16))]

    ವ್ಯಾಖ್ಯಾನ __init__(self, ಕಿಟಕಿ_ಗಾತ್ರ):
        ಪರ().__init__()
        self.ಸೂಚ್ಯಂಕ = (ctypes.c_int16 * (2 << ಕಿಟಕಿ_ಗಾತ್ರ))()


ವರ್ಗ HSE_sink_res(Enum):
    HSER_SINK_OK = 0
    HSER_SINK_ERROR_NULL = -1
    HSER_SINK_ERROR_MISUSE = -2


ವರ್ಗ HSE_poll_res(Enum):
    HSER_POLL_ಖಾಲಿಗಳು = 0
    HSER_POLL_ಇನ್ನಷ್ಟು = 1
    HSER_POLL_ERROR_NULL = -1
    HSER_POLL_ERROR_MISUSE = -2


ವರ್ಗ HSE_ಮುಕ್ತಾಯ_res(Enum):
    HSER_ಮುಕ್ತಾಯ_ಮುಗಿದ = 0
    HSER_ಮುಕ್ತಾಯ_ಇನ್ನಷ್ಟು = 1
    HSER_ಮುಕ್ತಾಯ_ERROR_NULL = -1


ವ್ಯಾಖ್ಯಾನ is_finishing(hse):
    ಹಿಂತಿರುಗಿ hse.flags & FLAG_IS_FINISHING


ವ್ಯಾಖ್ಯಾನ can_take_byte(oi):
    ಹಿಂತಿರುಗಿ oi.output_ಗಾತ್ರ[0] < oi.buf_ಗಾತ್ರ


ವ್ಯಾಖ್ಯಾನ get_ಒಳಹರಿವು_ಬಫರ್_ಗಾತ್ರ(hse):
    ಹಿಂತಿರುಗಿ 1 << hse.ಕಿಟಕಿ_ಗಾತ್ರ


ವ್ಯಾಖ್ಯಾನ get_lookahead_ಗಾತ್ರ(hse):
    ಹಿಂತಿರುಗಿ 1 << hse.lookahead_ಗಾತ್ರ


ವ್ಯಾಖ್ಯಾನ get_ಒಳಹರಿವು_ಸ್ಥಾನಾಂತರ(hse):
    ಹಿಂತಿರುಗಿ get_ಒಳಹರಿವು_ಬಫರ್_ಗಾತ್ರ(hse)


ವ್ಯಾಖ್ಯಾನ heatshrink_encoder_sink(hse, ಒಳ_buf, ಗಾತ್ರ_2, ಒಳಹರಿವು_ಗಾತ್ರ):
    ಆದರೆ hse ಆಗಿರುವ ಏನೂಇಲ್ಲ ಅಥವಾ ಒಳ_buf ಆಗಿರುವ ಏನೂಇಲ್ಲ ಅಥವಾ ಒಳಹರಿವು_ಗಾತ್ರ ಆಗಿರುವ ಏನೂಇಲ್ಲ:
        ಹಿಂತಿರುಗಿ HSE_sink_res.HSER_SINK_ERROR_NULL

    ಆದರೆ is_finishing(hse):
        ಹಿಂತಿರುಗಿ HSE_sink_res.HSER_SINK_ERROR_MISUSE

    ಆದರೆ hse.state != HSE_state.HSES_NOT_ತುಂಬಿದ:
        ಹಿಂತಿರುಗಿ HSE_sink_res.HSER_SINK_ERROR_MISUSE

    write_ಸ್ಥಾನಾಂತರ_2 = get_ಒಳಹರಿವು_ಸ್ಥಾನಾಂತರ(hse) + hse.ಒಳಹರಿವು_ಗಾತ್ರ
    ibs = get_ಒಳಹರಿವು_ಬಫರ್_ಗಾತ್ರ(hse)
    rem = ibs - hse.ಒಳಹರಿವು_ಗಾತ್ರ
    cp_sz = min(rem, ಗಾತ್ರ_2)

    ಗಾಗಿ i ಒಳಗೆ ಶ್ರೇಣಿ(cp_sz):
        hse.ಬಫರ್_2[write_ಸ್ಥಾನಾಂತರ_2 + i] = ಒಳ_buf[i]
    ಒಳಹರಿವು_ಗಾತ್ರ.ಮೌಲ್ಯ = cp_sz
    hse.ಒಳಹರಿವು_ಗಾತ್ರ += cp_sz

    ಆದರೆ cp_sz == rem:
        hse.state = HSE_state.HSES_FILLED

    ಹಿಂತಿರುಗಿ HSE_sink_res.HSER_SINK_OK


ವ್ಯಾಖ್ಯಾನ do_indexing(hse):
    # Build an index array I that contains flattened linked lists
    # for the previous instances of every byte in the buffer.

    hsi = hse.ಹುಡುಕು_ಸೂಚ್ಯಂಕ
    ಕೊನೆಯ = [0xffff] * 256
    buf = hse.ಬಫರ್_2
    ಸೂಚ್ಯಂಕ = hsi.ಸೂಚ್ಯಂಕ
    ಒಳಹರಿವು_ಸ್ಥಾನಾಂತರ = get_ಒಳಹರಿವು_ಸ್ಥಾನಾಂತರ(hse)
    ಕೊನೆ = ಒಳಹರಿವು_ಸ್ಥಾನಾಂತರ + hse.ಒಳಹರಿವು_ಗಾತ್ರ

    ಗಾಗಿ i ಒಳಗೆ ಶ್ರೇಣಿ(0, ಕೊನೆ):
        v = buf[i]
        lv = ಕೊನೆಯ[v]
        ಸೂಚ್ಯಂಕ[i] = lv
        ಕೊನೆಯ[v] = i


ವ್ಯಾಖ್ಯಾನ heatshrink_encoder_poll(hse, ಹೊರಹೋಗು_buf, ಹೊರಹೋಗು_buf_ಗಾತ್ರ, output_ಗಾತ್ರ):
    ಆದರೆ hse ಆಗಿರುವ ಏನೂಇಲ್ಲ ಅಥವಾ ಹೊರಹೋಗು_buf ಆಗಿರುವ ಏನೂಇಲ್ಲ ಅಥವಾ output_ಗಾತ್ರ ಆಗಿರುವ ಏನೂಇಲ್ಲ:
        ಹಿಂತಿರುಗಿ HSE_poll_res.HSER_POLL_ERROR_NULL

    ಆದರೆ ಹೊರಹೋಗು_buf_ಗಾತ್ರ == 0:
        ಹಿಂತಿರುಗಿ HSE_poll_res.HSER_POLL_ERROR_MISUSE

    output_ಗಾತ್ರ[0] = 0
    oi = output_info()
    oi.buf = ಹೊರಹೋಗು_buf
    oi.buf_ಗಾತ್ರ = ಹೊರಹೋಗು_buf_ಗಾತ್ರ
    oi.output_ಗಾತ್ರ = output_ಗಾತ್ರ

    ಆಗ ನಿಜ:
        ಒಳ_state = hse.state
        ಆದರೆ ಒಳ_state == HSE_state.HSES_NOT_ತುಂಬಿದ:
            ಹಿಂತಿರುಗಿ HSE_poll_res.HSER_POLL_ಖಾಲಿಗಳು
        ಅಥವಾಆದರೆ ಒಳ_state == HSE_state.HSES_ಮುಗಿದ:
            ಹಿಂತಿರುಗಿ HSE_poll_res.HSER_POLL_ಖಾಲಿಗಳು
        ಅಥವಾಆದರೆ ಒಳ_state == HSE_state.HSES_FILLED:
            do_indexing(hse)
            hse.state = HSE_state.HSES_ಹುಡುಕು
        ಅಥವಾಆದರೆ ಒಳ_state == HSE_state.HSES_ಹುಡುಕು:
            hse.state = st_ಹೆಜ್ಜೆ_ಹುಡುಕು(hse)
        ಅಥವಾಆದರೆ ಒಳ_state == HSE_state.HSES_YIELD_ಟ್ಯಾಗ್_ಬಿಟ್:
            hse.state = st_yield_ಟ್ಯಾಗ್_ಬಿಟ್(hse, oi)
        ಅಥವಾಆದರೆ ಒಳ_state == HSE_state.HSES_YIELD_LITERAL:
            hse.state = st_yield_literal(hse, oi)
        ಅಥವಾಆದರೆ ಒಳ_state == HSE_state.HSES_YIELD_BR_ಸೂಚ್ಯಂಕ:
            hse.state = st_yield_br_ಸೂಚ್ಯಂಕ(hse, oi)
        ಅಥವಾಆದರೆ ಒಳ_state == HSE_state.HSES_YIELD_BR_ಉದ್ದಳತೆ:
            hse.state = st_yield_br_ಉದ್ದಳತೆ(hse, oi)
        ಅಥವಾಆದರೆ ಒಳ_state == HSE_state.HSES_SAVE_BACKLOG:
            hse.state = st_save_backlog(hse)
        ಅಥವಾಆದರೆ ಒಳ_state == HSE_state.HSES_FLUSH_ಬಿಟ್ಟುಗಳು:
            hse.state = st_flush_ಬಿಟ್_ಬಫರ್(hse, oi)
        ಇಲ್ಲದಿದ್ದರೆ:
            ಹಿಂತಿರುಗಿ HSE_poll_res.HSER_POLL_ERROR_MISUSE

        ಆದರೆ hse.state == ಒಳ_state:
            ಆದರೆ oi.output_ಗಾತ್ರ == oi.buf_ಗಾತ್ರ:
                ಹಿಂತಿರುಗಿ HSE_poll_res.HSER_POLL_ಇನ್ನಷ್ಟು


ವ್ಯಾಖ್ಯಾನ heatshrink_encoder_ಮುಕ್ತಾಯ(hse):
    hse.flags |= FLAG_IS_FINISHING
    ಆದರೆ hse.state == HSE_state.HSES_NOT_ತುಂಬಿದ:
        hse.state = HSE_state.HSES_FILLED
    ಆದರೆ hse.state == HSE_state.HSES_ಮುಗಿದ:
        ಹಿಂತಿರುಗಿ HSE_ಮುಕ್ತಾಯ_res.HSER_ಮುಕ್ತಾಯ_ಮುಗಿದ
    ಹಿಂತಿರುಗಿ HSE_ಮುಕ್ತಾಯ_res.HSER_ಮುಕ್ತಾಯ_ಇನ್ನಷ್ಟು


ವ್ಯಾಖ್ಯಾನ st_ಹೆಜ್ಜೆ_ಹುಡುಕು(hse):
    ಕಿಟಕಿ_ಉದ್ದಳತೆ = get_ಒಳಹರಿವು_ಬಫರ್_ಗಾತ್ರ(hse)
    lookahead_sz = get_lookahead_ಗಾತ್ರ(hse)
    msi = hse.ಹೊಂದಿಕೆ_ಪರಿಶೀಲನೆ_ಸೂಚ್ಯಂಕ

    fin = is_finishing(hse)
    ಆದರೆ msi > hse.ಒಳಹರಿವು_ಗಾತ್ರ - (1 ಆದರೆ fin ಇಲ್ಲದಿದ್ದರೆ lookahead_sz):
        ಹಿಂತಿರುಗಿ HSE_state.HSES_FLUSH_ಬಿಟ್ಟುಗಳು ಆದರೆ fin ಇಲ್ಲದಿದ್ದರೆ HSE_state.HSES_SAVE_BACKLOG

    ಒಳಹರಿವು_ಸ್ಥಾನಾಂತರ = get_ಒಳಹರಿವು_ಸ್ಥಾನಾಂತರ(hse)
    ಕೊನೆ = ಒಳಹರಿವು_ಸ್ಥಾನಾಂತರ + msi
    ಆರಂಭ = ಕೊನೆ - ಕಿಟಕಿ_ಉದ್ದಳತೆ

    max_possible = lookahead_sz ಆದರೆ hse.ಒಳಹರಿವು_ಗಾತ್ರ - msi >= lookahead_sz ಇಲ್ಲದಿದ್ದರೆ hse.ಒಳಹರಿವು_ಗಾತ್ರ - msi

    ಹೊಂದಿಕೆ_pos, ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ = ಕಂಡುಹಿಡಿ_longest_ಹೊಂದಿಕೆ(hse, ಆರಂಭ, ಕೊನೆ, max_possible)

    ಆದರೆ ಹೊಂದಿಕೆ_pos == ಹೊಂದಿಕೆ_NOT_ಸಿಕ್ಕಿತು:
        hse.ಹೊಂದಿಕೆ_ಪರಿಶೀಲನೆ_ಸೂಚ್ಯಂಕ += 1
        hse.ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ = 0
        ಹಿಂತಿರುಗಿ HSE_state.HSES_YIELD_ಟ್ಯಾಗ್_ಬಿಟ್
    ಇಲ್ಲದಿದ್ದರೆ:
        hse.ಹೊಂದಿಕೆ_pos = ಹೊಂದಿಕೆ_pos
        hse.ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ = ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ
        ಹಿಂತಿರುಗಿ HSE_state.HSES_YIELD_ಟ್ಯಾಗ್_ಬಿಟ್


ವ್ಯಾಖ್ಯಾನ ಕಂಡುಹಿಡಿ_longest_ಹೊಂದಿಕೆ(hse, ಆರಂಭ, ಕೊನೆ, maxlen):
    buf = hse.ಬಫರ್_2

    ಹೊಂದಿಕೆ_maxlen = 0
    ಹೊಂದಿಕೆ_ಸೂಚ್ಯಂಕ = ಹೊಂದಿಕೆ_NOT_ಸಿಕ್ಕಿತು

    needlepoint = ಕೊನೆ

    pos = hse.ಹುಡುಕು_ಸೂಚ್ಯಂಕ.ಸೂಚ್ಯಂಕ[ಕೊನೆ]
    buf_needlepoint_maxlen = buf[needlepoint + ಹೊಂದಿಕೆ_maxlen]
    ಆಗ pos >= ಆರಂಭ:

        ಆದರೆ buf[pos + ಹೊಂದಿಕೆ_maxlen] != buf_needlepoint_maxlen:
            pos = hse.ಹುಡುಕು_ಸೂಚ್ಯಂಕ.ಸೂಚ್ಯಂಕ[pos]
            ಮುಂದುವರಿ

        ಉದ್ದಳತೆ = 1
        ಗಾಗಿ ಉದ್ದಳತೆ ಒಳಗೆ ಶ್ರೇಣಿ(1, maxlen):
            ಆದರೆ buf[pos + ಉದ್ದಳತೆ] != buf[needlepoint + ಉದ್ದಳತೆ]:
                ನಿಲ್ಲಿಸು
        ಆದರೆ ಉದ್ದಳತೆ > ಹೊಂದಿಕೆ_maxlen:
            ಹೊಂದಿಕೆ_maxlen = ಉದ್ದಳತೆ
            ಹೊಂದಿಕೆ_ಸೂಚ್ಯಂಕ = pos
            buf_needlepoint_maxlen = buf[needlepoint + ಹೊಂದಿಕೆ_maxlen]
            ಆದರೆ ಉದ್ದಳತೆ == maxlen:
                ನಿಲ್ಲಿಸು  # won't find better

        pos = hse.ಹುಡುಕು_ಸೂಚ್ಯಂಕ.ಸೂಚ್ಯಂಕ[pos]

    break_even_ಬಿಂದು = 1 + hse.ಕಿಟಕಿ_ಗಾತ್ರ + hse.lookahead_ಗಾತ್ರ

    ಆದರೆ ಹೊಂದಿಕೆ_maxlen > (break_even_ಬಿಂದು // 8):
        ಹಿಂತಿರುಗಿ ಕೊನೆ - ಹೊಂದಿಕೆ_ಸೂಚ್ಯಂಕ, ಹೊಂದಿಕೆ_maxlen
    ಹಿಂತಿರುಗಿ ಹೊಂದಿಕೆ_NOT_ಸಿಕ್ಕಿತು, 0


ವ್ಯಾಖ್ಯಾನ ತಳ್ಳು_outgoing_ಬಿಟ್ಟುಗಳು(hse, oi):
    ಆದರೆ hse.outgoing_ಬಿಟ್ಟುಗಳು_ಎಣಿಕೆ > 8:
        ಎಣಿಕೆ_2 = 8
        ಬಿಟ್ಟುಗಳು = hse.outgoing_ಬಿಟ್ಟುಗಳು >> (hse.outgoing_ಬಿಟ್ಟುಗಳು_ಎಣಿಕೆ - 8)
    ಇಲ್ಲದಿದ್ದರೆ:
        ಎಣಿಕೆ_2 = hse.outgoing_ಬಿಟ್ಟುಗಳು_ಎಣಿಕೆ
        ಬಿಟ್ಟುಗಳು = hse.outgoing_ಬಿಟ್ಟುಗಳು

    ಆದರೆ ಎಣಿಕೆ_2 > 0:
        ತಳ್ಳು_ಬಿಟ್ಟುಗಳು(hse, ಎಣಿಕೆ_2, ಬಿಟ್ಟುಗಳು, oi)
        hse.outgoing_ಬಿಟ್ಟುಗಳು_ಎಣಿಕೆ -= ಎಣಿಕೆ_2
    ಹಿಂತಿರುಗಿ ಎಣಿಕೆ_2


ವ್ಯಾಖ್ಯಾನ st_yield_ಟ್ಯಾಗ್_ಬಿಟ್(hse, oi):
    ಆದರೆ can_take_byte(oi):
        ಆದರೆ hse.ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ == 0:
            ಕೂಡಿಸು_ಟ್ಯಾಗ್_ಬಿಟ್(hse, oi, HEATSHRINK_LITERAL_MARKER)
            ಹಿಂತಿರುಗಿ HSE_state.HSES_YIELD_LITERAL
        ಇಲ್ಲದಿದ್ದರೆ:
            ಕೂಡಿಸು_ಟ್ಯಾಗ್_ಬಿಟ್(hse, oi, HEATSHRINK_BACKREF_MARKER)
            hse.outgoing_ಬಿಟ್ಟುಗಳು = hse.ಹೊಂದಿಕೆ_pos - 1
            hse.outgoing_ಬಿಟ್ಟುಗಳು_ಎಣಿಕೆ = hse.ಕಿಟಕಿ_ಗಾತ್ರ
            ಹಿಂತಿರುಗಿ HSE_state.HSES_YIELD_BR_ಸೂಚ್ಯಂಕ
    ಇಲ್ಲದಿದ್ದರೆ:
        ಹಿಂತಿರುಗಿ HSE_state.HSES_YIELD_ಟ್ಯಾಗ್_ಬಿಟ್


ವ್ಯಾಖ್ಯಾನ st_yield_literal(hse, oi):
    ಆದರೆ can_take_byte(oi):
        ತಳ್ಳು_literal_byte(hse, oi)
        ಹಿಂತಿರುಗಿ HSE_state.HSES_ಹುಡುಕು
    ಇಲ್ಲದಿದ್ದರೆ:
        ಹಿಂತಿರುಗಿ HSE_state.HSES_YIELD_LITERAL


ವ್ಯಾಖ್ಯಾನ st_yield_br_ಸೂಚ್ಯಂಕ(hse, oi):
    ಆದರೆ can_take_byte(oi):
        ಆದರೆ ತಳ್ಳು_outgoing_ಬಿಟ್ಟುಗಳು(hse, oi) > 0:
            ಹಿಂತಿರುಗಿ HSE_state.HSES_YIELD_BR_ಸೂಚ್ಯಂಕ
        ಇಲ್ಲದಿದ್ದರೆ:
            hse.outgoing_ಬಿಟ್ಟುಗಳು = hse.ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ - 1
            hse.outgoing_ಬಿಟ್ಟುಗಳು_ಎಣಿಕೆ = hse.lookahead_ಗಾತ್ರ
            ಹಿಂತಿರುಗಿ HSE_state.HSES_YIELD_BR_ಉದ್ದಳತೆ
    ಇಲ್ಲದಿದ್ದರೆ:
        ಹಿಂತಿರುಗಿ HSE_state.HSES_YIELD_BR_ಸೂಚ್ಯಂಕ


ವ್ಯಾಖ್ಯಾನ st_yield_br_ಉದ್ದಳತೆ(hse, oi):
    ಆದರೆ can_take_byte(oi):
        ಆದರೆ ತಳ್ಳು_outgoing_ಬಿಟ್ಟುಗಳು(hse, oi) > 0:
            ಹಿಂತಿರುಗಿ HSE_state.HSES_YIELD_BR_ಉದ್ದಳತೆ
        ಇಲ್ಲದಿದ್ದರೆ:
            hse.ಹೊಂದಿಕೆ_ಪರಿಶೀಲನೆ_ಸೂಚ್ಯಂಕ += hse.ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ
            hse.ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ = 0
            ಹಿಂತಿರುಗಿ HSE_state.HSES_ಹುಡುಕು
    ಇಲ್ಲದಿದ್ದರೆ:
        ಹಿಂತಿರುಗಿ HSE_state.HSES_YIELD_BR_ಉದ್ದಳತೆ


ವ್ಯಾಖ್ಯಾನ st_save_backlog(hse):
    save_backlog(hse)
    ಹಿಂತಿರುಗಿ HSE_state.HSES_NOT_ತುಂಬಿದ


ವ್ಯಾಖ್ಯಾನ st_flush_ಬಿಟ್_ಬಫರ್(hse, oi):
    ಆದರೆ hse.ಬಿಟ್_ಸೂಚ್ಯಂಕ == 0x80:
        ಹಿಂತಿರುಗಿ HSE_state.HSES_ಮುಗಿದ
    ಅಥವಾಆದರೆ can_take_byte(oi):
        oi.buf[oi.output_ಗಾತ್ರ[0]] = hse.current_byte
        oi.output_ಗಾತ್ರ[0] += 1
        ಹಿಂತಿರುಗಿ HSE_state.HSES_ಮುಗಿದ
    ಇಲ್ಲದಿದ್ದರೆ:
        ಹಿಂತಿರುಗಿ HSE_state.HSES_FLUSH_ಬಿಟ್ಟುಗಳು


ವ್ಯಾಖ್ಯಾನ save_backlog(hse):
    ಒಳಹರಿವು_buf_sz = get_ಒಳಹರಿವು_ಬಫರ್_ಗಾತ್ರ(hse)

    msi = hse.ಹೊಂದಿಕೆ_ಪರಿಶೀಲನೆ_ಸೂಚ್ಯಂಕ

    rem = ಒಳಹರಿವು_buf_sz - msi  # unprocessed bytes
    ಸರಿಸು_sz = ಒಳಹರಿವು_buf_sz + rem

    ಗಾಗಿ i ಒಳಗೆ ಶ್ರೇಣಿ(ಸರಿಸು_sz):
        hse.ಬಫರ್_2[i] = hse.ಬಫರ್_2[ಒಳಹರಿವು_buf_sz - rem + i]

    hse.ಹೊಂದಿಕೆ_ಪರಿಶೀಲನೆ_ಸೂಚ್ಯಂಕ = 0
    hse.ಒಳಹರಿವು_ಗಾತ್ರ -= ಒಳಹರಿವು_buf_sz - rem


ವ್ಯಾಖ್ಯಾನ encode(ದತ್ತಾಂಶ, ಕಿಟಕಿ_ಗಾತ್ರ, lookahead_ಗಾತ್ರ):
    hse = heatshrink_encoder(ಕಿಟಕಿ_ಗಾತ್ರ, lookahead_ಗಾತ್ರ)
    ಒಳಹರಿವು_buf = (ctypes.c_uint8 * ಉದ್ದ(ದತ್ತಾಂಶ))()
    ಗಾಗಿ i, d ಒಳಗೆ enumerate(ದತ್ತಾಂಶ):
        ಒಳಹರಿವು_buf[i] = d
    ಒಳ_ಗಾತ್ರ = ಉದ್ದ(ಒಳಹರಿವು_buf)

    ಹೊರಹೋಗು_buf_ಗಾತ್ರ = 4 * ಒಳ_ಗಾತ್ರ  # set output buffer size a bit larger
    ಹೊರಹೋಗು_buf = (ctypes.c_uint8 * ಹೊರಹೋಗು_buf_ಗಾತ್ರ)()

    sunk = 0
    ret = []
    ಆಗ sunk < ಒಳ_ಗಾತ್ರ:
        ಒಳಹರಿವು_ಗಾತ್ರ = ctypes.c_ಗಾತ್ರ_t(ಒಳ_ಗಾತ್ರ)
        heatshrink_encoder_sink(hse, ಒಳಹರಿವು_buf, ಒಳ_ಗಾತ್ರ - sunk, ಒಳಹರಿವು_ಗಾತ್ರ)
        ಒಳಹರಿವು_buf = ಒಳಹರಿವು_buf[ಒಳಹರಿವು_ಗಾತ್ರ.ಮೌಲ್ಯ:]
        sunk += ಒಳಹರಿವು_ಗಾತ್ರ.ಮೌಲ್ಯ
        ಆದರೆ sunk == ಒಳ_ಗಾತ್ರ:
            heatshrink_encoder_ಮುಕ್ತಾಯ(hse)

        poll_res = HSE_poll_res.HSER_POLL_ಇನ್ನಷ್ಟು
        ಆಗ poll_res == HSE_poll_res.HSER_POLL_ಇನ್ನಷ್ಟು:
            output_ಗಾತ್ರ = (ctypes.c_ಗಾತ್ರ_t * 1)()
            poll_res = heatshrink_encoder_poll(hse, ಹೊರಹೋಗು_buf, ಹೊರಹೋಗು_buf_ಗಾತ್ರ, output_ಗಾತ್ರ)
            ret += list(ಹೊರಹೋಗು_buf)[0:output_ಗಾತ್ರ[0]]

        ಆದರೆ sunk == ಒಳ_ಗಾತ್ರ:
            heatshrink_encoder_ಮುಕ್ತಾಯ(hse)
    ಹಿಂತಿರುಗಿ ret
