#!/usr/bin/env python3

from pathlib import Path

import numpy as np

HERE = Path(__file__).parent
OUT = HERE.parent / 'src' / 'arcsin.c'

x = np.arange(0, 1 + 1/64, 1/64)
y = np.round(np.arcsin(x) / (2 * np.pi) * (-1024))

with open(OUT, 'w') as fout:
    fout.write("""
    #include "bh.h"

    const s16 _ARCSIN_PRECALC[] = {
    """
    )
    fout.write(",\n".join(str(e) for e in y))
    fout.write("""
    };

    s16 arcsin(fix16 y) {
        return _ARCSIN_PRECALC[y & 127];
    }
    """
    )
