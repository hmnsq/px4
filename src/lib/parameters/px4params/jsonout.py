తః xml.sax.saxutils దిగుమతి escape
దిగుమతి codecs
దిగుమతి json
దిగుమతి sys


తరగతి JsonOutput():
    నిర్వచనం __init__(self, groups, board):
        all_json=dict()
        all_json['version']=1
        all_params=[]
        all_json['parameters']=all_params

        all_json["translation"] = {
            "items": {
                "parameters": {
                    "list": {
                        "key": "name",
                        "translate": [ "shortDesc", "longDesc" ],
                        "translate-global": ["category", "group"],
                        "items": {
                            "bitmask": {
                                "list": {
                                    "key": "index",
                                    "translate": [ "description" ]
                                }
                            },
                            "values": {
                                "list": {
                                    "key": "value",
                                    "translate": [ "description" ]
                                }
                            }
                        }
                    }
                }
            }
        }

        schema_map = {
                        "short_desc": "shortDesc",
			"long_desc": "longDesc",
			"unit": "units",
			}
        schema_map_typed = {
			"min": "min",
			"max": "max",
			"increment": "increment",
			}
        schema_map_fix_type = {
			"reboot_required": ("rebootRequired", bool),
			"decimal": ("decimalPlaces", పూర్ణాంకం),
			}
        allowed_types = { "Uint8", "Int8", "Uint16", "Int16", "Uint32", "Int32", "Float"}

        చివరి_param_name = ""
        board_specific_param_సమితి = అబద్ధం
        కొరకు సమూహం లో groups:
            సమూహం_name=సమూహం.GetName()

            నిర్వచనం get_typed_విలువ(విలువ: str, type_name: str):
                ఒకవేళ type_name == 'Float': తిరిగి దశాంశం(విలువ)
                ఒకవేళ type_name == 'Int32': తిరిగి పూర్ణాంకం(విలువ)
                తిరిగి విలువ

            కొరకు param లో సమూహం.GetParams():
                ఒకవేళ (చివరి_param_name == param.GetName() మరియు కాదు board_specific_param_సమితి) లేదా చివరి_param_name != param.GetName():
                    curr_param=dict()
                    curr_param['name'] = param.GetName()
                    type_name = param.GetType().capitalize()
                    curr_param['type'] = type_name
                    ఒకవేళ కాదు type_name లో allowed_types:
                        ముద్రించు("Error: %s type not supported: curr_param['type']" % (curr_param['name'],curr_param['type']) )
                        sys.Exit(1)
                    curr_param['default'] = get_typed_విలువ(param.GetDefault(), type_name)

                    curr_param['group'] = సమూహం_name
                    ఒకవేళ param.GetCategory():
                        curr_param['category'] = param.GetCategory()
                    లేకపోతే:
                        curr_param['category'] = 'Standard'

                    ఒకవేళ param.GetVolatile():
                        curr_param['volatile'] = నిజం
                    ఒకవేళ param.GetReadonly():
                        curr_param['readOnly'] = నిజం

                    చివరి_param_name = param.GetName()
                    కొరకు సంకేతం_2 లో param.Get_క్షేత్రం_Codes():
                        విలువ = param.Get_క్షేత్రం_విలువ(సంకేతం_2)
                        ఒకవేళ సంకేతం_2 == "board":
                            ఒకవేళ విలువ == board:
                                board_specific_param_సమితి = నిజం
                                # JSON schema has no field for board_specific schema. Ignore.
                            లేకపోతే:
                                #xml_group.remove(xml_param)
                                కొనసాగు
                        లేకపోతే:
                            #map PX4 param field names to schema names
                            ఒకవేళ సంకేతం_2 లో schema_map:
                                curr_param[schema_map[సంకేతం_2]] = విలువ
                            లేదావేళ సంకేతం_2 లో schema_map_typed:
                                curr_param[schema_map_typed[సంకేతం_2]] = get_typed_విలువ(విలువ, type_name)
                            లేదావేళ సంకేతం_2 లో schema_map_fix_type:
                                curr_param[schema_map_fix_type[సంకేతం_2][0]] = schema_map_fix_type[సంకేతం_2][1](విలువ)
                            లేకపోతే:
                                ముద్రించు('ERROR: Field not in json schema: %s' % సంకేతం_2)
                                sys.exit(1)


                ఒకవేళ చివరి_param_name != param.GetName():
                    board_specific_param_సమితి = అబద్ధం

                enum_codes=param.GetEnumCodes() లేదా '' # Gets numerical values for parameter.
                ఒకవేళ enum_codes:
                    enum_codes=sorted(enum_codes,కీలకం=దశాంశం)
                    codes_list=list()
                    కొరకు అంశం లో enum_codes:
                        సంకేతం_dict=dict()
                        సంకేతం_dict['value']=get_typed_విలువ(అంశం, type_name)
                        సంకేతం_dict['description']=param.Get_Enum_విలువ(అంశం)
                        codes_list.append(సంకేతం_dict)
                    curr_param['values'] = codes_list
                లేదావేళ param.GetBoolean():
                    curr_param['values'] = [
                        { 'value': 0, 'description': 'Disabled' },
                        { 'value': 1, 'description': 'Enabled' }
                    ]


                ఒకవేళ పొడవు(param.GetBitmaskList()) > 0:
                    bitmasks_list=list()
                    కొరకు సూచీ లో param.GetBitmaskList():
                        bitmask_dict=dict()
                        bitmask_dict['index']=పూర్ణాంకం(సూచీ)
                        bitmask_dict['description']=param.Get_Bitmask_బిట్టు(సూచీ)
                        bitmasks_list.append(bitmask_dict)
                    curr_param['bitmask'] = bitmasks_list


                all_params.append(curr_param)


        #Json string output.
        self.output = json.dumps(all_json, క్రమీకరించు_కీలకాలు=నిజం)


    నిర్వచనం Save(self, filename):
        తోడుగా codecs.open(filename, 'w', 'utf-8') ఇవ f:
            f.లిఖ(self.output)
