আমদানি os
আমদানি re
আমদানি codecs
আমদানি sys

শ্রেণী SourceScanner(object):
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
        জন্য srcdir মধ্যে srcdirs:
            জন্য filename মধ্যে os.listdir(srcdir):
                যদি filename.endswith(extensions1):
                    path = os.path.join(srcdir, filename)
                    যদি নয় self.ScanFile(path, parser):
                        ফেরত মিথ্যা
            জন্য filename মধ্যে os.listdir(srcdir):
                যদি filename.endswith(extensions2):
                    path = os.path.join(srcdir, filename)
                    যদি নয় self.ScanFile(path, parser):
                        ফেরত মিথ্যা
        ফেরত সত্য

    def ScanFile(self, path, parser):
        """
        Scans provided file and passes its contents to the parser using
        parser.Parse method.
        """

        with codecs.open(path, 'r', 'utf-8') ইব f:
            চেষ্টা:
                contents = f.পঠ()
            ব্যতিক্রম:
                contents = ''
                print('Failed reading file: %s, skipping content.' % path)
                pass
        ফেরত parser.Parse(contents)
