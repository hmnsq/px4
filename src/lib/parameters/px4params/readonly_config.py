"""
Shared helper to load readonly parameter configuration from YAML.
"""
ଆମଦାନୀ sys

ଚେଷ୍ଟା:
    ଆମଦାନୀ yaml
ବ୍ୟତିକ୍ରମ ImportError ଇଵ e:
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
    ଯଦି readonly_config is None:
        ଫେରନ୍ତୁ set()

    with open(readonly_config, 'r') ଇଵ f:
        config = yaml.safe_load(f)

    mode = config.get('mode', 'block')
    listed_params = set(config.get('parameters', []))

    # Validate that all listed parameters actually exist
    unknown = listed_params - all_param_names
    ଯଦି unknown:
        print("Error: readonly_params.yaml lists unknown parameters: %s" % ', '.join(sorted(unknown)))
        sys.exit(1)

    ଯଦି mode == 'block':
        # Listed params are read-only
        ଫେରନ୍ତୁ listed_params
    ନଚେତଯଦି mode == 'allow':
        # Only listed params are writable, all others are read-only
        ଫେରନ୍ତୁ all_param_names - listed_params
    ଅନ୍ୟଥା:
        print("Error: readonly_params.yaml has unknown mode '%s' (expected 'block' or 'allow')" % mode)
        sys.exit(1)
