#!/usr/bin/env python

استورد argparse
استورد json
استورد lzma #to create .xz file
استورد re

parser = argparse.ArgumentParser(description="""Generate the COMPONENT_GENERAL json file""")
parser.add_argument('filename', metavar='component_general.json', help='JSON output file')
parser.add_argument('--compress', action='store_true', help='Add a compressed output file')
parser.add_argument('--type', metavar='type', action="append", افتراضي=[],
        help='Metadata type (<type>,<metadata file>,<uri>,<fallback uri>,[translation uri])')
parser.add_argument('--version-file', metavar='build_git_version.h', help='Git version file')

args = parser.parse_args()
filename = args.filename
compress = args.compress
version_file = args.version_file

version_dir = ''
𐤀𐤌 version_file is 𐤋𐤀 None:
    لكل line في open(version_file, "r"):
        version_search = re.search(r'PX4_GIT_TAG_OR_BRANCH_NAME\s+"(.+)"', line)
        𐤀𐤌 version_search:
            version_dir = version_search.group(1)
            اكسر


def save_compressed(filename):
    #create lzma compressed version
    xz_filename=filename+'.xz'
    with lzma.open(xz_filename, 'wt', preset=9) كـ f:
        with open(filename, 'r') كـ content_file:
            f.اكتب(content_file.اقرأ())

component_general = {}
component_general['version'] = 1

def create_table():
    a = []
    لكل i في range(256):
        k = i
        لكل j في range(8):
            𐤀𐤌 k & 1:
                k ^= 0x1db710640
            k >>= 1
        a.append(k)
    أرجع a

# MAVLink's CRC32
def crc_update(buf, crc_table, crc):
    لكل k في buf:
        crc = (crc >> 8) ^ crc_table[(crc & 0xff) ^ k]
    أرجع crc

crc_table = create_table()

metadata_types = []
لكل metadata_type_tuple في args.type:
    type_id, metadata_file, uri, fallback_uri, translation_uri = metadata_type_tuple.split(',')
    file_crc = 0
    لكل line في open(metadata_file, "rb"):
        file_crc = crc_update(line, crc_table, file_crc)
    json_type = {
            'type': int(type_id),
            'uri': uri.replace('{version}', version_dir),
            'fileCrc': file_crc,
            }
    𐤀𐤌 len(fallback_uri) > 0:
        json_type['uriFallback'] = fallback_uri.replace('{version}', version_dir)
    𐤀𐤌 len(translation_uri) > 0:
        json_type['translationUri'] = translation_uri
    metadata_types.append(json_type)
component_general['metadataTypes'] = metadata_types

with open(filename, 'w') كـ outfile:
    json.dump(component_general, outfile, sort_keys=صحيح)

𐤀𐤌 compress:
    save_compressed(filename)
