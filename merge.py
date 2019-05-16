import codecs
import os
import re

src = ["json/json.h", "json/json-forwards.h", "json/jsoncpp.cpp",
       "src/template.h", "src/template.cpp", 
       "src/bot.h", "src/bot.cpp", "main.cpp"]

lines = []
for filename in src:
    assert (os.path.isfile(filename))
    fobj = codecs.open(filename, "r", "utf-8")
    lines += fobj.readlines()
    lines[-1] += "\n"
    fobj.close()

lines = "".join(lines)

# suppress redundant include header macros
lines = re.sub(r"\s*#include *\"(?!json).*\".*\n", "\n", lines)

target = "merged.cpp"

fobj = codecs.open(target, "w", "utf-8")
fobj.writelines(lines)
fobj.close()

with open(target, "r", encoding="utf-8") as f:
    data = f.readlines()
    f.close()

with open(target, "w", encoding="utf-8") as f:
    for l in data:
        if '#include "json.h"' not in l:
            f.write(l)
    f.close()
