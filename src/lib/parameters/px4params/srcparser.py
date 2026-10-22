ਆਯਾਤ sys
ਆਯਾਤ re
ਆਯਾਤ math

ਵੈਸ਼੍ਵਿਕ default_var
default_var = {}

ਵਰਗ Parameter_ਸਮੂਹ(object):
    """
    Single parameter group
    """
    ਪਰਿਭਾਸ਼ਾ __init__(self, name):
        self.name = name
        self.no_ਕੋਡ_generation = ਝੂਠ #for injected parameters
        self.params = []

    ਪਰਿਭਾਸ਼ਾ ਜੋੜੋ_Parameter(self, param):
        """
        Add parameter to the group
        """
        self.params.append(param)

    ਪਰਿਭਾਸ਼ਾ GetName(self):
        """
        Get parameter group name
        """
        ਵਾਪਸ self.name

    ਪਰਿਭਾਸ਼ਾ GetParams(self):
        """
        Returns the parsed list of parameters. Every parameter is a Parameter
        object. Note that returned object is not a copy. Modifications affect
        state of the parser.
        """
        ਵਾਪਸ sorted(self.params, ਕੁੰਜੀ=ਅਨਾਮ param: param.name)

ਵਰਗ Parameter(object):
    """
    Single parameter
    """

    # Define sorting order of the fields
    ਤਰਜੀਹ = {
        "board": 9,
        "short_desc": 8,
        "long_desc": 7,
        "min": 5,
        "max": 4,
        "unit": 3,
        "decimal": 2,
        # all others == 0 (sorted alphabetically)
    }

    ਪਰਿਭਾਸ਼ਾ __init__(self, name, type, ਮੂਲ = ""):
        self.fields = {}
        self.ਮੁੱਲਾਂ_2 = {}
        self.bitmask = {}
        self.name = name
        self.type = type
        self.ਮੂਲ = ਮੂਲ
        self.category = ""
        self.ਅਸਥਿਰ = ਝੂਠ
        self.ਤਰਕੀ = ਝੂਠ
        self.readonly = ਝੂਠ

    ਪਰਿਭਾਸ਼ਾ GetName(self):
        ਵਾਪਸ self.name

    ਪਰਿਭਾਸ਼ਾ GetType(self):
        ਵਾਪਸ self.type

    ਪਰਿਭਾਸ਼ਾ GetDefault(self):
        ਵਾਪਸ self.ਮੂਲ

    ਪਰਿਭਾਸ਼ਾ GetCategory(self):
        ਵਾਪਸ self.category.title()

    ਪਰਿਭਾਸ਼ਾ GetVolatile(self):
        ਵਾਪਸ self.ਅਸਥਿਰ

    ਪਰਿਭਾਸ਼ਾ GetBoolean(self):
        ਵਾਪਸ self.ਤਰਕੀ

    ਪਰਿਭਾਸ਼ਾ GetReadonly(self):
        ਵਾਪਸ self.readonly

    ਪਰਿਭਾਸ਼ਾ ਸੈੱਟ_ਖੇਤਰ_2(self, ਕੋਡ, ਮੁੱਲ):
        """
        Set named field value
        """
        self.fields[ਕੋਡ] = ਮੁੱਲ

    ਪਰਿਭਾਸ਼ਾ ਸੈੱਟ_Enum_ਮੁੱਲ(self, ਕੋਡ, ਮੁੱਲ):
        """
        Set named enum value
        """
        self.ਮੁੱਲਾਂ_2[ਕੋਡ] = ਮੁੱਲ

    ਪਰਿਭਾਸ਼ਾ ਸੈੱਟ_Bitmask_ਦੋਆਧਾਰੀ_ਅੰਕ(self, ਸੂਚਕਾਂਕ, ਦੋਆਧਾਰੀ_ਅੰਕ):
        """
        Set named enum value
        """
        self.bitmask[ਸੂਚਕਾਂਕ] = ਦੋਆਧਾਰੀ_ਅੰਕ

    ਪਰਿਭਾਸ਼ਾ ਸੈੱਟ_Volatile(self):
        """
        Set volatile flag
        """
        self.ਅਸਥਿਰ = ਸੱਚ

    ਪਰਿਭਾਸ਼ਾ ਸੈੱਟ_Boolean(self):
        """
        Set boolean flag
        """
        self.ਤਰਕੀ = ਸੱਚ

    ਪਰਿਭਾਸ਼ਾ ਸੈੱਟ_Readonly(self):
        """
        Set readonly flag
        """
        self.readonly = ਸੱਚ

    ਪਰਿਭਾਸ਼ਾ ਸੈੱਟ_Category(self, category):
        """
        Set param category
        """
        self.category = category

    ਪਰਿਭਾਸ਼ਾ Get_ਖੇਤਰ_Codes(self):
        """
        Return list of existing field codes in convenient order
        """
        ਕੁੰਜੀਆਂ = self.fields.ਕੁੰਜੀਆਂ()
        ਕੁੰਜੀਆਂ = sorted(ਕੁੰਜੀਆਂ)
        ਕੁੰਜੀਆਂ = sorted(ਕੁੰਜੀਆਂ, ਕੁੰਜੀ=ਅਨਾਮ x: self.ਤਰਜੀਹ.get(x, 0), ਉਲਟ=ਸੱਚ)
        ਵਾਪਸ ਕੁੰਜੀਆਂ

    ਪਰਿਭਾਸ਼ਾ Get_ਖੇਤਰ_ਮੁੱਲ(self, ਕੋਡ):
        """
        Return value of the given field code or None if not found.
        """
        fv =  self.fields.get(ਕੋਡ)
        ਜੇ ਨਹੀਂ fv:
                # required because python 3 sorted does not accept None
                ਵਾਪਸ ""
        ਵਾਪਸ fv

    ਪਰਿਭਾਸ਼ਾ GetEnumCodes(self):
        """
        Return list of existing value codes in convenient order
        """
        ਵਾਪਸ sorted(self.ਮੁੱਲਾਂ_2.ਕੁੰਜੀਆਂ(), ਕੁੰਜੀ=ਦਸ਼ਮਲਵ)

    ਪਰਿਭਾਸ਼ਾ Get_Enum_ਮੁੱਲ(self, ਕੋਡ):
        """
        Return value of the given enum code or None if not found.
        """
        fv =  self.ਮੁੱਲਾਂ_2.get(ਕੋਡ)
        ਜੇ ਨਹੀਂ fv:
                # required because python 3 sorted does not accept None
                ਵਾਪਸ ""
        ਵਾਪਸ fv

    ਪਰਿਭਾਸ਼ਾ GetBitmaskList(self):
        """
        Return list of existing bitmask codes in convenient order
        """
        ਕੁੰਜੀਆਂ = self.bitmask.ਕੁੰਜੀਆਂ()
        ਵਾਪਸ sorted(ਕੁੰਜੀਆਂ, ਕੁੰਜੀ=ਦਸ਼ਮਲਵ)

    ਪਰਿਭਾਸ਼ਾ Get_Bitmask_ਦੋਆਧਾਰੀ_ਅੰਕ(self, ਸੂਚਕਾਂਕ):
        """
        Return value of the given bitmask code or None if not found.
        """
        fv = self.bitmask.get(ਸੂਚਕਾਂਕ)
        ਜੇ ਨਹੀਂ fv:
                # required because python 3 sorted does not accept None
                ਵਾਪਸ ""
        ਵਾਪਸ fv.strip()

