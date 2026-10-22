استورد os
استورد re
استورد codecs
استورد sys

صنف منبع_Scanner(object):
    """
    Traverses directory tree, reads all source files, and passes their contents
    to the Parser.
    """

    تعریف پیمایش_Dir(self, srcdirs, parser):
        """
        Scans provided path and passes all found contents to the parser using
        parser.Parse method.
        """
        extensions1 = tuple([".h"])
        extensions2 = tuple([".c"])
        لكل srcdir في srcdirs:
            لكل filename في os.listdir(srcdir):
                إذا filename.endswith(extensions1):
                    مسیر = os.مسیر.join(srcdir, filename)
                    إذا ليس self.پیمایش_File(مسیر, parser):
                        أرجع خطأ
            لكل filename في os.listdir(srcdir):
                إذا filename.endswith(extensions2):
                    مسیر = os.مسیر.join(srcdir, filename)
                    إذا ليس self.پیمایش_File(مسیر, parser):
                        أرجع خطأ
        أرجع درست

    تعریف پیمایش_File(self, مسیر, parser):
        """
        Scans provided file and passes its contents to the parser using
        parser.Parse method.
        """

        با codecs.open(مسیر, 'r', 'utf-8') كـ f:
            حاول:
                contents = f.اقرأ()
            استثناء:
                contents = ''
                چاپ('Failed reading file: %s, skipping content.' % مسیر)
                بگذر
        أرجع parser.Parse(contents)
