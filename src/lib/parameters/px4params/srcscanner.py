استورد os
استورد re
استورد codecs
استورد sys

صنف سرچینه_Scanner(object):
    """
    Traverses directory tree, reads all source files, and passes their contents
    to the Parser.
    """

    تعریف لټون_Dir(self, srcdirs, parser):
        """
        Scans provided path and passes all found contents to the parser using
        parser.Parse method.
        """
        extensions1 = tuple([".h"])
        extensions2 = tuple([".c"])
        لكل srcdir في srcdirs:
            لكل filename في os.listdir(srcdir):
                إذا filename.endswith(extensions1):
                    لار = os.لار.join(srcdir, filename)
                    إذا ليس self.لټون_File(لار, parser):
                        أرجع خطأ
            لكل filename في os.listdir(srcdir):
                إذا filename.endswith(extensions2):
                    لار = os.لار.join(srcdir, filename)
                    إذا ليس self.لټون_File(لار, parser):
                        أرجع خطأ
        أرجع ریښتیا

    تعریف لټون_File(self, لار, parser):
        """
        Scans provided file and passes its contents to the parser using
        parser.Parse method.
        """

        سره codecs.open(لار, 'r', 'utf-8') كـ f:
            حاول:
                contents = f.اقرأ()
            استثناء:
                contents = ''
                چاپ('Failed reading file: %s, skipping content.' % لار)
                تیرشه
        أرجع parser.Parse(contents)
