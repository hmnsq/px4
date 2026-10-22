தஃ xml.sax.saxutils இறக்குமதி escape
இறக்குமதி codecs
இறக்குமதி json
இறக்குமதி sys


வகுப்பு JsonOutput():
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
			"decimal": ("decimalPlaces", முழு),
			}
        allowed_types = { "Uint8", "Int8", "Uint16", "Int16", "Uint32", "Int32", "Float"}

        last_param_name = ""
        board_specific_param_set = பொய்
        க்கு group உள் groups:
            group_name=group.GetName()

            def get_typed_மதிப்பு(மதிப்பு: str, type_name: str):
                எனில் type_name == 'Float': திரும்பு தசம(மதிப்பு)
                எனில் type_name == 'Int32': திரும்பு முழு(மதிப்பு)
                திரும்பு மதிப்பு

            க்கு param உள் group.GetParams():
                எனில் (last_param_name == param.GetName() மற்றும் இல்லை board_specific_param_set) அல்லது last_param_name != param.GetName():
                    curr_param=dict()
                    curr_param['name'] = param.GetName()
                    type_name = param.GetType().capitalize()
                    curr_param['type'] = type_name
                    எனில் இல்லை type_name உள் allowed_types:
                        print("Error: %s type not supported: curr_param['type']" % (curr_param['name'],curr_param['type']) )
                        sys.Exit(1)
                    curr_param['default'] = get_typed_மதிப்பு(param.GetDefault(), type_name)

                    curr_param['group'] = group_name
                    எனில் param.GetCategory():
                        curr_param['category'] = param.GetCategory()
                    இல்லையெனில்:
                        curr_param['category'] = 'Standard'

                    எனில் param.GetVolatile():
                        curr_param['volatile'] = உண்மை
                    எனில் param.GetReadonly():
                        curr_param['readOnly'] = உண்மை

                    last_param_name = param.GetName()
                    க்கு code உள் param.GetFieldCodes():
                        மதிப்பு = param.Get_Field_மதிப்பு(code)
                        எனில் code == "board":
                            எனில் மதிப்பு == board:
                                board_specific_param_set = உண்மை
                                # JSON schema has no field for board_specific schema. Ignore.
                            இல்லையெனில்:
                                #xml_group.remove(xml_param)
                                தொடர்
                        இல்லையெனில்:
                            #map PX4 param field names to schema names
                            எனில் code உள் schema_map:
                                curr_param[schema_map[code]] = மதிப்பு
                            அல்லதெனில் code உள் schema_map_typed:
                                curr_param[schema_map_typed[code]] = get_typed_மதிப்பு(மதிப்பு, type_name)
                            அல்லதெனில் code உள் schema_map_fix_type:
                                curr_param[schema_map_fix_type[code][0]] = schema_map_fix_type[code][1](மதிப்பு)
                            இல்லையெனில்:
                                print('ERROR: Field not in json schema: %s' % code)
                                sys.exit(1)


                எனில் last_param_name != param.GetName():
                    board_specific_param_set = பொய்

                enum_codes=param.GetEnumCodes() அல்லது '' # Gets numerical values for parameter.
                எனில் enum_codes:
                    enum_codes=sorted(enum_codes,சாவி=தசம)
                    codes_list=list()
                    க்கு item உள் enum_codes:
                        code_dict=dict()
                        code_dict['value']=get_typed_மதிப்பு(item, type_name)
                        code_dict['description']=param.Get_Enum_மதிப்பு(item)
                        codes_list.append(code_dict)
                    curr_param['values'] = codes_list
                அல்லதெனில் param.GetBoolean():
                    curr_param['values'] = [
                        { 'value': 0, 'description': 'Disabled' },
                        { 'value': 1, 'description': 'Enabled' }
                    ]


                எனில் len(param.GetBitmaskList()) > 0:
                    bitmasks_list=list()
                    க்கு index உள் param.GetBitmaskList():
                        bitmask_dict=dict()
                        bitmask_dict['index']=முழு(index)
                        bitmask_dict['description']=param.GetBitmaskBit(index)
                        bitmasks_list.append(bitmask_dict)
                    curr_param['bitmask'] = bitmasks_list


                all_params.append(curr_param)


        #Json string output.
        self.output = json.dumps(all_json, sort_சாவிகள்=உண்மை)


    def Save(self, filename):
        with codecs.open(filename, 'w', 'utf-8') இவ f:
            f.லிக(self.output)
