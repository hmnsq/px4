ഇറക്കുമതി os
ഇറക്കുമതി re
ഇറക്കുമതി codecs
ഇറക്കുമതി sys

വർഗ്ഗം SourceScanner(object):
    """
    Traverses directory tree, reads all source files, and passes their contents
    to the Parser.
    """

    def ScanDir(self, srcdirs, parser):
        """
        Scans provided path and passes all found contents to the parser using
        parser.Parse method.
        """
        extensions1 = tuple([".h"])
        extensions2 = tuple([".c"])
        വേണ്ടി srcdir ഇൽ srcdirs:
            വേണ്ടി filename ഇൽ os.listdir(srcdir):
                എങ്കിൽ filename.endswith(extensions1):
                    path = os.path.join(srcdir, filename)
                    എങ്കിൽ അല്ല self.ScanFile(path, parser):
                        തിരികെ അസത്യം
            വേണ്ടി filename ഇൽ os.listdir(srcdir):
                എങ്കിൽ filename.endswith(extensions2):
                    path = os.path.join(srcdir, filename)
                    എങ്കിൽ അല്ല self.ScanFile(path, parser):
                        തിരികെ അസത്യം
        തിരികെ സത്യം

    def ScanFile(self, path, parser):
        """
        Scans provided file and passes its contents to the parser using
        parser.Parse method.
        """

        with codecs.open(path, 'r', 'utf-8') ഇവ f:
            ശ്രമിക്കുക:
                contents = f.പഠ()
            അപവാദം:
                contents = ''
                print('Failed reading file: %s, skipping content.' % path)
                pass
        തിരികെ parser.Parse(contents)
