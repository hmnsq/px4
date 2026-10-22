ಆಮದು sys
ಆಮದು re
ಆಮದು math

ವೈಶ್ವಿಕ default_var
default_var = {}

ವರ್ಗ Parameter_ಗುಂಪು(object):
    """
    Single parameter group
    """
    ವ್ಯಾಖ್ಯಾನ __init__(self, name):
        self.name = name
        self.no_ಸಂಕೇತ_generation = ಸುಳ್ಳು #for injected parameters
        self.params = []

    ವ್ಯಾಖ್ಯಾನ ಕೂಡಿಸು_Parameter(self, param):
        """
        Add parameter to the group
        """
        self.params.append(param)

    ವ್ಯಾಖ್ಯಾನ GetName(self):
        """
        Get parameter group name
        """
        ಹಿಂತಿರುಗಿ self.name

    ವ್ಯಾಖ್ಯಾನ GetParams(self):
        """
        Returns the parsed list of parameters. Every parameter is a Parameter
        object. Note that returned object is not a copy. Modifications affect
        state of the parser.
        """
        ಹಿಂತಿರುಗಿ sorted(self.params, ಕೀಲಿ=ಅನಾಮಿಕ param: param.name)

ವರ್ಗ Parameter(object):
    """
    Single parameter
    """

    # Define sorting order of the fields
    ಆದ್ಯತೆ = {
        "board": 9,
        "short_desc": 8,
        "long_desc": 7,
        "min": 5,
        "max": 4,
        "unit": 3,
        "decimal": 2,
        # all others == 0 (sorted alphabetically)
    }

    ವ್ಯಾಖ್ಯಾನ __init__(self, name, type, ಪೂರ್ವನಿಯೋಜಿತ = ""):
        self.fields = {}
        self.ಮೌಲ್ಯಗಳು_2 = {}
        self.bitmask = {}
        self.name = name
        self.type = type
        self.ಪೂರ್ವನಿಯೋಜಿತ = ಪೂರ್ವನಿಯೋಜಿತ
        self.category = ""
        self.ಅಸ್ಥಿರ = ಸುಳ್ಳು
        self.ತಾರ್ಕಿಕ = ಸುಳ್ಳು
        self.readonly = ಸುಳ್ಳು

    ವ್ಯಾಖ್ಯಾನ GetName(self):
        ಹಿಂತಿರುಗಿ self.name

    ವ್ಯಾಖ್ಯಾನ GetType(self):
        ಹಿಂತಿರುಗಿ self.type

    ವ್ಯಾಖ್ಯಾನ GetDefault(self):
        ಹಿಂತಿರುಗಿ self.ಪೂರ್ವನಿಯೋಜಿತ

    ವ್ಯಾಖ್ಯಾನ GetCategory(self):
        ಹಿಂತಿರುಗಿ self.category.title()

    ವ್ಯಾಖ್ಯಾನ GetVolatile(self):
        ಹಿಂತಿರುಗಿ self.ಅಸ್ಥಿರ

    ವ್ಯಾಖ್ಯಾನ GetBoolean(self):
        ಹಿಂತಿರುಗಿ self.ತಾರ್ಕಿಕ

    ವ್ಯಾಖ್ಯಾನ GetReadonly(self):
        ಹಿಂತಿರುಗಿ self.readonly

    ವ್ಯಾಖ್ಯಾನ ಗಣ_ಕ್ಷೇತ್ರ_2(self, ಸಂಕೇತ_2, ಮೌಲ್ಯ):
        """
        Set named field value
        """
        self.fields[ಸಂಕೇತ_2] = ಮೌಲ್ಯ

    ವ್ಯಾಖ್ಯಾನ ಗಣ_Enum_ಮೌಲ್ಯ(self, ಸಂಕೇತ_2, ಮೌಲ್ಯ):
        """
        Set named enum value
        """
        self.ಮೌಲ್ಯಗಳು_2[ಸಂಕೇತ_2] = ಮೌಲ್ಯ

    ವ್ಯಾಖ್ಯಾನ ಗಣ_Bitmask_ಬಿಟ್(self, ಸೂಚ್ಯಂಕ, ಬಿಟ್):
        """
        Set named enum value
        """
        self.bitmask[ಸೂಚ್ಯಂಕ] = ಬಿಟ್

    ವ್ಯಾಖ್ಯಾನ ಗಣ_Volatile(self):
        """
        Set volatile flag
        """
        self.ಅಸ್ಥಿರ = ನಿಜ

    ವ್ಯಾಖ್ಯಾನ ಗಣ_Boolean(self):
        """
        Set boolean flag
        """
        self.ತಾರ್ಕಿಕ = ನಿಜ

    ವ್ಯಾಖ್ಯಾನ ಗಣ_Readonly(self):
        """
        Set readonly flag
        """
        self.readonly = ನಿಜ

    ವ್ಯಾಖ್ಯಾನ ಗಣ_Category(self, category):
        """
        Set param category
        """
        self.category = category

    ವ್ಯಾಖ್ಯಾನ Get_ಕ್ಷೇತ್ರ_Codes(self):
        """
        Return list of existing field codes in convenient order
        """
        ಕೀಲಿಗಳು = self.fields.ಕೀಲಿಗಳು()
        ಕೀಲಿಗಳು = sorted(ಕೀಲಿಗಳು)
        ಕೀಲಿಗಳು = sorted(ಕೀಲಿಗಳು, ಕೀಲಿ=ಅನಾಮಿಕ x: self.ಆದ್ಯತೆ.get(x, 0), ಹಿಮ್ಮುಖ=ನಿಜ)
        ಹಿಂತಿರುಗಿ ಕೀಲಿಗಳು

    ವ್ಯಾಖ್ಯಾನ Get_ಕ್ಷೇತ್ರ_ಮೌಲ್ಯ(self, ಸಂಕೇತ_2):
        """
        Return value of the given field code or None if not found.
        """
        fv =  self.fields.get(ಸಂಕೇತ_2)
        ಆದರೆ ಅಲ್ಲ fv:
                # required because python 3 sorted does not accept None
                ಹಿಂತಿರುಗಿ ""
        ಹಿಂತಿರುಗಿ fv

    ವ್ಯಾಖ್ಯಾನ GetEnumCodes(self):
        """
        Return list of existing value codes in convenient order
        """
        ಹಿಂತಿರುಗಿ sorted(self.ಮೌಲ್ಯಗಳು_2.ಕೀಲಿಗಳು(), ಕೀಲಿ=ದಶಮಾಂಶ)

    ವ್ಯಾಖ್ಯಾನ Get_Enum_ಮೌಲ್ಯ(self, ಸಂಕೇತ_2):
        """
        Return value of the given enum code or None if not found.
        """
        fv =  self.ಮೌಲ್ಯಗಳು_2.get(ಸಂಕೇತ_2)
        ಆದರೆ ಅಲ್ಲ fv:
                # required because python 3 sorted does not accept None
                ಹಿಂತಿರುಗಿ ""
        ಹಿಂತಿರುಗಿ fv

    ವ್ಯಾಖ್ಯಾನ GetBitmaskList(self):
        """
        Return list of existing bitmask codes in convenient order
        """
        ಕೀಲಿಗಳು = self.bitmask.ಕೀಲಿಗಳು()
        ಹಿಂತಿರುಗಿ sorted(ಕೀಲಿಗಳು, ಕೀಲಿ=ದಶಮಾಂಶ)

    ವ್ಯಾಖ್ಯಾನ Get_Bitmask_ಬಿಟ್(self, ಸೂಚ್ಯಂಕ):
        """
        Return value of the given bitmask code or None if not found.
        """
        fv = self.bitmask.get(ಸೂಚ್ಯಂಕ)
        ಆದರೆ ಅಲ್ಲ fv:
                # required because python 3 sorted does not accept None
                ಹಿಂತಿರುಗಿ ""
        ಹಿಂತಿರುಗಿ fv.strip()

