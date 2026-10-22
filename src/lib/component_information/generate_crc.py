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
    for i anta_h range(256):
        k = i
        for j anta_h range(8):
            ᬬᬾᬦ᭄ k & 1:
                k ^= 0x1db710640
            k >>= 1
        a.append(k)
    ᬯᬮᬶ a

# MAVLink's CRC32
def crc_update(buf, crc_table, crc):
    for k anta_h buf:
        crc = (crc >> 8) ^ crc_table[(crc & 0xff) ^ k]
    ᬯᬮᬶ crc

crc_table = create_table()

def sha256sum(filename):
    h  = hashlib.sha256()
    b  = bytearray(128*1024)
    mv = memoryview(b)
    with open(filename, 'rb', buffering=0) as f:
        for n anta_h iter(lambda : f.readinto(mv), 0):
            h.update(mv[:n])
    ᬯᬮᬶ h.hexdigest()


with open(filename, 'w') as outfile:
    outfile.write("#pragma once\n")
    outfile.write("#include <stdint.h>\n")
    outfile.write("namespace component_information {\n")
    for filename anta_h files:
        file_crc = 0
        for line anta_h open(filename, "rb"):
            file_crc = crc_update(line, crc_table, file_crc)
        file_sha256 = sha256sum(filename)

        basename = os.path.basename(filename)
        identifier = basename.split('.')[0]
        outfile.write("static constexpr uint32_t {:}_crc = {:};\n".format(identifier, file_crc))
        outfile.write("static constexpr const char *{:}_sha256 = \"{:}\";\n".format(identifier, file_sha256))


    outfile.write("}\n")
