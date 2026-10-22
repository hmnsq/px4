తః xml.sax.saxutils దిగుమతి escape
దిగుమతి codecs
దిగుమతి html

తరగతి MarkdownTablesOutput():
    నిర్వచనం __init__(self, groups):
        ఫలితం = (
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

        కొరకు సమూహం లో groups:
            ఫలితం += f'## {group.GetName()}\n\n'

            # Wrap each group, and each parameter within it, so the docs theme can skip rendering
            # off-screen content (see .param-group in docs/.vitepress/theme/style.css).
            # The group heading stays outside the wrapper so the page outline can track it.
            ఫలితం += f'<div class="param-group" style="--param-count: {len(group.GetParams())}">\n\n'

            సమూహం_is_board_specific = (సమూహం.GetName() లో BOARD_SPECIFIC_GROUPS)

            కొరకు param లో సమూహం.GetParams():
                name = param.GetName()

                # Apply note if either the group is board specific
                apply_note_board_specific_సమూహం = సమూహం_is_board_specific

                # Apply note if name is in list of timer group parameters
                apply_timer_param_note = (name లో PARAM_IS_OUTPUT_TIMER)

                short_desc = param.Get_క్షేత్రం_విలువ("short_desc") లేదా ''

                # Add fullstop to short_desc if not present
                ఒకవేళ short_desc:
                    ఒకవేళ కాదు short_desc.strip().endswith('.'):
                        short_desc += "."

                long_desc = param.Get_క్షేత్రం_విలువ("long_desc") లేదా ''

                #Strip out short text from start of long text, if it ends in fullstop
                ఒకవేళ long_desc.startswith(short_desc):
                    long_desc = long_desc[పొడవు(short_desc):].lstrip()

                min_val = param.Get_క్షేత్రం_విలువ("min") లేదా ''
                max_val = param.Get_క్షేత్రం_విలువ("max") లేదా ''
                increment = param.Get_క్షేత్రం_విలువ("increment") లేదా ''
                def_val = param.GetDefault() లేదా ''
                unit = param.Get_క్షేత్రం_విలువ("unit") లేదా ''
                type = param.GetType()
                is_boolean = param.GetBoolean()
                reboot_required = param.Get_క్షేత్రం_విలువ("reboot_required") లేదా ''
                #board = param.GetFieldValue("board") or '' ## Disabled as no board values are defined in any parameters!
                #decimal = param.GetFieldValue("decimal") or '' #Disabled as is intended for GCS not people
                #field_codes = param.GetFieldCodes() ## Disabled as not needed for display.
                #boolean = param.GetFieldValue("boolean") # or '' # Disabled - does not appear useful.

                # Format values for display.
                enum_codes=param.GetEnumCodes() లేదా '' # Gets numerical values for parameter.
                enum_output=''
                # Format codes and their descriptions for display.
                ఒకవేళ enum_codes:
                    enum_output+='**Values:**\n\n'
                    enum_codes=sorted(enum_codes,కీలకం=దశాంశం)
                    కొరకు అంశం లో enum_codes:
                        enum_output+=f"- `{item}`: {param.GetEnumValue(item)}\n"
                    enum_output+='\n\n'

                bitmask_list=param.GetBitmaskList() #Gets bitmask values for parameter
                bitmask_output=''
                #Format bitmask values
                ఒకవేళ bitmask_list:
                    bitmask_output+='**Bitmask:**\n\n'
                    కొరకు బిట్టు లో bitmask_list:
                        బిట్టు_పాఠ్యం = param.Get_Bitmask_బిట్టు(బిట్టు)
                        bitmask_output+=f"- `{bit}`: {bit_text}\n"
                    bitmask_output+='\n\n'

                ఒకవేళ is_boolean:
                    BOOLEAN_LABELS = {
                        '0': 'Disabled',
                        '1': 'Enabled'
                    }
                    # Give def_value an explanatory string (is def_value if no match)
                    def_val = f"{BOOLEAN_LABELS[def_val]} ({def_val})" ఒకవేళ def_val లో BOOLEAN_LABELS లేకపోతే def_val

                    # Format values and their descriptions for display.
                    boolean_విలువలు = '**Values:**\n\n'
                    కొరకు కీలకం, label లో BOOLEAN_LABELS.వస్తువులు_2():
                        boolean_విలువలు += f"- `{key}`: {label}\n"
                    boolean_విలువలు += '\n'

                ఫలితం += '<div class="param">\n\n'
                ఫలితం += f'### {name} (`{type}`)' + ' {#' + name + '}\n\n'
                ఒకవేళ apply_note_board_specific_సమూహం:
                    ఫలితం += f'<Badge type="warning" text="This parameter is only present on some boards." />\n\n'
                ఒకవేళ apply_timer_param_note:
                    ఫలితం += f'<Badge type="info" text="The exact output numbers where this timer applies are board-specific." />\n\n'
                ఒకవేళ short_desc:
                    ఫలితం += f'{short_desc}\n\n'
                ఒకవేళ long_desc:
                    ఫలితం += f'{long_desc}\n\n'
                ఒకవేళ is_boolean:
                    ఫలితం += boolean_విలువలు
                ఒకవేళ enum_codes:
                    ఫలితం += enum_output
                ఒకవేళ bitmask_list:
                    ఫలితం += bitmask_output
                # Format the ranges as a table.
                is_readonly = param.GetReadonly()
                ఫలితం += f"Reboot | minValue | maxValue | increment | default | unit | Read-Only\n--- | --- | --- | --- | --- | --- | ---\n{'&check;' if reboot_required else '&nbsp;' } | {min_val} | {max_val} | {increment} | {def_val} | {unit} | {'&check;' if is_readonly else '&nbsp;'}\n\n"
                ఫలితం += '</div>\n\n'

            ఫలితం += '</div>\n\n'

        self.output = ఫలితం

    నిర్వచనం Save(self, filename):
        తోడుగా codecs.open(filename, 'w', 'utf-8') ఇవ f:
            f.లిఖ(self.output)
