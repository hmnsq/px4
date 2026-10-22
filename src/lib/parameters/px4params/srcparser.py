استورد sys
استورد re
استورد math

عام default_var
default_var = {}

صنف Parameter_گروه(object):
    """
    Single parameter group
    """
    تعریف __init__(self, name):
        self.name = name
        self.no_کد_generation = خطأ #for injected parameters
        self.params = []

    تعریف جمع_کن_Parameter(self, param):
        """
        Add parameter to the group
        """
        self.params.append(param)

    تعریف GetName(self):
        """
        Get parameter group name
        """
        أرجع self.name

    تعریف GetParams(self):
        """
        Returns the parsed list of parameters. Every parameter is a Parameter
        object. Note that returned object is not a copy. Modifications affect
        state of the parser.
        """
        أرجع sorted(self.params, کلید=مجهول param: param.name)

صنف Parameter(object):
    """
    Single parameter
    """

    # Define sorting order of the fields
    اولویت = {
        "board": 9,
        "short_desc": 8,
        "long_desc": 7,
        "min": 5,
        "max": 4,
        "unit": 3,
        "decimal": 2,
        # all others == 0 (sorted alphabetically)
    }

    تعریف __init__(self, name, type, افتراضي = ""):
        self.fields = {}
        self.مقادیر_2 = {}
        self.bitmask = {}
        self.name = name
        self.type = type
        self.افتراضي = افتراضي
        self.category = ""
        self.متطاير = خطأ
        self.منطقی = خطأ
        self.readonly = خطأ

    تعریف GetName(self):
        أرجع self.name

    تعریف GetType(self):
        أرجع self.type

    تعریف GetDefault(self):
        أرجع self.افتراضي

    تعریف GetCategory(self):
        أرجع self.category.title()

    تعریف GetVolatile(self):
        أرجع self.متطاير

    تعریف GetBoolean(self):
        أرجع self.منطقی

    تعریف GetReadonly(self):
        أرجع self.readonly

    تعریف مجموعه_فیلد_2(self, کد, مقدار):
        """
        Set named field value
        """
        self.fields[کد] = مقدار

    تعریف مجموعه_Enum_مقدار(self, کد, مقدار):
        """
        Set named enum value
        """
        self.مقادیر_2[کد] = مقدار

    تعریف مجموعه_Bitmask_بیت(self, شاخص, بیت):
        """
        Set named enum value
        """
        self.bitmask[شاخص] = بیت

    تعریف مجموعه_Volatile(self):
        """
        Set volatile flag
        """
        self.متطاير = درست

    تعریف مجموعه_Boolean(self):
        """
        Set boolean flag
        """
        self.منطقی = درست

    تعریف مجموعه_Readonly(self):
        """
        Set readonly flag
        """
        self.readonly = درست

    تعریف مجموعه_Category(self, category):
        """
        Set param category
        """
        self.category = category

    تعریف Get_فیلد_Codes(self):
        """
        Return list of existing field codes in convenient order
        """
        کلیدها = self.fields.کلیدها()
        کلیدها = sorted(کلیدها)
        کلیدها = sorted(کلیدها, کلید=مجهول x: self.اولویت.get(x, 0), معکوس=درست)
        أرجع کلیدها

    تعریف Get_فیلد_مقدار(self, کد):
        """
        Return value of the given field code or None if not found.
        """
        fv =  self.fields.get(کد)
        إذا ليس fv:
                # required because python 3 sorted does not accept None
                أرجع ""
        أرجع fv

    تعریف GetEnumCodes(self):
        """
        Return list of existing value codes in convenient order
        """
        أرجع sorted(self.مقادیر_2.کلیدها(), کلید=عائم)

    تعریف Get_Enum_مقدار(self, کد):
        """
        Return value of the given enum code or None if not found.
        """
        fv =  self.مقادیر_2.get(کد)
        إذا ليس fv:
                # required because python 3 sorted does not accept None
                أرجع ""
        أرجع fv

    تعریف GetBitmaskList(self):
        """
        Return list of existing bitmask codes in convenient order
        """
        کلیدها = self.bitmask.کلیدها()
        أرجع sorted(کلیدها, کلید=عائم)

    تعریف Get_Bitmask_بیت(self, شاخص):
        """
        Return value of the given bitmask code or None if not found.
        """
        fv = self.bitmask.get(شاخص)
        إذا ليس fv:
                # required because python 3 sorted does not accept None
                أرجع ""
        أرجع fv.strip()

