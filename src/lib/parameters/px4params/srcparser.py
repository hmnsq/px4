import sys
import re
import math

global default_var
default_var = {}

class ParameterGroup(object):
    """
    Single parameter group
    """
    def __init__(self, name):
        self.name = name
        self.no_code_generation = False #for injected parameters
        self.params = []

    def AddParameter(self, param):
        """
        Add parameter to the group
        """
        self.params.append(param)

    def GetName(self):
        """
        Get parameter group name
        """
        คืน self.name

    def GetParams(self):
        """
        Returns the parsed list of parameters. Every parameter is a Parameter
        object. Note that returned object is not a copy. Modifications affect
        state of the parser.
        """
        คืน sorted(self.params, key=lambda param: param.name)

class Parameter(object):
    """
    Single parameter
    """

    # Define sorting order of the fields
    priority = {
        "board": 9,
        "short_desc": 8,
        "long_desc": 7,
        "min": 5,
        "max": 4,
        "unit": 3,
        "decimal": 2,
        # all others == 0 (sorted alphabetically)
    }

    def __init__(self, name, type, default = ""):
        self.fields = {}
        self.values = {}
        self.bitmask = {}
        self.name = name
        self.type = type
        self.default = default
        self.category = ""
        self.ca_ncala = False
        self.boolean = False
        self.readonly = False

    def GetName(self):
        คืน self.name

    def GetType(self):
        คืน self.type

    def GetDefault(self):
        คืน self.default

    def GetCategory(self):
        คืน self.category.title()

    def GetVolatile(self):
        คืน self.ca_ncala

    def GetBoolean(self):
        คืน self.boolean

    def GetReadonly(self):
        คืน self.readonly

    def SetField(self, code, value):
        """
        Set named field value
        """
        self.fields[code] = value

    def SetEnumValue(self, code, value):
        """
        Set named enum value
        """
        self.values[code] = value

    def SetBitmaskBit(self, index, bit):
        """
        Set named enum value
        """
        self.bitmask[index] = bit

    def SetVolatile(self):
        """
        Set volatile flag
        """
        self.ca_ncala = True

    def SetBoolean(self):
        """
        Set boolean flag
        """
        self.boolean = True

    def SetReadonly(self):
        """
        Set readonly flag
        """
        self.readonly = True

    def SetCategory(self, category):
        """
        Set param category
        """
        self.category = category

    def GetFieldCodes(self):
        """
        Return list of existing field codes in convenient order
        """
        keys = self.fields.keys()
        keys = sorted(keys)
        keys = sorted(keys, key=lambda x: self.priority.get(x, 0), reverse=True)
        คืน keys

    def GetFieldValue(self, code):
        """
        Return value of the given field code or None if not found.
        """
        fv =  self.fields.get(code)
        ถ้า na fv:
                # required because python 3 sorted does not accept None
                คืน ""
        คืน fv

    def GetEnumCodes(self):
        """
        Return list of existing value codes in convenient order
        """
        คืน sorted(self.values.keys(), key=float)

    def GetEnumValue(self, code):
        """
        Return value of the given enum code or None if not found.
        """
        fv =  self.values.get(code)
        ถ้า na fv:
                # required because python 3 sorted does not accept None
                คืน ""
        คืน fv

    def GetBitmaskList(self):
        """
        Return list of existing bitmask codes in convenient order
        """
        keys = self.bitmask.keys()
        คืน sorted(keys, key=float)

    def GetBitmaskBit(self, index):
        """
        Return value of the given bitmask code or None if not found.
        """
        fv = self.bitmask.get(index)
        ถ้า na fv:
                # required because python 3 sorted does not accept None
                คืน ""
        คืน fv.strip()