ਵਰਗ ਸਰੋਤ_Parser(object):
    """
    Parses provided data and stores all found parameters internally.
    """

    re_ਵੰਡੋ_ਸਤਰਾਂ = re.compile(r'[\r\n]+')
    re_comment_ਸ਼ੁਰੂ = re.compile(r'^\/\*\*')
    re_comment_content = re.compile(r'^\*\s*(.*)')
    re_comment_ਟੈਗ = re.compile(r'@([a-zA-Z][a-zA-Z0-9_]*)\s*(.*)')
    re_comment_ਸਿਰਾ = re.compile(r'(.*?)\s*\*\/')
    re_parameter_definition = re.compile(r'PARAM_DEFINE_([A-Z_][A-Z0-9_]*)\s*\(([A-Z_][A-Z0-9_]*)\s*,\s*([^ ,\)]+)\s*\)\s*;')
    re_px4_parameter_definition = re.compile(r'PX4_PARAM_DEFINE_([A-Z_][A-Z0-9_]*)\s*\(([A-Z_][A-Z0-9_]*)\s*\)\s*;')
    re_px4_param_default_definition = re.compile(r'#define\s*PARAM_([A-Z_][A-Z0-9_]*)\s*([^ ,\)]+)\s*')
    re_cut_type_specifier = re.compile(r'[a-z]+$')
    re_is_a_ਸੰਖਿਆ = re.compile(r'^-?[0-9\.]')
    re_remove_dots = re.compile(r'\.+$')

    valid_ਟੈਗ_ਸੂਚੀ = ਸੈੱਟ(["group", "board", "min", "max", "unit", "decimal", "increment", "reboot_required", "value", "boolean", "bit", "category", "volatile"])

    # Order of parameter groups
    ਤਰਜੀਹ = {
        # All other groups = 0 (sort alphabetically)
        "Miscellaneous": -10
    }

    ਪਰਿਭਾਸ਼ਾ __init__(self):
        self.param_groups = {}

    ਪਰਿਭਾਸ਼ਾ Parse(self, contents):
        """
        Incrementally parse program contents and append all found parameters
        to the list.
        """
        # This code is essentially a comment-parsing grammar. "state"
        # represents parser state. It contains human-readable state
        # names.
        state = ਕੁਝ_ਨਹੀਂ
        ਲਈ ਸਤਰ ਵਿੱਚ self.re_ਵੰਡੋ_ਸਤਰਾਂ.ਵੰਡੋ(contents):
            ਸਤਰ = ਸਤਰ.strip()
            # Ignore empty lines
            ਜੇ ਸਤਰ == "":
                ਜਾਰੀ
            ਜੇ self.re_comment_ਸ਼ੁਰੂ.ਮੇਲ(ਸਤਰ):
                state = "wait-short"
                short_desc = ਕੁਝ_ਨਹੀਂ
                long_desc = ਕੁਝ_ਨਹੀਂ
                ਟੈਗ_ਸੂਚੀ = {}
                def_ਮੁੱਲਾਂ = {}
                def_bitmask = {}
            ਜਾਂਜੇ state ਹੈ ਨਹੀਂ ਕੁਝ_ਨਹੀਂ ਅਤੇ state != "comment-processed":
                m = self.re_comment_ਸਿਰਾ.ਖੋਜ(ਸਤਰ)
                ਜੇ m:
                    ਸਤਰ = m.ਸਮੂਹ(1)
                    ਆਖਰੀ_comment_ਸਤਰ = ਸੱਚ
                ਨਹੀਂਤਾਂ:
                    ਆਖਰੀ_comment_ਸਤਰ = ਝੂਠ
                m = self.re_comment_content.ਮੇਲ(ਸਤਰ)
                ਜੇ m:
                    comment_content = m.ਸਮੂਹ(1)
                    ਜੇ comment_content == "":
                        # When short comment ends with empty comment line,
                        # start waiting for the next part - long comment.
                        ਜੇ state == "wait-short-end":
                            state = "wait-long"
                        ਜੇ state == "wait-long-end":
                            # Long description includes empty lines
                            long_desc += "\n"
                    ਨਹੀਂਤਾਂ:
                        m = self.re_comment_ਟੈਗ.ਮੇਲ(comment_content)
                        ਜੇ m:
                            ਟੈਗ, desc = m.ਸਮੂਹ(1, 2)
                            ਜੇ (ਟੈਗ == "value"):
                                # Take the meta info string and split the code and description
                                metainfo = desc.ਵੰਡੋ(" ",  1)
                                def_ਮੁੱਲਾਂ[metainfo[0]] = metainfo[1]
                            ਜਾਂਜੇ (ਟੈਗ == "bit"):
                                # Take the meta info string and split the code and description
                                metainfo = desc.ਵੰਡੋ(" ",  1)
                                def_bitmask[metainfo[0]] = metainfo[1]
                            ਨਹੀਂਤਾਂ:
                                ਟੈਗ_ਸੂਚੀ[ਟੈਗ] = desc
                            current_ਟੈਗ = ਟੈਗ
                            state = "wait-tag-end"
                        ਜਾਂਜੇ state == "wait-short":
                            # Store first line of the short description
                            short_desc = comment_content
                            state = "wait-short-end"
                        ਜਾਂਜੇ state == "wait-short-end":
                            # Append comment line to the short description
                            short_desc += "\n" + comment_content
                        ਜਾਂਜੇ state == "wait-long":
                            # Store first line of the long description
                            long_desc = comment_content
                            state = "wait-long-end"
                        ਜਾਂਜੇ state == "wait-long-end":
                            # Append comment line to the long description
                            long_desc += "\n" + comment_content
                        ਜਾਂਜੇ state == "wait-tag-end":
                            # Append comment line to the tag text
                            ਟੈਗ_ਸੂਚੀ[current_ਟੈਗ] += "\n" + comment_content
                        ਨਹੀਂਤਾਂ:
                            ਉਠਾਓ AssertionError(
                                    "Invalid parser state: %s" % state)
                ਜਾਂਜੇ ਨਹੀਂ ਆਖਰੀ_comment_ਸਤਰ:
                    # Invalid comment line (inside comment, but not starting with
                    # "*" or "*/". Reset parsed content.
                    state = ਕੁਝ_ਨਹੀਂ
                ਜੇ ਆਖਰੀ_comment_ਸਤਰ:
                    state = "comment-processed"
            ਨਹੀਂਤਾਂ:
                tp = ਕੁਝ_ਨਹੀਂ
                name = ਕੁਝ_ਨਹੀਂ
                defval = ""
                # Non-empty line outside the comment
                m = self.re_px4_param_default_definition.ਮੇਲ(ਸਤਰ)
                # Default value handling
                ਜੇ m:
                    name_m, defval_m = m.ਸਮੂਹ(1,2)
                    default_var[name_m] = defval_m
                m = self.re_parameter_definition.ਮੇਲ(ਸਤਰ)
                ਜੇ m:
                    tp, name, defval = m.ਸਮੂਹ(1, 2, 3)
                ਨਹੀਂਤਾਂ:
                    m = self.re_px4_parameter_definition.ਮੇਲ(ਸਤਰ)
                    ਜੇ m:
                        tp, name = m.ਸਮੂਹ(1, 2)
                        ਜੇ (name+'_DEFAULT') ਵਿੱਚ default_var:
                            defval = default_var[name+'_DEFAULT']
                ਜੇ tp ਹੈ ਨਹੀਂ ਕੁਝ_ਨਹੀਂ:
                    # Remove trailing type specifier from numbers: 0.1f => 0.1
                    ਜੇ defval != "" ਅਤੇ self.re_is_a_ਸੰਖਿਆ.ਮੇਲ(defval):
                        defval = self.re_cut_type_specifier.sub('', defval)
                    param = Parameter(name, tp, defval)
                    param.ਸੈੱਟ_ਖੇਤਰ_2("short_desc", name)
                    # If comment was found before the parameter declaration,
                    # inject its data into the newly created parameter.
                    ਸਮੂਹ = "Miscellaneous"
                    ਜੇ state == "comment-processed":
                        ਜੇ short_desc ਹੈ ਨਹੀਂ ਕੁਝ_ਨਹੀਂ:
                            ਜੇ '\n' ਵਿੱਚ short_desc:
                                ਉਠਾਓ Exception('short description must be a single line (parameter: {:})'.format(name))
                            ਜੇ ਲੰਬਾਈ(short_desc) > 150:
                                ਉਠਾਓ Exception('short description too long (150 max, is {:}, parameter: {:})'.format(ਲੰਬਾਈ(short_desc), name))
                            param.ਸੈੱਟ_ਖੇਤਰ_2("short_desc", self.re_remove_dots.sub('', short_desc))
                        ਜੇ long_desc ਹੈ ਨਹੀਂ ਕੁਝ_ਨਹੀਂ:
                            param.ਸੈੱਟ_ਖੇਤਰ_2("long_desc", long_desc)
                        ਲਈ ਟੈਗ ਵਿੱਚ ਟੈਗ_ਸੂਚੀ:
                            ਜੇ ਟੈਗ == "group":
                                ਸਮੂਹ = ਟੈਗ_ਸੂਚੀ[ਟੈਗ]
                            ਜਾਂਜੇ ਟੈਗ == "volatile":
                                param.ਸੈੱਟ_Volatile()
                            ਜਾਂਜੇ ਟੈਗ == "category":
                                param.ਸੈੱਟ_Category(ਟੈਗ_ਸੂਚੀ[ਟੈਗ])
                            ਜਾਂਜੇ ਟੈਗ == "boolean":
                                param.ਸੈੱਟ_Boolean()
                            ਜਾਂਜੇ ਟੈਗ ਨਹੀਂ ਵਿੱਚ self.valid_ਟੈਗ_ਸੂਚੀ:
                                sys.stderr.ਲਿਖ("Skipping invalid documentation tag: '%s'\n" % ਟੈਗ)
                                ਵਾਪਸ ਝੂਠ
                            ਨਹੀਂਤਾਂ:
                                param.ਸੈੱਟ_ਖੇਤਰ_2(ਟੈਗ, ਟੈਗ_ਸੂਚੀ[ਟੈਗ])
                        ਲਈ def_ਮੁੱਲ ਵਿੱਚ def_ਮੁੱਲਾਂ:
                            param.ਸੈੱਟ_Enum_ਮੁੱਲ(def_ਮੁੱਲ, def_ਮੁੱਲਾਂ[def_ਮੁੱਲ])
                        ਲਈ def_ਦੋਆਧਾਰੀ_ਅੰਕ ਵਿੱਚ def_bitmask:
                            param.ਸੈੱਟ_Bitmask_ਦੋਆਧਾਰੀ_ਅੰਕ(def_ਦੋਆਧਾਰੀ_ਅੰਕ, def_bitmask[def_ਦੋਆਧਾਰੀ_ਅੰਕ])
                    # Store the parameter
                    ਜੇ ਸਮੂਹ ਨਹੀਂ ਵਿੱਚ self.param_groups:
                        self.param_groups[ਸਮੂਹ] = Parameter_ਸਮੂਹ(ਸਮੂਹ)
                    self.param_groups[ਸਮੂਹ].ਜੋੜੋ_Parameter(param)
                state = ਕੁਝ_ਨਹੀਂ
        ਵਾਪਸ ਸੱਚ

    ਪਰਿਭਾਸ਼ਾ Is_ਸੰਖਿਆ(self, ਸੰਖਿਆ_String):
        ਕੋਸ਼ਿਸ਼:
            ਦਸ਼ਮਲਵ(ਸੰਖਿਆ_String)
            ਵਾਪਸ ਸੱਚ
        ਅਪਵਾਦ ਮੁੱਲ_Error:
            ਵਾਪਸ ਝੂਠ

    ਪਰਿਭਾਸ਼ਾ Validate(self):
        """
        Validates the parameter meta data.
        """
        ਵੇਖਿਆ_Param_ਨਾਂ = []
        #allowedUnits should match set defined in /Firmware/validation/module_schema.yaml
        allowedUnits = ਸੈੱਟ ([
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
        ਲਈ ਸਮੂਹ ਵਿੱਚ self.GetParamGroups():
            ਲਈ param ਵਿੱਚ ਸਮੂਹ.GetParams():
                name  = param.GetName()
                ਜੇ ਲੰਬਾਈ(name) > 16:
                    sys.stderr.ਲਿਖ("Parameter Name {0} is too long (Limit is 16)\n".format(name))
                    ਵਾਪਸ ਝੂਠ
                board = param.Get_ਖੇਤਰ_ਮੁੱਲ("board")
                # Check for duplicates
                name_plus_board = name + "+" + board
                ਲਈ ਵੇਖਿਆ_Param_Name ਵਿੱਚ ਵੇਖਿਆ_Param_ਨਾਂ:
                    ਜੇ ਵੇਖਿਆ_Param_Name == name_plus_board:
                        sys.stderr.ਲਿਖ("Duplicate parameter definition: {0}\n".format(name_plus_board))
                        ਵਾਪਸ ਝੂਠ
                ਵੇਖਿਆ_Param_ਨਾਂ.append(name_plus_board)
                # Validate values
                ਮੂਲ = param.GetDefault()
                min = param.Get_ਖੇਤਰ_ਮੁੱਲ("min")
                max = param.Get_ਖੇਤਰ_ਮੁੱਲ("max")
                units = param.Get_ਖੇਤਰ_ਮੁੱਲ("unit")
                ਜੇ units ਨਹੀਂ ਵਿੱਚ allowedUnits:
                    sys.stderr.ਲਿਖ("Invalid unit in {0}: {1}\n".format(name, units))
                    ਵਾਪਸ ਝੂਠ
                #sys.stderr.write("{0} default:{1} min:{2} max:{3}\n".format(name, default, min, max))
                ਜੇ ਮੂਲ != "" ਅਤੇ ਨਹੀਂ self.Is_ਸੰਖਿਆ(ਮੂਲ):
                    sys.stderr.ਲਿਖ("Default value not number: {0} {1}\n".format(name, ਮੂਲ))
                    ਵਾਪਸ ਝੂਠ
                # if default != "" and "." not in default:
                #     sys.stderr.write("Default value does not contain dot (e.g. 10 needs to be written as 10.0): {0} {1}\n".format(name, default))
                #     return False
                ਜੇ min != "":
                    ਜੇ ਨਹੀਂ self.Is_ਸੰਖਿਆ(min):
                        sys.stderr.ਲਿਖ("Min value not number: {0} {1}\n".format(name, min))
                        ਵਾਪਸ ਝੂਠ
                    ਜੇ ਮੂਲ != "" ਅਤੇ ਦਸ਼ਮਲਵ(ਮੂਲ) < ਦਸ਼ਮਲਵ(min):
                        sys.stderr.ਲਿਖ("Default value is smaller than min: {0} default:{1} min:{2}\n".format(name, ਮੂਲ, min))
                        ਵਾਪਸ ਝੂਠ
                ਜੇ max != "":
                    ਜੇ ਨਹੀਂ self.Is_ਸੰਖਿਆ(max):
                        sys.stderr.ਲਿਖ("Max value not number: {0} {1}\n".format(name, max))
                        ਵਾਪਸ ਝੂਠ
                    ਜੇ ਮੂਲ != "" ਅਤੇ ਦਸ਼ਮਲਵ(ਮੂਲ) > ਦਸ਼ਮਲਵ(max):
                        sys.stderr.ਲਿਖ("Default value is larger than max: {0} default:{1} max:{2}\n".format(name, ਮੂਲ, max))
                        ਵਾਪਸ ਝੂਠ
                ਲਈ ਕੋਡ ਵਿੱਚ param.GetEnumCodes():
                        ਜੇ ਨਹੀਂ self.Is_ਸੰਖਿਆ(ਕੋਡ):
                            sys.stderr.ਲਿਖ("Min value not number: {0} {1}\n".format(name, ਕੋਡ))
                            ਵਾਪਸ ਝੂਠ
                        ਜੇ param.Get_Enum_ਮੁੱਲ(ਕੋਡ) == "":
                            sys.stderr.ਲਿਖ("Description for enum value is empty: {0} {1}\n".format(name, ਕੋਡ))
                            ਵਾਪਸ ਝੂਠ
                ਲਈ ਸੂਚਕਾਂਕ ਵਿੱਚ param.GetBitmaskList():
                        ਜੇ ਨਹੀਂ self.Is_ਸੰਖਿਆ(ਸੂਚਕਾਂਕ):
                            sys.stderr.ਲਿਖ("bit value not number: {0} {1}\n".format(name, ਸੂਚਕਾਂਕ))
                            ਵਾਪਸ ਝੂਠ
                        ਜੇ ਨਹੀਂ ਪੂਰਨ(min) <= math.pow(2, ਪੂਰਨ(ਸੂਚਕਾਂਕ)) <= ਪੂਰਨ(max):
                            sys.stderr.ਲਿਖ("Bitmask bit must be between {0} and {1}: {2} {3}\n".format(min, max, name, math.pow(2, ਪੂਰਨ(ਸੂਚਕਾਂਕ))))
                            ਵਾਪਸ ਝੂਠ
                        ਜੇ param.Get_Bitmask_ਦੋਆਧਾਰੀ_ਅੰਕ(ਸੂਚਕਾਂਕ) == "":
                            sys.stderr.ਲਿਖ("Description for bitmask bit is empty: {0} {1}\n".format(name, ਸੂਚਕਾਂਕ))
                            ਵਾਪਸ ਝੂਠ
        ਵਾਪਸ ਸੱਚ

    ਪਰਿਭਾਸ਼ਾ GetParamGroups(self):
        """
        Returns the parsed list of parameters. Every parameter is a Parameter
        object. Note that returned object is not a copy. Modifications affect
        state of the parser.
        """
        groups = self.param_groups.ਮੁੱਲਾਂ_2()
        groups = sorted(groups, ਕੁੰਜੀ=ਅਨਾਮ x: x.GetName())
        groups = sorted(groups, ਕੁੰਜੀ=ਅਨਾਮ x: self.ਤਰਜੀਹ.get(x.GetName(), 0), ਉਲਟ=ਸੱਚ)
        ਵਾਪਸ groups
