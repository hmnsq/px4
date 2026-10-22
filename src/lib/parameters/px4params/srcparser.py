واردکن sys
واردکن re
واردکن math

عام default_var
default_var = {}

رده ParameterGroup(object):
    """
    Single parameter group
    """
    def __init__(self, name):
        self.name = name
        self.no_code_generation = نادرست #for injected parameters
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
        بازگشت self.name

    def GetParams(self):
        """
        Returns the parsed list of parameters. Every parameter is a Parameter
        object. Note that returned object is not a copy. Modifications affect
        state of the parser.
        """
        بازگشت sorted(self.params, کلید=بینام param: param.name)

رده Parameter(object):
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

    def __init__(self, name, type, پیشفرض = ""):
        self.fields = {}
        self.مقادیر_2 = {}
        self.bitmask = {}
        self.name = name
        self.type = type
        self.پیشفرض = پیشفرض
        self.category = ""
        self.فرار = نادرست
        self.boolean = نادرست
        self.readonly = نادرست

    def GetName(self):
        بازگشت self.name

    def GetType(self):
        بازگشت self.type

    def GetDefault(self):
        بازگشت self.پیشفرض

    def GetCategory(self):
        بازگشت self.category.title()

    def GetVolatile(self):
        بازگشت self.فرار

    def GetBoolean(self):
        بازگشت self.boolean

    def GetReadonly(self):
        بازگشت self.readonly

    def SetField(self, code, مقدار):
        """
        Set named field value
        """
        self.fields[code] = مقدار

    def Set_Enum_مقدار(self, code, مقدار):
        """
        Set named enum value
        """
        self.مقادیر_2[code] = مقدار

    def SetBitmaskBit(self, index, bit):
        """
        Set named enum value
        """
        self.bitmask[index] = bit

    def SetVolatile(self):
        """
        Set volatile flag
        """
        self.فرار = درست

    def SetBoolean(self):
        """
        Set boolean flag
        """
        self.boolean = درست

    def SetReadonly(self):
        """
        Set readonly flag
        """
        self.readonly = درست

    def SetCategory(self, category):
        """
        Set param category
        """
        self.category = category

    def GetFieldCodes(self):
        """
        Return list of existing field codes in convenient order
        """
        کلیدها = self.fields.کلیدها()
        کلیدها = sorted(کلیدها)
        کلیدها = sorted(کلیدها, کلید=بینام x: self.priority.get(x, 0), reverse=درست)
        بازگشت کلیدها

    def Get_Field_مقدار(self, code):
        """
        Return value of the given field code or None if not found.
        """
        fv =  self.fields.get(code)
        اگر نه fv:
                # required because python 3 sorted does not accept None
                بازگشت ""
        بازگشت fv

    def GetEnumCodes(self):
        """
        Return list of existing value codes in convenient order
        """
        بازگشت sorted(self.مقادیر_2.کلیدها(), کلید=اعشاری)

    def Get_Enum_مقدار(self, code):
        """
        Return value of the given enum code or None if not found.
        """
        fv =  self.مقادیر_2.get(code)
        اگر نه fv:
                # required because python 3 sorted does not accept None
                بازگشت ""
        بازگشت fv

    def GetBitmaskList(self):
        """
        Return list of existing bitmask codes in convenient order
        """
        کلیدها = self.bitmask.کلیدها()
        بازگشت sorted(کلیدها, کلید=اعشاری)

    def GetBitmaskBit(self, index):
        """
        Return value of the given bitmask code or None if not found.
        """
        fv = self.bitmask.get(index)
        اگر نه fv:
                # required because python 3 sorted does not accept None
                بازگشت ""
        بازگشت fv.strip()

