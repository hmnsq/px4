سے xml.sax.saxutils درآمد escape
درآمد codecs
درآمد html

جماعت MarkdownTablesOutput():
    تعریف __init__(self, groups):
        نتیجہ = (
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

        برائے گروہ میں groups:
            نتیجہ += f'## {group.GetName()}\n\n'

            # Wrap each group, and each parameter within it, so the docs theme can skip rendering
            # off-screen content (see .param-group in docs/.vitepress/theme/style.css).
            # The group heading stays outside the wrapper so the page outline can track it.
            نتیجہ += f'<div class="param-group" style="--param-count: {len(group.GetParams())}">\n\n'

            گروہ_is_board_specific = (گروہ.GetName() میں BOARD_SPECIFIC_GROUPS)

            برائے param میں گروہ.GetParams():
                name = param.GetName()

                # Apply note if either the group is board specific
                apply_note_board_specific_گروہ = گروہ_is_board_specific

                # Apply note if name is in list of timer group parameters
                apply_timer_param_note = (name میں PARAM_IS_OUTPUT_TIMER)

                short_desc = param.Get_میدان_قدر("short_desc") یا ''

                # Add fullstop to short_desc if not present
                اگر short_desc:
                    اگر نہیں short_desc.strip().endswith('.'):
                        short_desc += "."

                long_desc = param.Get_میدان_قدر("long_desc") یا ''

                #Strip out short text from start of long text, if it ends in fullstop
                اگر long_desc.startswith(short_desc):
                    long_desc = long_desc[لمبائی(short_desc):].lstrip()

                min_val = param.Get_میدان_قدر("min") یا ''
                max_val = param.Get_میدان_قدر("max") یا ''
                increment = param.Get_میدان_قدر("increment") یا ''
                def_val = param.GetDefault() یا ''
                unit = param.Get_میدان_قدر("unit") یا ''
                type = param.GetType()
                is_boolean = param.GetBoolean()
                reboot_required = param.Get_میدان_قدر("reboot_required") یا ''
                #board = param.GetFieldValue("board") or '' ## Disabled as no board values are defined in any parameters!
                #decimal = param.GetFieldValue("decimal") or '' #Disabled as is intended for GCS not people
                #field_codes = param.GetFieldCodes() ## Disabled as not needed for display.
                #boolean = param.GetFieldValue("boolean") # or '' # Disabled - does not appear useful.

                # Format values for display.
                enum_codes=param.GetEnumCodes() یا '' # Gets numerical values for parameter.
                enum_output=''
                # Format codes and their descriptions for display.
                اگر enum_codes:
                    enum_output+='**Values:**\n\n'
                    enum_codes=sorted(enum_codes,کلید=اعشاری)
                    برائے شے_2 میں enum_codes:
                        enum_output+=f"- `{item}`: {param.GetEnumValue(item)}\n"
                    enum_output+='\n\n'

                bitmask_list=param.GetBitmaskList() #Gets bitmask values for parameter
                bitmask_output=''
                #Format bitmask values
                اگر bitmask_list:
                    bitmask_output+='**Bitmask:**\n\n'
                    برائے ثنائی_ہندسہ میں bitmask_list:
                        ثنائی_ہندسہ_متن = param.Get_Bitmask_ثنائی_ہندسہ(ثنائی_ہندسہ)
                        bitmask_output+=f"- `{bit}`: {bit_text}\n"
                    bitmask_output+='\n\n'

                اگر is_boolean:
                    BOOLEAN_LABELS = {
                        '0': 'Disabled',
                        '1': 'Enabled'
                    }
                    # Give def_value an explanatory string (is def_value if no match)
                    def_val = f"{BOOLEAN_LABELS[def_val]} ({def_val})" اگر def_val میں BOOLEAN_LABELS ورنہ def_val

                    # Format values and their descriptions for display.
                    boolean_اقدار = '**Values:**\n\n'
                    برائے کلید, label میں BOOLEAN_LABELS.اشیاء_2():
                        boolean_اقدار += f"- `{key}`: {label}\n"
                    boolean_اقدار += '\n'

                نتیجہ += '<div class="param">\n\n'
                نتیجہ += f'### {name} (`{type}`)' + ' {#' + name + '}\n\n'
                اگر apply_note_board_specific_گروہ:
                    نتیجہ += f'<Badge type="warning" text="This parameter is only present on some boards." />\n\n'
                اگر apply_timer_param_note:
                    نتیجہ += f'<Badge type="info" text="The exact output numbers where this timer applies are board-specific." />\n\n'
                اگر short_desc:
                    نتیجہ += f'{short_desc}\n\n'
                اگر long_desc:
                    نتیجہ += f'{long_desc}\n\n'
                اگر is_boolean:
                    نتیجہ += boolean_اقدار
                اگر enum_codes:
                    نتیجہ += enum_output
                اگر bitmask_list:
                    نتیجہ += bitmask_output
                # Format the ranges as a table.
                is_readonly = param.GetReadonly()
                نتیجہ += f"Reboot | minValue | maxValue | increment | default | unit | Read-Only\n--- | --- | --- | --- | --- | --- | ---\n{'&check;' if reboot_required else '&nbsp;' } | {min_val} | {max_val} | {increment} | {def_val} | {unit} | {'&check;' if is_readonly else '&nbsp;'}\n\n"
                نتیجہ += '</div>\n\n'

            نتیجہ += '</div>\n\n'

        self.output = نتیجہ

    تعریف Save(self, filename):
        ساتھ codecs.open(filename, 'w', 'utf-8') بطور f:
            f.اكتب(self.output)
