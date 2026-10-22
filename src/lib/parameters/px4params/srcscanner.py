import os
import re
import codecs
import sys

class SourceScanner(object):
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
        အတွက် srcdir anta_h srcdirs:
            အတွက် filename anta_h os.listdir(srcdir):
                အကယ်၍ filename.endswith(extensions1):
                    path = os.path.join(srcdir, filename)
                    အကယ်၍ na self.ScanFile(path, parser):
                        ပြန်ပေး False
            အတွက် filename anta_h os.listdir(srcdir):
                အကယ်၍ filename.endswith(extensions2):
                    path = os.path.join(srcdir, filename)
                    အကယ်၍ na self.ScanFile(path, parser):
                        ပြန်ပေး False
        ပြန်ပေး True

    def ScanFile(self, path, parser):
        """
        Scans provided file and passes its contents to the parser using
        parser.Parse method.
        """

        with codecs.open(path, 'r', 'utf-8') as f:
            try:
                contents = f.read()
            except:
                contents = ''
                print('Failed reading file: %s, skipping content.' % path)
                pass
        ပြန်ပေး parser.Parse(contents)
