#!/usr/bin/env python

واردکن argparse
واردکن json
واردکن lzma #to create .xz file
واردکن re

parser = argparse.ArgumentParser(description="""Generate the COMPONENT_GENERAL json file""")
parser.add_argument('filename', metavar='component_general.json', help='JSON output file')
parser.add_argument('--compress', action='store_true', help='Add a compressed output file')
parser.add_argument('--type', metavar='type', action="append", پیشفرض=[],
        help='Metadata type (<type>,<metadata file>,<uri>,<fallback uri>,[translation uri])')
parser.add_argument('--version-file', metavar='build_git_version.h', help='Git version file')

args = parser.parse_args()
filename = args.filename
compress = args.compress
version_file = args.version_file

version_dir = ''
اگر version_file is نه None:
    برای سطر در open(version_file, "r"):
        version_جستجو = re.جستجو(r'PX4_GIT_TAG_OR_BRANCH_NAME\s+"(.+)"', سطر)
        اگر version_جستجو:
            version_dir = version_جستجو.group(1)
            بشکن


def save_compressed(filename):
    #create lzma compressed version
    xz_filename=filename+'.xz'
    with lzma.open(xz_filename, 'wt', preset=9) بهعنوان f:
        with open(filename, 'r') بهعنوان content_file:
            f.اكتب(content_file.اقرأ())

component_general = {}
component_general['version'] = 1

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

metadata_types = []
برای metadata_type_tuple در args.type:
    type_id, metadata_file, uri, fallback_uri, translation_uri = metadata_type_tuple.split(',')
    file_crc = 0
    برای سطر در open(metadata_file, "rb"):
        file_crc = crc_update(سطر, crc_table, file_crc)
    json_type = {
            'type': صحیح(type_id),
            'uri': uri.replace('{version}', version_dir),
            'fileCrc': file_crc,
            }
    اگر len(fallback_uri) > 0:
        json_type['uriFallback'] = fallback_uri.replace('{version}', version_dir)
    اگر len(translation_uri) > 0:
        json_type['translationUri'] = translation_uri
    metadata_types.append(json_type)
component_general['metadataTypes'] = metadata_types

with open(filename, 'w') بهعنوان outfile:
    json.dump(component_general, outfile, sort_کلیدها=درست)

اگر compress:
    save_compressed(filename)
