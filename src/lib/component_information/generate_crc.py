#!/usr/bin/env python

આયાત argparse
આયાત os
આયાત hashlib

parser = argparse.ArgumentParser(description="""Generate the COMPONENT_INFORMATION checksums (CRC32) header file""")
parser.add_argument('--output', metavar='checksums.h', help='output file')
parser.add_argument('files', nargs='+', metavar='input_file.json', help='files to generate CRC from')

args = parser.parse_args()
filename = args.output
files = args.files

def create_table():
    a = []
    માટે i માં range(256):
        k = i
        માટે j માં range(8):
            જો k & 1:
                k ^= 0x1db710640
            k >>= 1
        a.append(k)
    પાછા a

# MAVLink's CRC32
def crc_update(buf, crc_table, crc):
    માટે k માં buf:
        crc = (crc >> 8) ^ crc_table[(crc & 0xff) ^ k]
    પાછા crc

crc_table = create_table()

def sha256sum(filename):
    h  = hashlib.sha256()
    b  = bytearray(128*1024)
    mv = memoryview(b)
    with open(filename, 'rb', buffering=0) ઇવ f:
        માટે n માં iter(અનામી : f.readinto(mv), 0):
            h.update(mv[:n])
    પાછા h.hexdigest()


with open(filename, 'w') ઇવ outfile:
    outfile.લિખ("#pragma once\n")
    outfile.લિખ("#include <stdint.h>\n")
    outfile.લિખ("namespace component_information {\n")
    માટે filename માં files:
        file_crc = 0
        માટે લીટી માં open(filename, "rb"):
            file_crc = crc_update(લીટી, crc_table, file_crc)
        file_sha256 = sha256sum(filename)

        basename = os.path.basename(filename)
        identifier = basename.split('.')[0]
        outfile.લિખ("static constexpr uint32_t {:}_crc = {:};\n".format(identifier, file_crc))
        outfile.લિખ("static constexpr const char *{:}_sha256 = \"{:}\";\n".format(identifier, file_sha256))


    outfile.લિખ("}\n")
