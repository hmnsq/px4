आनयन os
आनयन re
आनयन codecs
आनयन sys

वग्ग SourceScanner(object):
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
        अत्थाय srcdir अन्तः srcdirs:
            अत्थाय filename अन्तः os.listdir(srcdir):
                सचे filename.endswith(extensions1):
                    path = os.path.join(srcdir, filename)
                    सचे न self.ScanFile(path, parser):
                        पटिनिवत्त मिच्छा
            अत्थाय filename अन्तः os.listdir(srcdir):
                सचे filename.endswith(extensions2):
                    path = os.path.join(srcdir, filename)
                    सचे न self.ScanFile(path, parser):
                        पटिनिवत्त मिच्छा
        पटिनिवत्त सच्च

    def ScanFile(self, path, parser):
        """
        Scans provided file and passes its contents to the parser using
        parser.Parse method.
        """

        with codecs.open(path, 'r', 'utf-8') इव f:
            यत्न:
                contents = f.पठ()
            अपवाद:
                contents = ''
                print('Failed reading file: %s, skipping content.' % path)
                pass
        पटिनिवत्त parser.Parse(contents)
