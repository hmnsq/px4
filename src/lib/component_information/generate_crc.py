#!/usr/bin/env python

واردکن argparse
واردکن os
واردکن hashlib

parser = argparse.ArgumentParser(description="""Generate the COMPONENT_INFORMATION checksums (CRC32) header file""")
parser.add_argument('--output', metavar='checksums.h', help='output file')
parser.add_argument('files', nargs='+', metavar='input_file.json', help='files to generate CRC from')

args = parser.parse_args()
filename = args.output
files = args.files

def create_table():
    a = []
    برای i در range(256):
        k = i
        برای j در range(8):
            اگر k & 1:
                k ^= 0x1db710640
            k >>= 1
        a.append(k)
    بازگشت a

# MAVLink's CRC32
def crc_update(buf, crc_table, crc):
    برای k در buf:
        crc = (crc >> 8) ^ crc_table[(crc & 0xff) ^ k]
    بازگشت crc

crc_table = create_table()

def sha256sum(filename):
    h  = hashlib.sha256()
    b  = bytearray(128*1024)
    mv = memoryview(b)
    with open(filename, 'rb', buffering=0) بهعنوان f:
        برای n در iter(بینام : f.readinto(mv), 0):
            h.update(mv[:n])
    بازگشت h.hexdigest()


with open(filename, 'w') بهعنوان outfile:
    outfile.اكتب("#pragma once\n")
    outfile.اكتب("#include <stdint.h>\n")
    outfile.اكتب("namespace component_information {\n")
    برای filename در files:
        file_crc = 0
        برای سطر در open(filename, "rb"):
            file_crc = crc_update(سطر, crc_table, file_crc)
        file_sha256 = sha256sum(filename)

        basename = os.path.basename(filename)
        identifier = basename.split('.')[0]
        outfile.اكتب("static constexpr uint32_t {:}_crc = {:};\n".format(identifier, file_crc))
        outfile.اكتب("static constexpr const char *{:}_sha256 = \"{:}\";\n".format(identifier, file_sha256))


    outfile.اكتب("}\n")
