# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What This Is

AML (A Music Language) is a compiler that translates a text-based music notation language into MIDI files. Originally written in 1989, version 0.8 was revived in 2025.

## Build

All build commands run from `src/`:

```sh
cd src
make          # builds both 'aml' (compiler) and 'atom' (MIDI utility)
make clean    # removes *.o, *.out, *.mid, core
make install  # copies aml to /usr/local/bin (requires root)
```

The Makefile uses `cc` with `-g`. There are no external dependencies beyond standard C libraries.

## Usage

```sh
./aml [-t] [-P] [-o outfile] inputfile
```

- Input is an `.bch` text file; output defaults to `inputfile.mid`
- `-t`: enable trace output to stderr
- `-P`: print object representation to stdout
- `#`: comment character in AML source files

Test inputs are in `tests/` (t1–t9, t_cresc, t_rpt, t_turn, etc.) and `bach/` (real pieces). The `src/test/` directory contains older minimal tests; `tests/` is the primary location.

## Language Grammar

The BNF is in `src/bnf`. Key syntax:

- Notes: `a`–`g`, `r` (rest), `_` (tie)
- Accidentals: `+` (sharp), `-` (flat), `=` (natural)
- Octave: `/` (up), `\` (down)
- Dynamics: `!` (forte/louder), `?` (piano/softer)
- Sequence (melodic): `[elements...]` — notes play in succession
- Set (harmonic): `{elements...}` — notes play simultaneously
- Duration multiplier: prefix a number, e.g. `2c` = double-length C
- Parameters (top-level only unless noted): `-t <bpm>` (tempo), `-d <pct>` (duty cycle), `-ch <n>` (channel), `-o <n>` (octave), `-v <n>` (volume), `-ac` (accidental/key)

## Architecture

The compiler is a recursive-descent parser written in a single pass — no AST is built. Parsing and code generation are interleaved.

**Data flow:**
1. `init_io()` reads the entire input file into a buffer (max 32KB) and opens a MIDI output file
2. `song()` loops calling `element()` and `output()` for each top-level element
3. Each parser function (`element`, `dur`, `basic`, `seq`, `set`, `fun`, `dyn`, `note`) builds a linked list of `node` structs
4. `output()` converts `A_NOTE` nodes to `NOTE_ON`/`NOTE_OFF` pairs, sorts by time, and writes MIDI events

**Key types** (defined in `aml.h`):
- `node` — represents a musical event (start time, duration, volume, channel, MIDI note number, duty cycle)
- `ENVIRONMENT` — scoped state passed by value into recursive calls (tempo, octave, channel, key signature, volume, duty); modifications in inner scopes do not affect outer scopes

**Source files:**
- `aml.c` — `main()`, argument parsing
- `song.c` — top-level parser: `song()`, `element()`, `dur()`, `dyn()`, `basic()`
- `seq.c` — `seq()` parser (sequential/melodic groups)
- `set.c` — `set()` parser (simultaneous/harmonic groups)
- `note.c` — `note()` parser, handles accidentals, octave shifts, ties, converts note name to MIDI number
- `fun.c` — `fun()` parser; implemented: `turn`, `cresc`, `decresc`, `rpt`; `fn_element_count` global lets multi-element functions report their width to `seq()`
- `param.c` — `param()` parser, handles `-keyword value` directives
- `io.c` — character I/O, `nextc()`/`pushc()`, `output()`, `parse_error()`
- `node.c` — linked-list operations on `node` structs
- `util.c` — `error()`, trace helpers (`enter`, `leave`, etc.)
- `midifile.c` / `midifile.h` — low-level MIDI file writing

**Directories to ignore:** `src/BAK/`, `MISC/` — old snapshots; `src/RCS/` — RCS version control history.
