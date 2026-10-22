#!/usr/bin/env python

استورد argparse
استورد json
استورد lzma #to create .xz file
استورد re

parser = argparse.دلیل_Parser(description="""Generate the COMPONENT_GENERAL json file""")
parser.جوڑو_دلیل('filename', metavar='component_general.json', help='JSON output file')
parser.جوڑو_دلیل('--compress', action='store_true', help='Add a compressed output file')
parser.جوڑو_دلیل('--type', metavar='type', action="append", افتراضي=[],
        help='Metadata type (<type>,<metadata file>,<uri>,<fallback uri>,[translation uri])')
parser.جوڑو_دلیل('--version-file', metavar='build_git_version.h', help='Git version file')

دلیلاں = parser.parse_دلیلاں()
filename = دلیلاں.filename
compress = دلیلاں.compress
نسخہ_file = دلیلاں.نسخہ_file

نسخہ_dir = ''
إذا نسخہ_file اے ليس کجھ_نئیں:
    لكل سطر في open(نسخہ_file, "r"):
        نسخہ_تلاش = re.تلاش(r'PX4_GIT_TAG_OR_BRANCH_NAME\s+"(.+)"', سطر)
        إذا نسخہ_تلاش:
            نسخہ_dir = نسخہ_تلاش.گروہ(1)
            اكسر


تعریف save_compressed(filename):
    #create lzma compressed version
    xz_filename=filename+'.xz'
    نال lzma.open(xz_filename, 'wt', preset=9) كـ f:
        نال open(filename, 'r') كـ content_file:
            f.اكتب(content_file.اقرأ())

component_general = {}
component_general['version'] = 1

تعریف create_جدول():
    a = []
    لكل i في سلسلہ(256):
        k = i
        لكل j في سلسلہ(8):
            إذا k & 1:
                k ^= 0x1db710640
            k >>= 1
        a.append(k)
    أرجع a

# MAVLink's CRC32
تعریف چکری_جانچ_update(buf, چکری_جانچ_جدول, چکری_جانچ):
    لكل k في buf:
        چکری_جانچ = (چکری_جانچ >> 8) ^ چکری_جانچ_جدول[(چکری_جانچ & 0xff) ^ k]
    أرجع چکری_جانچ

چکری_جانچ_جدول = create_جدول()

metadata_types = []
لكل metadata_type_tuple في دلیلاں.type:
    type_id, metadata_file, uri, fallback_uri, translation_uri = metadata_type_tuple.ونڈو(',')
    file_چکری_جانچ = 0
    لكل سطر في open(metadata_file, "rb"):
        file_چکری_جانچ = چکری_جانچ_update(سطر, چکری_جانچ_جدول, file_چکری_جانچ)
    json_type = {
            'type': صحيح(type_id),
            'uri': uri.replace('{version}', نسخہ_dir),
            'fileCrc': file_چکری_جانچ,
            }
    إذا لمبائی(fallback_uri) > 0:
        json_type['uriFallback'] = fallback_uri.replace('{version}', نسخہ_dir)
    إذا لمبائی(translation_uri) > 0:
        json_type['translationUri'] = translation_uri
    metadata_types.append(json_type)
component_general['metadataTypes'] = metadata_types

نال open(filename, 'w') كـ outfile:
    json.dump(component_general, outfile, ترتیب_دیو_کلیدیں=سچ)

إذا compress:
    save_compressed(filename)
