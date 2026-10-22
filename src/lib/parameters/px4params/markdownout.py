তঃ xml.sax.saxutils আমদানি escape
আমদানি codecs
আমদানি html

শ্রেণী MarkdownTablesOutput():
    def __init__(self, groups):
        ফলাফল = (
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

        জন্য group মধ্যে groups:
            ফলাফল += f'## {group.GetName()}\n\n'

            # Wrap each group, and each parameter within it, so the docs theme can skip rendering
            # off-screen content (see .param-group in docs/.vitepress/theme/style.css).
            # The group heading stays outside the wrapper so the page outline can track it.
            ফলাফল += f'<div class="param-group" style="--param-count: {len(group.GetParams())}">\n\n'

            group_is_board_specific = (group.GetName() মধ্যে BOARD_SPECIFIC_GROUPS)

            জন্য param মধ্যে group.GetParams():
                name = param.GetName()

                # Apply note if either the group is board specific
                apply_note_board_specific_group = group_is_board_specific

                # Apply note if name is in list of timer group parameters
                apply_timer_param_note = (name মধ্যে PARAM_IS_OUTPUT_TIMER)

                short_desc = param.Get_Field_মান("short_desc") অথবা ''

                # Add fullstop to short_desc if not present
                যদি short_desc:
                    যদি নয় short_desc.strip().endswith('.'):
                        short_desc += "."

                long_desc = param.Get_Field_মান("long_desc") অথবা ''

                #Strip out short text from start of long text, if it ends in fullstop
                যদি long_desc.startswith(short_desc):
                    long_desc = long_desc[len(short_desc):].lstrip()

                min_val = param.Get_Field_মান("min") অথবা ''
                max_val = param.Get_Field_মান("max") অথবা ''
                increment = param.Get_Field_মান("increment") অথবা ''
                def_val = param.GetDefault() অথবা ''
                unit = param.Get_Field_মান("unit") অথবা ''
                type = param.GetType()
                is_boolean = param.GetBoolean()
                reboot_required = param.Get_Field_মান("reboot_required") অথবা ''
                #board = param.GetFieldValue("board") or '' ## Disabled as no board values are defined in any parameters!
                #decimal = param.GetFieldValue("decimal") or '' #Disabled as is intended for GCS not people
                #field_codes = param.GetFieldCodes() ## Disabled as not needed for display.
                #boolean = param.GetFieldValue("boolean") # or '' # Disabled - does not appear useful.

                # Format values for display.
                enum_codes=param.GetEnumCodes() অথবা '' # Gets numerical values for parameter.
                enum_output=''
                # Format codes and their descriptions for display.
                যদি enum_codes:
                    enum_output+='**Values:**\n\n'
                    enum_codes=sorted(enum_codes,চাবি=দশমিক)
                    জন্য item মধ্যে enum_codes:
                        enum_output+=f"- `{item}`: {param.GetEnumValue(item)}\n"
                    enum_output+='\n\n'

                bitmask_list=param.GetBitmaskList() #Gets bitmask values for parameter
                bitmask_output=''
                #Format bitmask values
                যদি bitmask_list:
                    bitmask_output+='**Bitmask:**\n\n'
                    জন্য bit মধ্যে bitmask_list:
                        bit_text = param.GetBitmaskBit(bit)
                        bitmask_output+=f"- `{bit}`: {bit_text}\n"
                    bitmask_output+='\n\n'

                যদি is_boolean:
                    BOOLEAN_LABELS = {
                        '0': 'Disabled',
                        '1': 'Enabled'
                    }
                    # Give def_value an explanatory string (is def_value if no match)
                    def_val = f"{BOOLEAN_LABELS[def_val]} ({def_val})" যদি def_val মধ্যে BOOLEAN_LABELS অন্যথা def_val

                    # Format values and their descriptions for display.
                    boolean_মানসমূহ = '**Values:**\n\n'
                    জন্য চাবি, label মধ্যে BOOLEAN_LABELS.বস্তুসমূহ_2():
                        boolean_মানসমূহ += f"- `{key}`: {label}\n"
                    boolean_মানসমূহ += '\n'

                ফলাফল += '<div class="param">\n\n'
                ফলাফল += f'### {name} (`{type}`)' + ' {#' + name + '}\n\n'
                যদি apply_note_board_specific_group:
                    ফলাফল += f'<Badge type="warning" text="This parameter is only present on some boards." />\n\n'
                যদি apply_timer_param_note:
                    ফলাফল += f'<Badge type="info" text="The exact output numbers where this timer applies are board-specific." />\n\n'
                যদি short_desc:
                    ফলাফল += f'{short_desc}\n\n'
                যদি long_desc:
                    ফলাফল += f'{long_desc}\n\n'
                যদি is_boolean:
                    ফলাফল += boolean_মানসমূহ
                যদি enum_codes:
                    ফলাফল += enum_output
                যদি bitmask_list:
                    ফলাফল += bitmask_output
                # Format the ranges as a table.
                is_readonly = param.GetReadonly()
                ফলাফল += f"Reboot | minValue | maxValue | increment | default | unit | Read-Only\n--- | --- | --- | --- | --- | --- | ---\n{'&check;' if reboot_required else '&nbsp;' } | {min_val} | {max_val} | {increment} | {def_val} | {unit} | {'&check;' if is_readonly else '&nbsp;'}\n\n"
                ফলাফল += '</div>\n\n'

            ফলাফল += '</div>\n\n'

        self.output = ফলাফল

    def Save(self, filename):
        with codecs.open(filename, 'w', 'utf-8') ইব f:
            f.লিখ(self.output)
