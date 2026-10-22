"""
Shared helper to load readonly parameter configuration from YAML.
"""
आयात sys

प्रयत्न:
    आयात yaml
अपवाद ImportError इव e:
    print("Failed to import yaml: " + str(e))
    print("")
    print("You may need to install it using:")
    print("    pip3 install --user pyyaml")
    print("")
    sys.exit(1)


def load_readonly_params(readonly_config, all_param_names):
    """
    Load readonly parameter config and return the set of readonly param names.

    @param readonly_config: path to readonly_params.yaml
    @param all_param_names: set of all known parameter names
    @return: set of readonly parameter names
    """
    जर readonly_config is None:
        परत set()

    with open(readonly_config, 'r') इव f:
        config = yaml.safe_load(f)

    mode = config.get('mode', 'block')
    listed_params = set(config.get('parameters', []))

    # Validate that all listed parameters actually exist
    unknown = listed_params - all_param_names
    जर unknown:
        print("Error: readonly_params.yaml lists unknown parameters: %s" % ', '.join(sorted(unknown)))
        sys.exit(1)

    जर mode == 'block':
        # Listed params are read-only
        परत listed_params
    किंवाजर mode == 'allow':
        # Only listed params are writable, all others are read-only
        परत all_param_names - listed_params
    अन्यथा:
        print("Error: readonly_params.yaml has unknown mode '%s' (expected 'block' or 'allow')" % mode)
        sys.exit(1)
