આયાત os
આયાત re
આયાત codecs
આયાત sys

વર્ગ SourceScanner(object):
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
        માટે srcdir માં srcdirs:
            માટે filename માં os.listdir(srcdir):
                જો filename.endswith(extensions1):
                    path = os.path.join(srcdir, filename)
                    જો નહીં self.ScanFile(path, parser):
                        પાછા ખોટું
            માટે filename માં os.listdir(srcdir):
                જો filename.endswith(extensions2):
                    path = os.path.join(srcdir, filename)
                    જો નહીં self.ScanFile(path, parser):
                        પાછા ખોટું
        પાછા સાચું

    def ScanFile(self, path, parser):
        """
        Scans provided file and passes its contents to the parser using
        parser.Parse method.
        """

        with codecs.open(path, 'r', 'utf-8') ઇવ f:
            પ્રયાસ:
                contents = f.પઠ()
            અપવાદ:
                contents = ''
                print('Failed reading file: %s, skipping content.' % path)
                pass
        પાછા parser.Parse(contents)
