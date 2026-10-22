استورد sys
استورد re
استورد math

عام default_var
default_var = {}

מחלקה ParameterGroup(object):
    """
    Single parameter group
    """
    def __init__(self, name):
        self.name = name
        self.no_code_generation = שקר #for injected parameters
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
        החזר self.name

    def GetParams(self):
        """
        Returns the parsed list of parameters. Every parameter is a Parameter
        object. Note that returned object is not a copy. Modifications affect
        state of the parser.
        """
        החזר sorted(self.params, key=مجهول param: param.name)

מחלקה Parameter(object):
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

    def __init__(self, name, type, افتراضي = ""):
        self.fields = {}
        self.values = {}
        self.bitmask = {}
        self.name = name
        self.type = type
        self.افتراضي = افتراضي
        self.category = ""
        self.متطاير = שקר
        self.boolean = שקר
        self.readonly = שקר

    def GetName(self):
        החזר self.name

    def GetType(self):
        החזר self.type

    def GetDefault(self):
        החזר self.افتراضي

    def GetCategory(self):
        החזר self.category.title()

    def GetVolatile(self):
        החזר self.متطاير

    def GetBoolean(self):
        החזר self.boolean

    def GetReadonly(self):
        החזר self.readonly

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
        self.متطاير = אמת

    def SetBoolean(self):
        """
        Set boolean flag
        """
        self.boolean = אמת

    def SetReadonly(self):
        """
        Set readonly flag
        """
        self.readonly = אמת

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
        keys = sorted(keys, key=مجهول x: self.priority.get(x, 0), reverse=אמת)
        החזר keys

    def GetFieldValue(self, code):
        """
        Return value of the given field code or None if not found.
        """
        fv =  self.fields.get(code)
        אם לא fv:
                # required because python 3 sorted does not accept None
                החזר ""
        החזר fv

    def GetEnumCodes(self):
        """
        Return list of existing value codes in convenient order
        """
        החזר sorted(self.values.keys(), key=عائم)

    def GetEnumValue(self, code):
        """
        Return value of the given enum code or None if not found.
        """
        fv =  self.values.get(code)
        אם לא fv:
                # required because python 3 sorted does not accept None
                החזר ""
        החזר fv

    def GetBitmaskList(self):
        """
        Return list of existing bitmask codes in convenient order
        """
        keys = self.bitmask.keys()
        החזר sorted(keys, key=عائم)

    def GetBitmaskBit(self, index):
        """
        Return value of the given bitmask code or None if not found.
        """
        fv = self.bitmask.get(index)
        אם לא fv:
                # required because python 3 sorted does not accept None
                החזר ""
        החזר fv.strip()

