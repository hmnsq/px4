तः xml.sax.saxutils आयात escape
आयात codecs
आयात html

वर्ग MarkdownTablesOutput():
    def __init__(self, groups):
        result = (
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

        साठी group मध्ये groups:
            result += f'## {group.GetName()}\n\n'

            # Wrap each group, and each parameter within it, so the docs theme can skip rendering
            # off-screen content (see .param-group in docs/.vitepress/theme/style.css).
            # The group heading stays outside the wrapper so the page outline can track it.
            result += f'<div class="param-group" style="--param-count: {len(group.GetParams())}">\n\n'

            group_is_board_specific = (group.GetName() मध्ये BOARD_SPECIFIC_GROUPS)

            साठी param मध्ये group.GetParams():
                name = param.GetName()

                # Apply note if either the group is board specific
                apply_note_board_specific_group = group_is_board_specific

                # Apply note if name is in list of timer group parameters
                apply_timer_param_note = (name मध्ये PARAM_IS_OUTPUT_TIMER)

                short_desc = param.Get_Field_मूल्य("short_desc") किंवा ''

                # Add fullstop to short_desc if not present
                जर short_desc:
                    जर नाही short_desc.strip().endswith('.'):
                        short_desc += "."

                long_desc = param.Get_Field_मूल्य("long_desc") किंवा ''

                #Strip out short text from start of long text, if it ends in fullstop
                जर long_desc.startswith(short_desc):
                    long_desc = long_desc[len(short_desc):].lstrip()

                min_val = param.Get_Field_मूल्य("min") किंवा ''
                max_val = param.Get_Field_मूल्य("max") किंवा ''
                increment = param.Get_Field_मूल्य("increment") किंवा ''
                def_val = param.GetDefault() किंवा ''
                unit = param.Get_Field_मूल्य("unit") किंवा ''
                type = param.GetType()
                is_boolean = param.GetBoolean()
                reboot_required = param.Get_Field_मूल्य("reboot_required") किंवा ''
                #board = param.GetFieldValue("board") or '' ## Disabled as no board values are defined in any parameters!
                #decimal = param.GetFieldValue("decimal") or '' #Disabled as is intended for GCS not people
                #field_codes = param.GetFieldCodes() ## Disabled as not needed for display.
                #boolean = param.GetFieldValue("boolean") # or '' # Disabled - does not appear useful.

                # Format values for display.
                enum_codes=param.GetEnumCodes() किंवा '' # Gets numerical values for parameter.
                enum_output=''
                # Format codes and their descriptions for display.
                जर enum_codes:
                    enum_output+='**Values:**\n\n'
                    enum_codes=sorted(enum_codes,किल्ली=दशांश)
                    साठी item मध्ये enum_codes:
                        enum_output+=f"- `{item}`: {param.GetEnumValue(item)}\n"
                    enum_output+='\n\n'

                bitmask_list=param.GetBitmaskList() #Gets bitmask values for parameter
                bitmask_output=''
                #Format bitmask values
                जर bitmask_list:
                    bitmask_output+='**Bitmask:**\n\n'
                    साठी bit मध्ये bitmask_list:
                        bit_text = param.GetBitmaskBit(bit)
                        bitmask_output+=f"- `{bit}`: {bit_text}\n"
                    bitmask_output+='\n\n'

                जर is_boolean:
                    BOOLEAN_LABELS = {
                        '0': 'Disabled',
                        '1': 'Enabled'
                    }
                    # Give def_value an explanatory string (is def_value if no match)
                    def_val = f"{BOOLEAN_LABELS[def_val]} ({def_val})" जर def_val मध्ये BOOLEAN_LABELS अन्यथा def_val

                    # Format values and their descriptions for display.
                    boolean_मूल्ये = '**Values:**\n\n'
                    साठी किल्ली, label मध्ये BOOLEAN_LABELS.वस्तुसंच_2():
                        boolean_मूल्ये += f"- `{key}`: {label}\n"
                    boolean_मूल्ये += '\n'

                result += '<div class="param">\n\n'
                result += f'### {name} (`{type}`)' + ' {#' + name + '}\n\n'
                जर apply_note_board_specific_group:
                    result += f'<Badge type="warning" text="This parameter is only present on some boards." />\n\n'
                जर apply_timer_param_note:
                    result += f'<Badge type="info" text="The exact output numbers where this timer applies are board-specific." />\n\n'
                जर short_desc:
                    result += f'{short_desc}\n\n'
                जर long_desc:
                    result += f'{long_desc}\n\n'
                जर is_boolean:
                    result += boolean_मूल्ये
                जर enum_codes:
                    result += enum_output
                जर bitmask_list:
                    result += bitmask_output
                # Format the ranges as a table.
                is_readonly = param.GetReadonly()
                result += f"Reboot | minValue | maxValue | increment | default | unit | Read-Only\n--- | --- | --- | --- | --- | --- | ---\n{'&check;' if reboot_required else '&nbsp;' } | {min_val} | {max_val} | {increment} | {def_val} | {unit} | {'&check;' if is_readonly else '&nbsp;'}\n\n"
                result += '</div>\n\n'

            result += '</div>\n\n'

        self.output = result

    def Save(self, filename):
        with codecs.open(filename, 'w', 'utf-8') इव f:
            f.लिख(self.output)
