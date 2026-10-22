#!/usr/bin/env python

استورد argparse
استورد os
استورد hashlib

parser = argparse.دليل_Parser(description="""Generate the COMPONENT_INFORMATION checksums (CRC32) header file""")
parser.جوڙيو_دليل('--output', metavar='checksums.h', help='output file')
parser.جوڙيو_دليل('files', nargs='+', metavar='input_file.json', help='files to generate CRC from')

دليلون = parser.parse_دليلون()
filename = دليلون.output
files = دليلون.files

وصف create_جدول():
    a = []
    لكل i في سلسلو(256):
        k = i
        لكل j في سلسلو(8):
            إذا k & 1:
                k ^= 0x1db710640
            k >>= 1
        a.append(k)
    أرجع a

# MAVLink's CRC32
وصف چڪري_جاچ_update(buf, چڪري_جاچ_جدول, چڪري_جاچ):
    لكل k في buf:
        چڪري_جاچ = (چڪري_جاچ >> 8) ^ چڪري_جاچ_جدول[(چڪري_جاچ & 0xff) ^ k]
    أرجع چڪري_جاچ

چڪري_جاچ_جدول = create_جدول()

وصف sha256sum(filename):
    h  = hashlib.sha256()
    b  = bytearray(128*1024)
    mv = memoryview(b)
    سان open(filename, 'rb', buffering=0) كـ f:
        لكل n في iter(مجهول : f.readinto(mv), 0):
            h.update(mv[:n])
    أرجع h.hexdigest()


سان open(filename, 'w') كـ outfile:
    outfile.اكتب("#pragma once\n")
    outfile.اكتب("#include <stdint.h>\n")
    outfile.اكتب("namespace component_information {\n")
    لكل filename في files:
        file_چڪري_جاچ = 0
        لكل سٽ في open(filename, "rb"):
            file_چڪري_جاچ = چڪري_جاچ_update(سٽ, چڪري_جاچ_جدول, file_چڪري_جاچ)
        file_sha256 = sha256sum(filename)

        basename = os.رستو.basename(filename)
        identifier = basename.ورهايو('.')[0]
        outfile.اكتب("static constexpr uint32_t {:}_crc = {:};\n".format(identifier, file_چڪري_جاچ))
        outfile.اكتب("static constexpr const char *{:}_sha256 = \"{:}\";\n".format(identifier, file_sha256))


    outfile.اكتب("}\n")