رده SourceParser(object):
    """
    Parses provided data and stores all found parameters internally.
    """

    re_split_سطرها = re.compile(r'[\r\n]+')
    re_comment_start = re.compile(r'^\/\*\*')
    re_comment_content = re.compile(r'^\*\s*(.*)')
    re_comment_tag = re.compile(r'@([a-zA-Z][a-zA-Z0-9_]*)\s*(.*)')
    re_comment_end = re.compile(r'(.*?)\s*\*\/')
    re_parameter_definition = re.compile(r'PARAM_DEFINE_([A-Z_][A-Z0-9_]*)\s*\(([A-Z_][A-Z0-9_]*)\s*,\s*([^ ,\)]+)\s*\)\s*;')
    re_px4_parameter_definition = re.compile(r'PX4_PARAM_DEFINE_([A-Z_][A-Z0-9_]*)\s*\(([A-Z_][A-Z0-9_]*)\s*\)\s*;')
    re_px4_param_default_definition = re.compile(r'#define\s*PARAM_([A-Z_][A-Z0-9_]*)\s*([^ ,\)]+)\s*')
    re_cut_type_specifier = re.compile(r'[a-z]+$')
    re_is_a_عدد = re.compile(r'^-?[0-9\.]')
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
        برای سطر در self.re_split_سطرها.split(contents):
            سطر = سطر.strip()
            # Ignore empty lines
            اگر سطر == "":
                ادامه
            اگر self.re_comment_start.match(سطر):
                state = "wait-short"
                short_desc = None
                long_desc = None
                tags = {}
                def_مقادیر = {}
                def_bitmask = {}
            وگرنهاگر state is نه None و state != "comment-processed":
                m = self.re_comment_end.جستجو(سطر)
                اگر m:
                    سطر = m.group(1)
                    last_comment_سطر = درست
                وگرنه:
                    last_comment_سطر = نادرست
                m = self.re_comment_content.match(سطر)
                اگر m:
                    comment_content = m.group(1)
                    اگر comment_content == "":
                        # When short comment ends with empty comment line,
                        # start waiting for the next part - long comment.
                        اگر state == "wait-short-end":
                            state = "wait-long"
                        اگر state == "wait-long-end":
                            # Long description includes empty lines
                            long_desc += "\n"
                    وگرنه:
                        m = self.re_comment_tag.match(comment_content)
                        اگر m:
                            tag, desc = m.group(1, 2)
                            اگر (tag == "value"):
                                # Take the meta info string and split the code and description
                                metainfo = desc.split(" ",  1)
                                def_مقادیر[metainfo[0]] = metainfo[1]
                            وگرنهاگر (tag == "bit"):
                                # Take the meta info string and split the code and description
                                metainfo = desc.split(" ",  1)
                                def_bitmask[metainfo[0]] = metainfo[1]
                            وگرنه:
                                tags[tag] = desc
                            current_tag = tag
                            state = "wait-tag-end"
                        وگرنهاگر state == "wait-short":
                            # Store first line of the short description
                            short_desc = comment_content
                            state = "wait-short-end"
                        وگرنهاگر state == "wait-short-end":
                            # Append comment line to the short description
                            short_desc += "\n" + comment_content
                        وگرنهاگر state == "wait-long":
                            # Store first line of the long description
                            long_desc = comment_content
                            state = "wait-long-end"
                        وگرنهاگر state == "wait-long-end":
                            # Append comment line to the long description
                            long_desc += "\n" + comment_content
                        وگرنهاگر state == "wait-tag-end":
                            # Append comment line to the tag text
                            tags[current_tag] += "\n" + comment_content
                        وگرنه:
                            بالاببر AssertionError(
                                    "Invalid parser state: %s" % state)
                وگرنهاگر نه last_comment_سطر:
                    # Invalid comment line (inside comment, but not starting with
                    # "*" or "*/". Reset parsed content.
                    state = None
                اگر last_comment_سطر:
                    state = "comment-processed"
            وگرنه:
                tp = None
                name = None
                defval = ""
                # Non-empty line outside the comment
                m = self.re_px4_param_default_definition.match(سطر)
                # Default value handling
                اگر m:
                    name_m, defval_m = m.group(1,2)
                    default_var[name_m] = defval_m
                m = self.re_parameter_definition.match(سطر)
                اگر m:
                    tp, name, defval = m.group(1, 2, 3)
                وگرنه:
                    m = self.re_px4_parameter_definition.match(سطر)
                    اگر m:
                        tp, name = m.group(1, 2)
                        اگر (name+'_DEFAULT') در default_var:
                            defval = default_var[name+'_DEFAULT']
                اگر tp is نه None:
                    # Remove trailing type specifier from numbers: 0.1f => 0.1
                    اگر defval != "" و self.re_is_a_عدد.match(defval):
                        defval = self.re_cut_type_specifier.sub('', defval)
                    param = Parameter(name, tp, defval)
                    param.SetField("short_desc", name)
                    # If comment was found before the parameter declaration,
                    # inject its data into the newly created parameter.
                    group = "Miscellaneous"
                    اگر state == "comment-processed":
                        اگر short_desc is نه None:
                            اگر '\n' در short_desc:
                                بالاببر Exception('short description must be a single line (parameter: {:})'.format(name))
                            اگر len(short_desc) > 150:
                                بالاببر Exception('short description too long (150 max, is {:}, parameter: {:})'.format(len(short_desc), name))
                            param.SetField("short_desc", self.re_remove_dots.sub('', short_desc))
                        اگر long_desc is نه None:
                            param.SetField("long_desc", long_desc)
                        برای tag در tags:
                            اگر tag == "group":
                                group = tags[tag]
                            وگرنهاگر tag == "volatile":
                                param.SetVolatile()
                            وگرنهاگر tag == "category":
                                param.SetCategory(tags[tag])
                            وگرنهاگر tag == "boolean":
                                param.SetBoolean()
                            وگرنهاگر tag نه در self.valid_tags:
                                sys.stderr.اكتب("Skipping invalid documentation tag: '%s'\n" % tag)
                                بازگشت نادرست
                            وگرنه:
                                param.SetField(tag, tags[tag])
                        برای def_مقدار در def_مقادیر:
                            param.Set_Enum_مقدار(def_مقدار, def_مقادیر[def_مقدار])
                        برای def_bit در def_bitmask:
                            param.SetBitmaskBit(def_bit, def_bitmask[def_bit])
                    # Store the parameter
                    اگر group نه در self.param_groups:
                        self.param_groups[group] = ParameterGroup(group)
                    self.param_groups[group].AddParameter(param)
                state = None
        بازگشت درست

    def Is_عدد(self, عدد_String):
        تلاش:
            اعشاری(عدد_String)
            بازگشت درست
        استثنا مقدار_Error:
            بازگشت نادرست

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
        برای group در self.GetParamGroups():
            برای param در group.GetParams():
                name  = param.GetName()
                اگر len(name) > 16:
                    sys.stderr.اكتب("Parameter Name {0} is too long (Limit is 16)\n".format(name))
                    بازگشت نادرست
                board = param.Get_Field_مقدار("board")
                # Check for duplicates
                name_plus_board = name + "+" + board
                برای seenParamName در seenParamNames:
                    اگر seenParamName == name_plus_board:
                        sys.stderr.اكتب("Duplicate parameter definition: {0}\n".format(name_plus_board))
                        بازگشت نادرست
                seenParamNames.append(name_plus_board)
                # Validate values
                پیشفرض = param.GetDefault()
                min = param.Get_Field_مقدار("min")
                max = param.Get_Field_مقدار("max")
                units = param.Get_Field_مقدار("unit")
                اگر units نه در allowedUnits:
                    sys.stderr.اكتب("Invalid unit in {0}: {1}\n".format(name, units))
                    بازگشت نادرست
                #sys.stderr.write("{0} default:{1} min:{2} max:{3}\n".format(name, default, min, max))
                اگر پیشفرض != "" و نه self.Is_عدد(پیشفرض):
                    sys.stderr.اكتب("Default value not number: {0} {1}\n".format(name, پیشفرض))
                    بازگشت نادرست
                # if default != "" and "." not in default:
                #     sys.stderr.write("Default value does not contain dot (e.g. 10 needs to be written as 10.0): {0} {1}\n".format(name, default))
                #     return False
                اگر min != "":
                    اگر نه self.Is_عدد(min):
                        sys.stderr.اكتب("Min value not number: {0} {1}\n".format(name, min))
                        بازگشت نادرست
                    اگر پیشفرض != "" و اعشاری(پیشفرض) < اعشاری(min):
                        sys.stderr.اكتب("Default value is smaller than min: {0} default:{1} min:{2}\n".format(name, پیشفرض, min))
                        بازگشت نادرست
                اگر max != "":
                    اگر نه self.Is_عدد(max):
                        sys.stderr.اكتب("Max value not number: {0} {1}\n".format(name, max))
                        بازگشت نادرست
                    اگر پیشفرض != "" و اعشاری(پیشفرض) > اعشاری(max):
                        sys.stderr.اكتب("Default value is larger than max: {0} default:{1} max:{2}\n".format(name, پیشفرض, max))
                        بازگشت نادرست
                برای code در param.GetEnumCodes():
                        اگر نه self.Is_عدد(code):
                            sys.stderr.اكتب("Min value not number: {0} {1}\n".format(name, code))
                            بازگشت نادرست
                        اگر param.Get_Enum_مقدار(code) == "":
                            sys.stderr.اكتب("Description for enum value is empty: {0} {1}\n".format(name, code))
                            بازگشت نادرست
                برای index در param.GetBitmaskList():
                        اگر نه self.Is_عدد(index):
                            sys.stderr.اكتب("bit value not number: {0} {1}\n".format(name, index))
                            بازگشت نادرست
                        اگر نه صحیح(min) <= math.pow(2, صحیح(index)) <= صحیح(max):
                            sys.stderr.اكتب("Bitmask bit must be between {0} and {1}: {2} {3}\n".format(min, max, name, math.pow(2, صحیح(index))))
                            بازگشت نادرست
                        اگر param.GetBitmaskBit(index) == "":
                            sys.stderr.اكتب("Description for bitmask bit is empty: {0} {1}\n".format(name, index))
                            بازگشت نادرست
        بازگشت درست

    def GetParamGroups(self):
        """
        Returns the parsed list of parameters. Every parameter is a Parameter
        object. Note that returned object is not a copy. Modifications affect
        state of the parser.
        """
        groups = self.param_groups.مقادیر_2()
        groups = sorted(groups, کلید=بینام x: x.GetName())
        groups = sorted(groups, کلید=بینام x: self.priority.get(x.GetName(), 0), reverse=درست)
        بازگشت groups