صنف منبع_Parser(object):
    """
    Parses provided data and stores all found parameters internally.
    """

    re_تقسیم_سطرها = re.compile(r'[\r\n]+')
    re_comment_شروع = re.compile(r'^\/\*\*')
    re_comment_content = re.compile(r'^\*\s*(.*)')
    re_comment_برچسب = re.compile(r'@([a-zA-Z][a-zA-Z0-9_]*)\s*(.*)')
    re_comment_انتها = re.compile(r'(.*?)\s*\*\/')
    re_parameter_definition = re.compile(r'PARAM_DEFINE_([A-Z_][A-Z0-9_]*)\s*\(([A-Z_][A-Z0-9_]*)\s*,\s*([^ ,\)]+)\s*\)\s*;')
    re_px4_parameter_definition = re.compile(r'PX4_PARAM_DEFINE_([A-Z_][A-Z0-9_]*)\s*\(([A-Z_][A-Z0-9_]*)\s*\)\s*;')
    re_px4_param_default_definition = re.compile(r'#define\s*PARAM_([A-Z_][A-Z0-9_]*)\s*([^ ,\)]+)\s*')
    re_cut_type_specifier = re.compile(r'[a-z]+$')
    re_is_a_عدد = re.compile(r'^-?[0-9\.]')
    re_remove_dots = re.compile(r'\.+$')

    valid_برچسبها = مجموعه(["group", "board", "min", "max", "unit", "decimal", "increment", "reboot_required", "value", "boolean", "bit", "category", "volatile"])

    # Order of parameter groups
    اولویت = {
        # All other groups = 0 (sort alphabetically)
        "Miscellaneous": -10
    }

    تعریف __init__(self):
        self.param_groups = {}

    تعریف Parse(self, contents):
        """
        Incrementally parse program contents and append all found parameters
        to the list.
        """
        # This code is essentially a comment-parsing grammar. "state"
        # represents parser state. It contains human-readable state
        # names.
        state = هیچ
        لكل سطر في self.re_تقسیم_سطرها.تقسیم(contents):
            سطر = سطر.strip()
            # Ignore empty lines
            إذا سطر == "":
                استمر
            إذا self.re_comment_شروع.تطابق(سطر):
                state = "wait-short"
                short_desc = هیچ
                long_desc = هیچ
                برچسبها = {}
                def_مقادیر = {}
                def_bitmask = {}
            وإلاإذا state است ليس هیچ و state != "comment-processed":
                m = self.re_comment_انتها.جستجو(سطر)
                إذا m:
                    سطر = m.گروه(1)
                    آخرین_comment_سطر = درست
                وإلا:
                    آخرین_comment_سطر = خطأ
                m = self.re_comment_content.تطابق(سطر)
                إذا m:
                    comment_content = m.گروه(1)
                    إذا comment_content == "":
                        # When short comment ends with empty comment line,
                        # start waiting for the next part - long comment.
                        إذا state == "wait-short-end":
                            state = "wait-long"
                        إذا state == "wait-long-end":
                            # Long description includes empty lines
                            long_desc += "\n"
                    وإلا:
                        m = self.re_comment_برچسب.تطابق(comment_content)
                        إذا m:
                            برچسب, desc = m.گروه(1, 2)
                            إذا (برچسب == "value"):
                                # Take the meta info string and split the code and description
                                metainfo = desc.تقسیم(" ",  1)
                                def_مقادیر[metainfo[0]] = metainfo[1]
                            وإلاإذا (برچسب == "bit"):
                                # Take the meta info string and split the code and description
                                metainfo = desc.تقسیم(" ",  1)
                                def_bitmask[metainfo[0]] = metainfo[1]
                            وإلا:
                                برچسبها[برچسب] = desc
                            current_برچسب = برچسب
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
                            برچسبها[current_برچسب] += "\n" + comment_content
                        وإلا:
                            ارفع AssertionError(
                                    "Invalid parser state: %s" % state)
                وإلاإذا ليس آخرین_comment_سطر:
                    # Invalid comment line (inside comment, but not starting with
                    # "*" or "*/". Reset parsed content.
                    state = هیچ
                إذا آخرین_comment_سطر:
                    state = "comment-processed"
            وإلا:
                tp = هیچ
                name = هیچ
                defval = ""
                # Non-empty line outside the comment
                m = self.re_px4_param_default_definition.تطابق(سطر)
                # Default value handling
                إذا m:
                    name_m, defval_m = m.گروه(1,2)
                    default_var[name_m] = defval_m
                m = self.re_parameter_definition.تطابق(سطر)
                إذا m:
                    tp, name, defval = m.گروه(1, 2, 3)
                وإلا:
                    m = self.re_px4_parameter_definition.تطابق(سطر)
                    إذا m:
                        tp, name = m.گروه(1, 2)
                        إذا (name+'_DEFAULT') في default_var:
                            defval = default_var[name+'_DEFAULT']
                إذا tp است ليس هیچ:
                    # Remove trailing type specifier from numbers: 0.1f => 0.1
                    إذا defval != "" و self.re_is_a_عدد.تطابق(defval):
                        defval = self.re_cut_type_specifier.sub('', defval)
                    param = Parameter(name, tp, defval)
                    param.مجموعه_فیلد_2("short_desc", name)
                    # If comment was found before the parameter declaration,
                    # inject its data into the newly created parameter.
                    گروه = "Miscellaneous"
                    إذا state == "comment-processed":
                        إذا short_desc است ليس هیچ:
                            إذا '\n' في short_desc:
                                ارفع Exception('short description must be a single line (parameter: {:})'.format(name))
                            إذا طول(short_desc) > 150:
                                ارفع Exception('short description too long (150 max, is {:}, parameter: {:})'.format(طول(short_desc), name))
                            param.مجموعه_فیلد_2("short_desc", self.re_remove_dots.sub('', short_desc))
                        إذا long_desc است ليس هیچ:
                            param.مجموعه_فیلد_2("long_desc", long_desc)
                        لكل برچسب في برچسبها:
                            إذا برچسب == "group":
                                گروه = برچسبها[برچسب]
                            وإلاإذا برچسب == "volatile":
                                param.مجموعه_Volatile()
                            وإلاإذا برچسب == "category":
                                param.مجموعه_Category(برچسبها[برچسب])
                            وإلاإذا برچسب == "boolean":
                                param.مجموعه_Boolean()
                            وإلاإذا برچسب ليس في self.valid_برچسبها:
                                sys.stderr.اكتب("Skipping invalid documentation tag: '%s'\n" % برچسب)
                                أرجع خطأ
                            وإلا:
                                param.مجموعه_فیلد_2(برچسب, برچسبها[برچسب])
                        لكل def_مقدار في def_مقادیر:
                            param.مجموعه_Enum_مقدار(def_مقدار, def_مقادیر[def_مقدار])
                        لكل def_بیت في def_bitmask:
                            param.مجموعه_Bitmask_بیت(def_بیت, def_bitmask[def_بیت])
                    # Store the parameter
                    إذا گروه ليس في self.param_groups:
                        self.param_groups[گروه] = Parameter_گروه(گروه)
                    self.param_groups[گروه].جمع_کن_Parameter(param)
                state = هیچ
        أرجع درست

    تعریف Is_عدد(self, عدد_String):
        حاول:
            عائم(عدد_String)
            أرجع درست
        استثناء مقدار_Error:
            أرجع خطأ

    تعریف Validate(self):
        """
        Validates the parameter meta data.
        """
        دیده_Param_نامها = []
        #allowedUnits should match set defined in /Firmware/validation/module_schema.yaml
        allowedUnits = مجموعه ([
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
        لكل گروه في self.GetParamGroups():
            لكل param في گروه.GetParams():
                name  = param.GetName()
                إذا طول(name) > 16:
                    sys.stderr.اكتب("Parameter Name {0} is too long (Limit is 16)\n".format(name))
                    أرجع خطأ
                board = param.Get_فیلد_مقدار("board")
                # Check for duplicates
                name_plus_board = name + "+" + board
                لكل دیده_Param_Name في دیده_Param_نامها:
                    إذا دیده_Param_Name == name_plus_board:
                        sys.stderr.اكتب("Duplicate parameter definition: {0}\n".format(name_plus_board))
                        أرجع خطأ
                دیده_Param_نامها.append(name_plus_board)
                # Validate values
                افتراضي = param.GetDefault()
                min = param.Get_فیلد_مقدار("min")
                max = param.Get_فیلد_مقدار("max")
                units = param.Get_فیلد_مقدار("unit")
                إذا units ليس في allowedUnits:
                    sys.stderr.اكتب("Invalid unit in {0}: {1}\n".format(name, units))
                    أرجع خطأ
                #sys.stderr.write("{0} default:{1} min:{2} max:{3}\n".format(name, default, min, max))
                إذا افتراضي != "" و ليس self.Is_عدد(افتراضي):
                    sys.stderr.اكتب("Default value not number: {0} {1}\n".format(name, افتراضي))
                    أرجع خطأ
                # if default != "" and "." not in default:
                #     sys.stderr.write("Default value does not contain dot (e.g. 10 needs to be written as 10.0): {0} {1}\n".format(name, default))
                #     return False
                إذا min != "":
                    إذا ليس self.Is_عدد(min):
                        sys.stderr.اكتب("Min value not number: {0} {1}\n".format(name, min))
                        أرجع خطأ
                    إذا افتراضي != "" و عائم(افتراضي) < عائم(min):
                        sys.stderr.اكتب("Default value is smaller than min: {0} default:{1} min:{2}\n".format(name, افتراضي, min))
                        أرجع خطأ
                إذا max != "":
                    إذا ليس self.Is_عدد(max):
                        sys.stderr.اكتب("Max value not number: {0} {1}\n".format(name, max))
                        أرجع خطأ
                    إذا افتراضي != "" و عائم(افتراضي) > عائم(max):
                        sys.stderr.اكتب("Default value is larger than max: {0} default:{1} max:{2}\n".format(name, افتراضي, max))
                        أرجع خطأ
                لكل کد في param.GetEnumCodes():
                        إذا ليس self.Is_عدد(کد):
                            sys.stderr.اكتب("Min value not number: {0} {1}\n".format(name, کد))
                            أرجع خطأ
                        إذا param.Get_Enum_مقدار(کد) == "":
                            sys.stderr.اكتب("Description for enum value is empty: {0} {1}\n".format(name, کد))
                            أرجع خطأ
                لكل شاخص في param.GetBitmaskList():
                        إذا ليس self.Is_عدد(شاخص):
                            sys.stderr.اكتب("bit value not number: {0} {1}\n".format(name, شاخص))
                            أرجع خطأ
                        إذا ليس صحيح(min) <= math.pow(2, صحيح(شاخص)) <= صحيح(max):
                            sys.stderr.اكتب("Bitmask bit must be between {0} and {1}: {2} {3}\n".format(min, max, name, math.pow(2, صحيح(شاخص))))
                            أرجع خطأ
                        إذا param.Get_Bitmask_بیت(شاخص) == "":
                            sys.stderr.اكتب("Description for bitmask bit is empty: {0} {1}\n".format(name, شاخص))
                            أرجع خطأ
        أرجع درست

    تعریف GetParamGroups(self):
        """
        Returns the parsed list of parameters. Every parameter is a Parameter
        object. Note that returned object is not a copy. Modifications affect
        state of the parser.
        """
        groups = self.param_groups.مقادیر_2()
        groups = sorted(groups, کلید=مجهول x: x.GetName())
        groups = sorted(groups, کلید=مجهول x: self.اولویت.get(x.GetName(), 0), معکوس=درست)
        أرجع groups
