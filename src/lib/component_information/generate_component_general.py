#!/usr/bin/env python

استورد argparse
استورد json
استورد lzma #to create .xz file
استورد re

parser = argparse.دلیل_Parser(description="""Generate the COMPONENT_GENERAL json file""")
parser.جمع_کړه_دلیل('filename', metavar='component_general.json', help='JSON output file')
parser.جمع_کړه_دلیل('--compress', action='store_true', help='Add a compressed output file')
parser.جمع_کړه_دلیل('--type', metavar='type', action="append", افتراضي=[],
        help='Metadata type (<type>,<metadata file>,<uri>,<fallback uri>,[translation uri])')
parser.جمع_کړه_دلیل('--version-file', metavar='build_git_version.h', help='Git version file')

دلیلونه = parser.parse_دلیلونه()
filename = دلیلونه.filename
compress = دلیلونه.compress
نسخه_file = دلیلونه.نسخه_file

نسخه_dir = ''
إذا نسخه_file دی ليس هیڅ:
    لكل کرښه في open(نسخه_file, "r"):
        نسخه_پلټنه = re.پلټنه(r'PX4_GIT_TAG_OR_BRANCH_NAME\s+"(.+)"', کرښه)
        إذا نسخه_پلټنه:
            نسخه_dir = نسخه_پلټنه.ډله(1)
            اكسر


تعریف save_compressed(filename):
    #create lzma compressed version
    xz_filename=filename+'.xz'
    سره lzma.open(xz_filename, 'wt', preset=9) كـ f:
        سره open(filename, 'r') كـ content_file:
            f.اكتب(content_file.اقرأ())

component_general = {}
component_general['version'] = 1

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

metadata_types = []
لكل metadata_type_tuple في دلیلونه.type:
    type_id, metadata_file, uri, fallback_uri, translation_uri = metadata_type_tuple.وېشل(',')
    file_دوراني_ازموینه = 0
    لكل کرښه في open(metadata_file, "rb"):
        file_دوراني_ازموینه = دوراني_ازموینه_update(کرښه, دوراني_ازموینه_جدول, file_دوراني_ازموینه)
    json_type = {
            'type': صحيح(type_id),
            'uri': uri.replace('{version}', نسخه_dir),
            'fileCrc': file_دوراني_ازموینه,
            }
    إذا اوږدوالی(fallback_uri) > 0:
        json_type['uriFallback'] = fallback_uri.replace('{version}', نسخه_dir)
    إذا اوږدوالی(translation_uri) > 0:
        json_type['translationUri'] = translation_uri
    metadata_types.append(json_type)
component_general['metadataTypes'] = metadata_types

سره open(filename, 'w') كـ outfile:
    json.dump(component_general, outfile, ترتیبول_کلیګانې=ریښتیا)

إذا compress:
    save_compressed(filename)
