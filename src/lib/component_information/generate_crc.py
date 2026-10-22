#!/usr/bin/env python

আমদানি argparse
আমদানি os
আমদানি hashlib

parser = argparse.ArgumentParser(description="""Generate the COMPONENT_INFORMATION checksums (CRC32) header file""")
parser.add_argument('--output', metavar='checksums.h', help='output file')
parser.add_argument('files', nargs='+', metavar='input_file.json', help='files to generate CRC from')

args = parser.parse_args()
filename = args.output
files = args.files

def create_table():
    a = []
    জন্য i মধ্যে range(256):
        k = i
        জন্য j মধ্যে range(8):
            যদি k & 1:
                k ^= 0x1db710640
            k >>= 1
        a.append(k)
    ফেরত a

# MAVLink's CRC32
def crc_update(buf, crc_table, crc):
    জন্য k মধ্যে buf:
        crc = (crc >> 8) ^ crc_table[(crc & 0xff) ^ k]
    ফেরত crc

crc_table = create_table()

def sha256sum(filename):
    h  = hashlib.sha256()
    b  = bytearray(128*1024)
    mv = memoryview(b)
    with open(filename, 'rb', buffering=0) ইব f:
        জন্য n মধ্যে iter(অনামা : f.readinto(mv), 0):
            h.update(mv[:n])
    ফেরত h.hexdigest()


with open(filename, 'w') ইব outfile:
    outfile.লিখ("#pragma once\n")
    outfile.লিখ("#include <stdint.h>\n")
    outfile.লিখ("namespace component_information {\n")
    জন্য filename মধ্যে files:
        file_crc = 0
        জন্য সারি মধ্যে open(filename, "rb"):
            file_crc = crc_update(সারি, crc_table, file_crc)
        file_sha256 = sha256sum(filename)

        basename = os.path.basename(filename)
        identifier = basename.split('.')[0]
        outfile.লিখ("static constexpr uint32_t {:}_crc = {:};\n".format(identifier, file_crc))
        outfile.লিখ("static constexpr const char *{:}_sha256 = \"{:}\";\n".format(identifier, file_sha256))


    outfile.লিখ("}\n")
