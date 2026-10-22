आनयन os
आनयन re
आनयन codecs
आनयन sys

वर्ग SourceScanner(object):
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
        लागि srcdir अन्तः srcdirs:
            लागि filename अन्तः os.listdir(srcdir):
                यदि filename.endswith(extensions1):
                    path = os.path.join(srcdir, filename)
                    यदि होइन self.ScanFile(path, parser):
                        फर्काऊ असत्य
            लागि filename अन्तः os.listdir(srcdir):
                यदि filename.endswith(extensions2):
                    path = os.path.join(srcdir, filename)
                    यदि होइन self.ScanFile(path, parser):
                        फर्काऊ असत्य
        फर्काऊ सत्य

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
        फर्काऊ parser.Parse(contents)
