# AML -- A Music Language

## Introduction

AML the "language" represents notes and other musical events in text form.
AML the compiler converts that text into a format (a MIDI file) that can be
used on a computer to play those notes. Here, in AML, are the first few
notes of "Mary Had a Little Lamb":

    e d c d e e 2e

At the most abstract level, however, AML is a language that describes a
temporal structure for events. That is, AML describes rhythms in events a
computer can control. The constructs in AML, then, fall into two classes --
constructs for describing events; and constructs defining rhythm (or temporal
structure).

Controllable events are intrinsically platform specific, so the language
constructs describing events are somewhat ad hoc. Currently the basic event
descriptions are oriented to the idea of a "musical note", and the output is
converted to MIDI files, which can be played on a computer.

For the purpose of describing rhythms, however, events have only two
important characteristics -- a start time, and a duration. So the language
constructs defining rhythm are abstract and constant. Basically, there are
only two of them -- "sequences", and "sets". Sequences are lists of events
that happen one after another; sets are lists of events that start at the
same time and take up the same duration. Sets and sequences are themselves
events, so they can be nested to arbitrary complexity.

It is worth being clear about what kind of thing AML is.  Despite the word
"language" and the existence of a "compiler", AML is spiritually a form of
music notation, not a programming language.  An AML file describes a fixed
musical artifact -- there is a direct, static correspondence between the AML
source and the MIDI file it produces.  Nothing in the output depends on
runtime state, execution history, or context that changes as the file is
"run".  A MIDI file has no mechanism for altering its own behavior, and
neither does AML.

This shapes the design in a concrete way.  Constructs that make sense in a
notation system -- shorthands, abbreviations, parametric ornaments, repeat
marks -- are natural additions to AML.  Constructs that belong to a
programming language -- conditionals, loops with evolving state, variables
whose values change during execution -- are outside the model entirely.
When AML compilation is finished, every shorthand has been expanded and
every event has a fixed start time and duration.  The notation disappears;
only the music remains.


## A Quick Start to AML

AML is a language, and the compiler for AML is called `aml`. AML programs
are just text files, like "C" or "perl" or any other computer language. The
output of the aml compiler is a MIDI file, called `aml.mid` by default. This
MIDI file can be fed to any program that will play MIDI files, and you can
hear the result. (In fact, this section will be much more useful if you
compile the examples and play them as you go through. More information on
running the compiler is in Appendix A.)

The following line is a very simple AML program:

    c

This program plays the note middle C for one second. It is easy to elaborate
on this theme:

    c c

This program will play 2 middle C notes, one after another, each one lasting
1 second.

You might think that

    2c

would do the same thing, but it doesn't. Instead, it plays a single note
that lasts two seconds.

It's easy to play other notes as well:

    c d e f g 2r 2c

plays the first 5 notes of the C major scale, one after another, each one
lasting 1 second. Then it plays the special event `r` (a "rest") for 2
seconds. And finally, it plays a C note for 2 seconds.

The special event "rest" just takes up time without doing anything -- it's an
instruction for the musician to take a rest. This illustrates that notes
aren't the only thing described in AML. In fact, AML views the above short
program as a sequence of events, like this:

    event event event event event 2event 2event

AML has a basic idea of how long an event should last. If it sees that the
beginning of an event is a number, it multiplies the basic duration by that
number. The default duration for an event depends on the context surrounding
the event in the program; here, where you just have a top-level list of
events, the default duration is one second per event.

What if you want to play more than one note at a time? In AML, you do that
by specifying a "set" of notes:

    {c e g}

will play the three notes `c`, `e`, and `g`, all at the same time. The `{`
marks the beginning of a set; the `}` marks its end. AML considers a set to
be a single event (which is further composed of other events). So, in this
case the whole set will last for one second, and each of the notes in the set
persists during that one second. We can elaborate on this theme as well:

    {c e g} {d f a} {b d f g} 2{c e g}

will play 4 sets of notes. The first three sets last one second each; the
last one takes 2 seconds. Remember that AML sees this program as:

    event event event 2event

The `{` and `}` characters group subsidiary events into a larger event. The
`[` and `]` characters also group events into a larger event, but in a more
interesting way called a "sequence". For example, AML sees

    [c e g]

