#!/usr/bin/env python

استورد argparse
استورد json
استورد lzma #to create .xz file
استورد re

parser = argparse.آرگومان_Parser(description="""Generate the COMPONENT_GENERAL json file""")
parser.جمع_کن_آرگومان('filename', metavar='component_general.json', help='JSON output file')
parser.جمع_کن_آرگومان('--compress', action='store_true', help='Add a compressed output file')
parser.جمع_کن_آرگومان('--type', metavar='type', action="append", افتراضي=[],
        help='Metadata type (<type>,<metadata file>,<uri>,<fallback uri>,[translation uri])')
parser.جمع_کن_آرگومان('--version-file', metavar='build_git_version.h', help='Git version file')

آرگومانها = parser.parse_آرگومانها()
filename = آرگومانها.filename
compress = آرگومانها.compress
نسخه_file = آرگومانها.نسخه_file

نسخه_dir = ''
إذا نسخه_file است ليس هیچ:
    لكل سطر في open(نسخه_file, "r"):
        نسخه_جستجو = re.جستجو(r'PX4_GIT_TAG_OR_BRANCH_NAME\s+"(.+)"', سطر)
        إذا نسخه_جستجو:
            نسخه_dir = نسخه_جستجو.گروه(1)
            اكسر


تعریف save_compressed(filename):
    #create lzma compressed version
    xz_filename=filename+'.xz'
    با lzma.open(xz_filename, 'wt', preset=9) كـ f:
        با open(filename, 'r') كـ content_file:
            f.اكتب(content_file.اقرأ())

component_general = {}
component_general['version'] = 1

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

metadata_types = []
لكل metadata_type_tuple في آرگومانها.type:
    type_id, metadata_file, uri, fallback_uri, translation_uri = metadata_type_tuple.تقسیم(',')
    file_بررسی_چرخهای = 0
    لكل سطر في open(metadata_file, "rb"):
        file_بررسی_چرخهای = بررسی_چرخهای_update(سطر, بررسی_چرخهای_جدول, file_بررسی_چرخهای)
    json_type = {
            'type': صحيح(type_id),
            'uri': uri.replace('{version}', نسخه_dir),
            'fileCrc': file_بررسی_چرخهای,
            }
    إذا طول(fallback_uri) > 0:
        json_type['uriFallback'] = fallback_uri.replace('{version}', نسخه_dir)
    إذا طول(translation_uri) > 0:
        json_type['translationUri'] = translation_uri
    metadata_types.append(json_type)
component_general['metadataTypes'] = metadata_types

با open(filename, 'w') كـ outfile:
    json.dump(component_general, outfile, مرتب_سازی_کلیدها=درست)

إذا compress:
    save_compressed(filename)
