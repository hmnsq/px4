#!/usr/bin/env python

استورد argparse
استورد os
استورد hashlib

parser = argparse.آرگومان_Parser(description="""Generate the COMPONENT_INFORMATION checksums (CRC32) header file""")
parser.جمع_کن_آرگومان('--output', metavar='checksums.h', help='output file')
parser.جمع_کن_آرگومان('files', nargs='+', metavar='input_file.json', help='files to generate CRC from')

آرگومانها = parser.parse_آرگومانها()
filename = آرگومانها.output
files = آرگومانها.files

تعریف create_جدول():
    a = []
    لكل i في دامنه(256):
        k = i
        لكل j في دامنه(8):
            إذا k & 1:
                k ^= 0x1db710640
            k >>= 1
        a.append(k)
    أرجع a

# MAVLink's CRC32
تعریف بررسی_چرخهای_update(buf, بررسی_چرخهای_جدول, بررسی_چرخهای):
    لكل k في buf:
        بررسی_چرخهای = (بررسی_چرخهای >> 8) ^ بررسی_چرخهای_جدول[(بررسی_چرخهای & 0xff) ^ k]
    أرجع بررسی_چرخهای

بررسی_چرخهای_جدول = create_جدول()

تعریف sha256sum(filename):
    h  = hashlib.sha256()
    b  = bytearray(128*1024)
    mv = memoryview(b)
    با open(filename, 'rb', buffering=0) كـ f:
        لكل n في iter(مجهول : f.readinto(mv), 0):
            h.update(mv[:n])
    أرجع h.hexdigest()


با open(filename, 'w') كـ outfile:
    outfile.اكتب("#pragma once\n")
    outfile.اكتب("#include <stdint.h>\n")
    outfile.اكتب("namespace component_information {\n")
    لكل filename في files:
        file_بررسی_چرخهای = 0
        لكل سطر في open(filename, "rb"):
            file_بررسی_چرخهای = بررسی_چرخهای_update(سطر, بررسی_چرخهای_جدول, file_بررسی_چرخهای)
        file_sha256 = sha256sum(filename)

        basename = os.مسیر.basename(filename)
        identifier = basename.تقسیم('.')[0]
        outfile.اكتب("static constexpr uint32_t {:}_crc = {:};\n".format(identifier, file_بررسی_چرخهای))
        outfile.اكتب("static constexpr const char *{:}_sha256 = \"{:}\";\n".format(identifier, file_sha256))


    outfile.اكتب("}\n")