as a single event, composed of 3 smaller events. The meaning of a "sequence"
event is to play the included events one after another, dividing up the time
among them. So the above example would play the three notes `c`, `e`, and `g`
one after the other, and give each note a third of a second, because the time
allocated for the whole sequence is 1 second, and there are three events in
the sequence.

Inside a sequence the time is divided in the same way as described previously
-- if an event has a number in front of it, that number represents its portion
of the total time. So, for example, the program consisting of the single
sequence

    [2c d e f g 2c]

lasts 1 second. There are 6 events inside the sequence, but the first and
last events have a leading `2`, and so are supposed to last twice as long as
they otherwise would. So the first event lasts for a quarter of a second, the
next 4 events last an eighth of a second apiece, and the last event also
lasts a quarter second. Here's "Mary Had a Little Lamb" (MHALL) in one
second:

    [e d c d e e 2e d d 2d e g 2g e d c d e e e e d d e d 4c]

That's a little fast. We can easily spread it out to take 8 seconds:

    8[e d c d e e 2e d d 2d e g 2g e d c d e e e e d d e d 4c]

Of course, sequences and sets can be freely intermixed. Here's an embellished
version of MHALL:

    8{
        [e d c d  e e 2e  d d 2d  e g 2g  e d c d  e e e e  d d e d  4c]
        [2c  r r  4c      4b\     4c      2c  r r  4c       4b\      4r]
    }

This is an AML program consisting of a single set that lasts 8 seconds. The
set is in turn composed of two sequences, each of which inherits the full
duration of the set. The top sequence has the MHALL melody; the bottom
sequence is a very simple harmony. They both play at the same time. The
bottom line has two new bits of notation -- the `r` note, which is a "rest",
and the `\` character, which modifies the preceding note to play an octave
lower than it otherwise would. Notice that the spacing between the notes in
the bottom sequence has been carefully adjusted so that things line up --
experience has shown that it is *very* important to arrange things so they
make sense visually. For example, the program could be written like this:

    8{[e d c d e e 2e d d 2d e g 2g e d c d e e e e d d e d 4c] [2c r r 4c 4b\
    4c 2c r r 4c 4b\ 4r]}

It will sound exactly the same. However, the following

    8{[e d c d e e 2e d d 2d e g 2g e d c d e e e e d d e d 4c] [2c r r
    4c 4b\ 4c 2c r r 4c 4b\]}

will sound quite different, and it can be quite difficult to spot the
problem: the final `4r` is missing from the second sequence, and the timing
is off -- all the other notes and rests have to stretch their time out a
little to compensate for the missing time.


## Notes

The preceding examples have used a fairly restricted set of notes. In
generality, a note is considerably more complicated. Notes consist of a basic
note name, with additional characters that modify the meaning. Notes (and
other events) are separated from each other by space characters.

The seven lower case letters `a`, `b`, `c`, `d`, `e`, `f`, and `g` are the
"note name characters". A note name character, optionally followed by an
accidental, defines a "note name". Sharps and flats are represented by the
`+` and `-` characters, respectively, added after the primary note name.
Thus, `c+` refers to C sharp, `e-` is an E flat. Double flats and double
sharps are allowed. A mechanism analogous to a key signature can change the
interpretation of a note name, so that `b` would refer to B flat, for
example. The `=` is a natural, so, in such a case, `b=` would play a B
natural.

Note names do not specify absolute pitches. The absolute pitch of a note is
determined either through context, or by an explicit octave specifier. The
octave specifier, if present, is a digit that follows the note name. The
octave specifiers are ranged so that `a4` is the standard tuning fork pitch
A440:

    a0 =    27.5 Hz
    a1 =    55   Hz
    a2 =   110   Hz
    a3 =   220   Hz
    a4 =   440   Hz
    a5 =   880   Hz
    a6 =  1760   Hz
    a7 =  3520   Hz
    a8 =  7040   Hz
    a9 = 14080   Hz

In accordance with common musical usage, we start the scale at `c`. So the
ascending notes of the C major scale, starting at `c4` (middle C), are:

    c4 d4 e4 f4 g4 a4 b4 c5

That is, the `a` and `b` represent pitches *above* `c`, not below it.
Incidentally, 4 is the default octave number.

The characters `/` and `\` are used to indicate a shift in pitch of one
octave -- `/` indicates raising the pitch one octave; `\` indicates that the
pitch should be dropped one octave. These characters can be "cascaded" -- `//`
means raise the pitch two octaves, for example.

