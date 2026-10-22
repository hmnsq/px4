واردکن os
واردکن re
واردکن codecs
واردکن sys

رده SourceScanner(object):
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
        برای srcdir در srcdirs:
            برای filename در os.listdir(srcdir):
                اگر filename.endswith(extensions1):
                    path = os.path.join(srcdir, filename)
                    اگر نه self.ScanFile(path, parser):
                        بازگشت نادرست
            برای filename در os.listdir(srcdir):
                اگر filename.endswith(extensions2):
                    path = os.path.join(srcdir, filename)
                    اگر نه self.ScanFile(path, parser):
                        بازگشت نادرست
        بازگشت درست

    def ScanFile(self, path, parser):
        """
        Scans provided file and passes its contents to the parser using
        parser.Parse method.
        """

        with codecs.open(path, 'r', 'utf-8') بهعنوان f:
            تلاش:
                contents = f.اقرأ()
            استثنا:
                contents = ''
                print('Failed reading file: %s, skipping content.' % path)
                pass
        بازگشت parser.Parse(contents)