ವರ್ಗ ಮೂಲ_Parser(object):
    """
    Parses provided data and stores all found parameters internally.
    """

    re_ವಿಭಜಿಸು_ಸಾಲುಗಳು = re.compile(r'[\r\n]+')
    re_comment_ಆರಂಭ = re.compile(r'^\/\*\*')
    re_comment_content = re.compile(r'^\*\s*(.*)')
    re_comment_ಟ್ಯಾಗ್ = re.compile(r'@([a-zA-Z][a-zA-Z0-9_]*)\s*(.*)')
    re_comment_ಕೊನೆ = re.compile(r'(.*?)\s*\*\/')
    re_parameter_definition = re.compile(r'PARAM_DEFINE_([A-Z_][A-Z0-9_]*)\s*\(([A-Z_][A-Z0-9_]*)\s*,\s*([^ ,\)]+)\s*\)\s*;')
    re_px4_parameter_definition = re.compile(r'PX4_PARAM_DEFINE_([A-Z_][A-Z0-9_]*)\s*\(([A-Z_][A-Z0-9_]*)\s*\)\s*;')
    re_px4_param_default_definition = re.compile(r'#define\s*PARAM_([A-Z_][A-Z0-9_]*)\s*([^ ,\)]+)\s*')
    re_cut_type_specifier = re.compile(r'[a-z]+$')
    re_is_a_ಸಂಖ್ಯೆ = re.compile(r'^-?[0-9\.]')
    re_remove_dots = re.compile(r'\.+$')

    valid_ಟ್ಯಾಗುಗಳು = ಗಣ(["group", "board", "min", "max", "unit", "decimal", "increment", "reboot_required", "value", "boolean", "bit", "category", "volatile"])

    # Order of parameter groups
    ಆದ್ಯತೆ = {
        # All other groups = 0 (sort alphabetically)
        "Miscellaneous": -10
    }

    ವ್ಯಾಖ್ಯಾನ __init__(self):
        self.param_groups = {}

    ವ್ಯಾಖ್ಯಾನ Parse(self, contents):
        """
        Incrementally parse program contents and append all found parameters
        to the list.
        """
        # This code is essentially a comment-parsing grammar. "state"
        # represents parser state. It contains human-readable state
        # names.
        state = ಏನೂಇಲ್ಲ
        ಗಾಗಿ ಸಾಲು ಒಳಗೆ self.re_ವಿಭಜಿಸು_ಸಾಲುಗಳು.ವಿಭಜಿಸು(contents):
            ಸಾಲು = ಸಾಲು.strip()
            # Ignore empty lines
            ಆದರೆ ಸಾಲು == "":
                ಮುಂದುವರಿ
            ಆದರೆ self.re_comment_ಆರಂಭ.ಹೊಂದಿಕೆ(ಸಾಲು):
                state = "wait-short"
                short_desc = ಏನೂಇಲ್ಲ
                long_desc = ಏನೂಇಲ್ಲ
                ಟ್ಯಾಗುಗಳು = {}
                def_ಮೌಲ್ಯಗಳು = {}
                def_bitmask = {}
            ಅಥವಾಆದರೆ state ಆಗಿರುವ ಅಲ್ಲ ಏನೂಇಲ್ಲ ಮತ್ತು state != "comment-processed":
                m = self.re_comment_ಕೊನೆ.ಹುಡುಕು(ಸಾಲು)
                ಆದರೆ m:
                    ಸಾಲು = m.ಗುಂಪು(1)
                    ಕೊನೆಯ_comment_ಸಾಲು = ನಿಜ
                ಇಲ್ಲದಿದ್ದರೆ:
                    ಕೊನೆಯ_comment_ಸಾಲು = ಸುಳ್ಳು
                m = self.re_comment_content.ಹೊಂದಿಕೆ(ಸಾಲು)
                ಆದರೆ m:
                    comment_content = m.ಗುಂಪು(1)
                    ಆದರೆ comment_content == "":
                        # When short comment ends with empty comment line,
                        # start waiting for the next part - long comment.
                        ಆದರೆ state == "wait-short-end":
                            state = "wait-long"
                        ಆದರೆ state == "wait-long-end":
                            # Long description includes empty lines
                            long_desc += "\n"
                    ಇಲ್ಲದಿದ್ದರೆ:
                        m = self.re_comment_ಟ್ಯಾಗ್.ಹೊಂದಿಕೆ(comment_content)
                        ಆದರೆ m:
                            ಟ್ಯಾಗ್, desc = m.ಗುಂಪು(1, 2)
                            ಆದರೆ (ಟ್ಯಾಗ್ == "value"):
                                # Take the meta info string and split the code and description
                                metainfo = desc.ವಿಭಜಿಸು(" ",  1)
                                def_ಮೌಲ್ಯಗಳು[metainfo[0]] = metainfo[1]
                            ಅಥವಾಆದರೆ (ಟ್ಯಾಗ್ == "bit"):
                                # Take the meta info string and split the code and description
                                metainfo = desc.ವಿಭಜಿಸು(" ",  1)
                                def_bitmask[metainfo[0]] = metainfo[1]
                            ಇಲ್ಲದಿದ್ದರೆ:
                                ಟ್ಯಾಗುಗಳು[ಟ್ಯಾಗ್] = desc
                            current_ಟ್ಯಾಗ್ = ಟ್ಯಾಗ್
                            state = "wait-tag-end"
                        ಅಥವಾಆದರೆ state == "wait-short":
                            # Store first line of the short description
                            short_desc = comment_content
                            state = "wait-short-end"
                        ಅಥವಾಆದರೆ state == "wait-short-end":
                            # Append comment line to the short description
                            short_desc += "\n" + comment_content
                        ಅಥವಾಆದರೆ state == "wait-long":
                            # Store first line of the long description
                            long_desc = comment_content
                            state = "wait-long-end"
                        ಅಥವಾಆದರೆ state == "wait-long-end":
                            # Append comment line to the long description
                            long_desc += "\n" + comment_content
                        ಅಥವಾಆದರೆ state == "wait-tag-end":
                            # Append comment line to the tag text
                            ಟ್ಯಾಗುಗಳು[current_ಟ್ಯಾಗ್] += "\n" + comment_content
                        ಇಲ್ಲದಿದ್ದರೆ:
                            ಎತ್ತು AssertionError(
                                    "Invalid parser state: %s" % state)
                ಅಥವಾಆದರೆ ಅಲ್ಲ ಕೊನೆಯ_comment_ಸಾಲು:
                    # Invalid comment line (inside comment, but not starting with
                    # "*" or "*/". Reset parsed content.
                    state = ಏನೂಇಲ್ಲ
                ಆದರೆ ಕೊನೆಯ_comment_ಸಾಲು:
                    state = "comment-processed"
            ಇಲ್ಲದಿದ್ದರೆ:
                tp = ಏನೂಇಲ್ಲ
                name = ಏನೂಇಲ್ಲ
                defval = ""
                # Non-empty line outside the comment
                m = self.re_px4_param_default_definition.ಹೊಂದಿಕೆ(ಸಾಲು)
                # Default value handling
                ಆದರೆ m:
                    name_m, defval_m = m.ಗುಂಪು(1,2)
                    default_var[name_m] = defval_m
                m = self.re_parameter_definition.ಹೊಂದಿಕೆ(ಸಾಲು)
                ಆದರೆ m:
                    tp, name, defval = m.ಗುಂಪು(1, 2, 3)
                ಇಲ್ಲದಿದ್ದರೆ:
                    m = self.re_px4_parameter_definition.ಹೊಂದಿಕೆ(ಸಾಲು)
                    ಆದರೆ m:
                        tp, name = m.ಗುಂಪು(1, 2)
                        ಆದರೆ (name+'_DEFAULT') ಒಳಗೆ default_var:
                            defval = default_var[name+'_DEFAULT']
                ಆದರೆ tp ಆಗಿರುವ ಅಲ್ಲ ಏನೂಇಲ್ಲ:
                    # Remove trailing type specifier from numbers: 0.1f => 0.1
                    ಆದರೆ defval != "" ಮತ್ತು self.re_is_a_ಸಂಖ್ಯೆ.ಹೊಂದಿಕೆ(defval):
                        defval = self.re_cut_type_specifier.sub('', defval)
                    param = Parameter(name, tp, defval)
                    param.ಗಣ_ಕ್ಷೇತ್ರ_2("short_desc", name)
                    # If comment was found before the parameter declaration,
                    # inject its data into the newly created parameter.
                    ಗುಂಪು = "Miscellaneous"
                    ಆದರೆ state == "comment-processed":
                        ಆದರೆ short_desc ಆಗಿರುವ ಅಲ್ಲ ಏನೂಇಲ್ಲ:
                            ಆದರೆ '\n' ಒಳಗೆ short_desc:
                                ಎತ್ತು Exception('short description must be a single line (parameter: {:})'.format(name))
                            ಆದರೆ ಉದ್ದ(short_desc) > 150:
                                ಎತ್ತು Exception('short description too long (150 max, is {:}, parameter: {:})'.format(ಉದ್ದ(short_desc), name))
                            param.ಗಣ_ಕ್ಷೇತ್ರ_2("short_desc", self.re_remove_dots.sub('', short_desc))
                        ಆದರೆ long_desc ಆಗಿರುವ ಅಲ್ಲ ಏನೂಇಲ್ಲ:
                            param.ಗಣ_ಕ್ಷೇತ್ರ_2("long_desc", long_desc)
                        ಗಾಗಿ ಟ್ಯಾಗ್ ಒಳಗೆ ಟ್ಯಾಗುಗಳು:
                            ಆದರೆ ಟ್ಯಾಗ್ == "group":
                                ಗುಂಪು = ಟ್ಯಾಗುಗಳು[ಟ್ಯಾಗ್]
                            ಅಥವಾಆದರೆ ಟ್ಯಾಗ್ == "volatile":
                                param.ಗಣ_Volatile()
                            ಅಥವಾಆದರೆ ಟ್ಯಾಗ್ == "category":
                                param.ಗಣ_Category(ಟ್ಯಾಗುಗಳು[ಟ್ಯಾಗ್])
                            ಅಥವಾಆದರೆ ಟ್ಯಾಗ್ == "boolean":
                                param.ಗಣ_Boolean()
                            ಅಥವಾಆದರೆ ಟ್ಯಾಗ್ ಅಲ್ಲ ಒಳಗೆ self.valid_ಟ್ಯಾಗುಗಳು:
                                sys.stderr.ಲಿಖ("Skipping invalid documentation tag: '%s'\n" % ಟ್ಯಾಗ್)
                                ಹಿಂತಿರುಗಿ ಸುಳ್ಳು
                            ಇಲ್ಲದಿದ್ದರೆ:
                                param.ಗಣ_ಕ್ಷೇತ್ರ_2(ಟ್ಯಾಗ್, ಟ್ಯಾಗುಗಳು[ಟ್ಯಾಗ್])
                        ಗಾಗಿ def_ಮೌಲ್ಯ ಒಳಗೆ def_ಮೌಲ್ಯಗಳು:
                            param.ಗಣ_Enum_ಮೌಲ್ಯ(def_ಮೌಲ್ಯ, def_ಮೌಲ್ಯಗಳು[def_ಮೌಲ್ಯ])
                        ಗಾಗಿ def_ಬಿಟ್ ಒಳಗೆ def_bitmask:
                            param.ಗಣ_Bitmask_ಬಿಟ್(def_ಬಿಟ್, def_bitmask[def_ಬಿಟ್])
                    # Store the parameter
                    ಆದರೆ ಗುಂಪು ಅಲ್ಲ ಒಳಗೆ self.param_groups:
                        self.param_groups[ಗುಂಪು] = Parameter_ಗುಂಪು(ಗುಂಪು)
                    self.param_groups[ಗುಂಪು].ಕೂಡಿಸು_Parameter(param)
                state = ಏನೂಇಲ್ಲ
        ಹಿಂತಿರುಗಿ ನಿಜ

    ವ್ಯಾಖ್ಯಾನ Is_ಸಂಖ್ಯೆ(self, ಸಂಖ್ಯೆ_String):
        ಪ್ರಯತ್ನಿಸು:
            ದಶಮಾಂಶ(ಸಂಖ್ಯೆ_String)
            ಹಿಂತಿರುಗಿ ನಿಜ
        ಅಪವಾದ ಮೌಲ್ಯ_Error:
            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು

    ವ್ಯಾಖ್ಯಾನ Validate(self):
        """
        Validates the parameter meta data.
        """
        ನೋಡಿದ_Param_ಹೆಸರುಗಳು = []
        #allowedUnits should match set defined in /Firmware/validation/module_schema.yaml
        allowedUnits = ಗಣ ([
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
        ಗಾಗಿ ಗುಂಪು ಒಳಗೆ self.GetParamGroups():
            ಗಾಗಿ param ಒಳಗೆ ಗುಂಪು.GetParams():
                name  = param.GetName()
                ಆದರೆ ಉದ್ದ(name) > 16:
                    sys.stderr.ಲಿಖ("Parameter Name {0} is too long (Limit is 16)\n".format(name))
                    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು
                board = param.Get_ಕ್ಷೇತ್ರ_ಮೌಲ್ಯ("board")
                # Check for duplicates
                name_plus_board = name + "+" + board
                ಗಾಗಿ ನೋಡಿದ_Param_Name ಒಳಗೆ ನೋಡಿದ_Param_ಹೆಸರುಗಳು:
                    ಆದರೆ ನೋಡಿದ_Param_Name == name_plus_board:
                        sys.stderr.ಲಿಖ("Duplicate parameter definition: {0}\n".format(name_plus_board))
                        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು
                ನೋಡಿದ_Param_ಹೆಸರುಗಳು.append(name_plus_board)
                # Validate values
                ಪೂರ್ವನಿಯೋಜಿತ = param.GetDefault()
                min = param.Get_ಕ್ಷೇತ್ರ_ಮೌಲ್ಯ("min")
                max = param.Get_ಕ್ಷೇತ್ರ_ಮೌಲ್ಯ("max")
                units = param.Get_ಕ್ಷೇತ್ರ_ಮೌಲ್ಯ("unit")
                ಆದರೆ units ಅಲ್ಲ ಒಳಗೆ allowedUnits:
                    sys.stderr.ಲಿಖ("Invalid unit in {0}: {1}\n".format(name, units))
                    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು
                #sys.stderr.write("{0} default:{1} min:{2} max:{3}\n".format(name, default, min, max))
                ಆದರೆ ಪೂರ್ವನಿಯೋಜಿತ != "" ಮತ್ತು ಅಲ್ಲ self.Is_ಸಂಖ್ಯೆ(ಪೂರ್ವನಿಯೋಜಿತ):
                    sys.stderr.ಲಿಖ("Default value not number: {0} {1}\n".format(name, ಪೂರ್ವನಿಯೋಜಿತ))
                    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು
                # if default != "" and "." not in default:
                #     sys.stderr.write("Default value does not contain dot (e.g. 10 needs to be written as 10.0): {0} {1}\n".format(name, default))
                #     return False
                ಆದರೆ min != "":
                    ಆದರೆ ಅಲ್ಲ self.Is_ಸಂಖ್ಯೆ(min):
                        sys.stderr.ಲಿಖ("Min value not number: {0} {1}\n".format(name, min))
                        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು
                    ಆದರೆ ಪೂರ್ವನಿಯೋಜಿತ != "" ಮತ್ತು ದಶಮಾಂಶ(ಪೂರ್ವನಿಯೋಜಿತ) < ದಶಮಾಂಶ(min):
                        sys.stderr.ಲಿಖ("Default value is smaller than min: {0} default:{1} min:{2}\n".format(name, ಪೂರ್ವನಿಯೋಜಿತ, min))
                        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು
                ಆದರೆ max != "":
                    ಆದರೆ ಅಲ್ಲ self.Is_ಸಂಖ್ಯೆ(max):
                        sys.stderr.ಲಿಖ("Max value not number: {0} {1}\n".format(name, max))
                        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು
                    ಆದರೆ ಪೂರ್ವನಿಯೋಜಿತ != "" ಮತ್ತು ದಶಮಾಂಶ(ಪೂರ್ವನಿಯೋಜಿತ) > ದಶಮಾಂಶ(max):
                        sys.stderr.ಲಿಖ("Default value is larger than max: {0} default:{1} max:{2}\n".format(name, ಪೂರ್ವನಿಯೋಜಿತ, max))
                        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು
                ಗಾಗಿ ಸಂಕೇತ_2 ಒಳಗೆ param.GetEnumCodes():
                        ಆದರೆ ಅಲ್ಲ self.Is_ಸಂಖ್ಯೆ(ಸಂಕೇತ_2):
                            sys.stderr.ಲಿಖ("Min value not number: {0} {1}\n".format(name, ಸಂಕೇತ_2))
                            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು
                        ಆದರೆ param.Get_Enum_ಮೌಲ್ಯ(ಸಂಕೇತ_2) == "":
                            sys.stderr.ಲಿಖ("Description for enum value is empty: {0} {1}\n".format(name, ಸಂಕೇತ_2))
                            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು
                ಗಾಗಿ ಸೂಚ್ಯಂಕ ಒಳಗೆ param.GetBitmaskList():
                        ಆದರೆ ಅಲ್ಲ self.Is_ಸಂಖ್ಯೆ(ಸೂಚ್ಯಂಕ):
                            sys.stderr.ಲಿಖ("bit value not number: {0} {1}\n".format(name, ಸೂಚ್ಯಂಕ))
                            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು
                        ಆದರೆ ಅಲ್ಲ ಪೂರ್ಣಾಂಕ(min) <= math.pow(2, ಪೂರ್ಣಾಂಕ(ಸೂಚ್ಯಂಕ)) <= ಪೂರ್ಣಾಂಕ(max):
                            sys.stderr.ಲಿಖ("Bitmask bit must be between {0} and {1}: {2} {3}\n".format(min, max, name, math.pow(2, ಪೂರ್ಣಾಂಕ(ಸೂಚ್ಯಂಕ))))
                            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು
                        ಆದರೆ param.Get_Bitmask_ಬಿಟ್(ಸೂಚ್ಯಂಕ) == "":
                            sys.stderr.ಲಿಖ("Description for bitmask bit is empty: {0} {1}\n".format(name, ಸೂಚ್ಯಂಕ))
                            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು
        ಹಿಂತಿರುಗಿ ನಿಜ

    ವ್ಯಾಖ್ಯಾನ GetParamGroups(self):
        """
        Returns the parsed list of parameters. Every parameter is a Parameter
        object. Note that returned object is not a copy. Modifications affect
        state of the parser.
        """
        groups = self.param_groups.ಮೌಲ್ಯಗಳು_2()
        groups = sorted(groups, ಕೀಲಿ=ಅನಾಮಿಕ x: x.GetName())
        groups = sorted(groups, ಕೀಲಿ=ಅನಾಮಿಕ x: self.ಆದ್ಯತೆ.get(x.GetName(), 0), ಹಿಮ್ಮುಖ=ನಿಜ)
        ಹಿಂತಿರುಗಿ groups
