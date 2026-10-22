#!/usr/bin/env python

استورد argparse
استورد os
استورد hashlib

parser = argparse.دلیل_Parser(description="""Generate the COMPONENT_INFORMATION checksums (CRC32) header file""")
parser.جمع_کړه_دلیل('--output', metavar='checksums.h', help='output file')
parser.جمع_کړه_دلیل('files', nargs='+', metavar='input_file.json', help='files to generate CRC from')

دلیلونه = parser.parse_دلیلونه()
filename = دلیلونه.output
files = دلیلونه.files

تعریف create_جدول():
    a = []
    لكل i في لړ(256):
        k = i
        لكل j في لړ(8):
            إذا k & 1:
                k ^= 0x1db710640
            k >>= 1
        a.append(k)
    أرجع a

# MAVLink's CRC32
تعریف دوراني_ازموینه_update(buf, دوراني_ازموینه_جدول, دوراني_ازموینه):
    لكل k في buf:
        دوراني_ازموینه = (دوراني_ازموینه >> 8) ^ دوراني_ازموینه_جدول[(دوراني_ازموینه & 0xff) ^ k]
    أرجع دوراني_ازموینه

دوراني_ازموینه_جدول = create_جدول()

تعریف sha256sum(filename):
    h  = hashlib.sha256()
    b  = bytearray(128*1024)
    mv = memoryview(b)
    سره open(filename, 'rb', buffering=0) كـ f:
        لكل n في iter(مجهول : f.readinto(mv), 0):
            h.update(mv[:n])
    أرجع h.hexdigest()


سره open(filename, 'w') كـ outfile:
    outfile.اكتب("#pragma once\n")
    outfile.اكتب("#include <stdint.h>\n")
    outfile.اكتب("namespace component_information {\n")
    لكل filename في files:
        file_دوراني_ازموینه = 0
        لكل کرښه في open(filename, "rb"):
            file_دوراني_ازموینه = دوراني_ازموینه_update(کرښه, دوراني_ازموینه_جدول, file_دوراني_ازموینه)
        file_sha256 = sha256sum(filename)

        basename = os.لار.basename(filename)
        identifier = basename.وېشل('.')[0]
        outfile.اكتب("static constexpr uint32_t {:}_crc = {:};\n".format(identifier, file_دوراني_ازموینه))
        outfile.اكتب("static constexpr const char *{:}_sha256 = \"{:}\";\n".format(identifier, file_sha256))


    outfile.اكتب("}\n")
