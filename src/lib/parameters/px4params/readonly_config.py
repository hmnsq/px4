"""
Shared helper to load readonly parameter configuration from YAML.
"""
ഇറക്കുമതി sys

ശ്രമിക്കുക:
    ഇറക്കുമതി yaml
അപവാദം ImportError ഇവ e:
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
    എങ്കിൽ readonly_config is None:
        തിരികെ set()

    with open(readonly_config, 'r') ഇവ f:
        config = yaml.safe_load(f)

    mode = config.get('mode', 'block')
    listed_params = set(config.get('parameters', []))

    # Validate that all listed parameters actually exist
    unknown = listed_params - all_param_names
    എങ്കിൽ unknown:
        print("Error: readonly_params.yaml lists unknown parameters: %s" % ', '.join(sorted(unknown)))
        sys.exit(1)

    എങ്കിൽ mode == 'block':
        # Listed params are read-only
        തിരികെ listed_params
    അല്ലെങ്കിലെങ്കിൽ mode == 'allow':
        # Only listed params are writable, all others are read-only
        തിരികെ all_param_names - listed_params
    അല്ലെങ്കിൽ:
        print("Error: readonly_params.yaml has unknown mode '%s' (expected 'block' or 'allow')" % mode)
        sys.exit(1)