מחלקה SourceParser(object):
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
        עבור line في self.re_split_lines.split(contents):
            line = line.strip()
            # Ignore empty lines
            אם line == "":
                استمر
            אם self.re_comment_start.match(line):
                state = "wait-short"
                short_desc = None
                long_desc = None
                tags = {}
                def_values = {}
                def_bitmask = {}
            وإلاإذا state is לא None וגם state != "comment-processed":
                m = self.re_comment_end.search(line)
                אם m:
                    line = m.group(1)
                    last_comment_line = אמת
                אחרת:
                    last_comment_line = שקר
                m = self.re_comment_content.match(line)
                אם m:
                    comment_content = m.group(1)
                    אם comment_content == "":
                        # When short comment ends with empty comment line,
                        # start waiting for the next part - long comment.
                        אם state == "wait-short-end":
                            state = "wait-long"
                        אם state == "wait-long-end":
                            # Long description includes empty lines
                            long_desc += "\n"
                    אחרת:
                        m = self.re_comment_tag.match(comment_content)
                        אם m:
                            tag, desc = m.group(1, 2)
                            אם (tag == "value"):
                                # Take the meta info string and split the code and description
                                metainfo = desc.split(" ",  1)
                                def_values[metainfo[0]] = metainfo[1]
                            وإلاإذا (tag == "bit"):
                                # Take the meta info string and split the code and description
                                metainfo = desc.split(" ",  1)
                                def_bitmask[metainfo[0]] = metainfo[1]
                            אחרת:
                                tags[tag] = desc
                            current_tag = tag
                            state = "wait-tag-end"
                        وإلاإذا state == "wait-short":
                            # Store first line of the short description
                            short_desc = comment_content
                            state = "wait-short-end"
                        وإلاإذا state == "wait-short-end":
                            # Append comment line to the short description
                            short_desc += "\n" + comment_content
                        وإلاإذا state == "wait-long":
                            # Store first line of the long description
                            long_desc = comment_content
                            state = "wait-long-end"
                        وإلاإذا state == "wait-long-end":
                            # Append comment line to the long description
                            long_desc += "\n" + comment_content
                        وإلاإذا state == "wait-tag-end":
                            # Append comment line to the tag text
                            tags[current_tag] += "\n" + comment_content
                        אחרת:
                            ارفع AssertionError(
                                    "Invalid parser state: %s" % state)
                وإلاإذا לא last_comment_line:
                    # Invalid comment line (inside comment, but not starting with
                    # "*" or "*/". Reset parsed content.
                    state = None
                אם last_comment_line:
                    state = "comment-processed"
            אחרת:
                tp = None
                name = None
                defval = ""
                # Non-empty line outside the comment
                m = self.re_px4_param_default_definition.match(line)
                # Default value handling
                אם m:
                    name_m, defval_m = m.group(1,2)
                    default_var[name_m] = defval_m
                m = self.re_parameter_definition.match(line)
                אם m:
                    tp, name, defval = m.group(1, 2, 3)
                אחרת:
                    m = self.re_px4_parameter_definition.match(line)
                    אם m:
                        tp, name = m.group(1, 2)
                        אם (name+'_DEFAULT') في default_var:
                            defval = default_var[name+'_DEFAULT']
                אם tp is לא None:
                    # Remove trailing type specifier from numbers: 0.1f => 0.1
                    אם defval != "" וגם self.re_is_a_number.match(defval):
                        defval = self.re_cut_type_specifier.sub('', defval)
                    param = Parameter(name, tp, defval)
                    param.SetField("short_desc", name)
                    # If comment was found before the parameter declaration,
                    # inject its data into the newly created parameter.
                    group = "Miscellaneous"
                    אם state == "comment-processed":
                        אם short_desc is לא None:
                            אם '\n' في short_desc:
                                ارفع Exception('short description must be a single line (parameter: {:})'.format(name))
                            אם len(short_desc) > 150:
                                ارفع Exception('short description too long (150 max, is {:}, parameter: {:})'.format(len(short_desc), name))
                            param.SetField("short_desc", self.re_remove_dots.sub('', short_desc))
                        אם long_desc is לא None:
                            param.SetField("long_desc", long_desc)
                        עבור tag في tags:
                            אם tag == "group":
                                group = tags[tag]
                            وإلاإذا tag == "volatile":
                                param.SetVolatile()
                            وإلاإذا tag == "category":
                                param.SetCategory(tags[tag])
                            وإلاإذا tag == "boolean":
                                param.SetBoolean()
                            وإلاإذا tag לא في self.valid_tags:
                                sys.stderr.اكتب("Skipping invalid documentation tag: '%s'\n" % tag)
                                החזר שקר
                            אחרת:
                                param.SetField(tag, tags[tag])
                        עבור def_value في def_values:
                            param.SetEnumValue(def_value, def_values[def_value])
                        עבור def_bit في def_bitmask:
                            param.SetBitmaskBit(def_bit, def_bitmask[def_bit])
                    # Store the parameter
                    אם group לא في self.param_groups:
                        self.param_groups[group] = ParameterGroup(group)
                    self.param_groups[group].AddParameter(param)
                state = None
        החזר אמת

    def IsNumber(self, numberString):
        حاول:
            عائم(numberString)
            החזר אמת
        استثناء ValueError:
            החזר שקר

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
        עבור group في self.GetParamGroups():
            עבור param في group.GetParams():
                name  = param.GetName()
                אם len(name) > 16:
                    sys.stderr.اكتب("Parameter Name {0} is too long (Limit is 16)\n".format(name))
                    החזר שקר
                board = param.GetFieldValue("board")
                # Check for duplicates
                name_plus_board = name + "+" + board
                עבור seenParamName في seenParamNames:
                    אם seenParamName == name_plus_board:
                        sys.stderr.اكتب("Duplicate parameter definition: {0}\n".format(name_plus_board))
                        החזר שקר
                seenParamNames.append(name_plus_board)
                # Validate values
                افتراضي = param.GetDefault()
                min = param.GetFieldValue("min")
                max = param.GetFieldValue("max")
                units = param.GetFieldValue("unit")
                אם units לא في allowedUnits:
                    sys.stderr.اكتب("Invalid unit in {0}: {1}\n".format(name, units))
                    החזר שקר
                #sys.stderr.write("{0} default:{1} min:{2} max:{3}\n".format(name, default, min, max))
                אם افتراضي != "" וגם לא self.IsNumber(افتراضي):
                    sys.stderr.اكتب("Default value not number: {0} {1}\n".format(name, افتراضي))
                    החזר שקר
                # if default != "" and "." not in default:
                #     sys.stderr.write("Default value does not contain dot (e.g. 10 needs to be written as 10.0): {0} {1}\n".format(name, default))
                #     return False
                אם min != "":
                    אם לא self.IsNumber(min):
                        sys.stderr.اكتب("Min value not number: {0} {1}\n".format(name, min))
                        החזר שקר
                    אם افتراضي != "" וגם عائم(افتراضي) < عائم(min):
                        sys.stderr.اكتب("Default value is smaller than min: {0} default:{1} min:{2}\n".format(name, افتراضي, min))
                        החזר שקר
                אם max != "":
                    אם לא self.IsNumber(max):
                        sys.stderr.اكتب("Max value not number: {0} {1}\n".format(name, max))
                        החזר שקר
                    אם افتراضي != "" וגם عائم(افتراضي) > عائم(max):
                        sys.stderr.اكتب("Default value is larger than max: {0} default:{1} max:{2}\n".format(name, افتراضي, max))
                        החזר שקר
                עבור code في param.GetEnumCodes():
                        אם לא self.IsNumber(code):
                            sys.stderr.اكتب("Min value not number: {0} {1}\n".format(name, code))
                            החזר שקר
                        אם param.GetEnumValue(code) == "":
                            sys.stderr.اكتب("Description for enum value is empty: {0} {1}\n".format(name, code))
                            החזר שקר
                עבור index في param.GetBitmaskList():
                        אם לא self.IsNumber(index):
                            sys.stderr.اكتب("bit value not number: {0} {1}\n".format(name, index))
                            החזר שקר
                        אם לא صحيح(min) <= math.pow(2, صحيح(index)) <= صحيح(max):
                            sys.stderr.اكتب("Bitmask bit must be between {0} and {1}: {2} {3}\n".format(min, max, name, math.pow(2, صحيح(index))))
                            החזר שקר
                        אם param.GetBitmaskBit(index) == "":
                            sys.stderr.اكتب("Description for bitmask bit is empty: {0} {1}\n".format(name, index))
                            החזר שקר
        החזר אמת

    def GetParamGroups(self):
        """
        Returns the parsed list of parameters. Every parameter is a Parameter
        object. Note that returned object is not a copy. Modifications affect
        state of the parser.
        """
        groups = self.param_groups.values()
        groups = sorted(groups, key=مجهول x: x.GetName())
        groups = sorted(groups, key=مجهول x: self.priority.get(x.GetName(), 0), reverse=אמת)
        החזר groups
