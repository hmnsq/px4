तः xml.sax.saxutils आयात escape
आयात codecs
आयात json
आयात sys


वर्ग JsonOutput():
    def __init__(self, groups, board):
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
			"decimal": ("decimalPlaces", पूर्णांक),
			}
        allowed_types = { "Uint8", "Int8", "Uint16", "Int16", "Uint32", "Int32", "Float"}

        last_param_name = ""
        board_specific_param_set = असत्य
        साठी group मध्ये groups:
            group_name=group.GetName()

            def get_typed_मूल्य(मूल्य: str, type_name: str):
                जर type_name == 'Float': परत दशांश(मूल्य)
                जर type_name == 'Int32': परत पूर्णांक(मूल्य)
                परत मूल्य

            साठी param मध्ये group.GetParams():
                जर (last_param_name == param.GetName() आणि नाही board_specific_param_set) किंवा last_param_name != param.GetName():
                    curr_param=dict()
                    curr_param['name'] = param.GetName()
                    type_name = param.GetType().capitalize()
                    curr_param['type'] = type_name
                    जर नाही type_name मध्ये allowed_types:
                        print("Error: %s type not supported: curr_param['type']" % (curr_param['name'],curr_param['type']) )
                        sys.Exit(1)
                    curr_param['default'] = get_typed_मूल्य(param.GetDefault(), type_name)

                    curr_param['group'] = group_name
                    जर param.GetCategory():
                        curr_param['category'] = param.GetCategory()
                    अन्यथा:
                        curr_param['category'] = 'Standard'

                    जर param.GetVolatile():
                        curr_param['volatile'] = सत्य
                    जर param.GetReadonly():
                        curr_param['readOnly'] = सत्य

                    last_param_name = param.GetName()
                    साठी code मध्ये param.GetFieldCodes():
                        मूल्य = param.Get_Field_मूल्य(code)
                        जर code == "board":
                            जर मूल्य == board:
                                board_specific_param_set = सत्य
                                # JSON schema has no field for board_specific schema. Ignore.
                            अन्यथा:
                                #xml_group.remove(xml_param)
                                सुरूठेवा
                        अन्यथा:
                            #map PX4 param field names to schema names
                            जर code मध्ये schema_map:
                                curr_param[schema_map[code]] = मूल्य
                            किंवाजर code मध्ये schema_map_typed:
                                curr_param[schema_map_typed[code]] = get_typed_मूल्य(मूल्य, type_name)
                            किंवाजर code मध्ये schema_map_fix_type:
                                curr_param[schema_map_fix_type[code][0]] = schema_map_fix_type[code][1](मूल्य)
                            अन्यथा:
                                print('ERROR: Field not in json schema: %s' % code)
                                sys.exit(1)


                जर last_param_name != param.GetName():
                    board_specific_param_set = असत्य

                enum_codes=param.GetEnumCodes() किंवा '' # Gets numerical values for parameter.
                जर enum_codes:
                    enum_codes=sorted(enum_codes,किल्ली=दशांश)
                    codes_list=list()
                    साठी item मध्ये enum_codes:
                        code_dict=dict()
                        code_dict['value']=get_typed_मूल्य(item, type_name)
                        code_dict['description']=param.Get_Enum_मूल्य(item)
                        codes_list.append(code_dict)
                    curr_param['values'] = codes_list
                किंवाजर param.GetBoolean():
                    curr_param['values'] = [
                        { 'value': 0, 'description': 'Disabled' },
                        { 'value': 1, 'description': 'Enabled' }
                    ]


                जर len(param.GetBitmaskList()) > 0:
                    bitmasks_list=list()
                    साठी index मध्ये param.GetBitmaskList():
                        bitmask_dict=dict()
                        bitmask_dict['index']=पूर्णांक(index)
                        bitmask_dict['description']=param.GetBitmaskBit(index)
                        bitmasks_list.append(bitmask_dict)
                    curr_param['bitmask'] = bitmasks_list


                all_params.append(curr_param)


        #Json string output.
        self.output = json.dumps(all_json, sort_किल्ल्या=सत्य)


    def Save(self, filename):
        with codecs.open(filename, 'w', 'utf-8') इव f:
            f.लिख(self.output)
