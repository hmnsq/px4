"""
Shared helper to load readonly parameter configuration from YAML.
"""
درآمد sys

کوشش:
    درآمد yaml
استثنا ImportError بطور e:
    چھاپو("Failed to import yaml: " + str(e))
    چھاپو("")
    چھاپو("You may need to install it using:")
    چھاپو("    pip3 install --user pyyaml")
    چھاپو("")
    sys.exit(1)


تعریف بھرو_readonly_params(readonly_config, all_param_نام):
    """
    Load readonly parameter config and return the set of readonly param names.

    @param readonly_config: path to readonly_params.yaml
    @param all_param_names: set of all known parameter names
    @return: set of readonly parameter names
    """
    اگر readonly_config ہے کچھ_نہیں:
        واپس سیٹ()

    ساتھ open(readonly_config, 'r') بطور f:
        config = yaml.محفوظ_بھرو(f)

    منوال = config.get('mode', 'block')
    listed_params = سیٹ(config.get('parameters', []))

    # Validate that all listed parameters actually exist
    unknown = listed_params - all_param_نام
    اگر unknown:
        چھاپو("Error: readonly_params.yaml lists unknown parameters: %s" % ', '.join(sorted(unknown)))
        sys.exit(1)

    اگر منوال == 'block':
        # Listed params are read-only
        واپس listed_params
    وگرنہ_اگر منوال == 'allow':
        # Only listed params are writable, all others are read-only
        واپس all_param_نام - listed_params
    ورنہ:
        چھاپو("Error: readonly_params.yaml has unknown mode '%s' (expected 'block' or 'allow')" % منوال)
        sys.exit(1)
