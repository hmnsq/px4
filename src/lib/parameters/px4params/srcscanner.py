ଆମଦାନୀ os
ଆମଦାନୀ re
ଆମଦାନୀ codecs
ଆମଦାନୀ sys

ଶ୍ରେଣୀ SourceScanner(object):
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
        ପାଇଁ srcdir ଭିତରେ srcdirs:
            ପାଇଁ filename ଭିତରେ os.listdir(srcdir):
                ଯଦି filename.endswith(extensions1):
                    path = os.path.join(srcdir, filename)
                    ଯଦି ନୁହେଁ self.ScanFile(path, parser):
                        ଫେରନ୍ତୁ ମିଥ୍ୟା
            ପାଇଁ filename ଭିତରେ os.listdir(srcdir):
                ଯଦି filename.endswith(extensions2):
                    path = os.path.join(srcdir, filename)
                    ଯଦି ନୁହେଁ self.ScanFile(path, parser):
                        ଫେରନ୍ତୁ ମିଥ୍ୟା
        ଫେରନ୍ତୁ ସତ୍ୟ

    def ScanFile(self, path, parser):
        """
        Scans provided file and passes its contents to the parser using
        parser.Parse method.
        """

        with codecs.open(path, 'r', 'utf-8') ଇଵ f:
            ଚେଷ୍ଟା:
                contents = f.ପଠ()
            ବ୍ୟତିକ୍ରମ:
                contents = ''
                print('Failed reading file: %s, skipping content.' % path)
                pass
        ଫେରନ୍ତୁ parser.Parse(contents)
