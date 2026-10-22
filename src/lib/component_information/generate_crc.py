#!/usr/bin/env python

आनयन argparse
आनयन os
आनयन hashlib

parser = argparse.तर्कः_Parser(description="""Generate the COMPONENT_INFORMATION checksums (CRC32) header file""")
parser.सङ्कलय_तर्कः('--output', metavar='checksums.h', help='output file')
parser.सङ्कलय_तर्कः('files', nargs='+', metavar='input_file.json', help='files to generate CRC from')

तर्काः = parser.parse_तर्काः()
filename = तर्काः.output
files = तर्काः.files

परिभाषा create_सारणी():
    a = []
    प्रति i अन्तः परिसरः(256):
        k = i
        प्रति j अन्तः परिसरः(8):
            यदि k & 1:
                k ^= 0x1db710640
            k >>= 1
        a.append(k)
    प्रत्यागम a

# MAVLink's CRC32
परिभाषा crc_update(buf, crc_सारणी, crc):
    प्रति k अन्तः buf:
        crc = (crc >> 8) ^ crc_सारणी[(crc & 0xff) ^ k]
    प्रत्यागम crc

crc_सारणी = create_सारणी()

परिभाषा sha256sum(filename):
    h  = hashlib.sha256()
    b  = bytearray(128*1024)
    mv = memoryview(b)
    सह open(filename, 'rb', buffering=0) इव f:
        प्रति n अन्तः iter(अनामन् : f.readinto(mv), 0):
            h.update(mv[:n])
    प्रत्यागम h.hexdigest()


सह open(filename, 'w') इव outfile:
    outfile.लिख("#pragma once\n")
    outfile.लिख("#include <stdint.h>\n")
    outfile.लिख("namespace component_information {\n")
    प्रति filename अन्तः files:
        file_crc = 0
        प्रति पङ्क्तिः अन्तः open(filename, "rb"):
            file_crc = crc_update(पङ्क्तिः, crc_सारणी, file_crc)
        file_sha256 = sha256sum(filename)

        basename = os.मार्गः.basename(filename)
        identifier = basename.विभज('.')[0]
        outfile.लिख("static constexpr uint32_t {:}_crc = {:};\n".format(identifier, file_crc))
        outfile.लिख("static constexpr const char *{:}_sha256 = \"{:}\";\n".format(identifier, file_sha256))


    outfile.लिख("}\n")
