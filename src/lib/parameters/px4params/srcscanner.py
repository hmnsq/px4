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
        សម្រាប់ srcdir anta_h srcdirs:
            សម្រាប់ filename anta_h os.listdir(srcdir):
                ប្រសិនបើ filename.endswith(extensions1):
                    path = os.path.join(srcdir, filename)
                    ប្រសិនបើ na self.ScanFile(path, parser):
                        ត្រឡប់ False
            សម្រាប់ filename anta_h os.listdir(srcdir):
                ប្រសិនបើ filename.endswith(extensions2):
                    path = os.path.join(srcdir, filename)
                    ប្រសិនបើ na self.ScanFile(path, parser):
                        ត្រឡប់ False
        ត្រឡប់ True

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
        ត្រឡប់ parser.Parse(contents)
