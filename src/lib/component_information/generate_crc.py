#!/usr/bin/env python

আনযন argparse
আনযন os
আনযন hashlib

parser = argparse.ArgumentParser(description="""Generate the COMPONENT_INFORMATION checksums (CRC32) header file""")
parser.add_argument('--output', metavar='checksums.h', help='output file')
parser.add_argument('files', nargs='+', metavar='input_file.json', help='files to generate CRC from')

args = parser.parse_args()
filename = args.output
files = args.files

def create_table():
    a = []
    বাবে i অন্তঃ range(256):
        k = i
        বাবে j অন্তঃ range(8):
            যদি k & 1:
                k ^= 0x1db710640
            k >>= 1
        a.append(k)
    ঘূৰাই a

# MAVLink's CRC32
def crc_update(buf, crc_table, crc):
    বাবে k অন্তঃ buf:
        crc = (crc >> 8) ^ crc_table[(crc & 0xff) ^ k]
    ঘূৰাই crc

crc_table = create_table()

def sha256sum(filename):
    h  = hashlib.sha256()
    b  = bytearray(128*1024)
    mv = memoryview(b)
    with open(filename, 'rb', buffering=0) ইৱ f:
        বাবে n অন্তঃ iter(অনামন্ : f.readinto(mv), 0):
            h.update(mv[:n])
    ঘূৰাই h.hexdigest()


with open(filename, 'w') ইৱ outfile:
    outfile.লিখ("#pragma once\n")
    outfile.লিখ("#include <stdint.h>\n")
    outfile.লিখ("namespace component_information {\n")
    বাবে filename অন্তঃ files:
        file_crc = 0
        বাবে সাৰি অন্তঃ open(filename, "rb"):
            file_crc = crc_update(সাৰি, crc_table, file_crc)
        file_sha256 = sha256sum(filename)

        basename = os.path.basename(filename)
        identifier = basename.split('.')[0]
        outfile.লিখ("static constexpr uint32_t {:}_crc = {:};\n".format(identifier, file_crc))
        outfile.লিখ("static constexpr const char *{:}_sha256 = \"{:}\";\n".format(identifier, file_sha256))


    outfile.লিখ("}\n")