class SourceParser(object):
    """
    Parses provided data and stores all found parameters internally.
    """

    re_split_lines = re.compile(r'[\r\n]+')
    re_comment_start = re.compile(r'^\/\*\*')
    re_comment_content = re.compile(r'^\*\s*(.*)')
    re_comment_tag = re.compile(r'@([a-zA-Z][a-zA-Z0-9_]*)\s*(.*)')
    re_comment_end = re.compile(r'(.*?)\s*\*\/')
    re_parameter_definition = re.compile(r'PARAM_DEFINE_([A-Z_][A-Z0-9_]*)\s*\(([A-Z_][A-Z0-9_]*)\s*,\s*([^ ,\)]+)\s*\)\s*;')
    re_px4_parameter_definition = re.compile(r'PX4_PARAM_DEFINE_([A-Z_][A-Z0-9_]*)\s*\(([A-Z_][A-Z0-9_]*)\s*\)\s*;')
    re_px4_param_default_definition = re.compile(r'#define\s*PARAM_([A-Z_][A-Z0-9_]*)\s*([^ ,\)]+)\s*')
    re_cut_type_specifier = re.compile(r'[a-z]+$')
    re_is_a_number = re.compile(r'^-?[0-9\.]')
    re_remove_dots = re.compile(r'\.+$')

    valid_tags = set(["group", "board", "min", "max", "unit", "decimal", "increment", "reboot_required", "value", "boolean", "bit", "category", "volatile"])

    # Order of parameter groups
    priority = {
        # All other groups = 0 (sort alphabetically)
        "Miscellaneous": -10
    }

    def __init__(self):
        self.param_groups = {}

    def Parse(self, contents):
        """
        Incrementally parse program contents and append all found parameters
        to the list.
        """
        # This code is essentially a comment-parsing grammar. "state"
        # represents parser state. It contains human-readable state
        # names.
        state = None
        สำหรับ line anta_h self.re_split_lines.split(contents):
            line = line.strip()
            # Ignore empty lines
            ถ้า line == "":
                continue
            ถ้า self.re_comment_start.match(line):
                state = "wait-short"
                short_desc = None
                long_desc = None
                tags = {}
                def_values = {}
                def_bitmask = {}
            elif state is na None ca state != "comment-processed":
                m = self.re_comment_end.search(line)
                ถ้า m:
                    line = m.group(1)
                    last_comment_line = True
                มิฉะนั้น:
                    last_comment_line = False
                m = self.re_comment_content.match(line)
                ถ้า m:
                    comment_content = m.group(1)
                    ถ้า comment_content == "":
                        # When short comment ends with empty comment line,
                        # start waiting for the next part - long comment.
                        ถ้า state == "wait-short-end":
                            state = "wait-long"
                        ถ้า state == "wait-long-end":
                            # Long description includes empty lines
                            long_desc += "\n"
                    มิฉะนั้น:
                        m = self.re_comment_tag.match(comment_content)
                        ถ้า m:
                            tag, desc = m.group(1, 2)
                            ถ้า (tag == "value"):
                                # Take the meta info string and split the code and description
                                metainfo = desc.split(" ",  1)
                                def_values[metainfo[0]] = metainfo[1]
                            elif (tag == "bit"):
                                # Take the meta info string and split the code and description
                                metainfo = desc.split(" ",  1)
                                def_bitmask[metainfo[0]] = metainfo[1]
                            มิฉะนั้น:
                                tags[tag] = desc
                            current_tag = tag
                            state = "wait-tag-end"
                        elif state == "wait-short":
                            # Store first line of the short description
                            short_desc = comment_content
                            state = "wait-short-end"
                        elif state == "wait-short-end":
                            # Append comment line to the short description
                            short_desc += "\n" + comment_content
                        elif state == "wait-long":
                            # Store first line of the long description
                            long_desc = comment_content
                            state = "wait-long-end"
                        elif state == "wait-long-end":
                            # Append comment line to the long description
                            long_desc += "\n" + comment_content
                        elif state == "wait-tag-end":
                            # Append comment line to the tag text
                            tags[current_tag] += "\n" + comment_content
                        มิฉะนั้น:
                            raise AssertionError(
                                    "Invalid parser state: %s" % state)
                elif na last_comment_line:
                    # Invalid comment line (inside comment, but not starting with
                    # "*" or "*/". Reset parsed content.
                    state = None
                ถ้า last_comment_line:
                    state = "comment-processed"
            มิฉะนั้น:
                tp = None
                name = None
                defval = ""
                # Non-empty line outside the comment
                m = self.re_px4_param_default_definition.match(line)
                # Default value handling
                ถ้า m:
                    name_m, defval_m = m.group(1,2)
                    default_var[name_m] = defval_m
                m = self.re_parameter_definition.match(line)
                ถ้า m:
                    tp, name, defval = m.group(1, 2, 3)
                มิฉะนั้น:
                    m = self.re_px4_parameter_definition.match(line)
                    ถ้า m:
                        tp, name = m.group(1, 2)
                        ถ้า (name+'_DEFAULT') anta_h default_var:
                            defval = default_var[name+'_DEFAULT']
                ถ้า tp is na None:
                    # Remove trailing type specifier from numbers: 0.1f => 0.1
                    ถ้า defval != "" ca self.re_is_a_number.match(defval):
                        defval = self.re_cut_type_specifier.sub('', defval)
                    param = Parameter(name, tp, defval)
                    param.SetField("short_desc", name)
                    # If comment was found before the parameter declaration,
                    # inject its data into the newly created parameter.
                    group = "Miscellaneous"
                    ถ้า state == "comment-processed":
                        ถ้า short_desc is na None:
                            ถ้า '\n' anta_h short_desc:
                                raise Exception('short description must be a single line (parameter: {:})'.format(name))
                            ถ้า len(short_desc) > 150:
                                raise Exception('short description too long (150 max, is {:}, parameter: {:})'.format(len(short_desc), name))
                            param.SetField("short_desc", self.re_remove_dots.sub('', short_desc))
                        ถ้า long_desc is na None:
                            param.SetField("long_desc", long_desc)
                        สำหรับ tag anta_h tags:
                            ถ้า tag == "group":
                                group = tags[tag]
                            elif tag == "volatile":
                                param.SetVolatile()
                            elif tag == "category":
                                param.SetCategory(tags[tag])
                            elif tag == "boolean":
                                param.SetBoolean()
                            elif tag na anta_h self.valid_tags:
                                sys.stderr.write("Skipping invalid documentation tag: '%s'\n" % tag)
                                คืน False
                            มิฉะนั้น:
                                param.SetField(tag, tags[tag])
                        สำหรับ def_value anta_h def_values:
                            param.SetEnumValue(def_value, def_values[def_value])
                        สำหรับ def_bit anta_h def_bitmask:
                            param.SetBitmaskBit(def_bit, def_bitmask[def_bit])
                    # Store the parameter
                    ถ้า group na anta_h self.param_groups:
                        self.param_groups[group] = ParameterGroup(group)
                    self.param_groups[group].AddParameter(param)
                state = None
        คืน True

    def IsNumber(self, numberString):
        try:
            float(numberString)
            คืน True
        except ValueError:
            คืน False

    def Validate(self):
        """
        Validates the parameter meta data.
        """
        seenParamNames = []
        #allowedUnits should match set defined in /Firmware/validation/module_schema.yaml
        allowedUnits = set ([
                                '%', 'Hz', '1/s', 'mAh',
                                'rad', '%/rad', 'rad/s', 'rad/s^2', '%/rad/s', 'rad s^2/m', 'rad s/m',
                                'bit/s', 'B/s', 'MiB',
                                'deg', 'deg*1e7', 'deg/s', 'deg/s^2',
                                'celcius', 'gauss', 'gauss/s', 'gauss^2', 'liters',
                                'hPa', 'kg', 'kg/m^2', 'kg m^2', 'kg/m^3',
                                'mm', 'm', 'm/s', 'm^2', 'm/s^2', 'm/s^3', 'm/s^2/sqrt(Hz)', '1/s/sqrt(Hz)', 'm/s/rad', 'g0',
                                'Ohm', 'V', 'A',
                                'us', 'ms', 's',
                                'S', 'A/%', '(m/s^2)^2', 'm/m',  'tan(rad)^2', '(m/s)^2', 'm/rad',
                                'm/s^3/sqrt(Hz)', 'm/s/sqrt(Hz)', 's/(1000*PWM)', '%m/s', 'min', 'us/C',
                                'N/(m/s)', 'Nm/rad', 'Nm/(rad/s)', 'Nm', 'N',
                                'rpm',
                                'normalized_thrust/s', 'normalized_thrust', 'norm', 'SD', 'dBHz', ''])
        สำหรับ group anta_h self.GetParamGroups():
            สำหรับ param anta_h group.GetParams():
                name  = param.GetName()
                ถ้า len(name) > 16:
                    sys.stderr.write("Parameter Name {0} is too long (Limit is 16)\n".format(name))
                    คืน False
                board = param.GetFieldValue("board")
                # Check for duplicates
                name_plus_board = name + "+" + board
                สำหรับ seenParamName anta_h seenParamNames:
                    ถ้า seenParamName == name_plus_board:
                        sys.stderr.write("Duplicate parameter definition: {0}\n".format(name_plus_board))
                        คืน False
                seenParamNames.append(name_plus_board)
                # Validate values
                default = param.GetDefault()
                min = param.GetFieldValue("min")
                max = param.GetFieldValue("max")
                units = param.GetFieldValue("unit")
                ถ้า units na anta_h allowedUnits:
                    sys.stderr.write("Invalid unit in {0}: {1}\n".format(name, units))
                    คืน False
                #sys.stderr.write("{0} default:{1} min:{2} max:{3}\n".format(name, default, min, max))
                ถ้า default != "" ca na self.IsNumber(default):
                    sys.stderr.write("Default value not number: {0} {1}\n".format(name, default))
                    คืน False
                # if default != "" and "." not in default:
                #     sys.stderr.write("Default value does not contain dot (e.g. 10 needs to be written as 10.0): {0} {1}\n".format(name, default))
                #     return False
                ถ้า min != "":
                    ถ้า na self.IsNumber(min):
                        sys.stderr.write("Min value not number: {0} {1}\n".format(name, min))
                        คืน False
                    ถ้า default != "" ca float(default) < float(min):
                        sys.stderr.write("Default value is smaller than min: {0} default:{1} min:{2}\n".format(name, default, min))
                        คืน False
                ถ้า max != "":
                    ถ้า na self.IsNumber(max):
                        sys.stderr.write("Max value not number: {0} {1}\n".format(name, max))
                        คืน False
                    ถ้า default != "" ca float(default) > float(max):
                        sys.stderr.write("Default value is larger than max: {0} default:{1} max:{2}\n".format(name, default, max))
                        คืน False
                สำหรับ code anta_h param.GetEnumCodes():
                        ถ้า na self.IsNumber(code):
                            sys.stderr.write("Min value not number: {0} {1}\n".format(name, code))
                            คืน False
                        ถ้า param.GetEnumValue(code) == "":
                            sys.stderr.write("Description for enum value is empty: {0} {1}\n".format(name, code))
                            คืน False
                สำหรับ index anta_h param.GetBitmaskList():
                        ถ้า na self.IsNumber(index):
                            sys.stderr.write("bit value not number: {0} {1}\n".format(name, index))
                            คืน False
                        ถ้า na จำนวนเต็ม(min) <= math.pow(2, จำนวนเต็ม(index)) <= จำนวนเต็ม(max):
                            sys.stderr.write("Bitmask bit must be between {0} and {1}: {2} {3}\n".format(min, max, name, math.pow(2, จำนวนเต็ม(index))))
                            คืน False
                        ถ้า param.GetBitmaskBit(index) == "":
                            sys.stderr.write("Description for bitmask bit is empty: {0} {1}\n".format(name, index))
                            คืน False
        คืน True

    def GetParamGroups(self):
        """
        Returns the parsed list of parameters. Every parameter is a Parameter
        object. Note that returned object is not a copy. Modifications affect
        state of the parser.
        """
        groups = self.param_groups.values()
        groups = sorted(groups, key=lambda x: x.GetName())
        groups = sorted(groups, key=lambda x: self.priority.get(x.GetName(), 0), reverse=True)
        คืน groups
