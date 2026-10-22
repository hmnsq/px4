"""
Shared helper to load readonly parameter configuration from YAML.
"""
استورد sys

حاول:
    استورد yaml
استثناء ImportError كـ e:
    چاپ("Failed to import yaml: " + str(e))
    چاپ("")
    چاپ("You may need to install it using:")
    چاپ("    pip3 install --user pyyaml")
    چاپ("")
    sys.exit(1)


تعریف بارگیری_readonly_params(readonly_config, all_param_نامها):
    """
    Load readonly parameter config and return the set of readonly param names.

    @param readonly_config: path to readonly_params.yaml
    @param all_param_names: set of all known parameter names
    @return: set of readonly parameter names
    """
    إذا readonly_config است هیچ:
        أرجع مجموعه()

    با open(readonly_config, 'r') كـ f:
        config = yaml.امن_بارگیری(f)

    نما = config.get('mode', 'block')
    listed_params = مجموعه(config.get('parameters', []))

    # Validate that all listed parameters actually exist
    unknown = listed_params - all_param_نامها
    إذا unknown:
        چاپ("Error: readonly_params.yaml lists unknown parameters: %s" % ', '.join(sorted(unknown)))
        sys.exit(1)

    إذا نما == 'block':
        # Listed params are read-only
        أرجع listed_params
    وإلاإذا نما == 'allow':
        # Only listed params are writable, all others are read-only
        أرجع all_param_نامها - listed_params
    وإلا:
        چاپ("Error: readonly_params.yaml has unknown mode '%s' (expected 'block' or 'allow')" % نما)
        sys.exit(1)
