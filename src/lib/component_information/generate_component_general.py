#!/usr/bin/env python

ଆମଦାନୀ argparse
ଆମଦାନୀ json
ଆମଦାନୀ lzma #to create .xz file
ଆମଦାନୀ re

parser = argparse.ArgumentParser(description="""Generate the COMPONENT_GENERAL json file""")
parser.add_argument('filename', metavar='component_general.json', help='JSON output file')
parser.add_argument('--compress', action='store_true', help='Add a compressed output file')
parser.add_argument('--type', metavar='type', action="append", ପୂର୍ବନିର୍ଦ୍ଧାରିତ=[],
        help='Metadata type (<type>,<metadata file>,<uri>,<fallback uri>,[translation uri])')
parser.add_argument('--version-file', metavar='build_git_version.h', help='Git version file')

args = parser.parse_args()
filename = args.filename
compress = args.compress
version_file = args.version_file

version_dir = ''
ଯଦି version_file is ନୁହେଁ None:
    ପାଇଁ ଧାଡ଼ି ଭିତରେ open(version_file, "r"):
        version_ସନ୍ଧାନ = re.ସନ୍ଧାନ(r'PX4_GIT_TAG_OR_BRANCH_NAME\s+"(.+)"', ଧାଡ଼ି)
        ଯଦି version_ସନ୍ଧାନ:
            version_dir = version_ସନ୍ଧାନ.group(1)
            ଭାଙ୍ଗ


def save_compressed(filename):
    #create lzma compressed version
    xz_filename=filename+'.xz'
    with lzma.open(xz_filename, 'wt', preset=9) ଇଵ f:
        with open(filename, 'r') ଇଵ content_file:
            f.ଲିଖ(content_file.ପଠ())

component_general = {}
component_general['version'] = 1

def create_table():
    a = []
    ପାଇଁ i ଭିତରେ range(256):
        k = i
        ପାଇଁ j ଭିତରେ range(8):
            ଯଦି k & 1:
                k ^= 0x1db710640
            k >>= 1
        a.append(k)
    ଫେରନ୍ତୁ a

# MAVLink's CRC32
def crc_update(buf, crc_table, crc):
    ପାଇଁ k ଭିତରେ buf:
        crc = (crc >> 8) ^ crc_table[(crc & 0xff) ^ k]
    ଫେରନ୍ତୁ crc

crc_table = create_table()

metadata_types = []
ପାଇଁ metadata_type_tuple ଭିତରେ args.type:
    type_id, metadata_file, uri, fallback_uri, translation_uri = metadata_type_tuple.split(',')
    file_crc = 0
    ପାଇଁ ଧାଡ଼ି ଭିତରେ open(metadata_file, "rb"):
        file_crc = crc_update(ଧାଡ଼ି, crc_table, file_crc)
    json_type = {
            'type': ପୂର୍ଣ୍ଣସଂଖ୍ୟା(type_id),
            'uri': uri.replace('{version}', version_dir),
            'fileCrc': file_crc,
            }
    ଯଦି len(fallback_uri) > 0:
        json_type['uriFallback'] = fallback_uri.replace('{version}', version_dir)
    ଯଦି len(translation_uri) > 0:
        json_type['translationUri'] = translation_uri
    metadata_types.append(json_type)
component_general['metadataTypes'] = metadata_types

with open(filename, 'w') ଇଵ outfile:
    json.dump(component_general, outfile, sort_ଚାବିଗୁଡ଼ିକ=ସତ୍ୟ)

ଯଦି compress:
    save_compressed(filename)
