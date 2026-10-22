"""
Shared helper to load readonly parameter configuration from YAML.
"""
استورد sys

حاول:
    استورد yaml
استثناء ImportError كـ e:
    ڇاپيو("Failed to import yaml: " + str(e))
    ڇاپيو("")
    ڇاپيو("You may need to install it using:")
    ڇاپيو("    pip3 install --user pyyaml")
    ڇاپيو("")
    sys.exit(1)


وصف ڀريو_readonly_params(readonly_config, all_param_نالا):
    """
    Load readonly parameter config and return the set of readonly param names.

    @param readonly_config: path to readonly_params.yaml
    @param all_param_names: set of all known parameter names
    @return: set of readonly parameter names
    """
    إذا readonly_config آهي ڪجهه_نه:
        أرجع سيٽ()

    سان open(readonly_config, 'r') كـ f:
        config = yaml.محفوظ_ڀريو(f)

    گهڻائي = config.get('mode', 'block')
    listed_params = سيٽ(config.get('parameters', []))

    # Validate that all listed parameters actually exist
    unknown = listed_params - all_param_نالا
    إذا unknown:
        ڇاپيو("Error: readonly_params.yaml lists unknown parameters: %s" % ', '.join(sorted(unknown)))
        sys.exit(1)

    إذا گهڻائي == 'block':
        # Listed params are read-only
        أرجع listed_params
    وإلاإذا گهڻائي == 'allow':
        # Only listed params are writable, all others are read-only
        أرجع all_param_نالا - listed_params
    وإلا:
        ڇاپيو("Error: readonly_params.yaml has unknown mode '%s' (expected 'block' or 'allow')" % گهڻائي)
        sys.exit(1)