The following AML program would play a C major scale, one note per second,
starting on middle C, and ending on the `c` an octave higher:

    c d e f g a b c/

The chromatic scale starting with middle C looks like this:

    c c+ d d+ e f f+ g g+ a a+ b c/ b- a a- g g- f e e- d d- c

Absolute octave numbers are "modal" -- when one appears, it sets the default
octave number. For example,

    c6 d e f g a b c/

plays a C major scale 2 octaves above middle C. Once the `6` appears, all
the notes following will be assumed to be in the sixth octave.

However, the relative octave designators `/` and `\` are NOT modal.

An AML program consisting of the following two lines will play the same 10
note sequence twice:

    c\\\\ c\\\ c\\ c\ c  c/ c// c/// c//// c/////
    c0    c1   c2  c3 c4 c5 c6  c7   c8    c9

> Note: not all hardware will play this full range.

The important difference between the two lines is that the first line leaves
the default octave number at 4, and the second line leaves it at 9.

Notes have a duration, and that duration is defined in terms of a "current
tempo". In all the above examples the current tempo has been the initial
default tempo of 60 beats per minute, or one beat per second. The tempo can
be changed. The beat is very frequently subdivided into parts, in a way that
will be described when we get to sequences. Notes can be any number of beats
long. The following line illustrates this:

    7c 6d 5e 4f 3g 2a 1b c/

The first C lasts 7 beats, the D lasts 6 beats, the E lasts 5, and so on.
The `1` in front of the B is redundant -- both the `1b` and the `c/` last one
beat. The numbers 7, 6, and so on in the above examples are known as
"duration modifiers", and can be applied to any event, not just notes. They
will be described more fully below. One important point about them -- they are
always whole numbers; there is no direct way to express a fractional
duration. Instead, beats are subdivided using sequences.


## Sequences

A sequence is a list of events that will occur one after another. Because
sequences can be arbitrarily nested they are a surprisingly powerful and
compact way to express very complex rhythms.

A sequence is notated by enclosing the listed events in square brackets, like
this:

    [c d e f g a b c/]

Here the events are individual note events, and this sequence plays a C major
scale. For long sequences it is sometimes nice to have AML check to be sure
that you really have the number of notes in the sequence that you think you
do. If a number immediately follows the opening `[`, and the number is
followed by a blank, AML will treat the number as a check of the count. Thus

    [8 c d e f g a c/]

will cause AML to detect an error, because there are really only 7 notes in
the sequence -- you might not have noticed that at first look. But

    [8c d e f g a c/]

is a sequence with 14 sub-beats, or "counts". The first C gets 8 counts, and
the remaining notes get one count each.

A sequence can appear any place that a note can, and its total time is the
same as the time for an individual note that it might replace. Here is a
short AML program:

    c d e f g a b c/

Here is the same program with the last note replaced by a sequence:

    c d e f g a b [c d e f g a b c/]

Both of these programs take 8 beats (8 seconds, at the default tempo). The
eighth beat of the last program is occupied by a fast 8-note scale passage.
If we think of this as 4/4 time, each of the single notes outside the
sequence would be a quarter note, and the notes inside the sequence would be
32nd notes.

Remembering that a sequence can appear anywhere a note can, the example could
be further expanded as follows:

    c d e f g a b  [c d e f g a b [c d e f g a b c/]]

This program also takes 8 seconds, but the last sequence happens in a 32nd
note interval -- this is a short run of 256th notes. (At this point we are in
the vicinity of MIDI's fundamental limits, though with practice the ear can
pick up a little information from runs this fast.)

Since a sequence can appear anywhere a note can, the following is legal:

    c d 2[e f g]

If this is 4/4 time, the last element represents a quarter-note triplet. The
whole sequence takes two beats, and within those two beats there are three
notes. A slightly more complex example is this:

    c d 2[2e f]

The sequence contains 3 counts, so we still have a triplet figure, but with
the `e` occupying 2 of the three counts.

Complex rhythms are easy to do with sequences:

    c [2 d e] [3 f g [4 a e c e]] [5 b g 3g+] a b c/ c

Sometimes it is nice to be able to tie a note from one sequence to another.
The underscore character `_` is used to indicate this:

    c 3[7 4d e f g_] 2[7 _g a a a+ b 2c]

There are a couple of interesting things to notice about that example. One is
the tied G -- essentially what happens is that only the "note on" MIDI signal
is sent when a `_` character is seen following a note, and the corresponding
"note off" isn't sent until a `_` preceding the exact same pitch is seen.
Another interesting feature is the complexity of the rhythm -- this would be
very difficult to notate with conventional notation. Notice that each of the
sequences is 7 sub-beats long, but that the total durations are different.
The duration of the `d` in the first sequence is 12/7ths of a second. The
duration of the G is 3/7 + 2/7 or 5/7 of a second.

Finally, notice the `3[7 4d`. It couldn't be written without the space
between the `7` and the `4`. What would `3[74d e f g_]` mean? It could mean
`3[74 e f g_]`, a sequence of 74 sub-beats that was incorrectly written, or
it could mean `3[ 74d e f g_]`, which would be a sequence of 77 sub-beats,
with 74 of them applied to the first note. In fact, it would mean the latter.
AML only uses the first number inside a `[` to mean a sequence count if it is
separated from the contents of the sequence by a space.


## Chords

Sequences order events in time. Frequently one wants to have events occur at
the same time. In AML, if things are enclosed in braces `{` and `}`, then
they happen at the same time. Furthermore, each item in the chord takes the
same amount of time. For example,

    3{c e g}

is a C major chord that lasts 3 beats -- each note inside gets the full three
beats.

Chords can also appear anywhere a note can. Chords can appear in sequences
and sequences can occur in chords. From this you can see that the term
"chord" is really a little misleading. What AML calls a chord is really a
general simultaneous grouping. For example,

    [{c e} {d f} {e g}]

represents a triplet run of thirds. This could also be notated as

    { [c d e] [e f g] }

AML doesn't care about lines, so it might be better to write the preceding
example as

    {
      [e f g]
      [c d e]
    }

In transcribing contrapuntal music or orchestral music, where one thinks of
the music as parallel sequences, it works out fairly neatly to organize it
something like the following example:

    12{
       [12 c5 c d | e e f | g g a | [f+ g f+] [g f+ e= f+] g_  ]
       [12 2c3  r | c c d | e e c | d         c            b=\ ]
    }

This example is the beginning of a simple minuet by Bach. It continues like
this:

    12{
      [12 -o5 [_g a] [f= e=] f_ | [_f g] [e d] e | [c b=\] c d | 3g\             ]
      [12 -o2 2b             a= | 2a           g | 2a        f | g [g3 f] [e3 d] ]
    }

Several new things are introduced by these examples. First of all, the letter
`r` in the second sequence means "rest". It is considered to be a note name
character, just like `c`, but it doesn't make any noise.

This piece is in 3/4 time, and each set of braces represents 4 measures. The
`12` preceding each opening brace `{` means that the "chord" is 12 beats
long. However, each of the parallel sequences is also 12 long, so the
individual notes inside end up being 1 second (or one beat) each. The `|`
marks are measure marks. AML really doesn't pay any attention to them, but
experience has taught that it really helps to have some mark like that
available to keep track of where you are.

Notice the `-o5` and the `-o2` strings in the sequences. The `-o` means "set
the default octave". These are "Parameters", and they will be explained in
the next section.

Notice that some attempt has been made to keep notes that sound at the same
time in vertical alignment. It isn't really necessary, but it helps a great
deal when transcribing music. AML is a very terse language, and it is
extremely easy to lose your place while you are typing.

AML doesn't know anything about the vertical alignment, however. It always
reads characters left to right, one line after another. As far as AML is
concerned these examples might as well be written this way:

    12{[12 c5 c d e e f g g a [f+ g f+] [g f+ e= f+] g_] [12 2c3 r c c d e e
    c d c b=\]} 12{[12 -o5 [_g a] [f= e=] f_ [_f g] [e d] e [c b=\] c d 3g\
    ] [12 -o2 2b a= 2a g 2a f g [g3 f] [e3 d]]}

You would go blind or insane in a short time if you tried to read stuff like
this.


## Parameters

We just saw an example of a parameter. Here are some more -- the actual first
line of the Bach minuet looks like this:

    -t108 -ac e- -ac b- -ac a-

This line reads "set the default tempo to 108 beats per minute, set the
default accidental for E to flat, the default accidental for B to flat, and
the default accidental for A to flat". That is, set the key signature to C
minor. These are all examples of parameters.

A parameter always begins with a dash. After the dash comes an abbreviation
for the parameter name. After the parameter name may come a number, or a note
name, or anything else that is appropriate for the particular parameter
setting. The current parameters are:

| Parameter | Description |
|-----------|-------------|
| `-ac`     | Set accidental (using a note name; default all naturals) |
| `-ch`     | Set MIDI channel (0-15; default 0) |
| `-v`      | Set overall volume (0-127; default 64) |
| `-o`      | Set default octave number (0-9; default 4) |


## Dynamics

Two marks can be used to change the relative volume of a note, sequence, or
chord. The "emphasize" character is `!`, and it multiplies the volume (as
represented by the MIDI velocity number) by about 1.2. The "subdue" character
is `?`, and it multiplies the MIDI velocity number by the inverse of 1.2.
That is, `?!` leaves the volume unchanged. These operators can be stacked,
like the relative octave designators. The following AML program plays a short
scale gradually increasing in volume:

    ???c ??c ?c c !c !!c !!!c

Dynamic marks are like relative octave designators, in that they only change
the volume of the entity to which they are prefixed. This might seem to be a
severe limitation, but in practice it isn't, since the operator can be
applied to whole lines of music making the line into a sequence, and
prefixing the sequence with the appropriate dynamic marking.


## Functions and Macros

Functions are invoked with parentheses:

    (functionname arguments...)

A function call is an event, just like a note or a sequence, and can appear
anywhere an event can.  The arguments, if any, follow the function name
inside the parentheses.  Functions are transparent to the surrounding
context -- a function that expands to N events in a sequence is treated as
though those N events appeared directly in the sequence.


### (turn note)

Plays a four-note ornament: upper neighbor, the note itself, lower neighbor,
the note itself again.  The neighbors are diatonic (they follow the key
signature).  The four notes divide the allocated time equally.

    (turn c)

plays B, C, D, C in rapid succession (in the default key of C major).  The
turn can be used inside a sequence like any other event:

    [a b (turn c) d e]


### (cresc start-dyn end-dyn elements...)
### (decresc start-dyn end-dyn elements...)

Apply a linear volume ramp across one or more elements.  The dynamic
levels are specified using runs of `!` (louder) and `?` (softer), just
like the prefix dynamics described above.  `cresc` ramps from the start
level up to the end level; `decresc` ramps from the start level down to
the end level.  For example:

    (cresc ?? !!! [c d e f g])

plays a five-note ascending scale with volume increasing from two steps
below the base level to three steps above it.  The ramp is applied
linearly across all the notes.

    (decresc !! ? {c e g})

plays a chord with volume decreasing from two steps above the base to one
step below it.

The dynamics notation follows the same rules as prefix dynamics: each `!`
multiplies the velocity by approximately 1.2, and each `?` divides by 1.2,
so `?!` leaves the volume unchanged and the marks are true inverses of
each other.


### (rpt n elements...)

Repeats the given elements n times in sequence.  The numeric argument is
optional; if omitted, the default is 2 (repeat once, i.e. play twice total).

    (rpt 3 [c d e])

plays the sequence [c d e] three times back to back.

    (rpt [c d e])

plays it twice.  The repeated elements can be any combination of notes,
sequences, chords, or other function calls:

    (rpt 4 {c e g})

plays a C major chord four times in succession.


### (def name body)

Defines a zero-parameter macro named `name` with the given body.  After
the definition, `(name)` anywhere in the program is replaced by the body
text and re-parsed in place.  The body can contain any AML notation.

    (def run1 c d e f g)

After this definition, writing `(run1)` is exactly equivalent to writing
`c d e f g`.  In a sequence:

    [(run1) a b]

is treated as though it were `[c d e f g a b]` -- the macro contributes
5 elements to the sequence count, not 1.

Macro bodies may contain function calls, sequences, chords, or other
macros (including recursive references, up to the nesting limit).  A macro
definition itself produces no musical output; it is a compile-time
directive only.  Definitions must appear at the top level of the program,
not inside a sequence, chord, or function call.

A macro may be redefined at the top level -- for example, to use one
pattern in the first half of a piece and a different one in the second
half.  Redefining a macro causes a warning on stderr; suppress it with the
`-w` compiler flag (see Appendix A).


### (def (name p1 p2 ... pn) body)

Defines a parametric macro.  The parameter names are listed inside
parentheses after the macro name.  In the body, each parameter is
referenced as `$pname`.  When the macro is invoked, the arguments
are substituted for the corresponding `$pname` references and the
result is re-parsed in place.

    (def (triad root) $root . {. .// .//})

After this definition:

    (triad c)

expands and plays: C, C (current note), then the chord {C, E, G}.

    (triad e)

expands and plays: E, E, then the chord {E, G, B}.

Arguments at the call site are separated by whitespace.  A bracketed
group `[...]`, `{...}`, or `(...)` is treated as a single argument,
so compound expressions can be passed:

    (def (loud x) !!!$x)
    (loud [c d e])          # plays [c d e] at high volume

Parameter names consist of letters and digits and must start with a
letter.  The `$name` reference in the body uses greedy longest-match,
so `$root` does not fire inside `$rootnote` -- they are distinct names.
If you need a parameter reference immediately followed by a literal
digit (e.g. param `v` then `2c`), insert a space: `$v 2c`.


### The '.' notation: current note and diatonic stepping

The `.` character refers to the "current note" -- the last note played.
It can appear anywhere a note can.  Suffix `/` and `\` modifiers step
the current note up or down by diatonic scale degrees (following the
current key signature), exactly as `/` and `\` shift octaves on a
regular note.

    .       play the current note again
    ./      step up one diatonic degree and play
    .//     step up two diatonic degrees and play
    .\      step down one diatonic degree and play
    .\\     step down two diatonic degrees and play

Every note event -- whether a named note, a `.` expression, or a macro
expansion -- updates the current note.  This includes notes inside chords,
which are processed left to right, so successive `.` expressions within
a chord build on each other:

    c . {. .// .//}

plays C, C, then the chord {C, E, G} -- each `.//` steps two degrees
from wherever the current note currently stands.

The current note is global and persists across sequences and chords.  At
the top level:

    a b c . ./ c

is equivalent to `a b c c d c` -- the `.` repeats C, the `./` steps to D,
then `c` plays C and resets the current note to C.

The initial default current note is middle C.


## Scope, Operator Order, and Other Technical Details

Sequences, chords, and functions are nested constructs. That is, they define
a strict hierarchy wherever they occur -- one can't overlap another. For
example,

    { [...] [...] {...} }
    [ {...} { [...] [...] } ]

are correctly nested. On the other hand, strings like `{ [ } ]` are illegal.
The first line is a chord that strictly contains two sequences and a
subsidiary chord. The second line is a sequence that contains two chords. The
second chord itself contains two sequences.

Sometimes it is clearer to indicate the levels by using extra lines and
indentation. The above two lines might be more clearly represented as:

    {           # level 1
      [...]     # level 2
      [...]     # level 2
      {...}     # level 2
    }           # level 1
    [           # level 1
      {...}     # level 2
      {         # level 2
        [...]   # level 3
        [...]   # level 3
      }         # level 2
    ]           # level 1


---

## Appendix A: Running the Compiler

The AML compiler is invoked as:

    aml [options] inputfile

The input file is conventionally given a `.bch` extension.  The output is
a MIDI file; by default it is written to `inputfile.mid`.

Options:

| Flag        | Description |
|-------------|-------------|
| `-o file`   | Write MIDI output to `file` instead of the default |
| `-t`        | Enable trace output to stderr (for debugging) |
| `-P`        | Print the internal event representation to stdout |
| `-w`        | Suppress compiler warnings (e.g. macro redefinition) |
| `-p`        | Play the MIDI file after compiling |

Example:

    aml -o minuet.mid minuet.bch
    aml -p minuet.bch

The `-p` flag always writes the MIDI file first, then invokes the player.
The player command is selected in this order:

1. The `AML_PLAYER` environment variable, if set
2. The default compiled into the binary at build time
3. The hardcoded fallback `fluidsynth -i`

The player is invoked as `player-command midifile`, so any player that
accepts a filename as its last argument will work.  To change the
compiled-in default (e.g. to use timidity on a machine without
fluidsynth), build with:

    make PLAYER="timidity"


---

*Copyright (c) 1989 by Songbird Software and Kent Crispin. All rights reserved.*
