आयात os
आयात re
आयात codecs
आयात sys

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
        साठी srcdir मध्ये srcdirs:
            साठी filename मध्ये os.listdir(srcdir):
                जर filename.endswith(extensions1):
                    path = os.path.join(srcdir, filename)
                    जर नाही self.ScanFile(path, parser):
                        परत असत्य
            साठी filename मध्ये os.listdir(srcdir):
                जर filename.endswith(extensions2):
                    path = os.path.join(srcdir, filename)
                    जर नाही self.ScanFile(path, parser):
                        परत असत्य
        परत सत्य

    def ScanFile(self, path, parser):
        """
        Scans provided file and passes its contents to the parser using
        parser.Parse method.
        """

        with codecs.open(path, 'r', 'utf-8') इव f:
            प्रयत्न:
                contents = f.पठ()
            अपवाद:
                contents = ''
                print('Failed reading file: %s, skipping content.' % path)
                pass
        परत parser.Parse(contents)
