आनयन os
आनयन re
आनयन codecs
आनयन sys

वर्ग SourceScanner(object):
    """
    Traverses directory tree, reads all source files, and passes their contents
    to the Parser.
    """

    परिभाषा अवलोकनम्_Dir(self, srcdirs, parser):
        """
        Scans provided path and passes all found contents to the parser using
        parser.Parse method.
        """
        extensions1 = tuple([".h"])
        extensions2 = tuple([".c"])
        प्रति srcdir अन्तः srcdirs:
            प्रति filename अन्तः os.listdir(srcdir):
                यदि filename.endswith(extensions1):
                    मार्गः = os.मार्गः.join(srcdir, filename)
                    यदि न self.अवलोकनम्_File(मार्गः, parser):
                        प्रत्यागम असत्य
            प्रति filename अन्तः os.listdir(srcdir):
                यदि filename.endswith(extensions2):
                    मार्गः = os.मार्गः.join(srcdir, filename)
                    यदि न self.अवलोकनम्_File(मार्गः, parser):
                        प्रत्यागम असत्य
        प्रत्यागम सत्य

    परिभाषा अवलोकनम्_File(self, मार्गः, parser):
        """
        Scans provided file and passes its contents to the parser using
        parser.Parse method.
        """

        सह codecs.open(मार्गः, 'r', 'utf-8') इव f:
            यत्न:
                contents = f.पठ()
            अपवाद:
                contents = ''
                मुद्रय('Failed reading file: %s, skipping content.' % मार्गः)
                अतिक्रम
        प्रत्यागम parser.Parse(contents)
