तः xml.sax.saxutils आनयन escape
आनयन codecs
आनयन html

वर्ग MarkdownTablesOutput():
    परिभाषा __init__(self, groups):
        परिणाम = (
"""# Parameter Reference

::: info
This documentation was auto-generated from the source code for this PX4 version (using `make parameters_metadata`).
:::

::: tip
If a listed parameter is missing from the Firmware see: [Finding/Updating Parameters](../advanced_config/parameters.md#parameter-not-in-firmware).
:::

<!-- markdown generator: src/lib/parameters/px4params/markdownout.py -->

"""
                  )

        # Define the groups and parameters that are specific to certain boards, to allow for a note to be added to the documentation.
        BOARD_SPECIFIC_GROUPS = ['Actuator Outputs']
        PARAM_IS_OUTPUT_TIMER = ['PWM_MAIN_TIM0', 'PWM_MAIN_TIM1', 'PWM_MAIN_TIM2', 'PWM_AUX_TIM0', 'PWM_AUX_TIM1', 'PWM_AUX_TIM2', 'PWM_AUX_TIM3']

        प्रति गणः अन्तः groups:
            परिणाम += f'## {group.GetName()}\n\n'

            # Wrap each group, and each parameter within it, so the docs theme can skip rendering
            # off-screen content (see .param-group in docs/.vitepress/theme/style.css).
            # The group heading stays outside the wrapper so the page outline can track it.
            परिणाम += f'<div class="param-group" style="--param-count: {len(group.GetParams())}">\n\n'

            गणः_is_board_specific = (गणः.GetName() अन्तः BOARD_SPECIFIC_GROUPS)

            प्रति param अन्तः गणः.GetParams():
                name = param.GetName()

                # Apply note if either the group is board specific
                apply_note_board_specific_गणः = गणः_is_board_specific

                # Apply note if name is in list of timer group parameters
                apply_timer_param_note = (name अन्तः PARAM_IS_OUTPUT_TIMER)

                short_desc = param.Get_क्षेत्रम्_मानम्("short_desc") वा ''

                # Add fullstop to short_desc if not present
                यदि short_desc:
                    यदि न short_desc.strip().endswith('.'):
                        short_desc += "."

                long_desc = param.Get_क्षेत्रम्_मानम्("long_desc") वा ''

                #Strip out short text from start of long text, if it ends in fullstop
                यदि long_desc.startswith(short_desc):
                    long_desc = long_desc[दीर्घता(short_desc):].lstrip()

                min_val = param.Get_क्षेत्रम्_मानम्("min") वा ''
                max_val = param.Get_क्षेत्रम्_मानम्("max") वा ''
                increment = param.Get_क्षेत्रम्_मानम्("increment") वा ''
                def_val = param.GetDefault() वा ''
                unit = param.Get_क्षेत्रम्_मानम्("unit") वा ''
                type = param.GetType()
                is_boolean = param.GetBoolean()
                reboot_required = param.Get_क्षेत्रम्_मानम्("reboot_required") वा ''
                #board = param.GetFieldValue("board") or '' ## Disabled as no board values are defined in any parameters!
                #decimal = param.GetFieldValue("decimal") or '' #Disabled as is intended for GCS not people
                #field_codes = param.GetFieldCodes() ## Disabled as not needed for display.
                #boolean = param.GetFieldValue("boolean") # or '' # Disabled - does not appear useful.

                # Format values for display.
                enum_codes=param.GetEnumCodes() वा '' # Gets numerical values for parameter.
                enum_output=''
                # Format codes and their descriptions for display.
                यदि enum_codes:
                    enum_output+='**Values:**\n\n'
                    enum_codes=sorted(enum_codes,कुञ्चिका=भिन्न)
                    प्रति item अन्तः enum_codes:
                        enum_output+=f"- `{item}`: {param.GetEnumValue(item)}\n"
                    enum_output+='\n\n'

                bitmask_list=param.GetBitmaskList() #Gets bitmask values for parameter
                bitmask_output=''
                #Format bitmask values
                यदि bitmask_list:
                    bitmask_output+='**Bitmask:**\n\n'
                    प्रति bit अन्तः bitmask_list:
                        bit_text = param.GetBitmaskBit(bit)
                        bitmask_output+=f"- `{bit}`: {bit_text}\n"
                    bitmask_output+='\n\n'

                यदि is_boolean:
                    BOOLEAN_LABELS = {
                        '0': 'Disabled',
                        '1': 'Enabled'
                    }
                    # Give def_value an explanatory string (is def_value if no match)
                    def_val = f"{BOOLEAN_LABELS[def_val]} ({def_val})" यदि def_val अन्तः BOOLEAN_LABELS अन्यथा def_val

                    # Format values and their descriptions for display.
                    boolean_मानानि = '**Values:**\n\n'
                    प्रति कुञ्चिका, label अन्तः BOOLEAN_LABELS.वस्तूनि_2():
                        boolean_मानानि += f"- `{key}`: {label}\n"
                    boolean_मानानि += '\n'

                परिणाम += '<div class="param">\n\n'
                परिणाम += f'### {name} (`{type}`)' + ' {#' + name + '}\n\n'
                यदि apply_note_board_specific_गणः:
                    परिणाम += f'<Badge type="warning" text="This parameter is only present on some boards." />\n\n'
                यदि apply_timer_param_note:
                    परिणाम += f'<Badge type="info" text="The exact output numbers where this timer applies are board-specific." />\n\n'
                यदि short_desc:
                    परिणाम += f'{short_desc}\n\n'
                यदि long_desc:
                    परिणाम += f'{long_desc}\n\n'
                यदि is_boolean:
                    परिणाम += boolean_मानानि
                यदि enum_codes:
                    परिणाम += enum_output
                यदि bitmask_list:
                    परिणाम += bitmask_output
                # Format the ranges as a table.
                is_readonly = param.GetReadonly()
                परिणाम += f"Reboot | minValue | maxValue | increment | default | unit | Read-Only\n--- | --- | --- | --- | --- | --- | ---\n{'&check;' if reboot_required else '&nbsp;' } | {min_val} | {max_val} | {increment} | {def_val} | {unit} | {'&check;' if is_readonly else '&nbsp;'}\n\n"
                परिणाम += '</div>\n\n'

            परिणाम += '</div>\n\n'

        self.output = परिणाम

    परिभाषा Save(self, filename):
        सह codecs.open(filename, 'w', 'utf-8') इव f:
            f.लिख(self.output)
