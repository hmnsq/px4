#!/usr/bin/env python

import argparse
import os
import hashlib

parser = argparse.ArgumentParser(description="""Generate the COMPONENT_INFORMATION checksums (CRC32) header file""")
parser.add_argument('--output', metavar='checksums.h', help='output file')
parser.add_argument('files', nargs='+', metavar='input_file.json', help='files to generate CRC from')

args = parser.parse_args()
filename = args.output
files = args.files

def create_table():
    a = []
    ために i in range(256):
        k = i
        ために j in range(8):
            もし k & 1:
                k ^= 0x1db710640
            k >>= 1
        a.append(k)
    戻る a

# MAVLink's CRC32
def crc_update(buf, crc_table, crc):
    ために k in buf:
        crc = (crc >> 8) ^ crc_table[(crc & 0xff) ^ k]
    戻る crc

crc_table = create_table()

def sha256sum(filename):
    h  = hashlib.sha256()
    b  = bytearray(128*1024)
    mv = memoryview(b)
    with open(filename, 'rb', buffering=0) as f:
        ために n in iter(lambda : f.readinto(mv), 0):
            h.update(mv[:n])
    戻る h.hexdigest()


with open(filename, 'w') as outfile:
    outfile.write("#pragma once\n")
    outfile.write("#include <stdint.h>\n")
    outfile.write("namespace component_information {\n")
    ために filename in files:
        file_crc = 0
        ために line in open(filename, "rb"):
            file_crc = crc_update(line, crc_table, file_crc)
        file_sha256 = sha256sum(filename)

        basename = os.path.basename(filename)
        identifier = basename.split('.')[0]
        outfile.write("static constexpr uint32_t {:}_crc = {:};\n".format(identifier, file_crc))
        outfile.write("static constexpr const char *{:}_sha256 = \"{:}\";\n".format(identifier, file_sha256))


    outfile.write("}\n")
