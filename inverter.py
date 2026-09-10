import re

with open("kernel/src/ui/fonts/ter-u16n.c", "r") as f:
    text = f.read()

def reverse_glyph(match):
    values = re.findall(r'0x[0-9A-Fa-f]+', match.group(0))

    if len(values) != 16:
        return match.group(0)

    values.reverse()

    return "{" + ", ".join(values) + "}"

text = re.sub(
    r'\{(?:[^{}]|\n)*?\}',
    reverse_glyph,
    text
)

with open("kernel/src/ui/fonts/ter-u16n_fix.c", "w") as f:
    f.write(text)