استورد os
استورد re
استورد codecs
استورد sys

صنف ذريعو_Scanner(object):
    """
    Traverses directory tree, reads all source files, and passes their contents
    to the Parser.
    """

    وصف جاچ_Dir(self, srcdirs, parser):
        """
        Scans provided path and passes all found contents to the parser using
        parser.Parse method.
        """
        extensions1 = tuple([".h"])
        extensions2 = tuple([".c"])
        لكل srcdir في srcdirs:
            لكل filename في os.listdir(srcdir):
                إذا filename.endswith(extensions1):
                    رستو = os.رستو.join(srcdir, filename)
                    إذا ليس self.جاچ_File(رستو, parser):
                        أرجع خطأ
            لكل filename في os.listdir(srcdir):
                إذا filename.endswith(extensions2):
                    رستو = os.رستو.join(srcdir, filename)
                    إذا ليس self.جاچ_File(رستو, parser):
                        أرجع خطأ
        أرجع سچ

    وصف جاچ_File(self, رستو, parser):
        """
        Scans provided file and passes its contents to the parser using
        parser.Parse method.
        """

        سان codecs.open(رستو, 'r', 'utf-8') كـ f:
            حاول:
                contents = f.اقرأ()
            استثناء:
                contents = ''
                ڇاپيو('Failed reading file: %s, skipping content.' % رستو)
                لنگهه
        أرجع parser.Parse(contents)
