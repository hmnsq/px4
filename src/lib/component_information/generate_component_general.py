#!/usr/bin/env python

આયાત argparse
આયાત json
આયાત lzma #to create .xz file
આયાત re

parser = argparse.ArgumentParser(description="""Generate the COMPONENT_GENERAL json file""")
parser.add_argument('filename', metavar='component_general.json', help='JSON output file')
parser.add_argument('--compress', action='store_true', help='Add a compressed output file')
parser.add_argument('--type', metavar='type', action="append", મૂળભૂત=[],
        help='Metadata type (<type>,<metadata file>,<uri>,<fallback uri>,[translation uri])')
parser.add_argument('--version-file', metavar='build_git_version.h', help='Git version file')

args = parser.parse_args()
filename = args.filename
compress = args.compress
version_file = args.version_file

version_dir = ''
જો version_file is નહીં None:
    માટે લીટી માં open(version_file, "r"):
        version_શોધ = re.શોધ(r'PX4_GIT_TAG_OR_BRANCH_NAME\s+"(.+)"', લીટી)
        જો version_શોધ:
            version_dir = version_શોધ.group(1)
            તોડો


def save_compressed(filename):
    #create lzma compressed version
    xz_filename=filename+'.xz'
    with lzma.open(xz_filename, 'wt', preset=9) ઇવ f:
        with open(filename, 'r') ઇવ content_file:
            f.લિખ(content_file.પઠ())

component_general = {}
component_general['version'] = 1

def create_table():
    a = []
    માટે i માં range(256):
        k = i
        માટે j માં range(8):
            જો k & 1:
                k ^= 0x1db710640
            k >>= 1
        a.append(k)
    પાછા a

# MAVLink's CRC32
def crc_update(buf, crc_table, crc):
    માટે k માં buf:
        crc = (crc >> 8) ^ crc_table[(crc & 0xff) ^ k]
    પાછા crc

crc_table = create_table()

metadata_types = []
માટે metadata_type_tuple માં args.type:
    type_id, metadata_file, uri, fallback_uri, translation_uri = metadata_type_tuple.split(',')
    file_crc = 0
    માટે લીટી માં open(metadata_file, "rb"):
        file_crc = crc_update(લીટી, crc_table, file_crc)
    json_type = {
            'type': પૂર્ણાંક(type_id),
            'uri': uri.replace('{version}', version_dir),
            'fileCrc': file_crc,
            }
    જો len(fallback_uri) > 0:
        json_type['uriFallback'] = fallback_uri.replace('{version}', version_dir)
    જો len(translation_uri) > 0:
        json_type['translationUri'] = translation_uri
    metadata_types.append(json_type)
component_general['metadataTypes'] = metadata_types

with open(filename, 'w') ઇવ outfile:
    json.dump(component_general, outfile, sort_ચાવીઓ=સાચું)

જો compress:
    save_compressed(filename)
