#!/usr/bin/env python

استورد argparse
استورد json
استورد lzma #to create .xz file
استورد re

parser = argparse.دليل_Parser(description="""Generate the COMPONENT_GENERAL json file""")
parser.جوڙيو_دليل('filename', metavar='component_general.json', help='JSON output file')
parser.جوڙيو_دليل('--compress', action='store_true', help='Add a compressed output file')
parser.جوڙيو_دليل('--type', metavar='type', action="append", افتراضي=[],
        help='Metadata type (<type>,<metadata file>,<uri>,<fallback uri>,[translation uri])')
parser.جوڙيو_دليل('--version-file', metavar='build_git_version.h', help='Git version file')

دليلون = parser.parse_دليلون()
filename = دليلون.filename
compress = دليلون.compress
نسخو_file = دليلون.نسخو_file

نسخو_dir = ''
إذا نسخو_file آهي ليس ڪجهه_نه:
    لكل سٽ في open(نسخو_file, "r"):
        نسخو_ڳولا = re.ڳولا(r'PX4_GIT_TAG_OR_BRANCH_NAME\s+"(.+)"', سٽ)
        إذا نسخو_ڳولا:
            نسخو_dir = نسخو_ڳولا.گروهه(1)
            اكسر


وصف save_compressed(filename):
    #create lzma compressed version
    xz_filename=filename+'.xz'
    سان lzma.open(xz_filename, 'wt', preset=9) كـ f:
        سان open(filename, 'r') كـ content_file:
            f.اكتب(content_file.اقرأ())

component_general = {}
component_general['version'] = 1

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

metadata_types = []
لكل metadata_type_tuple في دليلون.type:
    type_id, metadata_file, uri, fallback_uri, translation_uri = metadata_type_tuple.ورهايو(',')
    file_چڪري_جاچ = 0
    لكل سٽ في open(metadata_file, "rb"):
        file_چڪري_جاچ = چڪري_جاچ_update(سٽ, چڪري_جاچ_جدول, file_چڪري_جاچ)
    json_type = {
            'type': صحيح(type_id),
            'uri': uri.replace('{version}', نسخو_dir),
            'fileCrc': file_چڪري_جاچ,
            }
    إذا ڊيگهه(fallback_uri) > 0:
        json_type['uriFallback'] = fallback_uri.replace('{version}', نسخو_dir)
    إذا ڊيگهه(translation_uri) > 0:
        json_type['translationUri'] = translation_uri
    metadata_types.append(json_type)
component_general['metadataTypes'] = metadata_types

سان open(filename, 'w') كـ outfile:
    json.dump(component_general, outfile, ترتيب_ڏيو_ڪنجيون=سچ)

إذا compress:
    save_compressed(filename)
